if #arg == 0 then
  io.write[[
Creates a Makefile to build the DDLParser library as well as a test, template
based code generator.

Usage: lua createmk.lua <platform> [ -I path ]... [ -L path ]...

Where <platform> can be mingw, linux, msvc-x86 or msvc-x64. If your Lua and
AStyle development files are not in the compiler's search paths, you can
specify additional include paths with -I and library paths with -L. This is
only needed for Linux, Lua and AStyle development headers and
libraries for MinGW and MSVC are included.

mingw:    Open a MSYS prompt, create the Makefile and run make. You must have
          a recent version of Lua compiled with doubles in your include and
          library search paths as well as gperf.

linux:    Open a bash prompt, create the Makefile and run make. You must have
          a recent version of Lua compiled with doubles in your include and
          library search paths as well as gperf.

msvc-x86: Open a Visual Studio command prompt, create the Makefile
          and run nmake.

msvc-x64: Open a Visual Studio x64 Win64 command prompt, create the Makefile
          and run nmake.
]]
  os.exit()
end

local platform = arg[ 1 ]

local incdirs = ''
local libdirs = ''

local i = 2

while i <= #arg do
  if arg[ i ] == '-I' then
    i = i + 1

    if i > #arg then
      io.stderr:write( 'Missing argument for -I\n' )
      os.exit( 1 )
    end
    
    incdirs = incdirs .. '-I' .. arg[ i ] .. ' '
  elseif arg[ i ] == '-L' then
    i = i + 1

    if i > #arg then
      io.stderr:write( 'Missing argument for -L\n' )
      os.exit( 1 )
    end

    libdirs = libdirs .. '-L' .. arg[ i ] .. ' '
  else
    io.stderr:write( 'Unknown option ', arg[ i ], '\n' )
    os.exit( 1 )
  end

  i = i + 1
end

local config = {}
config.INCDIRS = incdirs
config.LIBDIRS = libdirs

if platform == 'mingw' then
  config.EXEEXT  = '.exe'
  config.OBJEXT  = '.o'
  config.LIBNAME = 'lib%1.a'

  config.CC       = 'g++ -O2 -Iinclude -Ideps -D__WIN__ -DDDLT_TEMPLATE_DIR="\\"/usr/local/share/ddlt/\\"" -Wall -Wno-format -o $@ -c %1'
  config.CCLIB    = config.CC
  config.CCLIBD   = 'g++ -O0 -g -Iinclude -Ideps -D__WIN__ -DDDLT_TEMPLATE_DIR="\\"/usr/local/share/ddlt/\\"" -Wall -Wno-format -o $@ -c %1'
  config.LIB      = 'ar cru $@ ${ALLDEPS} && ranlib $@'
  config.LINK     = 'g++ -Ldeps -o $@ ${ALLDEPS}'
  config.GPERF    = 'gperf -c -C -l -L C++ -t -7 -m 100 -I'
  config.RM       = 'rm -f'
  config.LUA      = 'util/lua'
  config.INSTLIB  = 'cp output/release/libddlparser.a /usr/local/lib && cp include/DDLParser.h /usr/local/include'
  config.INSTDDLT = 'mkdir /usr/local/share/ddlt && cp util/*.lt util/extra.lua /usr/local/share/ddlt && cp ddlt/ddlt /usr/local/bin'

  config.DEPLIBS = '-llua -lastyle'
  config.DIRSEP  = '/'
  config.ALLDEPS = '$+'
