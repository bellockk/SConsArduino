// Arduino
#include <Arduino.h>

// Display
#include "LiquidCrystal.h"

// Water Sensors
int water_level_1_pin = 0;
int water_level_2_pin = 1;

// Real Time Clock
#include <Wire.h>
#include <RtcDS3231.h>

// Clock
RtcDS3231<TwoWire> Rtc(Wire);

// Display
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

// Button Pin
int button_pin = 6;

// Button States
int button_state = LOW;
int button_sensed = HIGH;
int button_last_state = LOW;

// Bounce Parameters
long time = 0;
long debounce = 200;

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

    // set up the LCD's number of columns and rows:
    lcd.begin(16, 2);

    // Print a message to the LCD.
    lcd.print("Lets Go Alex!");

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

    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(button_pin, INPUT_PULLUP);  

    for (;;) {
        /////////////////////
        //      LOOP       //
        /////////////////////

        // Get Button State
        button_sensed = digitalRead(button_pin);
        if (button_sensed == HIGH && button_last_state == LOW && millis() - time > debounce) {
            if (button_state == HIGH){
                button_state = LOW;
            }
            else {
                button_state = HIGH;
            }
            time = millis();
        }
        digitalWrite(LED_BUILTIN, button_state);

        // Water Level Sensor 1
        int sensed_level_1 = analogRead(water_level_1_pin);
        int sensed_level_2 = analogRead(water_level_2_pin);
        lcd.setCursor(9,1);
        lcd.print(sensed_level_1);
        lcd.setCursor(13,1);
        lcd.print(sensed_level_2);

        // Update Clock
        lcd.setCursor(15,0);
        if (!Rtc.IsDateTimeValid()) {
            lcd.print("0");
        }
        else {
            lcd.print("1");
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


        /////////
        // 1/Z //
        /////////

        // Button
        button_last_state = button_sensed;

        // Handle Serial Events (Do Last)
        if (serialEventRun) serialEventRun();
    }

    return 0;
}

