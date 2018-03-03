// Arduino
#include <Arduino.h>

// Display
#include "LiquidCrystal.h"

// Real Time Clock
#include <Wire.h>
#include <RtcDS3231.h>

// SD Writer
#include <SPI.h>
#include <SD.h>

// Temperature Sensor
#include <OneWire.h>
#include <DallasTemperature.h>

// Circular Buffer (Button Bounce Removal)
#include <CircularBuffer.h>

// Temperature Sensor
const byte temp_pin = 2;
OneWire oneWire(temp_pin);
DallasTemperature temperature_sensor(&oneWire);

// SD Card Reader
const byte chip_select = 3;
String data_filename;
File data_file_object;
unsigned long frame_counter = 0;

// Water Sensors
const byte water_level_1_pin = 1;
const byte water_level_2_pin = 0;

// Clock
RtcDS3231<TwoWire> Rtc(Wire);
uint8_t last_second = 0;

// Display
LiquidCrystal lcd(7, 8, 9, 10, 4, 5);

// Button
const byte button_pin = 6;
byte button_state = LOW;
byte button_last_state = LOW;
byte toggle_state = LOW;
byte toggle_last_state = LOW;
byte button_sensed;
CircularBuffer<byte, 10> button_buffer;

// Declared weak in Arduino.h to allow user redefinitions.
int atexit(void (* /*func*/ )()) { return 0; }

// Weak empty variant initialization function.
// May be redefined by variant files.
void initVariant() __attribute__((weak));
void initVariant() { }

void setupUSB() __attribute__((weak));
void setupUSB() { }

#define countof(a) (sizeof(a) / sizeof(a[0]))

int main(void)
{
    init();

    initVariant();

#if defined(USBCON)
    USBDevice.attach();
#endif
    /////////////////////
    //      INIT       //
    /////////////////////

    // Initialize Temperature Sensor
    temperature_sensor.begin();

    // set up the LCD's number of columns and rows:
    lcd.begin(16, 2);

    // Initialize SD Card
    lcd.setCursor(12,0);
    if (!SD.begin(chip_select)) {
        lcd.print("E");
    }
    else {
        lcd.print(" ");
    }

    // Start the Clock
    Rtc.Begin();
    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    RtcDateTime now = Rtc.GetDateTime();
    if (!Rtc.IsDateTimeValid() || now < compiled) 
    {
        Rtc.SetDateTime(compiled);
    }
    if (!Rtc.GetIsRunning())
    {
        Rtc.SetIsRunning(true);
    }
    Rtc.Enable32kHzPin(false);
    Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone); 
    last_second = now.Second();

    // Set built-in LED Mode
    pinMode(LED_BUILTIN, OUTPUT);

    // Setup Button Pin
    pinMode(button_pin, INPUT_PULLUP);  

    for (;;) {
        /////////////////////
        //      LOOP       //
        /////////////////////

        // Update Clock
        RtcDateTime now = Rtc.GetDateTime();
        char datestring[20];
        snprintf_P(datestring, 
                countof(datestring),
                PSTR("%02u:%02u:%02u"),
                now.Hour(),
                now.Minute(),
                now.Second() );
        lcd.setCursor(0,1);
        lcd.print(datestring);

        // Get Button State
        button_sensed = digitalRead(button_pin);
        button_buffer.push(byte(button_sensed));
        byte sum = 0;
        for (unsigned int i = 0; i < button_buffer.size(); i++){
            sum += button_buffer[i];
        }
        if (button_last_state == LOW && sum > 7) {
            button_state = HIGH;
            lcd.setCursor(14,0);
            lcd.print("B");
        }
        else if (button_last_state == HIGH && sum < 3) {
            button_state = LOW;
            lcd.setCursor(14,0);
            lcd.print(" ");
            if (toggle_last_state == LOW) {
                toggle_state = HIGH;
                frame_counter = 0;
                lcd.setCursor(15,0);
                lcd.print("R");
            }
            else {
                toggle_state = LOW;
                if (data_file_object) {
                    data_file_object.close();
                    lcd.setCursor(9,1);
                    lcd.print("       ");
                }
                lcd.setCursor(15,0);
                lcd.print(" ");
            }
            frame_counter = 0;
        }
        toggle_last_state = toggle_state;
        button_last_state = button_state;

        // Record Data at 1Hz
        uint8_t current_second = now.Second();
        if (current_second != last_second) {

            // Water Level Sensor
            int sensed_level_1 = analogRead(water_level_1_pin);
            int sensed_level_2 = analogRead(water_level_2_pin);
            lcd.setCursor(0,0);
            lcd.print(sensed_level_1);
            lcd.setCursor(4,0);
            lcd.print(sensed_level_2);

            // Get Temperature
            lcd.setCursor(8,0);
            temperature_sensor.requestTemperatures();
            lcd.print(temperature_sensor.getTempFByIndex(0));

            if (toggle_state == HIGH) {
                if (frame_counter == 0) {
                    // Initialize Log File
                    char filename[13];
                    snprintf_P(filename,
                            countof(filename),
                            PSTR("%02u%02u%02u%02u.csv"),
                            now.Month(),
                            now.Day(),
                            now.Hour(),
                            now.Minute());
                    data_file_object = SD.open(filename, FILE_WRITE);
                    lcd.setCursor(13,0);
                    if (!data_file_object) {
                        lcd.print("E");
                    }
                    else {
                        lcd.print(" ");
                        data_file_object.println("frame_s, level1, level2, temp_f");
                    }
                }
                frame_counter++;

                // Write Record
                lcd.setCursor(9,1);
                lcd.print(frame_counter);
                if (data_file_object) {
                    data_file_object.print(frame_counter);
                    data_file_object.print(",");
                    data_file_object.print(sensed_level_1);
                    data_file_object.print(",");
                    data_file_object.print(sensed_level_2);
                    data_file_object.print(",");
                    data_file_object.println(temperature_sensor.getTempFByIndex(0));
                    data_file_object.flush();
                }
            }

            // 1/Z
            last_second = current_second; 
        }

        // Handle Serial Events (Do Last)
        if (serialEventRun) serialEventRun();
    }

    return 0;
}
