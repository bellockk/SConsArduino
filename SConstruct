import pyparsing
import re

def parse_platform(platform_file_path):

    # Read file into memory
    with open(platform_file_path) as f:
        platform_content = f.read()

    # Assemble parsing expression
    key = pyparsing.Word(pyparsing.alphanums + '*-/._')('key')
    equals = pyparsing.Suppress('=')
    value = pyparsing.SkipTo('#' | pyparsing.lineEnd)('value')
    expression = key + equals + value
    expression.ignore('#' + pyparsing.restOfLine)

    # Scan the file for matches assembling the return dictionary
    result = {'': ''}
    for token in expression.scanString(platform_content):
        # Expand internal references to other fields
        re_obj = re.compile('|'.join(re.escape('{%s}') % s for s in result))
        result[token[0].key] = re_obj.sub(lambda m: str(
            result[m.group()[1:len(m.group()) - 1]]), token[0].value)
    del result['']
    return result

env = Environment(
    ARDUINO_PREFIX='/usr/share/arduino',
    ARDUINO_HARDWARE='${ARDUINO_PREFIX}/hardware',
    AVR_PREFIX='${ARDUINO_HARDWARE}/arduino/avr',
    AVR_PLATFORM='${AVR_PREFIX}/platform.txt',
    AVR_CORES='${AVR_PREFIX}/cores')
PLATFORM = parse_platform(env.subst('${AVR_PLATFORM}'))
cFlags = ['-ffunction-sections', '-fdata-sections', '-fno-exceptions',
          '-funsigned-char', '-funsigned-bitfields', '-fpack-struct',
          '-fshort-enums', '-Os', '-Wall', '-mmcu=${MCU}']
env.Replace(
    CC=PLATFORM['compiler.c.cmd'],
    AS=PLATFORM['compiler.c.cmd'],
    AR=PLATFORM['compiler.ar.cmd'],
    CPPDEFINES=['F_CPU=16000000L', 'ARDUINO=10606', 'ARDUINO_AVR_UNO', 'ARDUINO_ARCH_AVR'],
    CXX=PLATFORM['compiler.cpp.cmd'],
    CFLAGS=PLATFORM['compiler.c.flags'].split(),
    CPPFLAGS=PLATFORM['compiler.cpp.flags'].split(),
    ASFLAGS=PLATFORM['compiler.S.flags'].split())
env.Append(
    CFLAGS=['-mmcu=atmega328p'],
    CPPFLAGS=['-mmcu=atmega328p'],
    CPATH=['${AVR_PREFIX}/variants/standard'],
    CPPPATH=['${AVR_PREFIX}/variants/standard'])
Repository(env.subst('${AVR_CORES}'))
Export('env')
artifacts = SConscript('arduino/SConscript', variant_dir='build/arduino', duplicate=0)
artifacts = SConscript('src/SConscript', variant_dir='build/src', duplicate=0)