elseif platform == 'linux' then
  config.EXEEXT  = ''
  config.OBJEXT  = '.o'
  config.LIBNAME = 'lib%1.a'

  config.CC       = 'g++ -O2 -Iinclude ${INCDIRS} -DDDLT_TEMPLATE_DIR="\\"/usr/local/share/ddlt/\\"" -Wall -Wno-format -o $@ -c %1'
  config.CCLIB    = config.CC
  config.CCLIBD   = 'g++ -O0 -g -Iinclude ${INCDIRS} -DDDLT_TEMPLATE_DIR="\\"/usr/local/share/ddlt/\\"" -Wall -Wno-format -o $@ -c %1'
  config.LIB      = 'ar cru $@ ${ALLDEPS} && ranlib $@'
  config.LINK     = 'g++ ${LIBDIRS} -o $@ ${ALLDEPS}'
  config.GPERF    = 'gperf -c -C -l -L C++ -t -7 -m 100 -I'
  config.RM       = 'rm -f'
  config.LUA      = 'lua'
  config.INSTLIB  = 'cp output/release/libddlparser.a /usr/local/lib && cp include/DDLParser.h /usr/local/include'
  config.INSTDDLT = 'mkdir /usr/local/share/ddlt && cp util/*.lt util/extra.lua /usr/local/share/ddlt && cp ddlt/ddlt /usr/local/bin'

  config.DEPLIBS = '-llua -lastyle'
  config.DIRSEP  = '/'
  config.ALLDEPS = '$+'
elseif platform == 'msvc-x64' then
  config.EXEEXT = '.exe'
  config.OBJEXT = '.obj'
  config.LIBNAME = '%1.lib'

  config.CC     = 'cl /Iinclude /Ideps /nologo /D__WIN__ /Dsnprintf=_snprintf /Dgetcwd=_getcwd /D_USE_MATH_DEFINES /DNDEBUG /Ox /GF /EHsc /MD /GS- /Gy /fp:precise /Zc:forScope /Gd /Fo$@ /c %1'
  config.CCLIB  = 'cl /Iinclude /Ideps /nologo /D__WIN__ /Dsnprintf=_snprintf /Dgetcwd=_getcwd /D_USE_MATH_DEFINES /DNDEBUG /Zi /Ox /GF /EHsc /MD /GS- /Gy /fp:precise /Zc:forScope /Gd /Fdoutput\\release\\ddlparser.pdb /Fo$@ /c %1'
  config.CCLIBD = 'cl /Iinclude /Ideps /nologo /D__WIN__ /Dsnprintf=_snprintf /Dgetcwd=_getcwd /D_USE_MATH_DEFINES /UNDEBUG /Zi /Od /EHsc /RTC1 /MDd /GS /fp:precise /Zc:forScope /Gd /Fdoutput\\debug\\ddlparser.pdb /Fo$@ /c %1'
  config.LIB    = 'lib /nologo /out:$@ ${ALLDEPS}'
  config.LINK   = 'link /nologo /subsystem:console /out:$@ ${ALLDEPS}'
  config.GPERF  = '..\\util\\gperf -c -C -l -L C++ -t -7 -m 100 -I'
  config.RM     = 'util\\rmfiles'
  config.LUA    = 'util\\lua'

  config.DEPLIBS = 'deps\\lua-x64.lib deps\\AStyleLib-x64.lib'
  config.DIRSEP  = '\\'
  config.ALLDEPS = '$**'
elseif platform == 'msvc-x86' then
  config.EXEEXT = '.exe'
  config.OBJEXT = '.obj'
  config.LIBNAME = '%1.lib'

  config.CC     = 'cl /Iinclude /Ideps /nologo /D__WIN__ /Dsnprintf=_snprintf /D_USE_MATH_DEFINES /DNDEBUG /Ox /GF /EHsc /MD /GS- /Gy /fp:precise /Zc:forScope /Gd /Fo$@ /c %1'
  config.CCLIB  = 'cl /Iinclude /Ideps /nologo /D__WIN__ /Dsnprintf=_snprintf /D_USE_MATH_DEFINES /DNDEBUG /Zi /Ox /GF /EHsc /MD /GS- /Gy /fp:precise /Zc:forScope /Gd /Fdoutput\\release\\ddlparser.pdb /Fo$@ /c %1'
  config.CCLIBD = 'cl /Iinclude /Ideps /nologo /D__WIN__ /Dsnprintf=_snprintf /D_USE_MATH_DEFINES /UNDEBUG /Zi /Od /EHsc /RTC1 /MDd /GS /fp:precise /Zc:forScope /Gd /Fdoutput\\debug\\ddlparser.pdb /Fo$@ /c %1'
  config.LIB    = 'lib /nologo /out:$@ ${ALLDEPS}'
  config.LINK   = 'link /nologo /subsystem:console /out:$@ ${ALLDEPS}'
  config.GPERF  = '..\\util\\gperf -c -C -l -L C++ -t -7 -m 100 -I'
  config.RM     = 'util\\rmfiles'
  config.LUA    = 'util\\lua'

  config.DEPLIBS = 'deps\\lua-x86.lib deps\\AStyleLib-x86.lib'
  config.DIRSEP  = '\\'
  config.ALLDEPS = '$**'
