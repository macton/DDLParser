local function parseArguments( args )
  if #args > 1 and args[ 1 ]:sub( 1, 1 ) ~= '-' then
    error( 'Invalid option ' .. args[ 1 ] .. ', options must start with a dash' )
  end
  
  local settings = {}
  local i = 1
  
  while i <= #args do
    local option = args[ i ]
    
    --[[if option == '-t' then
      option = '--template'
    end]]
    
    local opt_args = settings[ option ] or {}
    
    i = i + 1
    while i <= #args and args[ i ]:sub( 1, 1 ) ~= '-' do
      opt_args[ #opt_args + 1 ] = args[ i ]
      i = i + 1
    end
    
    settings[ option ] = opt_args
  end
  
  return settings
end

local function validateDDL( ddl )
  for _, struct in ddl:structs() do
    if struct:getNumFields() > 64 then
      error( input_file .. ': Exceeded maximum number of fields in individual structure (64)' )
    end
  end
end

local function runTemplate( template_name, template_path, ddlc, first_aggregate, template_settings )
  local file, err = io.open( template_path )

  if not file then
    error( 'Could not open ' .. template_path .. ': ' .. err )
  end

  local source = file:read( '*a' )
  file:close()

  local code = compileTemplate( source )
  
  --[[file = io.open( 'template.lua', 'w' )
  file:write( code )
  file:close()]]
  
  local chunk, err = loadstring( code, template_name )
  if not chunk then
    error( 'Error in template ' .. template_name .. ': ' .. err )
  end
  
  chunk()
  -- Templates must define a global function instead of returning one because
  -- when the template is compiled it adds extra emit statements after the end
  -- of the file, invalidating the return
  main( ddlc, first_aggregate, template_settings )
end

local output = nil

function beginOutput()
  output = {}
end

function emit( ... )
  if output then
    for _, arg in ipairs( { ... } ) do
      output[ #output + 1 ] = tostring( arg )
    end
  end
end

function endOutput()
  output = nil
end

function getOutput()
  if output then
    return table.concat( output ):gsub( '\n%s+\n', '\n' )
  end
end

function checkSettings( settings, spec )
  for _, opt in ipairs( spec ) do
    if opt.short_name then
      settings[ opt.long_name ]  = settings[ opt.short_name ] or settings[ opt.long_name ]
      settings[ opt.short_name ] = nil
    end
    
    local setting = settings[ opt.long_name ]

    --[[if opt.mandatory and not setting then
      error( 'Missing mandatory option ' .. opt.long_name )
    end]]
    
    if setting then
      if #setting < opt.min or ( opt.max and #setting > opt.max ) then
        error( 'Wrong number of arguments to option ' .. opt.long_name )
      end
    end
  end
end

local function findFile( settings, name )
  local search_paths = settings[ '--search-path' ]
  local path, file

  if search_paths then
    for _, search_path in ipairs( search_paths ) do
      path = search_path .. '/' .. name

      file = io.open( path )

      if file then
        file:close()
        return path
      end
    end
  end

  path = getWorkingDir() .. name

  file = io.open( path )

  if file then
    file:close()
    return path
  end

  path = getShareDir()

  if path then
    path = path .. name

    file = io.open( path )

    if file then
      file:close()
      return path
    end
  end

  path = getExecutableDir() .. name

  file = io.open( path )

  if file then
    file:close()
    return path
  end
end

local function dumpargs( args, ident )
  ident = ident or 0
  
  for k, v in pairs( args ) do
    if type( v ) == 'table' then
      io.write( ( ' ' ):rep( ident ), k, ':\n' )
      dumpargs( v, ident + 2 )
    else
      io.write( ( ' ' ):rep( ident ), k, ': ', v )
    end
    io.write( '\n' )
  end
end

local function trace( event, line )
  local info = debug.getinfo( 2 )

  io.write( info.source, ':', line, '\n' )
end

return function( args, ddlc )
  --debug.sethook( trace, 'l' )

  settings = parseArguments( args )
  
  checkSettings( settings, {
    { short_name = '-h', long_name = '--help', min = 0 },
    { short_name = '-i', long_name = '--input-file', min = 1, max = 1 }, -- mandatory
    { short_name = '-d', long_name = '--dependent-file', min = 1 },
    { short_name = '-t', long_name = '--template', min = 1 }, -- mandatory
    { short_name = '-s', long_name = '--search-path', min = 1 }
  } )
  
  if settings[ '--help' ] then
    io.write(
      'DDLT version 1.0\n\n',
      'Usage: DDLT options..\n',
      '\n',
      '-h --help                      Shows this help page and exit\n',
      '-i --input-file <input file>   Defines the input file\n',
      '-d --dependent-file <file>...  Includes file with necessary additional\n',
      '                               definitions\n',
      '-t --template <template name>  Run the input file through this template\n',
      '-s --search-path <path>...     Additional search paths to look for templates\n',
      '\n'
    )
    
    for _, template_name in ipairs( settings[ '--template' ] ) do
      local template_path = findFile( settings, template_name .. '.lt' )
      
      if template_path then
        runTemplate( template_name, template_path, nil, nil, settings )
      else
        error( 'Unknown template ' .. template_name )
      end
    end
    
    os.exit( 0 )
  end

  if not settings[ '--input-file' ] then
    error( 'Missing mandatory option --input-file\n' )
  end

  if not settings[ '--template' ] then
    error( 'Missing mandatory option --template' )
  end

  local extra = findFile( settings, 'extra.lua' )

  if extra then
    dofile( extra )
  end
  
  local dependent_files = settings[ '--dependent-file' ]
  local first_aggregate = 1
  
  if dependent_files then
    for _, file in ipairs( dependent_files ) do
      local include, err = io.open( file, 'r' )
      if not include then
        error( err )
      end
      ddlc:compile( file, include:read( '*a' ) )
      include:close()
    end
    
    first_aggregate = #ddlc:getDefinition() + 1
  end
  
  local input, err = io.open( settings[ '--input-file' ][ 1 ], 'r' )
  if not input then
    error( err )
  end
  ddlc:compile( settings[ '--input-file' ][ 1 ], input:read( '*a' ) )
  input:close()
  
  validateDDL( ddlc:getDefinition() )
  
  for _, template_name in ipairs( settings[ '--template' ] ) do
    local template_path = findFile( settings, template_name .. '.lt' )
    
    if not template_path then
      error( 'Unknown template ' .. template_name )
    end
    
    io.write( 'Running template ', template_name, '\n' )
    io.flush()
    
    runTemplate( template_name, template_path, ddlc, first_aggregate, settings )
    output_file = false
  end
end
