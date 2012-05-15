function getCppType( field )
  local aggregate = field:getDeclaredType()

  if aggregate then
    return aggregate:getName()
  end

  local type = field:getType()

  return
  ({
    uint8 = 'uint8_t',
    uint16 = 'uint16_t',
    uint32 = 'uint32_t',
    uint64 = 'uint64_t',
    int8 = 'int8_t',
    int16 = 'int16_t',
    int32 = 'int32_t',
    int64 = 'int64_t',
    float32 = 'float',
    float64 = 'double',
    string = 'char*',
    boolean = 'bool',
    file = 'file',
    tuid = 'uint64_t',
    json = 'char*'
  })[ type ]
end

function getCppTypeSize( field )
  local type = field:getType()

  return
  ({
    uint8 = 1,
    uint16 = 2,
    uint32 = 4,
    uint64 = 8,
    int8 = 1,
    int16 = 2,
    int32 = 4,
    int64 = 8,
    float32 = 4,
    float64 = 8,
    boolean = 1,
    tuid = 8,
  })[ type ] or 0
end

function getFlagValue( flag )
  local value = flag:getValue()

  if type( value ) ~= 'table' then
    return ( '0x%08xUL' ):format( value )
  end

  local ret = ''

  for index, flag in ipairs( value ) do
    if index ~= 1 then
      ret = ret .. ' | '
    end
    ret = ret .. flag:getName()
  end

  return ret
end

function getFieldValue( field )
  local value = field:getValue()
  local type = field:getType()

  if value then
    if type == 'string' or type == 'file' or type == 'json' then
      return '"' .. value:escapeCPP() .. '"'
    elseif type == 'int32' then
      return value .. 'L'
    elseif type == 'int64' then
      return value .. 'LL'
    elseif type == 'uint32' then
      return value .. 'UL'
    elseif type == 'uint64' or type == 'tuid' then
      return value .. 'ULL'
    elseif type == 'float32' then
      return value .. 'f'
    elseif type == 'uint8' or type == 'uint16' then
      return value .. 'U'
    end

    return value
  end

  if type == 'string' or type == 'file' or type == 'json' then
    return '""'
  end

  return 0
end

local function dumpName( node )
  if node.previous then
    dumpName( node.previous )
    emit( '.' )
  end
  
  emit( 'm_', node.valueinfo:getName() )
end

function dumpStructValue( node )
  local value = node.valueinfo:getValue()
  
  if not value then
    return
  end
  
  if node.valueinfo:getArrayType() == 'scalar' then
    if node.valueinfo:getType() == 'struct' then
      for i, vi in ipairs( value ) do
        dumpStructValue( { valueinfo = vi, previous = node } )
      end
    else
      dumpName( node.previous )
      emit( '.Set', node.valueinfo:getName(), '( ', getFieldValue( node.valueinfo ), ' );\n' )
    end
  end
end
