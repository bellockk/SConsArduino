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

// Temperature Sensor
const int temp_pin = 2;
OneWire oneWire(temp_pin);
DallasTemperature temperature_sensor(&oneWire);

// SD Card Reader
const int chip_select = 3;
String data_filename;
File data_file_object;
unsigned long frame_counter = 0;

// Water Sensors
const int water_level_1_pin = 0;
const int water_level_2_pin = 1;

// Clock
RtcDS3231<TwoWire> Rtc(Wire);
uint8_t last_second = 0;

// Display
LiquidCrystal lcd(7, 8, 9, 10, 4, 5);

// Button Pin
int button_pin = 6;

// Button States
int button_state = LOW;
int button_sensed = LOW;
int button_last_state = LOW;

// Bounce Parameters
long bounce_time = 0;
const long debounce = 40;

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

    // Print a message to the LCD.
    // lcd.print("Lets Go Alex!");

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

    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(button_pin, INPUT_PULLUP);  

    for (;;) {
        /////////////////////
        //      LOOP       //
        /////////////////////

        // Update Clock
        lcd.setCursor(15,0);
        if (!Rtc.IsDateTimeValid()) {
            lcd.print("E");
        }
        else {
            lcd.print(" ");
        }
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
        if (button_sensed == HIGH && button_last_state == LOW && millis() - bounce_time > debounce) {
            lcd.setCursor(13,0);
            if (button_state == HIGH){
                button_state = LOW;
                lcd.print(" ");
                if (data_file_object) {
                    data_file_object.close();
                }
                frame_counter = 0;
            }
            else {
                button_state = HIGH;
                lcd.print("R");
            }
            bounce_time = millis();
        }
        digitalWrite(LED_BUILTIN, button_state);
        button_last_state = button_sensed;

        // Record Data at 1Hz
        uint8_t current_second = now.Second();
        if (current_second != last_second) {
            if (button_state == HIGH) {
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
                    lcd.setCursor(14,0);
                    if (!data_file_object) {
                        lcd.print("E");
                    }
                    else {
                        // Write Header
                        lcd.print(" ");
                        data_file_object.println("frame_s, level1, level2, temp_f");
                    }
                }
                frame_counter++;

                // Water Level Sensor
                int sensed_level_1 = analogRead(water_level_1_pin);
                int sensed_level_2 = analogRead(water_level_2_pin);
                lcd.setCursor(0,0);
                lcd.print(sensed_level_1);
                lcd.setCursor(5,0);
                lcd.print(sensed_level_2);

                // Get Temperature
                lcd.setCursor(10,1);
                temperature_sensor.requestTemperatures();
                lcd.print(temperature_sensor.getTempFByIndex(0));

                // Write Record
                lcd.setCursor(10,0);
                lcd.print(frame_counter);
                if (data_file_object) {
                    data_file_object.print(frame_counter);
                    data_file_object.print(", ");
                    data_file_object.print(sensed_level_1);
                    data_file_object.print(", ");
                    data_file_object.print(sensed_level_2);
                    data_file_object.print(", ");
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
