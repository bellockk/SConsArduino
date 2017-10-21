env
Repository('/usr/share/arduino/hardware/arduino/avr/cores')
env = Environment(AVR_BIN_PREFIX='avr-',
                  CC=AVR_BIN_PREFIX + 'gcc',
                  CXX=AVR_BIN_PREFIX + 'g++',
                  AS=AVR_BIN_PREFIX + 'gcc',
                  CPPPATH=['build/core'],
                  CPPDEFINES={'F_CPU': F_CPU, 'ARDUINO': ARDUINO_VER},
                  CFLAGS=cFlags + ['-std=gnu99'],
                  CCFLAGS=cFlags,
                  ASFLAGS=['-assembler-with-cpp', '-mmcu=%s' % MCU],
                  TOOLS=['gcc', 'g++', 'as'])
env = Environment()
Export('env')
artifacts = SConscript('src/SConscript', variant_dir='build', duplicate=0)
