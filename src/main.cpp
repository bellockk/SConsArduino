#include <Arduino.h>
#include "LiquidCrystal.h"

// initialize the library with the numbers of the interface pins
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
  lcd.print("Lets Science!");
  lcd.setCursor(8,1);
  lcd.print("Alex");

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

      // 1/Z
      button_last_state = button_sensed;

      // Handle Serial Events (Do Last)
      if (serialEventRun) serialEventRun();
  }

  return 0;
}

