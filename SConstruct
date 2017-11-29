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
          metavar='PORT', default='/dev/ttyACM0', help='tty port [default: %default]')
AddOption('--cpu', dest='CPU', type='string', nargs=1, action='store',
          metavar='CPU', default='atmega328p', help='processor [default: %default]')

env = Environment(
    ARDUINO_PREFIX='/usr/share/arduino',
    ARDUINO_HARDWARE='${ARDUINO_PREFIX}/hardware',
    AVR_PREFIX='${ARDUINO_HARDWARE}/arduino/avr',
    AVR_PLATFORM='${AVR_PREFIX}/platform.txt',
    AVR_BOARDS='${AVR_PREFIX}/boards.txt',
    AVR_CORES='${AVR_PREFIX}/cores',
    AVR_BOARD=GetOption('board'),
    build_mcu=GetOption('CPU'),
    source_file='${SOURCE}',
    object_file='${TARGET}',
    includes='${_CPPINCFLAGS}',
    build_path='${TARGET.dir}',
    archive_file='${TARGET.file}')
parse_platform(env.subst('${AVR_BOARDS}'), env, GetOption('board'))
parse_platform(env.subst('${AVR_PLATFORM}'), env)
env.Replace(
    CC='${compiler_c_cmd}',
    AS='${compiler_S_cmd}',
    AR='${compiler_ar_cmd}',
    CXX='${compiler_cpp_cmd}',
    CFLAGS='${compiler_c_flags}',
    CPPFLAGS='${compiler_cpp_flags}',
    LINKFLAGS='${compiler_ldflags}',
    CCCOM='${recipe_c_o_pattern}',
    ASCOM='${recipe_S_o_pattern}',
    CXXCOM='${recipe_cpp_o_pattern}',
    ARCOM='${recipe_ar_pattern}')
env.Append(CPPPATH=['${AVR_PREFIX}/variants/${build_variant}'])
Repository(env.subst('${AVR_CORES}'))
print(env.Dump())
Export('env')
artifacts = SConscript('arduino/SConscript', variant_dir='build/arduino', duplicate=0)
artifacts = SConscript('src/SConscript', variant_dir='build/src', duplicate=0)