else
  error( 'Unknown platform ' .. platform )
end

local search_paths = { 'include', 'src', 'ddlt', 'util' }

local function findfile( file_name )
  for _, search_path in ipairs( search_paths ) do
    local path = search_path .. config.DIRSEP .. file_name
    local file, err = io.open( path )

    if file then
      file:close()
      return path
    end
  end
end

local depcache = {}

local function getdeps( file_name )
  if depcache[ file_name ] then
    return depcache[ file_name ]
  end

  local deps = {}
  local file, err = io.open( file_name )

  if file then
    source = file:read( '*a' )
    file:close()

    for include in source:gmatch( '#include%s+[<"]([^>"]*)[>"]') do
      local dep = findfile( include )

      if dep and not deps[ dep ] then
        deps[ dep ] = true
        local deps2 = getdeps( dep )

        for dep2 in pairs( deps2 ) do
          deps[ dep2 ] = true
        end
      end
    end
  end

  depcache[ file_name ] = deps
  return deps
end

local function sub( key )
  if config[ key ] then
    return config[ key ]
  end

  if key:sub( 1, 5 ) == 'DEPS:' then
    local deps = getdeps( key:sub( 6, -1 ):gsub( '\\', config.DIRSEP ) )

    local str = ''

    for dep in pairs( deps ) do
      str = str .. dep .. ' '
    end

    return str .. key:sub( 6, -1 )
  elseif key:sub( 1, 3 ) == 'CC:' then
    return config.CC:gsub( '%%1', key:sub( 4, -1 ) )
  elseif key:sub( 1, 6 ) == 'CCLIB:' then
    return config.CCLIB:gsub( '%%1', key:sub( 7, -1 ) )
  elseif key:sub( 1, 7 ) == 'CCLIBD:' then
    return config.CCLIBD:gsub( '%%1', key:sub( 8, -1 ) )
  elseif key:sub( 1, 4 ) == 'LIB:' then
    return config.LIBNAME:gsub( '%%1', key:sub( 5, -1 ) )
  elseif key == 'INSTLIB' then
    return config.INSTLIB or 'echo "Add the include and either output/release or output/debug folders to your compiler paths."'
  elseif key == 'INSTDDLT' then
    return config.INSTDDLT or 'echo "Add the ddlt folder to your PATH or copy ddlt/ddlt.exe, ddlt/*.lt and ddlt/extra.lua to a folder in your path."'
  end

  return key
end

