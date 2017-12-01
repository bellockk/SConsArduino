import pyparsing
import re

REG_OBJ_KEY = re.compile(r'({(.*?)})')

def parse_platform(platform_file_path, env, prefix=None):

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
    for token in expression.scanString(platform_content):
        key = token[0].key.replace('.', '_').replace('-', '_')
        value = REG_OBJ_KEY.sub(
            lambda s: '${%s}' % s.group(2).replace('.', '_').replace(
                '-', '_'), token[0].value)
        if prefix:
            if key.startswith(prefix):
                env[key[len(prefix)+1:]] = value
        else:
            env[key] = value

AddOption('--board', dest='board', type='string', nargs=1, action='store',
          metavar='BOARD', default='nano', help='board tag [default: %default]')
AddOption('--port', dest='port', type='string', nargs=1, action='store',
          metavar='PORT', default='/dev/ttyUSB0', help='tty port [default: %default]')
AddOption('--cpu', dest='CPU', type='string', nargs=1, action='store',
          metavar='CPU', default='atmega328p', help='processor [default: %default]')
AddOption('--baud', dest='baud', type='string', nargs=1, action='store',
          metavar='BAUD', default='57600', help='baud rate [default: %default]')

env = Environment(
    ARDUINO_PREFIX='/usr/share/arduino',
    ARDUINO_HARDWARE='${ARDUINO_PREFIX}/hardware',
    AVR_PREFIX='${ARDUINO_HARDWARE}/arduino/avr',
    AVR_PLATFORM='${AVR_PREFIX}/platform.txt',
    AVR_BOARDS='${AVR_PREFIX}/boards.txt',
    AVR_CORES='${AVR_PREFIX}/cores',
    AVR_BOARD=GetOption('board'),
    build_mcu=GetOption('CPU'),
    serial_port=GetOption('port'),
    upload_speed=GetOption('baud'),
    path='/',
    includes='${_CPPINCFLAGS}',
    build_path='${TARGET.dir}',
    build_project_name='${TARGET.name}',
    build_arch='AVR')
parse_platform(env.subst('${AVR_BOARDS}'), env, GetOption('board'))
parse_platform(env.subst('${AVR_PLATFORM}'), env)
with open(env.subst('${ARDUINO_PREFIX}/lib/version.txt'), 'r') as f:
    major, minor, build = [int(n) for n in f.read().split('.')]
env.Replace(
    runtime_ide_version='%d%02d%02d' % (major, minor, build))
env.Append(CPPPATH=['${AVR_PREFIX}/variants/${build_variant}'])
Repository(env.subst('${AVR_CORES}'))
Export('env')
artifacts = SConscript('arduino/SConscript', variant_dir='build/arduino', duplicate=0)
artifacts = SConscript('src/SConscript', variant_dir='build/src', duplicate=0)
