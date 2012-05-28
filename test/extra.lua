function capitalize( str )
  return str:sub( 1, 1 ):upper() .. str:sub( 2, -1 )
end

function getCppType( field )
  local ftype
  
  if type( field ) ~= 'string' then
    local aggregate = field:getDeclaredType()

    if aggregate then
      return aggregate:getName()
    end

    ftype = field:getType()
  else
    ftype = field
  end

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
    string = 'std::string',
    boolean = 'bool',
    file = 'std::string',
    tuid = 'uint64_t',
    json = 'std::string'
  })[ ftype ]
end

function getCppTypeSize( field )
  local ftype 
  
  if type( field ) ~= 'string' then
    ftype = field:getType()
  else
    ftype = field
  end

  return
  ({
    uint8 = true,
    uint16 = true,
    uint32 = true,
    uint64 = true,
    int8 = true,
    int16 = true,
    int32 = true,
    int64 = true,
    float32 = true,
    float64 = true,
    boolean = true,
    tuid = true,
  })[ ftype ] and ( 'sizeof( ' .. getCppType( field ) .. ' )' )
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
    ret = ret .. 'k' .. capitalize( flag:getName() )
  end

  return ret
end

function getFieldValue( field, index )
  local value = field:getValue()
  local ftype = field:getType()
  
  if field:getArrayType() ~= 'scalar' then
    value = value[ index ]
  end

  if value then
    if ftype == 'string' or ftype == 'file' or ftype == 'json' then
      return '"' .. value:escapeCPP() .. '"'
    elseif ftype == 'int32' then
      return value .. 'L'
    elseif ftype == 'int64' then
      return value .. 'LL'
    elseif ftype == 'uint32' then
      return value .. 'UL'
    elseif ftype == 'uint64' or ftype == 'tuid' then
      return value .. 'ULL'
    elseif ftype == 'float32' then
      return value .. 'f'
    elseif ftype == 'uint8' or ftype == 'uint16' then
      return value .. 'U'
    elseif ftype == 'select' then
      return field:getDeclaredType():getName() .. '::k' .. capitalize( field:getValue():getName() )
    elseif ftype == 'bitfield' then
      if type( field:getValue() ) ~= 'table' then
        return field:getDeclaredType():getName() .. '::k' .. capitalize( field:getValue():getName() )
      else
        local ret = ''
        for index, flag in ipairs( field:getValue() ) do
          if index ~=1 then ret = ret .. '|' end
          ret = ret .. field:getDeclaredType():getName() .. '::k' .. capitalize( flag:getName() )
        end
        return ret
      end
    end

    return value
  end

  if ftype == 'string' or ftype == 'file' or ftype == 'json' then
    return '""'
  elseif ftype == 'select' then
    return field:getDeclaredType():getName() .. '::k' .. capitalize( field:getDeclaredType():getDefaultItem():getName() )
  elseif ftype == 'bitfield' then
    return field:getDeclaredType():getName() .. '::k' .. capitalize( field:getDeclaredType():getDefaultflag():getName() )
  else
    return '0'
  end
end

local function dumpName( node )
  if node.previous then
    dumpName( node.previous )
    emit( '.' )
  end
  
  emit( 'm_', capitalize( node.valueinfo:getName() ) )
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
      if node.index then emit( '[ ', node.index, ' ]' ) end
      emit( '.Set', capitalize( node.valueinfo:getName() ), '( ', getFieldValue( node.valueinfo ), ' );\n' )
    end
  elseif node.valueinfo:getArrayType() == 'fixed' then
    if node.valueinfo:getType() == 'struct' then
      for index, v in ipairs( value ) do
        for _, vi in ipairs( v ) do
          dumpStructValue( { valueinfo = vi, previous = node, index = index - 1 } )
        end
      end
    else
      for index, value in ipairs( value ) do
        dumpName( node.previous )
        if node.index then emit( '[ ', node.index, ' ]' ) end
        emit( '.Set', capitalize( node.valueinfo:getName() ), '( ', getFieldValue( node.valueinfo ), ' );\n' )
      end
    end
  end
end