local mkfile = [[
all: output~debug~${LIB:ddlparser} output~release~${LIB:ddlparser} ddlt~ddlt${EXEEXT} test~test${EXEEXT} test~test_nacl${EXEEXT} README README.html README.md

##       ##       #### ########  ########     ###    ########  ##    ##
####     ##        ##  ##     ## ##     ##   ## ##   ##     ##  ##  ##
######   ##        ##  ##     ## ##     ##  ##   ##  ##     ##   ####
######## ##        ##  ########  ########  ##     ## ########     ##
######   ##        ##  ##     ## ##   ##   ######### ##   ##      ##
####     ##        ##  ##     ## ##    ##  ##     ## ##    ##     ##
##       ######## #### ########  ##     ## ##     ## ##     ##    ##

LIBOBJSD=output~debug~AreaManager${OBJEXT} output~debug~DDLParser${OBJEXT} output~debug~Error${OBJEXT} output~debug~Hash${OBJEXT} output~debug~JSONChecker${OBJEXT} output~debug~Lexer${OBJEXT} output~debug~Parser${OBJEXT} output~debug~Str${OBJEXT} output~debug~StringArea${OBJEXT} output~debug~Value${OBJEXT}

output~debug~${LIB:ddlparser}: $(LIBOBJSD)
  ${LIB}

output~debug~AreaManager${OBJEXT}: ${DEPS:src~AreaManager.cpp}
  ${CCLIBD:src~AreaManager.cpp}

output~debug~DDLParser${OBJEXT}: ${DEPS:src~DDLParser.cpp}
  ${CCLIBD:src~DDLParser.cpp}

output~debug~Error${OBJEXT}: ${DEPS:src~Error.cpp}
  ${CCLIBD:src~Error.cpp}

output~debug~Hash${OBJEXT}: ${DEPS:src~Hash.cpp}
  ${CCLIBD:src~Hash.cpp}

output~debug~JSONChecker${OBJEXT}: ${DEPS:src~JSONChecker.cpp}
  ${CCLIBD:src~JSONChecker.cpp}

output~debug~Lexer${OBJEXT}: ${DEPS:src~Lexer.cpp} src~Tokens.inc
  ${CCLIBD:src~Lexer.cpp}

output~debug~Parser${OBJEXT}: ${DEPS:src~Parser.cpp}
  ${CCLIBD:src~Parser.cpp}

output~debug~Str${OBJEXT}: ${DEPS:src~Str.cpp}
  ${CCLIBD:src~Str.cpp}

output~debug~StringArea${OBJEXT}: ${DEPS:src~StringArea.cpp}
  ${CCLIBD:src~StringArea.cpp}

output~debug~Value${OBJEXT}: ${DEPS:src~Value.cpp}
  ${CCLIBD:src~Value.cpp}

LIBOBJSR=output~release~AreaManager${OBJEXT} output~release~DDLParser${OBJEXT} output~release~Error${OBJEXT} output~release~Hash${OBJEXT} output~release~JSONChecker${OBJEXT} output~release~Lexer${OBJEXT} output~release~Parser${OBJEXT} output~release~Str${OBJEXT} output~release~StringArea${OBJEXT} output~release~Value${OBJEXT}

output~release~${LIB:ddlparser}: $(LIBOBJSR)
  ${LIB}

output~release~AreaManager${OBJEXT}: ${DEPS:src~AreaManager.cpp}
  ${CCLIB:src~AreaManager.cpp}

output~release~DDLParser${OBJEXT}: ${DEPS:src~DDLParser.cpp}
  ${CCLIB:src~DDLParser.cpp}

output~release~Error${OBJEXT}: ${DEPS:src~Error.cpp}
  ${CCLIB:src~Error.cpp}

output~release~Hash${OBJEXT}: ${DEPS:src~Hash.cpp}
  ${CCLIB:src~Hash.cpp}

output~release~JSONChecker${OBJEXT}: ${DEPS:src~JSONChecker.cpp}
  ${CCLIB:src~JSONChecker.cpp}

output~release~Lexer${OBJEXT}: ${DEPS:src~Lexer.cpp} src~Tokens.inc
  ${CCLIB:src~Lexer.cpp}

output~release~Parser${OBJEXT}: ${DEPS:src~Parser.cpp}
  ${CCLIB:src~Parser.cpp}

output~release~Str${OBJEXT}: ${DEPS:src~Str.cpp}
  ${CCLIB:src~Str.cpp}

output~release~StringArea${OBJEXT}: ${DEPS:src~StringArea.cpp}
  ${CCLIB:src~StringArea.cpp}

output~release~Value${OBJEXT}: ${DEPS:src~Value.cpp}
  ${CCLIB:src~Value.cpp}

src~Tokens.inc: src~Tokens.gperf
  cd src && ${GPERF} --output-file=Tokens.inc Tokens.gperf && cd ..

##       ######## ######## ##     ## ########  #######   ######
####        ##    ##        ##   ##     ##    ##     ## ##    ##
######      ##    ##         ## ##      ##           ## ##
########    ##    ######      ###       ##     #######  ##
######      ##    ##         ## ##      ##    ##        ##
####        ##    ##        ##   ##     ##    ##        ##    ##
##          ##    ######## ##     ##    ##    #########  ######

util~text2c${OBJEXT}: ${DEPS:util~text2c.cpp}
  ${CC:util~text2c.cpp}

util~text2c${EXEEXT}: util~text2c${OBJEXT}
  ${LINK}

##       ########  ########  ##       ########
####     ##     ## ##     ## ##          ##
######   ##     ## ##     ## ##          ##
######## ##     ## ##     ## ##          ##
######   ##     ## ##     ## ##          ##
####     ##     ## ##     ## ##          ##
##       ########  ########  ########    ##

DDLTOBJS=ddlt~Allocator${OBJEXT} ddlt~BitfieldIf${OBJEXT} ddlt~CompilerIf${OBJEXT} ddlt~DefinitionIf${OBJEXT} ddlt~SelectIf${OBJEXT} ddlt~StructIf${OBJEXT} ddlt~TagIf${OBJEXT} ddlt~Util${OBJEXT} ddlt~ddlt${OBJEXT}

ddlt~ddlt${EXEEXT}: $(DDLTOBJS) output~release~${LIB:ddlparser}
  ${LINK} ${DEPLIBS}

ddlt~Allocator${OBJEXT}: ${DEPS:ddlt~Allocator.cpp}
  ${CC:ddlt~Allocator.cpp}

ddlt~BitfieldIf${OBJEXT}: ${DEPS:ddlt~BitfieldIf.cpp}
  ${CC:ddlt~BitfieldIf.cpp}

ddlt~CompilerIf${OBJEXT}: ${DEPS:ddlt~CompilerIf.cpp}
  ${CC:ddlt~CompilerIf.cpp}

ddlt~DefinitionIf${OBJEXT}: ${DEPS:ddlt~DefinitionIf.cpp}
  ${CC:ddlt~DefinitionIf.cpp}

ddlt~SelectIf${OBJEXT}: ${DEPS:ddlt~SelectIf.cpp}
  ${CC:ddlt~SelectIf.cpp}

ddlt~StructIf${OBJEXT}: ${DEPS:ddlt~StructIf.cpp}
  ${CC:ddlt~StructIf.cpp}

ddlt~TagIf${OBJEXT}: ${DEPS:ddlt~TagIf.cpp}
  ${CC:ddlt~TagIf.cpp}

ddlt~Util${OBJEXT}: ${DEPS:ddlt~Util.cpp}
  ${CC:ddlt~Util.cpp}

ddlt~ddlt${OBJEXT}: ${DEPS:ddlt~ddlt.cpp} ddlt~ddlc.h
  ${CC:ddlt~ddlt.cpp}

ddlt~ddlc.h: ddlt~ddlc.lua util~text2c${EXEEXT}
  util~text2c ddlt~ddlc.lua ddlc > ddlt~ddlc.h

##       ######## ########  ######  ########
####        ##    ##       ##    ##    ##
######      ##    ##       ##          ##
########    ##    ######    ######     ##
######      ##    ##             ##    ##
####        ##    ##       ##    ##    ##
##          ##    ########  ######     ##

test~test_ddl.h: ddlt~ddlt${EXEEXT} test~test.ddl
  ddlt~ddlt -i test~test.ddl -t hpp -o test~test_ddl.h

test~test_ddl.cpp: ddlt~ddlt${EXEEXT} test~test.ddl
  ddlt~ddlt -i test~test.ddl -t cpp -I test_ddl.h -o test~test_ddl.cpp

test~test_ddl${OBJEXT}: test~test_ddl.cpp test~test_ddl.h
  ${CC:test~test_ddl.cpp}

test~test${OBJEXT}: ${DEPS:test~test.cpp} test~test_ddl.h
  ${CC:test~test.cpp}

test~test${EXEEXT}: test~test${OBJEXT} test~test_ddl${OBJEXT}
  ${LINK}
  echo "Running test..."
  test~test

test~test_nacl_ddl.h: ddlt~ddlt${EXEEXT} test~test.ddl
  ddlt~ddlt -i test~test.ddl -t nacl_hpp -o test~test_nacl_ddl.h

test~test_nacl_ddl.cpp: ddlt~ddlt${EXEEXT} test~test.ddl
  ddlt~ddlt -i test~test.ddl -t nacl_cpp -I test_nacl_ddl.h -o test~test_nacl_ddl.cpp

test~test_nacl_ddl${OBJEXT}: test~test_nacl_ddl.cpp test/test_nacl_ddl.h
  ${CC:test~test_nacl_ddl.cpp}

test~test_nacl${OBJEXT}: test~test_nacl.cpp test~test_nacl_ddl.h test~test_nacl_ddl.cpp
  ${CC:test~test_nacl.cpp}

test~test_nacl${EXEEXT}: test~test_nacl${OBJEXT} test~test_nacl_ddl${OBJEXT}
  ${LINK}
  echo "Running test_nacl..."
  test~test_nacl

##       ########   #######   ######  ##     ## ##     ## ######## ##    ## ########    ###    ######## ####  #######  ##    ##
####     ##     ## ##     ## ##    ## ##     ## ###   ### ##       ###   ##    ##      ## ##      ##     ##  ##     ## ###   ##
######   ##     ## ##     ## ##       ##     ## #### #### ##       ####  ##    ##     ##   ##     ##     ##  ##     ## ####  ##
######## ##     ## ##     ## ##       ##     ## ## ### ## ######   ## ## ##    ##    ##     ##    ##     ##  ##     ## ## ## ##
######   ##     ## ##     ## ##       ##     ## ##     ## ##       ##  ####    ##    #########    ##     ##  ##     ## ##  ####
####     ##     ## ##     ## ##    ## ##     ## ##     ## ##       ##   ###    ##    ##     ##    ##     ##  ##     ## ##   ###
##       ########   #######   ######   #######  ##     ## ######## ##    ##    ##    ##     ##    ##    ####  #######  ##    ##

README: util~createdoc.lua
  ${LUA} util~createdoc.lua txt > $@

README.html: util~createdoc.lua
  ${LUA} util~createdoc.lua html > $@

README.md: util~createdoc.lua
  ${LUA} util~createdoc.lua md > $@

##       #### ##    ##  ######  ########    ###    ##       ##
####      ##  ###   ## ##    ##    ##      ## ##   ##       ##
######    ##  ####  ## ##          ##     ##   ##  ##       ##
########  ##  ## ## ##  ######     ##    ##     ## ##       ##
######    ##  ##  ####       ##    ##    ######### ##       ##
####      ##  ##   ### ##    ##    ##    ##     ## ##       ##
##       #### ##    ##  ######     ##    ##     ## ######## ########

install:
  ${INSTLIB}
  ${INSTDDLT}

##        ######  ##       ########    ###    ##    ##
####     ##    ## ##       ##         ## ##   ###   ##
######   ##       ##       ##        ##   ##  ####  ##
######## ##       ##       ######   ##     ## ## ## ##
######   ##       ##       ##       ######### ##  ####
####     ##    ## ##       ##       ##     ## ##   ###
##        ######  ######## ######## ##     ## ##    ##

clean:
  ${RM} src~Tokens.inc
  ${RM} output~debug~${LIB:ddlparser} $(LIBOBJSD) output~debug~ddlparser.pdb
  ${RM} output~release~${LIB:ddlparser} $(LIBOBJSR) output~release~ddlparser.pdb
  ${RM} ddlt~ddlt${EXEEXT} $(DDLTOBJS) ddlt~ddlc.h
  ${RM} etc~text2c${EXEEXT} etc~text2c${OBJEXT}
  ${RM} test~test_ddl.h test~test_ddl.cpp test~test_ddl${OBJEXT} test~test${OBJEXT} test~test${EXEEXT}
  ${RM} test~test_nacl_ddl.h test~test_nacl_ddl.cpp test~test_nacl_ddl${OBJEXT} test~test_nacl${OBJEXT} test~test_nacl${EXEEXT}
  ${RM} util~text2c${EXEEXT} util~text2c${OBJEXT}
]]

while true do
  local mkfile2 = mkfile:gsub( '%${(%w+:?[^}]*)}', sub )

  if mkfile2 == mkfile then
    break
  end

  mkfile = mkfile2
end

mkfile = mkfile:gsub( '\n  ', '\n\t' ):gsub( '~', config.DIRSEP )

io.write( mkfile )
