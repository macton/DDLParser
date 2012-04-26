if #arg ~= 1 then
  io.write[[
Creates the DDLParser documentation.

Usage: lua createdoc.lua <format>

Where <format> can be txt, html and md.
]]
  os.exit()
end

local format_ = arg[ 1 ]

-- ########   #######   ######  ##     ## ##     ## ######## ##    ## ########
-- ##     ## ##     ## ##    ## ##     ## ###   ### ##       ###   ##    ##
-- ##     ## ##     ## ##       ##     ## #### #### ##       ####  ##    ##
-- ##     ## ##     ## ##       ##     ## ## ### ## ######   ## ## ##    ##
-- ##     ## ##     ## ##       ##     ## ##     ## ##       ##  ####    ##
-- ##     ## ##     ## ##    ## ##     ## ##     ## ##       ##   ###    ##
-- ########   #######   ######   #######  ##     ## ######## ##    ##    ##

local doc = [===[
\header(1, 'DDL Specification')

\p[[DDL files can contain three constructs: selects, bitfields and structures. All constructs must have a name, and they can have an author, a description and a label assigned to them. These fields can be used by tools when rendering the constructs as UI elements.]]

\p[[There is a annotated DDL grammar at the end of this document for further reference.]]

\header(2, 'Selects')

\p[[A select is just a list of options much like an C enum. They represent exclusive values, like a drop-down where just one value can be selected. But unlike C enums, numeric values cannot be assigned to select fields; their values are just the hash of their names. This way new fields can be added without constraints while maintaining compatibility with cold that uses the select.]]

\p[[Each select item can have an author, a description and a label.]]

\header(3, 'Example')

\source('DDL', [[
select Weapon, author( "Andre de Leiradella" ), description( "Weapons available to the player" ), label( "Weapon" )
{
  kFist,           description( "Bare hands" ),               label( "Fist" );
  kChainsaw,       description( "A la Chainsaw Massacre" ),   label( "Chainsaw" );
  kPistol,         description( "Simple pistol" ),            label( "Pistol" ), default;
  kShotgun,        description( "A single-barrel shotgun" ),  label( "Shotgun" );
  kChaingun,       description( "A machine gun" ),            label( "Chaingun" );
  kRocketLauncher, description( "Portable rocket launcher" ), label( "Rocket launcher" );
  kPlasmaGun,      description( "Plasma gun" ),               label( "Plasma gun" );
  kBFG9000,        description( "*The* BFG 9000" ),           label( "BFG 9000" );
}]])

\p[[An item can be marked as default (see item \code'kPistol') and it will be the item used in fields declared with the select type that don't have an explicit value to override the default.]]

\header(2, 'Bitfields')

\p[[Bitfields represent a set of related flags. Their flags can be or'ed to form the value of another flag. Bitfields should be used where more than one value can be selected, like a checkbox group.]]

\p[[As with select items, each bitfield flag can have an author, a description and a label.]]

\header(3, 'Example')

\source('DDL', [[
bitfield Powerup, author( "Andre de Leiradella" ), description( "Powerup pickups" ), label( "Powerup" )
{
  kNone,                description( "Help me!" ), empty;
  kRadiationSuit,       description( "Makes the player immune to radiation for a limited time" ),             label( "Radiation suit" );
  kPartialInvisibility, description( "Makes the player almost invisible to enemies for a limited time" ),     label( "Partial invisibility" );
  kInvulnerability,     description( "Makes the player invulnerable for a limited time" ),                    label( "Invulnerability" );
  kComputerMap,         description( "Gives the complete map of the level to the player" ),                   label( "Computer map" );
  kLightVisor,          description( "Allows the player to see in the dark for a limited time" ),             label( "Light visor" );
  kBerserk,             description( "Gives the player the ability to quickly kill enemies with his fists" ), label( "Berserk" );
  kAll,                 value( kRadiationSuit | kPartialInvisibility | kInvulnerability | kComputerMap | kLightVisor | kBerserk );
}]])

\p[[Flags without a value will get an unique bit index that can be queried at runtime. Flags can be marked as default. If there isn't a default flag in the bitfield the empty flag will be used, or the first declared flag will be used if there isn't an empty flag. To get a zeroed flag, tag it with \code'empty' like the \code'kNone' flag above.]]

\header(2, 'Structures')

\p[[While selects and bitfields only define custom types and take no memory, structures are aggregates of fields which actually take up memory to hold their fields' values.]]

\p[[Structure fields can be of the following types:]]

\ul{
[[\code'u8' (or \code'uint8_t')]],
[[\code'u16' (or \code'uint16_t')]],
[[\code'u32' (or \code'uint32_t')]],
[[\code'u64' (or \code'uint64_t')]],
[[\code'i8' (or \code'int8_t')]],
[[\code'i16' (or \code'int16_t')]],
[[\code'i32' (or \code'int32_t')]],
[[\code'i64' (or \code'int64_t')]],
[[\code'f32' (or \code'float')]],
[[\code'f64' (or \code'double')]],
[[\code'boolean']],
[[\code'string']],
[[\code'file']],
[[\code'tuid']],
[[\code'json']],
[[Any previously declared select, bitfield or structure]],
}

\p[[Fields can also be arrays of any of these types by just adding \code'[]' right after the type name. These arrays cannot take default values, and can grow as needed at runtime. Adding a \code'[ x ]', where \code'x' is an integer expression, makes the array fixed and up to \code'x' default values can be specified for the field. Just like select items and bitfield flags, structure fields can have an author, a description and a label.]]

\p[[To inform the default value of a field, use the \code'value' keyword and write any expression inside the parenthesis. The expression must evaluate to a value which fits in the data type of the field.]]

\header(3, 'Examples')

\source('DDL', [[
struct Position
{
  f32 m_X,     value( 0 );
  f32 m_Y,     value( 0 );
  f32 m_Angle, value( 0 ), description("The direction the player is looking at (degrees)");
}

struct Mariner, author( "Andre de Leiradella" ), description( "The player character" ), label( "Player" )
{
  u32        m_Health,     value( 100 ),                              description( "The player's health" );
  Weapon     m_Weapon,     value( kPistol ),                          description( "The player's current weapon" );
  Powerup    m_Powerup,    value( kBerserk ),                         description( "The player's powerups" );
  i32[ 8 ]   m_Ammunition, value( { 0, 0, 20, -1, -1, -1, -1, -1 } ), description( "The ammunition of each weapon, -1 means the player doesn't have it" );
  string     m_Name,       value( "Mariner" ),                        description( "The player's name for multiplayer sessions" );
  Position   m_Position,   value( { m_X = 100, m_Y = 120 } ),         description( "The player's position" );
  Position[] m_Deaths,                                                description( "Places the player has died in" );
}]])

\p[[Array values must be written inside braces with commas separating each expression. Values of structured fields, like \code'm_Position' above, must also appear inside braces. It's not mandatory that all fields are present in the value.]]

\header(1, 'Expressions')

\p[[DDL expressions are very similar to C expressions, and care has been taken to implement the same operators with the same precedence order. The operators, in decreasing order of precedence, are:]]

\ul{
[[\code'?' (ternary operator)]],
[[\code'||' (logic or)]],
[[\code'&&' (logic and)]],
[[\code'|' (arithmetic or)]],
[[\code'^' (arithmetic xor)]],
[[\code'&' (arithmetic and)]],
[[\code'==' and \code'!=' (equality)]],
[[\code'<', \code'<=', \code'>' and \code'>=' (comparison)]],
[[\code'<<' and \code'>>' (shift)]],
[[\code'+' and \code'-' (add and subtract)]],
[[\code'*', \code'/' and \code'%' (multiply, divide and modulus)]],
[[\code'+', \code'-', \code'~' and \code'!' (unary positive, negative, complement and logic not)]],
}

\p[[Terminals can be binary, octal, decimal and hexadecimal integer constants, floating point constants, string literals, the \code'true' and \code'false' constants, \code'pi', the natural number \code'e', and expressions between parenthesis.]]

\p[[Binary constants are written similarly to hexadecimal constants but using the \code'0b' prefix instead of \code'0x'. String literals are specified inside single or double quotes, and any occurrence of \code'%xx' in the string is substituted by the character with hexadecimal value \code'xx'.]]

\p[[The constants \code'true' and \code'false' evaluates to the integers 1 and 0 respectively.]]

\header(1, 'Comments')

\p[[DDL files can have C and C++ style comments:]]

\ul{
[[\code'/* this is a comment */']],
[[\code'// this is a comment that ends at the end of the line']],
}

\header(1, 'DDL API')

\p[[The DDL API are a collection of functions and structures that allows for DDL source code compilation and subsequent querying of the components found in the source file for i.e. code generation.]]

\header(2, 'Common methods')

\p[[Many structures present a set of common methods that are explained here.]]

\header(3, [[\code'const char* GetName() const']], 'GetName')

\p[[Returns the name of the component, i.e. a select's name or a structure's field's name.]]

\header(3, [[\code'const char* GetAuthor() const']], 'GetAuthor')

\p[[If the component has been tagged with the \code'author' tag, \code'GetAuthor' returns the contents of the tag, otherwise it returns NULL. This tag can be used to document the person who authored the component.]]

\header(3, [[\code'const char* GetDescription() const']], 'GetDescription')

\p[[If the component has been tagged with the \code'description' tag, \code'GetDescription' returns the contents of the tag, otherwise it returns NULL. This tag can be used to give the component a verbose description.]]

\header(3, [[\code'const char* GetLabel() const']], 'GetLabel')

\p[[If the component has been tagged with the \code'label' tag, \code'GetLabel' returns the contents of the tag, otherwise it returns NULL. This tag can be used to give the component a label to be used in the user interface, for example.]]

\header(3, [[\code'const char* GetDisplayLabel() const']], 'GetDisplayLabel')

\p[[Returns the contents of the \code'label' tag if it has been defined and is not the empty string. Otherwise it returns the name of the component.]]

\header(3, [[\code'uint32_t GetNameHash() const']], 'GetNameHash')

\p[[Returns the hash of the components's name. The hash is created with DDLParser::StringCrc32.]]

\header(3, [[\code'DDLParser::Tag* GetTags() const']], 'GetTags')

\p[[Returns the first tag defined for the component, or NULL if the component has no tags defined.]]

\header(3, [[\code'DDLParser::Tag* GetTag( uint32_t type ) const']], 'GetTag')

\p[[Finds and returns a tag by its type. Alternatively, type can be the hash of the generic tag to be searched for. Either way, if the tag isn't found NULL is returned.]]

\header(1, 'DDLParser::Compile')

\code[[DDLParser::Definition* DDLParser::Compile( DDLParser::LinearAllocator* definition, DDLParser::LinearAllocator* scratch, const void* source, size_t source_size, char* error, size_t error_size, bool two_us_reserved, int bitfield_limit )]]

\p[[The DDLParser::Compile function compiles a DDL source file into a DDLParser::Definition ready to be used to generate code.]]

\header(2, 'Parameters')

\ul{
[[\code'definition': A DDLParser::LinearAllocator where the definition is assembled. When DDLParser::Compile returns, this allocator will have the complete definition.]],
[[\code'scratch': A DDLParser::LinearAllocator used for temporary work. Can be discarded after DDLParser::Compile returns.]],
[[\code'source': A pointer to the DDL source code. This buffer doesn't have to be null-terminated and cannot be NULL.]],
[[\code'source_size': The size of the \code'source' buffer.]],
[[\code'error': A pointer to a buffer where the error message, if there's one, will be written to. Cannot be NULL.]],
[[\code'error_size': The size of the \code'error' buffer.]],
[[\code'two_us_reserved': If true, identifiers starting with two underlines are reserved and disallowed in the source code.]],
[[\code'bitfield_limit': The maximum number of flags allowed in a bitfield. If this value is zero, there is no limit on the number of flags per bitfield.]],
}

\header(2, 'Return value')

\p[[If the parse succeeds it returns the definition assembled in \code'definition' casted to a DDLParser::Definition pointer. If there was an error, DDLParser::Compile returns NULL and \code'error' is filled with the error message.]]

\header(1, 'DDLParser::StringCrc32')

\p[[\code'uint32_t DDLParser::StringCrc32( const char* data )']]

\p[[Returns the CRC32 of the string.]]

\header(1, 'DDLParser::Definition')

\p[[After a successful parsing, DDLParser::Compile returns a DDLParser::Definition, which is used as the starting point to query all selects, bitfields, and structures, defined in the source code.]]

\p[[DDLParser::Definition has the following methods:]]

\header(2, [[\code'uint32_t DDLParser::Definition::GetNumAggregates() const']], 'DDLParser::Definition::GetNumAggregates')

\p[[Returns the number of aggregates (selects, bitfields and structures) in the definition.]]

\header(2, [[\code'uint32_t DDLParser::Definition::GetTotalSize() const']], 'DDLParser::Definition::GetTotalSize')

\p[[Returns the total size in bytes of the definition. This can be used if you want to write it into a file for example.]]

\header(2, [[\code'static DDLParser::Definition* DDLParser::Definition::FromBinRep( void* bin_rep )']], 'DDLParser::Definition::FromBinRep')

\p[[Just returns the \code'bin_rep' pointer casted to a DDLParser::Definition.]]

\header(2, [[\code'DDLParser::Aggregate* DDLParser::Definition::GetAggregate( unsigned int index )']], 'DDLParser::Definition::GetAggregate')

\p[[Returns an aggregate given its index. Aggregates are saved into the definition in the order they appear in the source code. No bounds check is done with \code'index'.]]

\header(2, [[\code'DDLParser::Aggregate* DDLParser::Definition::FindAggregate( const char* name )']], 'DDLParser::Definition::FindAggregate')

\p[[Finds and returns an aggregate by name. If the aggregate does not exist, NULL is returned.]]

\header(2, [[\code'DDLParser::Aggregate* DDLParser::Definition::FindAggregate( uint32_t hash )']], 'DDLParser::Definition::FindAggregate/hash')

\p[[Finds and returns an aggregate by the hash of its name. If the aggregate does not exist, NULL is returned. The hash must be created with DDLParser::StringCrc32.]]

\header(1, 'DDLParser::Aggregate')

\p[[An aggregate holds information common to selects, bitfields, and structures. It has the following common methods: \code'GetName', \code'GetAuthor', \code'GetDescription', \code'GetLabel', \code'GetDisplayLabel', and \code'GetNameHash'.]]

\header(2, [[\code'uint32_t DDLParser::Aggregate::GetType() const']], 'DDLParser::Aggregate::GetType')

\p[[Returns the type of the aggregate, DDLParser::kSelect, DDLParser::kBitfield, or DDLParser::kStruct.]]

\header(2, [[\code'DDLParser::Select* DDLParser::Aggregate::ToSelect() const']], 'DDLParser::Aggregate::ToSelect')

\p[[Returns the aggregate casted to DDLParser::Select without checking for the actual aggregate type.]]

\header(2, [[\code'DDLParser::Bitfield* DDLParser::Aggregate::ToBitfield() const']], 'DDLParser::Aggregate::ToBitfield')

\p[[Returns the aggregate casted to DDLParser::Bitfield without checking for the actual aggregate type.]]

\header(2, [[\code'DDLParser::Struct* DDLParser::Aggregate::ToStruct() const']], 'DDLParser::Aggregate::ToStruct')

\p[[Returns the aggregate casted to DDLParser::Struct without checking for the actual aggregate type.]]

\header(1, 'DDLParser::Select')

\p[[A select is like a C enum, but you can't define the value of the items. Their value is always the hash code of their identifiers. A select holds information about its items.]]

\p[[In addition to the methods listed here, selects have the following common methods: \code'GetName', \code'GetAuthor', \code'GetDescription', \code'GetLabel', \code'GetDisplayLabel', \code'GetNameHash', \code'GetTags', and \code'GetTag'.]]

\header(2, [[\code'uint32_t DDLParser::Select::GetNumItems() const']], 'DDLParser::Select::GetNumItems')

\p[[Returns the number of items contained in the select.]]

\header(2, [[\code'int32_t DDLParser::Select::GetDefaultItem() const']], 'DDLParser::Select::GetDefaultItem')

\p[[Returns the index of the item defined as the default item for the select. If no item was defined as the default, the default item is the first defined. This can be used to initialize variables to a default value if none is given for example.]]

\header(2, [[\code'DDLParser::SelectItem* DDLParser::Select::FindItem( const char* name )']], 'DDLParser::Select::FindItem')

\p[[Finds and returns an item by name. If the item does not exist, NULL is returned.]]

\header(2, [[\code'DDLParser::SelectItem* DDLParser::Select::FindItem( uint32_t hash )']], 'DDLParser::Select::FindItem/hash')

\p[[Finds and returns an item by the hash of its name. If the item does not exist, NULL is returned. The hash must be created with DDLParser::StringCrc32.]]

\header(1, 'DDLParser::SelectItem')

\p[[A select item represents an item declared in a select and has the following common methods: \code'GetName', \code'GetAuthor', \code'GetDescription', \code'GetLabel', \code'GetDisplayLabel', \code'GetNameHash', \code'GetTags', and \code'GetTag'.]]

\p[[Select items doesn't have any particular method besides the common ones listed above.]]

\header(1, 'DDLParser::Bitfield')

\p[[A bitfield is a set, and bitfield flags are the set's items. Unlike select items, a bitfield flag has a value associated to it. This value can be an explicit zero, an unique, automatically assigned value or a set which is the union of other flags. If the value is automatically assigned, it starts with one and is incremented for each assignment.]]

\p[[In addition to the methods listed here, bitfields have the following common methods: \code'GetName', \code'GetAuthor', \code'GetDescription', \code'GetLabel', \code'GetDisplayLabel', \code'GetNameHash', \code'GetTags', and \code'GetTag'.]]

\header(2, [[\code'uint32_t DDLParser::Bitfield::GetNumFlags() const']], 'DDLParser::Bitfield::GetNumFlags')

\p[[Returns the number of flags contained in the bitfield.]]

\header(2, [[\code'int32_t DDLParser::Bitfield::GetDefaultFlag() const']], 'DDLParser::Bitfield::GetDefaultFlag')

\p[[Returns the index of the flag defined as the default flag for the bitfield. If no flag was defined as the default, the default item is the empty flag. If an empty flag wasn't declared, The first flag is elected the default. This can be used to initialize variables to a default value if none is given for example.]]

\header(2, [[\code'DDLParser::BitfieldFlag* DDLParser::Bitfield::FindFlag( const char* name )']], 'DDLParser::Bitfield::FindFlag')

\p[[Finds and returns a flag by name. If the flag does not exist, NULL is returned.]]

\header(2, [[\code'DDLParser::BitfieldFlag* DDLParser::Bitfield::FindFlag( uint32_t hash )']], 'DDLParser::Bitfield::FindFlag/hash')

\p[[Finds and returns a flag by the hash of its name. If the flag does not exist, NULL is returned. The hash must be created with DDLParser::StringCrc32.]]

\header(1, 'DDLParser::BitfieldFlag')

\p[[A flag is a unique element in a set, or a set of other flags. A flag has the following common methods: \code'GetName', \code'GetAuthor', \code'GetDescription', \code'GetLabel', \code'GetDisplayLabel', \code'GetNameHash', \code'GetTags', and \code'GetTag'.]]

\p[[Flags also have the following unique methods:]]

\header(2, [[\code'DDLParser::BitfieldFlagValue* DDLParser::BitfieldFlag::GetValue() const']], 'DDLParser::BitfieldFlag::GetValue')

\p[[Returns the value of the flag in case it's the empty flag or a set. If neither is true, this function returns NULL.]]

\header(2, [[\code'uint32_t DDLParser::BitfieldFlag::GetBit() const']], 'DDLParser::BitfieldFlag::GetBit')

\p[[If the flag is the empty flag, or if it's a set, this functions returns zero. Otherwise, it returns an automatically assigned value for the flag which is unique in the bitfield. This values starts at one and is incremented for each flag that uses it. It can be used to generate code for flags as bits in an integer, i.e. \code'1 << ( flag->GetBit() - 1 )'.]]

\header(1, 'DDLParser::BitfieldFlagValue')

\p[[Defines the value of a flag when it's either the empty flag or a set.]]

\header(2, [[\code'uint32_t DDLParser::BitfieldFlagValue::GetCount() const']], 'DDLParser::BitfieldFlagValue::GetCount')

\p[[If the flag is the empty flag, this function returns zero. Otherwise, it returns the number of flags contained in the flag.]]

\header(2, [[\code'uint32_t DDLParser::BitfieldFlagValue::GetFlagIndex( uint32_t index ) const']], 'DDLParser::BitfieldFlagValue::GetFlagIndex')

\p[[Returns the index of the flag which is part of this set.]]

\header(2, 'Bitfield example')

\p[[As an example, this code generates C++ code that outputs a bitfield's flags as bits in an unsigned integer:]]

\source('C', [[
static void GenerateBitfield( DDLParser::Bitfield* bitfield )
{
  printf( "namespace %s\n", bitfield->GetName() );
  printf( "{\n" );

  for ( uint32_t i = 0; i < bitfield->GetNumFlags(); i++ )
  {
    DDLParser::BitfieldFlag* flag = bitfield->GetFlag( i );
    DDLParser::BitfieldFlagValue* value = flag->GetValue();

    printf( "  static const uint32_t %-20s = ", flag->GetName() );

    if ( value == NULL )
    {
      // The flag has an automatically assigned value.
      printf( "0x%08x;\n", 1 << ( flag->GetBit() - 1 ) );
    }
    else
    {
      if ( value->GetCount() == 0 )
      {
        // The flag is the empty flag.
        printf( "0x%08x;\n", 0 );
      }
      else
      {
        // The flag is a set containing other flags.
        DDLParser::BitfieldFlag* other = bitfield->GetFlag( 0 );
        printf( "%s", other->GetName() );

        for ( uint32_t j = 1; j < value->GetCount(); j++ )
        {
          other = bitfield->GetFlag( value->GetFlagIndex( j ) );
          printf( " | %s", other->GetName() );
        }

        printf( ";\n" );
      }
    }
  }

  printf( "}\n" );
}]])

\header(1, 'DDLParser::Struct')

\p[[Structures are collections of fields. Each field has its own type, which can be a native type, a select, a bitfield, another structure, a fixed or dynamic array of the previous types, or a hashmap mapping an integer, a string, a file, or a tuid to any other type.]]

\p[[In addition to the methods listed here, structures have the following common methods: \code'GetName', \code'GetAuthor', \code'GetDescription', \code'GetLabel', \code'GetDisplayLabel', \code'GetNameHash', \code'GetTags', and \code'GetTag'.]]

\header(2, [[\code'uint32_t DDLParser::Struct::GetNumFields() const']], 'DDLParser::Struct::GetNumFields')

\p[[Returns the number of fields contained in the structure.]]

\header(2, [[\code'DDLParser::Struct* DDLParser::Struct::GetParent() const']], 'DDLParser::Struct::GetParent')

\p[[Returns the parent structure of this structure. If the structure doesn't inherit from another structure, it returns NULL.]]

\header(2, [[\code'DDLParser::Definition* DDLParser::Struct::GetDefinition() const']], 'DDLParser::Struct::GetDefinition')

\p[[Returns the definition where the structure was defined.]]

\header(2, [[\code'DDLParser::StructField* DDLParser::Struct::FindField( const char* name )']], 'DDLParser::Struct::FindField')

\p[[Finds and returns a field by name. If the field does not exist, NULL is returned.]]

\header(2, [[\code'DDLParser::StructField* DDLParser::Struct::FindField( uint32_t hash )']], 'DDLParser::Struct::FindField/hash')

\p[[Finds and returns a field by the hash of its name. If the field does not exist, NULL is returned. The hash must be created with DDLParser::StringCrc32.]]

\header(2, [[\code'bool DDLParser::Struct::IsInherited( DDLParser::StructField* field )']], 'DDLParser::Struct::IsInherited')

\p[[Returns \code'true' if the field is from a parent structure.]]

\header(2, [[\code'uint32_t DDLParser::Struct::GetSchemaCrc() const']], 'DDLParser::Struct::GetSchemaCrc')

\p[[The schema crc reflects the type, count, name, and order of each field. If any of those change, then the crc changes. It does not reflect default values or any tags.]]

\header(1, 'DDLParser::StructField')

\p[[A field is an instance of a given type. A field has these common methods: \code'GetName', \code'GetAuthor', \code'GetDescription', \code'GetLabel', \code'GetDisplayLabel', \code'GetNameHash', \code'GetTags', and \code'GetTag'.]]

\p[[Fields also have the following unique method.]]

\header(2, [[\code'DDLParser::StructValueInfo* DDLParser::StructField::GetValueInfo()']], 'DDLParser::StructField::GetValueInfo')

\p[[Returns the value information of the field. A field always have a DDLParser::StructField::GetValueInfo so this method never returns NULL.]]

\header(1, 'DDLParser::StructValueInfo')

\p[[A DDLParser::StructValueInfo holds information for both a field declared in a structure and its default values if the field has a structure for its type. It has the the \code'GetTags' and \code'GetTag' common methods.]]

\header(2, [[\code'uint32_t DDLParser::StructValueInfo::GetNameHash() const']], 'DDLParser::StructValueInfo::GetNameHash')

\p[[Returns the hash of the field's name.]]

\header(2, [[\code'Type DDLParser::StructValueInfo::GetType() const']], 'DDLParser::StructValueInfo::GetType')

\p[[Returns the type of the field which is a value from the DDLParser::Type enumeration.]]

\header(2, [[\code'uint32_t DDLParser::StructValueInfo::GetTypeNameHash() const']], 'DDLParser::StructValueInfo::GetTypeNameHash')

\p[[Returns the hash of the field's type name. Only usefull if the field has a select, bitfield or structure type so it's possible to query the definition for the actual aggregate.]]

\header(2, [[\code'const char* DDLParser::StructValueInfo::GetTypeName() const']], 'DDLParser::StructValueInfo::GetTypeName')

\p[[Deprecated.]]

\header(2, [[\code'ArrayType DDLParser::StructValueInfo::GetArrayType() const']], 'DDLParser::StructValueInfo::GetArrayType')

\p[[Returns the array type of the field. Possible values are DDLParser::kScalar (the field is not an array), DDLParser::kFixed (the field is an array with a fixed dimension), DDLParser::kDynamic (the field is an array without an specified dimention) and DDLParser::kHashmap (the field is a hashmap), from the DDLParser::ArrayType enumeration.]]

\header(2, [[\code'DDLParser::StructFieldValue* DDLParser::StructValueInfo::GetValue() const']], 'DDLParser::StructValueInfo::GetValue')

\p[[Returns the array of default values for the field. If the field hasn't a default value, it returns NULL. Only scalar and fixed array types of fields can have a default value. In the former, only the first position of the array of values is valid. In the later, the array of values is valid from 0 to DDLParser::StructValueInfo::GetCount.]]

\header(2, [[\code'uint32_t DDLParser::StructValueInfo::GetCount() const']], 'DDLParser::StructValueInfo::GetCount')

\p[[Returns the size of the array if the array is fixed. Always returns one for scalars and zero for dynamic arrays and hashmaps.]]

\header(2, [[\code'uint32_t DDLParser::StructValueInfo::GetKeyType() const']], 'DDLParser::StructValueInfo::GetKeyType')

\p[[Returns the DDL type of a hashmap key. If the field is not a hashmap, this method doesn't return anything meaningful.]]

\header(2, [[\code'int DDLParser::StructValueInfo::GetKeyBitSize() const']], 'DDLParser::StructValueInfo::GetKeyBitSize')

\p[[Convenience method that returns the bit size of the hashmap key. Returns 64 for DDLParser::kInt64, DDLParser::kUint64, DDLParser::kFloat64, and DDLParser::kTuid, and 32 for all other types.]]

\header(2, [[\code'bool DDLParser::StructValueInfo::AllowSubStruct() const']], 'DDLParser::StructValueInfo::AllowSubStruct')

\p[[Convenience method that returns \code'true' if the field was tagged with the \code'tag( AllowSubstruct )'' generic tag.]]

\header(2, [[\code'uint32_t DDLParser::StructValueInfo::GetSchemaCrc() const']], 'DDLParser::StructValueInfo::GetSchemaCrc')

\p[[The schema crc reflects the type, count, and name of the field. If any of those change, then the crc changes. It does not reflect default values or any tags.]]

\header(2, [[\code'DDLParser::Aggregate* DDLParser::StructValueInfo::GetAggregate( Definition* ddl ) const'']], 'DDLParser::StructValueInfo::GetAggregate')

\p[[Returns the aggregate which is the type of the field. If the field has not a select, bitfield or structure as its type, this method returns NULL.]]

\header(1, 'DDLParser::StructFieldValue')

\p[[DDLParser::StructFieldValue is an union which holds the values defined for fields.]]

\source('C', [[
union DDLParser::StructFieldValue
{
  uint8_t                m_Uint8[ 0 ];
  uint16_t               m_Uint16[ 0 ];
  uint32_t               m_Uint32[ 0 ];
  uint64_t               m_Uint64[ 0 ];
  int8_t                 m_Int8[ 0 ];
  int16_t                m_Int16[ 0 ];
  int32_t                m_Int32[ 0 ];
  int64_t                m_Int64[ 0 ];
  float                  m_Float32[ 0 ];
  double                 m_Float64[ 0 ];
  String                 m_String[ 0 ];
  uint32_t               m_Select[ 0 ];
  StructBitfieldValuePtr m_Bitfield[ 0 ];
  StructStructValuePtr   m_Struct[ 0 ];
  StructUnknownValuePtr  m_Unknown[ 0 ]; // Deprecated
  uint8_t                m_Boolean[ 0 ];
  String                 m_File[ 0 ];
  uint64_t               m_Tuid[ 0 ];
  String                 m_Json[ 0 ];
};]])

\p[[When a field's DDLParser::StructValueInfo has a pointer to a DDLParser::StructFieldValue (via DDLParser::StructValueInfo::GetValue), it means the field has default values. The member of the union that is to be accessed depends on DDLParser::StructValueInfo::GetType, and how many entries are valid depends on the field's array type. For DDLParser::kScalar fields, one position only is valid. For DDLParser::kFixed fields, positions from zero to DDLParser::StructValueInfo::GetCount\code' - 1' are valid. DDLParser::kDynamic and DDLParser::kHashmap fields don't have default values.]]

\header(1, 'DDLParser::StructBitfieldValue')

\p[[When a field is of a bitfield type and has a default value, this value is held in the \code'm_Bitfield' member of the DDLParser::StructFieldValue union. It has the following methods:]]

\header(2, [[\code'uint32_t DDLParser::StructBitfieldValue::GetCount() const']], 'DDLParser::StructBitfieldValue::GetCount')

\p[[Returns the number of flags that make the default value of the field.]]

\header(2, [[\code'uint32_t DDLParser::StructBitfieldValue::GetHash( uint32_t index ) const']], 'DDLParser::StructBitfieldValue::GetHash')

\p[[Returns the hash of the flag at the given index. This hash can be used to get the actual flag from the bitfield via DDLParser::Bitfield::FindFlag.]]

\header(1, 'DDLParser::StructStructValue')

\p[[When a field is of a structure type and has a default value, it's default value is held in the \code'm_Struct' member of the DDLParser::StructFieldValue union. For each field receiving a default value, DDLParser::StructStructValue has a DDLParser::StructValueInfo with details of which field is being initialized and what value it's receiving.]]

\p[[DDLParser::StructStructValue has the following methods:]]

\header(2, [[\code'uint32_t DDLParser::StructStructValue::GetCount() const']], 'DDLParser::StructStructValue::GetCount')

\p[[Returns the number of value infos in this structure value.]]

\header(2, [[\code'DDLParser::StructValueInfo* DDLParser::StructStructValue::GetValueInfo( uint32_t index ) const']], 'DDLParser::StructStructValue::GetValueInfo')

\p[[Returns the value info at the given index.]]

\header(2, 'Example')

\source('DDL', [[
struct A
{
  uint32_t a, value( 1 );
  uint32_t b;
}

struct B
{
  uint32_t[ 2 ] c, value( { 1, 2 } );
  uint32_t[] d;
  string{ uint32_t } f;
  A g, value( { a = 2 } );
}]])

\p[[The DDLParser::StructValueInfo for the fields declared in the DDL code above are:]]

\table{[[\code'A.a']], { {2, 2, 11, 2, align='right'}, { 1, 1, 11, 1, width="15%" }, { 1, 2, 11, 2, width="15%" } },
  { [[Method]],                   [[Return Value]], [[Meaning]] },
  { [[\code'GetNameHash()']],     [[0x0136c985]],   [[DDLParser::StringCrc32 of \code'a', the field name.]] },
  { [[\code'GetType()']],         [[2]],            [[DDLParser::kUint32.]] },
  { [[\code'GetTypeNameHash()']], [[0x0d5d2ca7]],   [[DDLParser::StringCrc32 of \code'uint32_t', but meaningless.]] },
  { [[\code'GetArrayType()']],    [[0]],            [[DDLParser::kScalar.]] },
  { [[\code'GetValue()']],        [[0x009d0074]],   [[The address of the DDLParser::StructFieldValue, since this field has a default value defined for it. This DDLParser::StructFieldValue has the value 1 at \code'm_Uint32[ 0 ]'.]] },
  { [[\code'GetTags()']],         [[0x00000000]],   [[NULL since this field doesn't have any tag.]] },
  { [[\code'GetKeyType()']],      [[0]],            [[Meaningless.]] },
  { [[\code'GetKeyBitSize()']],   [[32]],           [[Meaningless.]] },
  { [[\code'GetAggregate()']],    [[0x00000000]],   [[NULL since this field has a scalar type.]] },
  { [[\code'GetCount()']],        [[1]],            [[This field holds one \code'uint32_t' value.]] },
}

\table{[[\code'A.b']], { {2, 2, 11, 2, align='right'}, { 1, 1, 11, 1, width="15%" }, { 1, 2, 11, 2, width="15%" } },
  { [[Method]],                   [[Return Value]], [[Meaning]] },
  { [[\code'GetNameHash()']],     [[0x983f983f]],   [[DDLParser::StringCrc32 of \code'a', the field name.]] },
  { [[\code'GetType()']],         [[2]],            [[DDLParser::kUint32.]] },
  { [[\code'GetTypeNameHash()']], [[0x0d5d2ca7]],   [[DDLParser::StringCrc32 of \code'uint32_t', but meaningless.]] },
  { [[\code'GetArrayType()']],    [[0]],            [[DDLParser::kScalar.]] },
  { [[\code'GetValue()']],        [[0x00000000]],   [[NULL since this field hasn't a default value defined for it.]] },
  { [[\code'GetTags()']],         [[0x00000000]],   [[NULL since this field doesn't have any tag.]] },
  { [[\code'GetKeyType()']],      [[0]],            [[Meaningless.]] },
  { [[\code'GetKeyBitSize()']],   [[32]],           [[Meaningless.]] },
  { [[\code'GetAggregate()']],    [[0x00000000]],   [[NULL since this field has a scalar type.]] },
  { [[\code'GetCount()']],        [[1]],            [[This field holds one \code'uint32_t' value.]] },
}

\table{[[\code'B.c']], { {2, 2, 11, 2, align='right'}, { 1, 1, 11, 1, width="15%" }, { 1, 2, 11, 2, width="15%" } },
  { [[Method]],                   [[Return Value]], [[Meaning]] },
  { [[\code'GetNameHash()']],     [[0xef38a8a9]],   [[DDLParser::StringCrc32 of \code'c', the field name.]] },
  { [[\code'GetType()']],         [[2]],            [[DDLParser::kUint32.]] },
  { [[\code'GetTypeNameHash()']], [[0x0d5d2ca7]],   [[DDLParser::StringCrc32 of \code'uint32_t', but meaningless.]] },
  { [[\code'GetArrayType()']],    [[1]],            [[DDLParser::kFixed.]] },
  { [[\code'GetValue()']],        [[0x00780128]],   [[The address of the DDLParser::StructFieldValue, since this field has a default value defined for it. This DDLParser::StructFieldValue has the values 1 and 2 at \code'm_Uint32[ 0 ]' and \code'm_Uint32[ 1 ]'.]] },
  { [[\code'GetTags()']],         [[0x00000000]],   [[NULL since this field doesn't have any tag.]] },
  { [[\code'GetKeyType()']],      [[0]],            [[Meaningless.]] },
  { [[\code'GetKeyBitSize()']],   [[32]],           [[Meaningless.]] },
  { [[\code'GetAggregate()']],    [[0x00000000]],   [[NULL since this field has a scalar type.]] },
  { [[\code'GetCount()']],        [[2]],            [[This field holds two \code'uint32_t' values.]] },
}

\table{[[\code'B.d']], { {2, 2, 11, 2, align='right'}, { 1, 1, 11, 1, width="15%" }, { 1, 2, 11, 2, width="15%" } },
  { [[Method]],                   [[Return Value]], [[Meaning]] },
  { [[\code'GetNameHash()']],     [[0x715c3d0a]],   [[DDLParser::StringCrc32 of \code'd', the field name.]] },
  { [[\code'GetType()']],         [[2]],            [[DDLParser::kUint32.]] },
  { [[\code'GetTypeNameHash()']], [[0x0d5d2ca7]],   [[DDLParser::StringCrc32 of \code'uint32_t', but meaningless.]] },
  { [[\code'GetArrayType()']],    [[2]],            [[DDLParser::kDynamic.]] },
  { [[\code'GetValue()']],        [[0x00000000]],   [[NULL since this field hasn't (and can't have) a default value defined for it.]] },
  { [[\code'GetTags()']],         [[0x00000000]],   [[NULL since this field doesn't have any tag.]] },
  { [[\code'GetKeyType()']],      [[0]],            [[Meaningless.]] },
  { [[\code'GetKeyBitSize()']],   [[32]],           [[Meaningless.]] },
  { [[\code'GetAggregate()']],    [[0x00000000]],   [[NULL since this field has a scalar type.]] },
  { [[\code'GetCount()']],        [[0]],            [[This field is a dynamic array, which initialy has no elements.]] },
}

\table{[[\code'B.f']], { {2, 2, 11, 2, align='right'}, { 1, 1, 11, 1, width="15%" }, { 1, 2, 11, 2, width="15%" } },
  { [[Method]],                   [[Return Value]], [[Meaning]] },
  { [[\code'GetNameHash()']],     [[0x9f525c26]],   [[DDLParser::StringCrc32 of "f", the field name.]] },
  { [[\code'GetType()']],         [[10]],           [[DDLParser::kString.]] },
  { [[\code'GetTypeNameHash()']], [[0xa76af9f8]],   [[DDLParser::StringCrc32 of "string", but meaningless.]] },
  { [[\code'GetArrayType()']],    [[3]],            [[DDLParser::kHashmap.]] },
  { [[\code'GetValue()']],        [[0x00000000]],   [[NULL since this field hasn't (and can't have) a default value defined for it.]] },
  { [[\code'GetTags()']],         [[0x00000000]],   [[NULL since this field doesn't have any tag.]] },
  { [[\code'GetKeyType()']],      [[2]],            [[DDLParser::kUint32.]] },
  { [[\code'GetKeyBitSize()']],   [[32]],           [[The size of the key of this hashmap, in bits.]] },
  { [[\code'GetAggregate()']],    [[0x00000000]],   [[NULL since this field has a scalar type (strings are scalars in DDL).]] },
  { [[\code'GetCount()']],        [[0]],            [[This field is a hashmap, which initialy has no elements.]] },
}

\table{[[\code'B.g']], { {2, 2, 11, 2, align='right'}, { 1, 1, 11, 1, width="15%" }, { 1, 2, 11, 2, width="15%" } },
  { [[Method]],                   [[Return Value]], [[Meaning]] },
  { [[\code'GetNameHash()']],     [[0xe8556cb0]],   [[DDLParser::StringCrc32 of "g", the field name.]] },
  { [[\code'GetType()']],         [[13]],           [[DDLParser::kStruct.]] },
  { [[\code'GetTypeNameHash()']], [[0x3a58e94d]],   [[DDLParser::StringCrc32 of "A", the name of the aggregate which is the type of this field.]] },
  { [[\code'GetArrayType()']],    [[0]],            [[DDLParser::kScalar.]] },
  { [[\code'GetValue()']],        [[0x007801e4]],   [[The address of the DDLParser::StructFieldValue, since this field has a default value defined for it. This DDLParser::StructFieldValue has a DDLParser::StructStructValue pointer at \code'm_Struct[ 0 ]'.]] },
  { [[\code'GetTags()']],         [[0x00000000]],   [[NULL since this field doesn't have any tag.]] },
  { [[\code'GetKeyType()']],      [[0]],            [[Meaningless.]] },
  { [[\code'GetKeyBitSize()']],   [[32]],           [[Meaningless.]] },
  { [[\code'GetAggregate()']],    [[0x00780014]],   [[The address of DDLParser::Struct \code'A' which is the type of this field.]] },
  { [[\code'GetCount()']],        [[1]],            [[This field holds one structure \code'A' value.]] },
}

\p[[The details of the DDLParser::StructStructValue for \code'B.g' are:]]

\table{[[Details of the DDLParser::StructStructValue for \code'B.g']], { {2, 2, 11, 2, align='right'}, { 1, 1, 11, 1, width="15%" }, { 1, 2, 11, 2, width="15%" } },
  { [[Method]],                   [[Return Value]], [[Meaning]] },
  { [[\code'GetNameHash()']],     [[0x0136c985]],   [[DDLParser::StringCrc32 of "a", the name of the field being initialized.]] },
  { [[\code'GetType()']],         [[2]],            [[DDLParser::kUint32.]] },
  { [[\code'GetTypeNameHash()']], [[0x0d5d2ca7]],   [[DDLParser::StringCrc32 of "uint32_t", but meaningless.]] },
  { [[\code'GetArrayType()']],    [[0]],            [[DDLParser::kScalar.]] },
  { [[\code'GetValue()']],        [[0x00780214]],   [[The address of the DDLParser::StructFieldValue, since this field has a default value defined for it. This DDLParser::StructFieldValue has the value 2 at \code'm_Uint32[ 0 ]', which overrides the default value 1 of \code'A.a'.]] },
  { [[\code'GetTags()']],         [[0x00000000]],   [[NULL as value infos of fields being initialized can't have tags.]] },
  { [[\code'GetKeyType()']],      [[0]],            [[Meaningless.]] },
  { [[\code'GetKeyBitSize()']],   [[32]],           [[Meaningless.]] },
  { [[\code'GetAggregate()']],    [[0x00000000]],   [[NULL since this field has a scalar type.]] },
  { [[\code'GetCount()']],        [[1]],            [[This field holds one \code'uint32_t' value.]] },
}

\header(1, 'DDLParser::Tag')

\p[[Tags are decorations that can be added to structures, fields, selects, items, bitfields, and flags. Their purpose is to pass down information to code generators to change the way code is generated for them.]]

\p[[DDLParser::Tag is the parent of all other tags, and has the following methods:]]

\header(2, [[\code'uint32_t DDLParser::Tag::GetType() const']], 'DDLParser::Tag::GetType')

\p[[Returns the type of the tag, which is a value from the DDLParser::TagType enum.]]

\header(2, [[\code'DDLParser::Tag* DDLParser::Tag::GetNext() const']], 'DDLParser::Tag::GetNext')

\p[[The tags of a DDL component form a linked list. \code'GetTags' returns the first tag for the component, and DDLParser::Tag::GetNext is used to return the next tag in the linked list. Returns NULL at the end of the list.]]

\header(1, 'DDLParser::GenericTag')

\p[[All tag types are deprecated, new code should only use generic tags that can be defined without changing the DDL parser. Generic tags have the DDLParser::kGeneric type and the following methods in addition to the methods from DDLParser::Tag:]]

\header(2, [[\code'const char* DDLParser::GenericTag::GetName() const']], 'DDLParser::GenericTag::GetName')

\p[[Returns the identifier used in the tag definition.]]

\header(2, [[\code'uint32_t DDLParser::GenericTag::GetNameHash() const']], 'DDLParser::GenericTag::GetNameHash')

\p[[Returns the hash of the identifier used in the tag definition, created with DDLParser::StringCrc32.]]

\header(2, [[\code'uint32_t DDLParser::GenericTag::GetNumValues() const']], 'DDLParser::GenericTag::GetNumValues')

\p[[Returns the number of values defined in the tag after its name.]]

\header(2, [[\code'DDLParser::GenericTagValue* DDLParser::GenericTag::operator[]( unsigned int index ) const']], 'DDLParser::GenericTag::operator[]')

\p[[Returns the value defined for the tag at the given index.]]

\header(1, 'DDLParser::GenericTagValue')

\p[[Represents a value defined in a generic tag after its name:]]

\source('C', [[
struct DDLParser::GenericTagValue
{
  uint32_t m_Type;

  union
  {
    int64_t m_Int64;
    double  m_Float64;
    String  m_String;
  };
};]])

\p[[The \code'm_Type' member can be \code'DDLParser::kInt64', \code'DDLParser::kFloat64', or \code'DDLParser::kString'. Use this member to know the type of the value and which of the other members to access in a value.]]

\header(2, 'Validating Generic Tags')

\p[[Every time the parser parses a generic tag it calls one of the following callback functions to validate the tag:]]

\ul{
  [[\code'typedef bool DDLParser::CheckStructTag( DDLParser::Definition* def, DDLParser::Struct* structure, DDLParser::GenericTag* tag, DDLParser::Str* tag_name, DDLParser::TagSet* tag_set, char* error, size_t error_size );']],
  [[\code'typedef bool DDLParser::CheckFieldTag( DDLParser::Definition* def, DDLParser::Struct* structure, DDLParser::StructField* field, DDLParser::GenericTag* tag, DDLParser::Str* tag_name, DDLParser::TagSet* tag_set, char* error, size_t error_size );']],
  [[\code'typedef bool DDLParser::CheckSelectTag( DDLParser::Definition* def, DDLParser::Select* select, DDLParser::GenericTag* tag, DDLParser::Str* tag_name, DDLParser::TagSet* tag_set, char* error, size_t error_size );']],
  [[\code'typedef bool DDLParser::CheckItemTag( DDLParser::Definition* def, DDLParser::Select* select, DDLParser::SelectItem* item, DDLParser::GenericTag* tag, DDLParser::Str* tag_name, DDLParser::TagSet* tag_set, char* error, size_t error_size );']],
  [[\code'typedef bool DDLParser::CheckBitfieldTag( DDLParser::Definition* def, DDLParser::Bitfield* bitfield, DDLParser::GenericTag* tag, DDLParser::Str* tag_name, DDLParser::TagSet* tag_set, char* error, size_t error_size );']],
  [[\code'typedef bool DDLParser::CheckFlagTag( DDLParser::Definition* def, DDLParser::Bitfield* bitfield, DDLParser::BitfieldFlag* flag, DDLParser::GenericTag* tag, DDLParser::Str* tag_name, DDLParser::TagSet* tag_set, char* error, size_t error_size );']],
}

\p[[These callbacks must return \code'true' to inform the parser that the tag was accepted, or \code'false' to inform that it was rejected. In the later case, the callback must also fill the \code'error' buffer with an error message up to \code'error_size' characters.]]

\p[[If a callback isn't defined, the tag is automatically accepted.]]

\note[[DDLParser::Compile doesn't give the programmer the chance to install callbacks in the parser. For that, duplicate the source code of DDLParser::Compile and add the callbacks as needed.]]

\header(1, 'DDLParser::Str')

\p[[DDLParser::Str represent internal strings during the parsing. They point directly into the DDL source code being parsed so they cannot be changed. The methods that can be used to validate a generic tag are:]]

\ul{
  [[\code'bool DDLParser::Str::operator==( const DDLParser::Str::Str& other ) const']],
  [[\code'bool DDLParser::Str::operator==( const char* other ) const']],
  [[\code'const char* DDLParser::Str::GetChars() const']],
  [[\code'unsigned int DDLParser::Str::GetLength() const']],
  [[\code'uint32_t DDLParser::Str::GetHash() const']],
}

\header(1, 'DDLParser::TagSet')

\p[[When one of the callbacks is called to validate a generic tag, a DDLParser::TagSet is provided to allow for keeping a record of which tags have already been defined and i.e. disallow duplicates. The methods that can be used during validation are:]]

\ul{
  [[\code'bool DDLParser::TagSet::Insert( uint32_t element )']],
  [[\code'bool DDLParser::TagSet::Exists( uint32_t element )']],
}

\p[[Note that the tag's name hash (as returned by DDLParser::GenericTag::GetNameHash) should be used with tag sets.]]

\header(1, 'DDL Grammar')

\p[[The language permits the declaration of any number of structures, bitfields and selects. At least one such declaration must exist in a valid DDL file.]]

\source('EBF', [[
grammar ddl;

ddl
  : ( typedef | struct | bitfield | select )+
  ;]])

\p[[A typedef is an alias to a data type, array type and a set of tags. After defining a typedef it can be used to declare fields, which will have everything that was defined in the typedef.]]

\source('EBF', [[
typedef
  : 'typedef' data_type ( '[' ( expression )? ']' | '{' data_type '}' )? ( ',' typedef_info )*
  ;

typedef_info
  : 'author' '(' LITERAL ')'
  | 'description' '(' LITERAL ')'
  | 'label' '(' LITERAL ')'
  | 'uirender' '(' LITERAL ')'
  | 'callback' '(' LITERAL ')'
  | 'key' '(' LITERAL ')'
  | 'units' '(' LITERAL ')'
  | 'extensions' '(' LITERAL ( ',' LITERAL )* ')'
  | 'vaulthints' '(' LITERAL ( ',' LITERAL )* ')'
  | 'uirange' '(' expression ',' expression ( ',' expression ',' expression ( ',' expression )? )? ')'
  | generic_tag
  ;]])

\p[[A structure begins with the \code'struct' keyword and with the structure name, an identifier. After the name the author, description, and label of the structure can be specified but they're not mandatory. The fields can then be specified enclosed by curly braces.]]

\source('EBF', [[
struct
  : 'struct' ID ( ',' struct_info )* '{' field* '}'
  ;]])

\p[[\code'struct_info' allows for additional information about aggregates, namely its author, description and label. Labels are used to name UI controls, and descriptions can be used as tips for these controls. Although the grammar allows for repetitions, the parser checks for duplicates and errors accordingly.]]

\p[[The structure can also inherit fields from a parent structure, specified by the \code'base' tag.]]

\source('EBF', [[
struct_info
  : 'author' '(' LITERAL ')'
  | 'description' '(' LITERAL ')'
  | 'label' '(' LITERAL ')'
  | 'base' '(' ID ')'
  | 'uirender' '(' LITERAL ')'
  | 'version' '(' LITERAL ')'
  | 'callback' '(' LITERAL ')'
  | 'key' '(' LITERAL ')'
  | generic_tag
  ;

generic_tag:
  : 'tag' '(' ID ( ',' expression )* ')'
  ;]])

\p[[A field declaration begins with the field type of a typedef identifier. After that the field name (an identifier), is parsed and additional information about the field can be specified.]]

\source('EBF', [[
field
  : ( field_type | ID ) ID ( ',' field_info )* ';'
  ;]])

\p[[The field type is a data type. If the data type is followed by a opening square brace, it denotes that the field is an array. If an integer expression is provided within the braces the array has a fixed size, otherwise the array is dynamic and elements can added to/removed from it at runtime (depending on the code that is generated). If the data type is followed by opening curly brace, it denotes a hashmap mapping values of the data type inside the curly braces to values of the data type of the field.]]

\source('EBF', [[
field_type
  : data_type ( '[' ( expression )? ']' | '{' data_type '}' )?
  ;]])

\p[[The field data type can be one of the native types or a custom type. The native types are signed and unsigned integers with 8, 16, 32 or 64 bits, float, double, string, file, boolean, tuid and json. A custom type is just an identifier denoting a select, bitfield or structure.]]

\source('EBF', [[
data_type
  : 'uint8_t'  | 'u8'
  | 'uint16_t' | 'u16'
  | 'uint32_t' | 'u32'
  | 'uint64_t' | 'u64'
  | 'int8_t'   | 'i8'
  | 'int16_t'  | 'i16'
  | 'int32_t'  | 'i32'
  | 'int64_t'  | 'i64'
  | 'float'    | 'f32'
  | 'double'   | 'f64'
  | 'boolean'  | 'bool'
  | 'string'
  | 'file'
  | 'tuid'
  | 'json'
  | ID
  ;]])

\p[[\code'field_info' allows for additional information about fields, namely its author, description, label and a default value. The default value is the value used to initialize the field.]]

\source('EBF', [[
field_info
  : 'author' '(' LITERAL ')'
  | 'description' '(' LITERAL ')'
  | 'label' '(' LITERAL ')'
  | 'value' '(' default_value ')'
  | 'extensions' '(' LITERAL ( ',' LITERAL )* ')'
  | 'vaulthints' '(' LITERAL ( ',' LITERAL )* ')'
  | 'uirange' '(' expression ',' expression ( ',' expression ',' expression ( ',' expression )? )? ')'
  | 'uirender' '(' LITERAL ')'
  | 'parallel' '(' ID ')'
  | 'units' '(' LITERAL ')'
  | generic_tag
  ;]])

\p[[The \code'default_value' is a complex rule, and which option is taken depends on the context. In other words, it's a parser decision depending on what it was parsing.]]

\source('EBF', [[
default_value
  /* 1 */ : native_value
  /* 2 */ | select_value
  /* 3 */ | bitfield_value
  /* 4 */ | struct_value
  /* 5 */ | '{' native_value   ( ',' native_value   )* '}'
  /* 6 */ | '{' select_value   ( ',' select_value   )* '}'
  /* 7 */ | '{' bitfield_value ( ',' bitfield_value )* '}'
  /* 8 */ | '{' struct_value   ( ',' struct_value   )* '}'
  ;

native_value
  : expression
  ;

select_value
  : ID
  ;

bitfield_value
  : ID ( '|' ID )*
  ;

struct_value
  : '{' ID '=' default_value ( ',' ID '=' default_value )* '}'
  ;]])

\ul{
  [[Option #1 is taken when the field is of a native type and is not an array.]],
  [[Option #2 is taken when the field is of a select type and is not an array.]],
  [[Option #3 is taken when the field is of a bitfield type and is not an array.]],
  [[Option #4 is taken when the field is of a structure type and is not an array.]],
  [[Options #5 to #8 correspond to options #1 to #4 but are for arrays of native, select, bitfield and structure values.]],
}

\p[[A bitfield is declared with the \code'bitfield' keyword followed by its name, an identifier, which may be followed by additional information. Its flags are declared inside enclosing braces.]]

\source('EBF', [[
bitfield
  : 'bitfield' ID ( ',' bitfield_info )* '{' flag+ '}'
  ;

bitfield_info
  : 'author' '(' LITERAL ')'
  | 'description' '(' LITERAL ')'
  | 'label' '(' LITERAL ')'
  | generic_tag
  ;]])

\p[[Declarations of bitfield flags begin with an identifier which is its name, followed by additional flag information.]]

\source('EBF', [[
flag
  : ID ( ',' flag_info )* ';'
  ;]])

\p[[Besides author, description and label, a flag can be specified as the default flag for the bitfield. Flags can also be declared as empty, meaning that their value is zero. If a value is specified, the flag is a set containing all listed flags.]]

\source('EBF', [[
flag_info
  : 'author' '(' LITERAL ')'
  | 'description' '(' LITERAL ')'
  | 'label' '(' LITERAL ')'
  | 'default'
  | 'value' '(' ID ( '|' ID )* ')'
  | 'empty'
  | generic_tag
  ;]])

\p[[A select is declared in the same way of a bitfield.]]

\source('EBF', [[
select
  : 'select' ID ( ',' select_info )* '{' item+ '}'
  ;

select_info
  : 'author' '(' LITERAL ')'
  | 'description' '(' LITERAL ')'
  | 'label' '(' LITERAL ')'
  | generic_tag
  ;]])

\p[[A select item has an implicit type of \code'uint32_t' so it's not possible to define a type for it. Its declaration begin with an identifier which is its name, followed by additional information. The value of a select item is the DDLParser::StringCrc32 of its name so it's not possible to define a value for it.]]

\p[[The item defined as the default item is used to initialize the default value of structure fields having the select type. As with bitfields, this default can be overridden by using the \code'value' tag in the field declaration. If no item is declared as the default, the first item declared in the select will be the default.]]

\source('EBF', [[
item
  : ID ( ',' item_info )* ';'
  ;

item_info
  : 'author' '(' LITERAL ')'
  | 'description' '(' LITERAL ')'
  | 'label' '(' LITERAL ')'
  | 'default'
  | generic_tag
  ;]])

\p[[The expression rule parses any valid C-like expression. It's defined as the ternary operator.]]

\source('EBF', [[
expression
  : ternary
  ;]])

\p[[The ternary operator is a \code'logic_or' expression optionally followed by a \code'?' and two expressions. If the logic_or expression is true (different from zero), the value of the ternary operator is the result of the first expression. If it's false, the value of the ternary operator is the result of the second expression. If \code'?' is not present in the expression, the value of the ternary operator is the value of the \code'logic_or' expression.]]

\source('EBF', [[
ternary
  : logic_or ( '?' expression ':' expression )?
  ;]])

\p[[The \code'logic_or' expression is the value of a \code'logic_and' expression, optionally or'ed with other \code'logic_and' expressions.]]

\source('EBF', [[
logic_or
  : logic_and ( '||' logic_and )*
  ;]])

\p[[The \code'logic_and' expression is the value of an \code'arithmetic_or' expression, optionally and'ed with other \code'arithmetic_or' expressions.]]

\source('EBF', [[
logic_and
  : arithmetic_or ( '&&' arithmetic_or )*
  ;]])

\p[[The \code'arithmetic_or' expression is the value of an \code'arithmetic_xor' expression, optionally bitwise-or'ed with other \code'arithmetic_xor' expressions.]]

\source('EBF', [[
arithmetic_or
  : arithmetic_xor ( '|' arithmetic_xor )*
  ;]])

\p[[The \code'arithmetic_xor' expression is the value of an \code'arithmetic_and' expression, optionally bitwise-xor'ed with other \code'arithmetic_and' expressions.]]

\source('EBF', [[
arithmetic_xor
  : arithmetic_and ( '^' arithmetic_and )*
  ;]])

\p[[The \code'arithmetic_and' expression is the value of an \code'equal' expression, optionally bitwise-and'ed with other \code'equal' expressions.]]

\source('EBF', [[
arithmetic_and
  : equal ( '&' equal )*
  ;]])

\p[[The \code'equal' expression is the value of a \code'conditional' expression, optionally compared for equality or inequality with other \code'conditional' expressions. It's thus possible to write \code'a > b == c > d' meaning \code'a > b' and \code'c > d' are both either false or true, and \code'a > b != c > d' meaning \code'a > b' is true and \code'c > d' is false or vice-versa.]]

\source('EBF', [[
equal
  : conditional ( ( '==' | '!=' ) conditional )*
  ;]])

\p[[The \code'conditional' expression is the value of a \code'shift' expression, optionally compared with other \code'shift' expressions with one of the operators \code'<', \code'>', \code'<=' or \code'>='.]]

\source('EBF', [[
conditional
  : shift ( ( '<' | '>' | '<=' | '>=' ) shift )*
  ;]])

\p[[The \code'shift' expression is the value of a \code'term' expression, optionally shifted left or right by other \code'term' expressions.]]

\source('EBF', [[
shift
  : term ( ( '<<' | '>>' ) term )*
  ;]])

\p[[The \code'term' expression is the value of a \code'factor' expression, optionally added to or subtracted from other \code'factor' expressions.]]

\source('EBF', [[
term
  : factor ( ( '+' | '-' ) factor )*
  ;]])

\p[[The factor expression is the value of an \code'unary' expression, optionally multiplied by, divided by, mod'ed by other \code'unary' expressions.]]

\source('EBF', [[
factor
  : unary ( ( '*' | '/' | '%' ) unary )*
  ;]])

\p[[The \code'unary' expression is either the value of a \code'terminal' or the value of another \code'unary' changed by the unary plus, minus, negate or not operators.]]

\source('EBF', [[
unary
  : ( '+' | '-' | '~' | '!' ) unary
  | terminal
  ;]])

\p[[The terminal expression is an octal, hexadecimal, decimal or real (in base 10) number, a string, the constants \code'true', \code'false', \code'pi' or \code'e', or another expression enclosed in parenthesis.]]

\source('EBF', [[
terminal
  : OCTAL | BINARY | HEXADECIMAL | DECIMAL | REAL | LITERAL
  | 'true' | 'false'
  | 'pi' | 'e'
  | '(' expression ')'
  ;]])

\p[[Octal constants must begin with a 0, and can only have digits from 0 to 7.]]

\source('EBF', [[
OCTAL
  : '0' ( '0' .. '7' )+
  ;]])

\p[[Binary constants must begin with \code'0b', and can only have digits 0 and 1.]]

\source('EBF', [[
BINARY
  : '0b' ( '0' | '1' )+
  ;]])

\p[[Hexadecimal constants must begin with \code'0x', and can have digits from 0 to 9 and from A to F (case insensitive).]]

\source('EBF', [[
HEXADECIMAL
  : '0x' ( HEX_DIGIT )+
  ;

HEX_DIGIT:
  : '0' .. '9'
  | 'a' .. 'f'
  | 'A' .. 'f'
  ;]])
  
\p[[Decimal numbers can only have digits from 0 to 9. In the lexical analyser, if a number starts with 0 it's considered to be an octal constant.]]

\source('EBF', [[
DECIMAL
  : ( '0' .. '9' )+
  ;]])

\p[[Real numbers are decimal numbers with an optional decimal period followed by another decimal number. They can have an optional exponent (an \code'e' or \code'E') followed by the decimal exponent value which can be signed, and optionally ended with an \code'f'. If the real number does not have a decimal period and neither an exponent, it must end with an \code'f' otherwise it will be analyzed as a decimal number. Note that this is different from what is expected from C.]]

\source('EBF', [[
REAL
  : DECIMAL ( '.' DECIMAL )? ( ( 'e' | 'E' ) ( '+' | '-' ) DECIMAL )? 'f'?
  ;]])

\p[[Identifiers are case sensitive sequences of upper case letters, lower case letters, decimal digits and underscores. They cannot begin with a decimal digit.]]

\source('EBF', [[
ID
  : ( 'a' .. 'z' | 'A' .. 'Z' | '_' ) ( 'a' .. 'z' | 'A' .. 'Z' | '_' | '0' .. '9' )*
  ;]])

\p[[Literals are string constants which are sequences of any characters except the percent sign (which is the beginning of an escape sequence), the double quote (which marks the end of the string) and line feed. Strings are enclosed in double quotes.]]

\source('EBF', [[
LITERAL
  : '"' ( '%' HEX_DIGIT HEX_DIGIT | ~( '%' | '"' ) )* '"'
  ;]])

\p[[Escape sequences always start with a \code'%', and must be followed by two hexadecimal digits. These three characters will be converted to one character in the resulting literal, and this character will have the code specified by the two hexadecimal digits.]]

\p[[Literals are codepage agnostic, so the meaning of characters with codes outside of the [0, 127] range are platform dependent.]]

\header(1, 'License')

\p[[Copyright (c) 2012, Insomniac Games\br()All rights reserved.]]

\p[[Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:]]

\ul{
  [[Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.]],
  [[Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.]],
}

\p[[THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.]]

]===]

--  ######  ##    ## ##    ## ########    ###    ##     ##
-- ##    ##  ##  ##  ###   ##    ##      ## ##    ##   ##
-- ##         ####   ####  ##    ##     ##   ##    ## ##
--  ######     ##    ## ## ##    ##    ##     ##    ###
--       ##    ##    ##  ####    ##    #########   ## ##
-- ##    ##    ##    ##   ###    ##    ##     ##  ##   ##
--  ######     ##    ##    ##    ##    ##     ## ##     ##

local ddlClasses =
{
  uint8_t = 'type',
  uint16_t = 'type',
  uint32_t = 'type',
  uint64_t = 'type',
  int8_t = 'type',
  int16_t = 'type',
  int32_t = 'type',
  int64_t = 'type',
  float = 'type',
  double = 'type',
  switch = 'flow',
  struct = 'keyword',
  select = 'keyword',
  bitfield = 'keyword',
  author = 'keyword',
  description = 'keyword',
  label = 'keyword',
  default = 'keyword',
  empty = 'keyword',
  value = 'keyword',
  [ 'true' ] = 'bool',
  [ 'false' ] = 'bool',
}

local cClasses =
{
  unsigned = 'type',
  signed = 'type',
  short = 'type',
  long = 'type',
  char = 'type',
  bool = 'type',
  void = 'type',
  uint8_t = 'type',
  uint16_t = 'type',
  uint32_t = 'type',
  uint64_t = 'type',
  int8_t = 'type',
  int16_t = 'type',
  int32_t = 'type',
  int64_t = 'type',
  float = 'type',
  double = 'type',
  [ 'if' ] = 'flow',
  [ 'else' ] = 'flow',
  [ 'do' ] = 'flow',
  [ 'while' ] = 'flow',
  [ 'for' ] = 'flow',
  switch = 'flow',
  struct = 'keyword',
  default = 'keyword',
  case = 'keyword',
  static = 'keyword',
  union = 'keyword',
  [ 'true' ] = 'bool',
  [ 'false' ] = 'bool',
}

local ebfClasses = {}

local function getClass( code, i, classes )
  if i > #code then
    return nil, i
  end

  local k = code:sub( i, i )

  while k == ' ' or k == '\t' or k == '\n' do
    i = i + 1
    if i > #code then
      return nil, i
    end
    k = code:sub( i, i )
  end

  if k >= '0' and k <= '9' then
    if k == '0' then
      i = i + 1
      k = code:sub( i, i )
      if k == 'x' or k == 'X' then
        i = i + 1
        k = code:sub( i, i )
        while ( k >= '0' and k <= '9' ) or ( k >= 'a' and k <= 'f' ) or ( k >= 'A' and k <= 'F' ) do
          i = i  + 1
          k = code:sub( i, i )
        end
        return 'int', i
      elseif k == 'b' or k == 'B' then
        i = i + 1
        k = code:sub( i, i )
        while k == '0' or k == '1' do
          i = i + 1
          k = code:sub( i, i )
        end
        return 'int', i
      elseif k >= '0' and k <= '7' then
        i = i + 1
        k = code:sub( i, i )
        while k >= '0' and k <= '7' do
          i = i + 1
          k = code:sub( i, i )
        end
        return 'int', i
      end
    end
    class = 'int'
    while k >= '0' and k <= '9' do
      i = i + 1
      k = code:sub( i, i )
    end
    if k == '.' then
      class = 'float'
      i = i + 1
      k = code:sub( i, i )
      while k >= '0' and k <= '9' do
        i = i + 1
        k = code:sub( i, i )
      end
    end
    if k == 'e' or k == 'E' then
      class = 'float'
      i = i + 1
      k = code:sub( i, i )
      if k == '+' or k == '-' then
        i = i + 1
        k = code:sub( i, i )
      end
      while k >= '0' and k <= '9' do
        i = i + 1
        k = code:sub( i, i )
      end
      if k == 'f' then
        class = 'float'
        i = i + 1
      end
    end
    return class, i
  elseif ( k >= 'a' and k <= 'z' ) or ( k >= 'A' and k <= 'Z' ) or k == '_' then
    local s = i
    while ( k >= 'a' and k <= 'z' ) or ( k >= 'A' and k <= 'Z' ) or k == '_' or ( k >= '0' and k <= '9' ) do
      i = i + 1
      k = code:sub( i, i )
    end
    return classes[ code:sub( s, i - 1 ) ], i
  elseif k == '\'' then
    i = i + 1
    k = code:sub( i, i )
    while k ~= '\'' do
      i = i + 1
      k = code:sub( i, i )
    end
    return 'string', i + 1
  elseif k == '"' then
    i = i + 1
    k = code:sub( i, i )
    while k ~= '"' do
      i = i + 1
      k = code:sub( i, i )
    end
    return 'string', i + 1
  elseif k == '/' and code:sub( i + 1, i + 1 ) == '/' then
    i = i + 2
    k = code:sub( i, i )
    while k ~= '\n' do
      i = i  + 1
      k = code:sub( i, i )
    end
    return 'comment', i
  elseif k == '/' and code:sub( i + 1, i + 1 ) == '*' then
    i = i + 2
    k = code:sub( i, i )
    while k ~= '*' and code:sub( i + 1, i + 1 ) ~= '/' do
      i = i + 1
      k = code:sub( i, i )
    end
    return 'comment', i + 2
  else
    return 'operator', i + 1
  end
end

local function highlight( code, classes )
  local hl = ''
  local x, y = 1, #code

  while x < y do
    local z = x
    local class
    class, x = getClass( code, x, classes )

    if class then
      hl = hl .. '<span class="' .. class .. '">' .. code:sub( z, x - 1 ) .. '</span>'
    else
      hl = hl .. code:sub( z, x - 1 )
    end
  end

  return hl
end

-- ########  #######  ########  ##     ##    ###    ########
-- ##       ##     ## ##     ## ###   ###   ## ##      ##
-- ##       ##     ## ##     ## #### ####  ##   ##     ##
-- ######   ##     ## ########  ## ### ## ##     ##    ##
-- ##       ##     ## ##   ##   ##     ## #########    ##
-- ##       ##     ## ##    ##  ##     ## ##     ##    ##
-- ##        #######  ##     ## ##     ## ##     ##    ##

local function format( what, formatter )
  local function compare( c1, c2 )
    if c1[ 1 ] and c2[ 1 ] then
      return c1[ 1 ] < c2[ 1 ]
    elseif c1[ 1 ] then
      return true
    elseif c2[ 2 ] then
      return false
    else
      return false
    end
  end

  local cmds = {}
  local res = ''

  while true do
    cmds[ 1 ] = { what:find( '\\(%w+)(%b())' ) }
    cmds[ 2 ] = { what:find( '\\(%w+)(%b{})' ) }
    cmds[ 3 ] = { what:find( '\\(%w+)(\'[^\']*\')' ) }
    cmds[ 4 ] = { what:find( '\\(%w+)(%b[])' ) }
    cmds[ 5 ] = { what:find( '\\(%w+)%s' ) }

    table.sort( cmds, compare )

    if not cmds[ 1 ][ 1 ] then
      return res .. what
    end

    local i = cmds[ 1 ][ 1 ]
    local j = cmds[ 1 ][ 2 ]
    local cmd = cmds[ 1 ][ 3 ]
    local args = cmds[ 1 ][ 4 ]

    if not args or args == '()' then
      args = '(formatter)'
    elseif args:sub( 1, 1 ) == '(' then
      args = '(formatter, ' .. args:sub( 2, -1 )
    else
      args = '(formatter, ' .. args .. ')'
    end

    local func = 'return function( formatter ) return formatter.' .. cmd .. args .. ' end'
    res = res .. what:sub( 1, i -1 ) .. assert( loadstring( func ) )()( formatter )
    what = what:sub( j + 1, -1 )
  end
end

-- ########  ##          ###    #### ##    ##
-- ##     ## ##         ## ##    ##  ###   ##
-- ##     ## ##        ##   ##   ##  ####  ##
-- ########  ##       ##     ##  ##  ## ## ##
-- ##        ##       #########  ##  ##  ####
-- ##        ##       ##     ##  ##  ##   ###
-- ##        ######## ##     ## #### ##    ##

local plain =
{
  header = function( formatter, level, name )
    name = name and format( name, formatter ) or '?'

    local l1 = formatter.level1 or 0
    local l2 = formatter.level2 or 0
    local l3 = formatter.level3 or 0
    local section

    if level == 1 then
      l1 = l1 + 1
      formatter.level1 = l1
      formatter.level2 = 0
      formatter.level3 = 0
      section = '' .. l1
    elseif level == 2 then
      l2 = l2 + 1
      formatter.level2 = l2
      formatter.level3 = 0
      section = l1 .. '.' .. l2
    elseif level == 3 then
      l3 = l3 + 1
      formatter.level3 = l3
      section = l1 .. '.' .. l2 .. '.' .. l3
    else
      error( 'Invalid level ' .. level )
    end

    local sections = formatter.sections or {}
    formatter.sections = sections
    sections[ #sections + 1 ] = { level = level, l1 = l1, l2 = l2, l3 = l3, name = name }

    local res = section .. '. ' .. name
    return '+' .. ( '-' ):rep( 77 ) .. '+\n' .. '| ' .. res .. ( ' ' ):rep( 75 - res:len() ) .. ' |\n+' .. ( '-' ):rep( 77 ) .. '+\n\n'
  end,

  code = function( formatter, source )
    return source
  end,

  p = function( formatter, text, cols )
    text = format( text, formatter )
    cols = cols or 79
    local res = ''

    while #text > cols do
      local i = cols + 1

      while i > 1 and text:sub( i, i ) ~= ' ' do
        i = i - 1
      end

      res = res .. text:sub( 1, i - 1 ) .. '\n'
      text = text:sub( i + 1, -1 )
    end

    return res .. text .. '\n\n'
  end,

  source = function( formatter, language, code )
    return code .. '\n\n'
  end,

  ul = function( formatter, items )
    local res = ''

    for index, item in ipairs( items ) do
      local p = formatter.p( formatter, format( item, formatter ), 77 )
      local first = true

      for line in p:gmatch( '([^\n]+)\n?' ) do
        res = res .. ( first and '* ' or '  ' ) .. line .. '\n'
        first = false
      end
    end

    return res .. '\n'
  end,

  table = function( formatter, table )
    local rows, cols = #table - 2, 0

    for i = 3, #table do
      cols = math.max( cols, #table[ i ] )
    end

    local tbl, widths, width, freeWidth = {}, {}, 0

    for i = 3, #table do
      tbl[ i ] = { width = 4 + ( cols - 1 ) * 3, height = 1 }

      for j = 1, cols do
        tbl[ i ][ j ] = { attrs = {} }

        for _, attr in ipairs( table[ 2 ] ) do
          local i0 = attr[ 1 ]
          local j0 = attr[ 2 ]
          local i1 = attr[ 3 ]
          local j1 = attr[ 4 ]

          if ( i - 2 ) >= i0 and ( i - 2 ) <= i1 and j >= j0 and j <= j1 then
            for x, y in pairs( attr ) do
              if type( x ) == 'string' then
                local attrs = tbl[ i ][ j ].attrs
                attrs[ #attrs ] = { x = y }
                attrs[ x ] = y
                tbl[ i ][ j ].attrs = attrs
              end
            end
          end
        end

        local text = format( table[ i ][ j ] or '', formatter )
        tbl[ i ][ j ][ 1 ] = text

        if tbl[ i ][ j ].attrs.width then
          tbl[ i ][ j ].width = #text
          tbl[ i ][ j ].height = 1
          tbl[ i ].width = tbl[ i ].width + #text
          widths[ j ] = math.max( widths[ j ] or 0, #text )
        elseif i == 3 then
          if freeWidth then
            error( 'Table can only have one column width an unspecified width in PLAIN mode (' .. freeWidth .. ' and ' .. j .. ')' )
          end

          freeWidth = j
        end
      end

      width = math.max( width, tbl[ i ].width )
    end

    if freeWidth then
      local unspecifiedWidth = math.max( 20, 77 - width )
      widths[ freeWidth ] = unspecifiedWidth
      width = width + unspecifiedWidth + 2

      for i = 3, #table do
        tbl[ i ][ freeWidth ].width = unspecifiedWidth

        local p = formatter.p( formatter, tbl[ i ][ freeWidth ][ 1 ], unspecifiedWidth )
        local ndx = 1

        for line in p:gmatch( '([^\n]+)\n?' ) do
          tbl[ i ][ freeWidth ][ ndx ] = line
          ndx = ndx + 1
        end

        tbl[ i ].height = math.max( tbl[ i ].height, ndx - 1 )
      end
    end

    local title = format( table[ 1 ], formatter )

    local left, right = width - 4 - #title
    if left % 2 == 0 then
      left = left / 2
      right = left
    else
      left = math.floor( left / 2 )
      right = left + 1
    end

    local res = ( '-' ):rep( width ) .. '\n'
    res = res .. '| ' .. ( ' ' ):rep( left ) .. title .. ( ' ' ):rep( right ) .. ' |\n'
    res = res .. ( '-' ):rep( width ) .. '\n'

    for i = 3, #table do
      for h = 1, tbl[ i ].height do
        res = res .. '| '

        for j = 1, cols do
          local text = tbl[ i ][ j ][ h ] or ''

          if j ~= 1 then
            res = res .. ' | '
          end

          if tbl[ i ][ j ].attrs.align == 'center' then
            local left, right = widths[ j ] - #text
            if left % 2 == 0 then
              left = left / 2
              right = left
            else
              left = math.floor( left / 2 )
              right = left + 1
            end

            res = res .. ( ' ' ):rep( left ) .. text .. ( ' ' ):rep( right )
          elseif tbl[ i ][ j ].attrs.align == 'right' then
            res = res .. ( ' ' ):rep( widths[ j ] - #text ) .. text
          else
            res = res .. text .. ( ' ' ):rep( widths[ j ] - #text )
          end
        end

        res = res .. ' |\n'
      end

      res = res .. ( '-' ):rep( width ) .. '\n'
    end

    return res .. '\n'
  end,

  br = function( formatter )
    return '\n'
  end,

  note = function( formatter, text )
    return formatter.p( formatter, 'Note: ' .. text )
  end,

  pre = function( formatter, what )
    return what
  end,

  post = function( formatter, what )
    --table.sort( formatter.sections, function( k1, k2 ) return k1.key < k2.key end )
    local index = ''

    for _, section in ipairs( formatter.sections ) do
      local entry = ( ' ' ):rep( section.level * 2 - 2 )

      entry = entry .. section.l1

      if section.level >= 2 then
        entry = entry .. '.' .. section.l2

        if section.level == 3 then
          entry = entry .. '.' .. section.l3
        end
      end

      entry = entry .. '. ' .. section.name

      if section.level == 1 then
        entry = '\n' .. entry
      end

      index = index .. entry .. '\n'
    end

    local header = '+' .. ( '-' ):rep( 77 ) .. '+\n' .. '| Index' .. ( ' ' ):rep( 75 - 5 ) .. ' |\n+' .. ( '-' ):rep( 77 ) .. '+\n'

    return header .. index .. '\n' .. what:gsub( '\n\n', '\n' )
  end,
}

local function formatPlain( doc )
  return plain.post( plain, format( plain.pre( plain, doc ), plain ) )
end

-- ##     ## ######## ##     ## ##
-- ##     ##    ##    ###   ### ##
-- ##     ##    ##    #### #### ##
-- #########    ##    ## ### ## ##
-- ##     ##    ##    ##     ## ##
-- ##     ##    ##    ##     ## ##
-- ##     ##    ##    ##     ## ########

local html =
{
  header = function( formatter, level, name )
    name = name and format( name, formatter ) or '?'

    if level > 3 then
      error( 'Invalid level ' .. level )
    end

    local sections = formatter.sections or {}
    formatter.sections = sections
    sections[ #sections + 1 ] = { level = level, name = name }

    return ( level == 1 and '<hr/>' or '' ) .. '<h' .. level .. '>' .. name .. '</h' .. level .. '>\n\n'
  end,

  code = function( formatter, source )
    return '<code>' .. source .. '</code>'
  end,

  p = function( formatter, text )
    return '<p>' .. format( text, formatter ) .. '</p>\n\n'
  end,

  source = function( formatter, language, code )
    if language == 'DDL' then
      return '<code><pre>' .. highlight( code, ddlClasses ) .. '</pre></code>\n\n'
    elseif language == 'C' then
      return '<code><pre>' .. highlight( code, cClasses ) .. '</pre></code>\n\n'
    elseif language == 'EBF' then
      return '<code><pre>' .. highlight( code, ebfClasses ) .. '</pre></code>\n\n'
    end

    error( 'Invalid language ' .. language )
  end,

  ul = function( formatter, items )
    local res = '<ul>\n'

    for index, item in ipairs( items ) do
      res = res .. '<li>' .. format( item, formatter ) .. '</li>\n'
    end

    return res .. '</ul>\n\n'
  end,

  table = function( formatter, table )
    local title = table[ 1 ]
    local attrs = table[ 2 ]

    local rows, cols = 0, 0

    for i = 3, #table do
      local line = table[ i ]
      rows = rows + 1
      local c = 0

      for _, col in ipairs( line ) do
        c = c + 1
      end

      cols = math.max( cols, c )
    end

    local res = '<p><table border="1" width="80%" cellspacing="0" cellpadding="3">\n' ..
                '<tr><th colspan="' .. cols .. '" bgcolor="#e0e0e0">' .. format( title, formatter ) .. '</th></tr>\n'
    local td = 'th'

    for i = 3, #table do
      local line = table[ i ]
      res = res .. '<tr>'

      for j, col in ipairs( line ) do
        local a = ''

        for _, attr in ipairs( attrs ) do
          local i0 = attr[ 1 ]
          local j0 = attr[ 2 ]
          local i1 = attr[ 3 ]
          local j1 = attr[ 4 ]

          if ( i - 2 ) >= i0 and ( i - 2 ) <= i1 and j >= j0 and j <= j1 then
            for x, y in pairs( attr ) do
              if type( x ) ~= 'number' then
                a = a .. x .. '="' .. y .. '" '
              end
            end
          end
        end

        if #a == 0 then
          res = res .. '<' .. td .. '>' .. format( col, formatter ) .. '</' .. td .. '>'
        else
          res = res .. '<' .. td .. ' ' .. a:sub( 1, -2 ) .. '>' .. format( col, formatter ) .. '</' .. td .. '>'
        end
      end

      res = res .. '</tr>\n'
      td = 'td'
    end

    return res .. '</table></p>\n\n'
  end,

  br = function( formatter )
    return '<br/>\n'
  end,

  note = function( formatter, text )
    return formatter.p( formatter, 'Note: ' .. text )
  end,

  pre = function( formatter, what )
    return what
  end,

  post = function( formatter, what )
    return ( [[<html>
<head>
  <style type="text/css">
    h1 { font-size: 12pt; font-weight: bold; }
    h2 { font-size: 12pt; font-weight: bold; }
    h3 { font-size: 12pt; font-weight: bold; }
    body { font-size: 12pt; }
    code { font-size: 10pt; }
    p { margin-left:2em; }
    pre { margin-left: 2.5em; }
    ul { margin-left: 0.5em; }
    td { font-size: 12pt; }
    th { font-size: 12pt; font-weight: bold; }
  </style>
  <meta name="generator" content="C++2HTML by Jasper Bedaux" />
  <style type="text/css">
    .comment { color: #999999; font-style: italic; }
    .pre { color: #000099; }
    .string { color: #009900; }
    .char { color: #009900; }
    .float { color: #996600; }
    .int { color: #999900; }
    .bool { color: #000000; font-weight: bold; }
    .type { color: #FF6633; }
    .flow { color: #FF0000; }
    .keyword { color: #990000; }
    .operator { color: #663300; font-weight: bold; }
  </style>
</head>
<body>

]] .. what .. '\n</body></html>' ):gsub( '\n\n', '\n' )
  end,
}

local function formatHtml( doc )
  return html.post( html, format( html.pre( html, doc ), html ) )
end

-- ##     ##    ###    ########  ##    ## ########   #######  ##      ## ##    ##
-- ###   ###   ## ##   ##     ## ##   ##  ##     ## ##     ## ##  ##  ## ###   ##
-- #### ####  ##   ##  ##     ## ##  ##   ##     ## ##     ## ##  ##  ## ####  ##
-- ## ### ## ##     ## ########  #####    ##     ## ##     ## ##  ##  ## ## ## ##
-- ##     ## ######### ##   ##   ##  ##   ##     ## ##     ## ##  ##  ## ##  ####
-- ##     ## ##     ## ##    ##  ##   ##  ##     ## ##     ## ##  ##  ## ##   ###
-- ##     ## ##     ## ##     ## ##    ## ########   #######   ###  ###  ##    ##

local markdown =
{
  header = function( formatter, level, name )
    name = name and format( name, formatter ) or '?'

    local l1 = formatter.level1 or 0
    local l2 = formatter.level2 or 0
    local l3 = formatter.level3 or 0
    local section

    if level == 1 then
      l1 = l1 + 1
      formatter.level1 = l1
      formatter.level2 = 0
      formatter.level3 = 0
      section = '' .. l1
    elseif level == 2 then
      l2 = l2 + 1
      formatter.level2 = l2
      formatter.level3 = 0
      section = l1 .. '.' .. l2
    elseif level == 3 then
      l3 = l3 + 1
      formatter.level3 = l3
      section = l1 .. '.' .. l2 .. '.' .. l3
    else
      error( 'Invalid level ' .. level )
    end

    local sections = formatter.sections or {}
    formatter.sections = sections
    sections[ #sections + 1 ] = { level = level, l1 = l1, l2 = l2, l3 = l3, name = name }

    local res = section .. '. ' .. name
    return ( '#' ):rep( level ) .. ' ' .. res .. ' ' .. ( '#' ):rep( 79 - level - #res - 2 ) .. '\n\n'
  end,

  code = function( formatter, source )
    return '**' .. source:gsub( '\\', '\\\\' ):gsub( '*', '\\*' ):gsub( '_', '\\_' ) .. '**'
  end,

  p = function( formatter, text, cols )
    text = format( text, formatter )
    cols = cols or 79
    local res = ''

    while #text > cols do
      local i = cols + 1

      while i > 1 and text:sub( i, i ) ~= ' ' do
        i = i - 1
      end

      res = res .. text:sub( 1, i - 1 ) .. '\n'
      text = text:sub( i + 1, -1 )
    end

    return res .. text .. '\n\n'
  end,

  source = function( formatter, language, code )
    local res = ''

    for line in code:gmatch( '([^\n]+)\n?' ) do
      res = res .. '    ' .. line .. '\n'
    end

    return res .. '\n'
  end,

  ul = function( formatter, items )
    local res = ''

    for index, item in ipairs( items ) do
      local p = formatter.p( formatter, format( item, formatter ), 77 )
      local first = true

      for line in p:gmatch( '([^\n]+)\n?' ) do
        res = res .. ( first and '* ' or '  ' ) .. line .. '\n'
        first = false
      end
    end

    return res .. '\n'
  end,

  table = function( formatter, table )
    local res = '#### ' .. format( table[ 1 ], formatter ) .. '\n\n'

    for i = 4, #table do
      res = res .. '* '

      for j, col in ipairs( table[ i ] ) do
        res = res .. '**' .. format( table[ 3 ][ j ] or '', formatter ) .. '**: ' .. format( col or '', formatter ) .. ' '
      end

      res = res .. '\n'
    end

    return res .. '\n'
  end,

  table2 = function( formatter, table )
    local rows, cols = #table - 2, 0

    for i = 3, #table do
      cols = math.max( cols, #table[ i ] )
    end

    local tbl, widths, width, freeWidth = {}, {}, 0

    for i = 3, #table do
      tbl[ i ] = { width = 4 + ( cols - 1 ) * 3, height = 1 }

      for j = 1, cols do
        tbl[ i ][ j ] = { attrs = {} }

        for _, attr in ipairs( table[ 2 ] ) do
          local i0 = attr[ 1 ]
          local j0 = attr[ 2 ]
          local i1 = attr[ 3 ]
          local j1 = attr[ 4 ]

          if ( i - 2 ) >= i0 and ( i - 2 ) <= i1 and j >= j0 and j <= j1 then
            for x, y in pairs( attr ) do
              if type( x ) == 'string' then
                local attrs = tbl[ i ][ j ].attrs
                attrs[ #attrs ] = { x = y }
                attrs[ x ] = y
                tbl[ i ][ j ].attrs = attrs
              end
            end
          end
        end

        local text = format( table[ i ][ j ] or '', formatter )
        tbl[ i ][ j ][ 1 ] = text

        if tbl[ i ][ j ].attrs.width then
          tbl[ i ][ j ].width = #text
          tbl[ i ][ j ].height = 1
          tbl[ i ].width = tbl[ i ].width + #text
          widths[ j ] = math.max( widths[ j ] or 0, #text )
        elseif i == 3 then
          if freeWidth then
            error( 'Table can only have one column width an unspecified width in MARKDOWN mode (' .. freeWidth .. ' and ' .. j .. ')' )
          end

          freeWidth = j
        end
      end

      width = math.max( width, tbl[ i ].width )
    end

    if freeWidth then
      local unspecifiedWidth = math.max( 20, 72 - width )
      widths[ freeWidth ] = unspecifiedWidth
      width = width + unspecifiedWidth + 2

      for i = 3, #table do
        tbl[ i ][ freeWidth ].width = unspecifiedWidth

        local p = formatter.p( formatter, tbl[ i ][ freeWidth ][ 1 ], unspecifiedWidth )
        local ndx = 1

        for line in p:gmatch( '([^\n]+)\n?' ) do
          tbl[ i ][ freeWidth ][ ndx ] = line
          ndx = ndx + 1
        end

        tbl[ i ].height = math.max( tbl[ i ].height, ndx - 1 )
      end
    end

    local title = format( table[ 1 ], formatter )

    local left, right = width - 4 - #title
    if left % 2 == 0 then
      left = left / 2
      right = left
    else
      left = math.floor( left / 2 )
      right = left + 1
    end

    local res = '    ' .. ( '-' ):rep( width ) .. '\n'
    res = res .. '    | ' .. ( ' ' ):rep( left ) .. title .. ( ' ' ):rep( right ) .. ' |\n'
    res = res .. '    ' .. ( '-' ):rep( width ) .. '\n'

    for i = 3, #table do
      for h = 1, tbl[ i ].height do
        res = res .. '    | '

        for j = 1, cols do
          local text = tbl[ i ][ j ][ h ] or ''

          if j ~= 1 then
            res = res .. ' | '
          end

          if tbl[ i ][ j ].attrs.align == 'center' then
            local left, right = widths[ j ] - #text
            if left % 2 == 0 then
              left = left / 2
              right = left
            else
              left = math.floor( left / 2 )
              right = left + 1
            end

            res = res .. ( ' ' ):rep( left ) .. text .. ( ' ' ):rep( right )
          elseif tbl[ i ][ j ].attrs.align == 'right' then
            res = res .. ( ' ' ):rep( widths[ j ] - #text ) .. text
          else
            res = res .. text .. ( ' ' ):rep( widths[ j ] - #text )
          end
        end

        res = res .. ' |\n'
      end

      res = res .. '    ' .. ( '-' ):rep( width ) .. '\n'
    end

    return res .. '\n'
  end,

  br = function( formatter )
    return '  \n'
  end,

  note = function( formatter, text )
    return formatter.p( formatter, 'Note: ' .. text )
  end,

  pre = function( formatter, what )
    return what
  end,

  post = function( formatter, what )
    --table.sort( formatter.sections, function( k1, k2 ) return k1.key < k2.key end )
    local index = ''

    for _, section in ipairs( formatter.sections ) do
      local entry = ''

      entry = entry .. section.l1

      if section.level >= 2 then
        entry = entry .. '.' .. section.l2

        if section.level == 3 then
          entry = entry .. '.' .. section.l3
        end
      end

      entry = entry .. '. ' .. section.name

      if section.level == 1 then
        entry = '\n' .. entry
      end

      index = index .. entry .. '  \n'
    end

    local header = '# Index ' .. ( '#' ):rep( 79 - ( '# Index '):len() ) .. '\n\n'

    return header .. index .. '\n' .. what:gsub( '\n\n', '\n' )
  end,
}

local function formatMarkdown( doc )
  return markdown.post( markdown, format( markdown.pre( markdown, doc ), markdown ) )
end

if format_ == 'txt' then
  io.write( formatPlain( doc ) )
elseif format_ == 'html' then
  io.write( formatHtml( doc ) )
elseif format_ == 'md' then
  io.write( formatMarkdown( doc ) )
else
  error( 'Invalid format: ' .. format_ )
end
