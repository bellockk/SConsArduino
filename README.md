# SConsArduino
Template for Arduino Projects built with SCons

This is repository is meant to be a template for arduino projects I will be working on.  It requires the Arduino IDE be installed before running.  To simplify this script as much as possible, the Arduino IDE's build recipies and environments are loaded into a SCons environment, and Command builders are utilized to build the targets from the Arduino IDE build recipies.

To use:

1. Install Arduino IDE
2. Update src/main.cpp with your code.
3. Build
```
>> scons
```
4. Upload to Arduino
```
>> bin/upload
```

The followng options are available on SCons to configure Arduino options.
```
Local Options:
  --board=BOARD               board tag [default: nano]
  --port=PORT                 tty port [default: /dev/ttyUSB0]
  --cpu=CPU                   processor [default: atmega328p]
  --baud=BAUD                 baud rate [default: 57600]
```
