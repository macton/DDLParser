local function parseArguments( args )
  if #args > 1 and args[ 1 ]:sub( 1, 1 ) ~= '-' then
    error( 'Invalid option ' .. args[ 1 ] .. ', options must start with a dash' )
  end
  
  local settings = {}
  local i = 1
  
  while i <= #args do
    local option = args[ i ]
    
    if option == '-t' then
      option = '--template'
    end
    
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

local function runTemplate( template, template_name, ddlc, first_aggregate, template_settings )
  local code = compileTemplate( template )
  
  --[[file = io.open( 'template.lua', 'w' )
  file:write( code )
  file:close()]]
  
  local chunk, err = loadstring( code, template_name )
  if not chunk then
    error( 'Error in template ' .. template_name .. ': ' .. err )
  end
  
  chunk()
  -- Templates must define a global function instead of returning one because
  -- when the template is compiles it adds extra emit statements after the end
  -- of the file, invalidating the return
  main( ddlc, first_aggregate, template_settings )
end

local output_file = false

function setOutputFile( file_name )
  output_file = io.open( file_name, 'w' )
end

function emit( ... )
  if output_file then
    for _, arg in ipairs( { ... } ) do
      output_file:write( tostring( arg ) )
    end
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

function checkSettings( settings, spec )
  for _, opt in ipairs( spec ) do
    if opt.short_name then
      settings[ opt.long_name ]  = settings[ opt.short_name ] or settings[ opt.long_name ]
      settings[ opt.short_name ] = nil
    end
    
    local setting = settings[ opt.long_name ]

    if opt.mandatory and not setting then
      error( 'Missing mandatory option ' .. opt.long_name )
    end
    
    if setting then
      if #setting < opt.min or ( opt.max and #setting > opt.max ) then
        error( 'Wrong number of arguments to option ' .. opt.long_name )
      end
    end
  end
end

local function trace( event, line )
  local info = debug.getinfo( 2 )

  io.stdout:write( info.source, ':', line, '\n' )
end

return function( args, ddlc, templates )
  --debug.sethook( trace, 'l' )

  settings = parseArguments( args )
  
  if settings[ '-h' ] or settings[ '--help' ] then
    io.write(
      'DDLT version 1.0\n\n',
      'Usage: DDLT options..\n',
      '\n',
      '-h --help                      Shows this help page and exit\n',
      '-i --input-file <input file>   Defines the input file\n',
      '-d --dependent-file <file>...  Includes file with necessary additional\n',
      '                               definitions\n',
      '-t --template <template name > Run the input file through this template\n',
      '\n'
    )
    
    for _, template in ipairs( settings[ '--template' ] or {} ) do
      local template_name = template --:GetFilePathLeaf():StripFilePathSuffix()
      local template = templates[ template_name ]
      
      if template then
        runTemplate( template, template_name, nil, nil, { [ '--help' ] = {} } )
        output_file = false
      end
    end
    
    os.exit( 0 )
  end
  
  checkSettings( settings, {
    { short_name = '-i', long_name = '--input-file', mandatory = true, min = 1, max = 1 },
    { short_name = '-d', long_name = '--dependent-file', mandatory = false, min = 1 },
    { short_name = '-t', long_name = '--template', mandatory = true, min = 1 }
  } )
  
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
  
  for _, template in pairs( settings[ '--template' ] ) do
    local template_name = template --:GetFilePathLeaf():StripFilePathSuffix()
    local template = templates[ template_name ]
    
    if not template then
      error( 'Unknown template ' .. template_name )
    end
    
    io.write( 'Running template ', template_name, '\n' )
    io.flush()
    
    runTemplate( template, template_name, ddlc, first_aggregate, settings )
    output_file = false
  end
end
