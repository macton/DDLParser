local doc = [[
<html>
<head>
  <style type="text/css">
    h1 { font-size: 14pt; font-weight: bold; }
    h2 { font-size: 12pt; font-weight: bold; }
    p { margin-left:2em; }
    pre { margin-left: 2.5em; }
    ul { margin-left: 0.5em; }
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

<h1><section name="DDL Specification"></section></h1>

<p>DDL files can contain three constructs: selects, bitfields and structures. All constructs must have a name, and they can have an author, a description and a label assigned to them. These fields can be used by tools when rendering the constructs as UI elements.</p>

<p>There is a annotated DDL grammar at the end of this document for further reference.</p>

<hr/><h1><section name="Selects"></section></h2>

<p>A select is just a list of options much like an C enum. They represent exclusive values, like a drop-down where just one value can be selected. But unlike C enums, numeric values cannot be assigned to select fields; their values are just the hash of their names. This way new fields can be added without constraints while maintaining compatibility with cold that uses the select.</p>

<p>Each select item can have an author, a description and a label.</p>

<h2>Example</h2>

<source>select Weapon, author( "Andre de Leiradella" ), description( "Weapons available to the player" ), label( "Weapon" )
{
  kFist,           description( "Bare hands" ),               label( "Fist" );
  kChainsaw,       description( "A la Chainsaw Massacre" ),   label( "Chainsaw" );
  kPistol,         description( "Simple pistol" ),            label( "Pistol" ), default;
  kShotgun,        description( "A single-barrel shotgun" ),  label( "Shotgun" );
  kChaingun,       description( "A machine gun" ),            label( "Chaingun" );
  kRocketLauncher, description( "Portable rocket launcher" ), label( "Rocket launcher" );
  kPlasmaGun,      description( "Plasma gun" ),               label( "Plasma gun" );
  kBFG9000,        description( "*The* BFG 9000" ),           label( "BFG 9000" );
}</source>

<p>An item can be marked as default (see item <code>kPistol</code>) and it will be the item used in fields declared with the select type that don't have an explicit value to override the default.</p>

<hr/><h1><section name="Bitfields"></section></h1>

<p>Bitfields represent a set of related flags. Their flags can be or'ed to form the value of another flag. Bitfields should be used where more than one value can be selected, like a checkbox group.</p>

<p>As with select items, each bitfield flag can have an author, a description and a label.</p>

<h2>Example</h2>

<source>bitfield Powerup, author( "Andre de Leiradella" ), description( "Powerup pickups" ), label( "Powerup" )
{
  kNone,                description( "Help me!" ), empty;
  kRadiationSuit,       description( "Makes the player immune to radiation for a limited time" ),             label( "Radiation suit" );
  kPartialInvisibility, description( "Makes the player almost invisible to enemies for a limited time" ),     label( "Partial invisibility" );
  kInvulnerability,     description( "Makes the player invulnerable for a limited time" ),                    label( "Invulnerability" );
  kComputerMap,         description( "Gives the complete map of the level to the player" ),                   label( "Computer map" );
  kLightVisor,          description( "Allows the player to see in the dark for a limited time" ),             label( "Light visor" );
  kBerserk,             description( "Gives the player the ability to quickly kill enemies with his fists" ), label( "Berserk" );
  kAll,                 value( kRadiationSuit | kPartialInvisibility | kInvulnerability | kComputerMap | kLightVisor | kBerserk );
}</source>

<p>Flags without a value will get an unique bit index that can be queried at runtime. Flags can be marked as default. If there isn't a default flag in the bitfield the empty flag will be used, or the first declared flag will be used if there isn't an empty flag. To get a zeroed flag, tag it with <code>empty</code> like the <code>kNone</code> flag above.</p>

<hr/><h1><section name="Structures"></section></h1>

<p>While selects and bitfields only define custom types and take no memory, structures are aggregates of fields which actually take up memory to hold their fields' values.</p>

<p>Structure fields can be of the following types:</p>

<ul>
<li><code>u8</code> (or <code>uint8_t</code>)</li>
<li><code>u16</code> (or <code>uint16_t</code>)</li>
<li><code>u32</code> (or <code>uint32_t</code>)</li>
<li><code>u64</code> (or <code>uint64_t</code>)</li>
<li><code>i8</code> (or <code>int8_t</code>)</li>
<li><code>i16</code> (or <code>int16_t</code>)</li>
<li><code>i32</code> (or <code>int32_t</code>)</li>
<li><code>i64</code> (or <code>int64_t</code>)</li>
<li><code>f32</code> (or <code>float</code>)</li>
<li><code>f64</code> (or <code>double</code>)</li>
<li><code>boolean</code></li>
<li><code>string</code></li>
<li><code>file</code></li>
<li><code>tuid</code></li>
<li><code>json</code></li>
<li>Any previously declared select, bitfield or structure</li>
</ul>

<p>Fields can also be arrays of any of these types by just adding <code>[]</code> right after the type name. These arrays cannot take default values, and can grow as needed at runtime. Adding a <code>[ x ]</code>, where <code>x</code> is an integer expression, makes the array fixed and up to <code>x</code> default values can be specified for the field. Just like select items and bitfield flags, structure fields can have an author, a description and a label.</p>

<p>To inform the default value of a field, use the <code>value</code> keyword and write any expression inside the parenthesis. The expression must evaluate to a value which fits in the data type of the field.</p>

<h2>Examples</h2>

<source>struct Position
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
}</source>

<p>Array values must be written inside braces with commas separating each expression. Values of structured fields, like <code>m_Position</code> above, must also appear inside braces. It's not mandatory that all fields are present in the value.</p>

<hr/><h1><section name="Expressions"></section></h1>

<p>DDL expressions are very similar to C expressions, and care has been taken to implement the same operators with the same precedence order. The operators, in decreasing order of precedence, are:</p>

<ul>
<li><code>?</code> (ternary operator)</li>
<li><code>||</code> (logic or)</li>
<li><code>&&</code> (logic and)</li>
<li><code>|</code> (arithmetic or)</li>
<li><code>^</code> (arithmetic xor)</li>
<li><code>&</code> (arithmetic and)</li>
<li><code>==</code> and <code>!=</code> (equality)</li>
<li><code>&lt;</code>, <code>&lt;=</code>, <code>&gt;</code> and <code>&gt;=</code> (comparison)</li>
<li><code>&lt;&lt;</code> and <code>&gt;&gt;</code> (shift)</li>
<li><code>+</code> and <code>-</code> (add and subtract)</li>
<li><code>*</code>, <code>/</code> and <code>%</code> (multiply, divide and modulus)</li>
<li><code>+</code>, <code>-</code>, <code>~</code> and <code>!</code> (unary positive, negative, complement and logic not)</li>
</ul>

<p>Terminals can be binary, octal, decimal and hexadecimal integer constants, floating point constants, string literals, the <code>true</code> and <code>false</code> constants, <code>pi</code>, the natural number <code>e</code>, and expressions between parenthesis.</p>

<p>Binary constants are written similarly to hexadecimal constants but using the <code>0b</code> prefix instead of <code>0x</code>. String literals are specified inside single or double quotes, and any occurrence of <code>%xx</code> in the string is substituted by the character with hexadecimal value <code>xx</code>.</p>

<p>The constants <code>true</code> and <code>false</code> evaluates to the integers 1 and 0 respectively.</p>

<hr/><h1><section name="Comments"></section></h1>

<p>DDL files can have C and C++ style comments:</p>

<ul>
<li><code>/* this is a comment */</code></li>
<li><code>// this is a comment that ends at the end of the line</code></li>
</ul>

<hr/><h1><section name="DDL API"></section></h1>

<p>The DDL API are a collection of functions and structures that allows for DDL source code compilation and subsequent querying of the components found in the source file for i.e. code generation.</p>

<h2>Common methods</h2>

<p>Many structures present a set of common methods that are explained here.</p>

<section name="GetName"><code>const char* GetName() const</code></section>

<p>Returns the name of the component, i.e. a select's name or a structure's field's name.</p>

<section name="GetAuthor"><code>const char* GetAuthor() const</code></section>

<p>If the component has been tagged with the <code>author</code> tag, <code>GetAuthor</code> returns the contents of the tag, otherwise it returns NULL. This tag can be used to document the person who authored the component.</p>

<section name="GetDescription"><code>const char* GetDescription() const</code></section>

<p>If the component has been tagged with the <code>description</code> tag, <code>GetDescription</code> returns the contents of the tag, otherwise it returns NULL. This tag can be used to give the component a verbose description.</p>

<section name="GetLabel"><code>const char* GetLabel() const</code></section>

<p>If the component has been tagged with the <code>label</code> tag, <code>GetLabel</code> returns the contents of the tag, otherwise it returns NULL. This tag can be used to give the component a label to be used in the user interface, for example.</p>

<section name="GetDisplayLabel"><code>const char* GetDisplayLabel() const</code></section>

<p>Returns the contents of the <code>label</code> tag if it has been defined and is not the empty string. Otherwise it returns the name of the component.</p>

<section name="GetNameHash"><code>uint32_t GetNameHash() const</code></section>

<p>Returns the hash of the components's name. The hash is created with DDLParser::StringCrc32.</p>

<section name="GetTags"><code>DDLParser::Tag* GetTags() const</code></section>

<p>Returns the first tag defined for the component, or NULL if the component has no tags defined.</h2>

<section name = "GetTag"><code>DDLParser::Tag* GetTag( uint32_t type ) const</code></section>

<p>Finds and returns a tag by its type. Alternatively, type can be the hash of the generic tag to be searched for. Either way, if the tag isn't found NULL is returned.</p>

<hr/><h1><section name="DDLParser::Compile"></section></h1>

<p><code>DDLParser::Definition* DDLParser::Compile( DDLParser::LinearAllocator* definition, DDLParser::LinearAllocator* scratch, const void* source, size_t source_size, char* error, size_t error_size, bool two_us_reserved, int bitfield_limit )</code></p>

<p>The DDLParser::Compile function compiles a DDL source file into a DDLParser::Definition ready to be used to generate code.</p>

<h2>Parameters</h2>

<ul>
<li><code>definition</code>: A DDLParser::LinearAllocator where the definition is assembled. When DDLParser::Compile returns, this allocator will have the complete definition.</li>
<li><code>scratch</code>: A DDLParser::LinearAllocator used for temporary work. Can be discarded after DDLParser::Compile returns.</li>
<li><code>source</code>: A pointer to the DDL source code. This buffer doesn't have to be null-terminated and cannot be NULL.</li>
<li><code>source_size</code>: The size of the <code>source</code> buffer.</li>
<li><code>error</code>: A pointer to a buffer where the error message, if there's one, will be written to. Cannot be NULL.</li>
<li><code>error_size</code>: The size of the <code>error</code> buffer.</li>
<li><code>two_us_reserved</code>: If true, identifiers starting with two underlines are reserved and disallowed in the source code.</li>
<li><code>bitfield_limit</code>: The maximum number of flags allowed in a bitfield. If this value is zero, there is no limit on the number of flags per bitfield.</li>
</ul>

<h2>Return value</h2>

<p>If the parse succeeds it returns the definition assembled in <code>definition</code> casted to a DDLParser::Definition pointer. If there was an error, DDLParser::Compile returns NULL and <code>error</code> is filled with the error message.</p>

<hr/><h1><section name="DDLParser::StringCrc32"></section></h1>

<p><code>uint32_t DDLParser::StringCrc32( const char* data )</code></p>

<p>Returns the CRC32 of the string.</p>

<hr/><h1><section name="DDLParser::Definition"></section></h1>

<p>After a successful parsing, DDLParser::Compile returns a DDLParser::Definition, which is used as the starting point to query all selects, bitfields, and structures, defined in the source code.</p>

<p>DDLParser::Definition has the following methods:</p>

<section name="DDLParser::Definition::GetNumAggregates"><code>uint32_t DDLParser::Definition::GetNumAggregates() const</code></section>

<p>Returns the number of aggregates (selects, bitfields and structures) in the definition.</p>

<section name="DDLParser::Definition::GetTotalSize"><code>uint32_t DDLParser::Definition::GetTotalSize() const</code></section>

<p>Returns the total size in bytes of the definition. This can be used if you want to write it into a file for example.</p>

<section name="DDLParser::Definition::FromBinRep"><code>static DDLParser::Definition* DDLParser::Definition::FromBinRep( void* bin_rep )</code></section>

<p>Just returns the <code>bin_rep</code> pointer casted to a DDLParser::Definition.</p>

<section name="DDLParser::Definition::GetAggregate"><code>DDLParser::Aggregate* DDLParser::Definition::GetAggregate( unsigned int index )</code></section>

<p>Returns an aggregate given its index. Aggregates are saved into the definition in the order they appear in the source code. No bounds check is done with <code>index</code>.</p>

<section name="DDLParser::Definition::FindAggregate"><code>DDLParser::Aggregate* DDLParser::Definition::FindAggregate( const char* name )</code></section>

<p>Finds and returns an aggregate by name. If the aggregate does not exist, NULL is returned.</p>

<section name="DDLParser::Definition::FindAggregate/hash"><code>DDLParser::Aggregate* DDLParser::Definition::FindAggregate( uint32_t hash )</code></section>

<p>Finds and returns an aggregate by the hash of its name. If the aggregate does not exist, NULL is returned. The hash must be created with DDLParser::StringCrc32.</p>

<hr/><h1><section name="DDLParser::Aggregate"></section></h1>

<p>An aggregate holds information common to selects, bitfields, and structures. It has the following common methods: <code>GetName</code>, <code>GetAuthor</code>, <code>GetDescription</code>, <code>GetLabel</code>, <code>GetDisplayLabel</code>, and <code>GetNameHash</code>.</p>

<section name="DDLParser::Aggregate::GetType"><code>uint32_t DDLParser::Aggregate::GetType() const</code></section>

<p>Returns the type of the aggregate, DDLParser::kSelect, DDLParser::kBitfield, or DDLParser::kStruct.</p>

<section name="DDLParser::Aggregate::ToSelect"><code>DDLParser::Select* DDLParser::Aggregate::ToSelect() const</code></section>

<p>Returns the aggregate casted to DDLParser::Select without checking for the actual aggregate type.</p>

<section name="DDLParser::Aggregate::ToBitfield"><code>DDLParser::Bitfield* DDLParser::Aggregate::ToBitfield() const</code></section>

<p>Returns the aggregate casted to DDLParser::Bitfield without checking for the actual aggregate type.</p>

<section name="DDLParser::Aggregate::ToStruct"><code>DDLParser::Struct* DDLParser::Aggregate::ToStruct() const</code></section>

<p>Returns the aggregate casted to DDLParser::Struct without checking for the actual aggregate type.</p>

<hr/><h1><section name="DDLParser::Select"></section></h1>

<p>A select is like a C enum, but you can't define the value of the items. Their value is always the hash code of their identifiers. A select holds information about its items.</p>

<p>In addition to the methods listed here, selects have the following common methods: <code>GetName</code>, <code>GetAuthor</code>, <code>GetDescription</code>, <code>GetLabel</code>, <code>GetDisplayLabel</code>, <code>GetNameHash</code>, <code>GetTags</code>, and <code>GetTag</code>.</p>

<section name="DDLParser::Select::GetNumItems"><code>uint32_t DDLParser::Select::GetNumItems() const</code></section>

<p>Returns the number of items contained in the select.</p>

<section name="DDLParser::Select::GetDefaultItem"><code>int32_t DDLParser::Select::GetDefaultItem() const</code></section>

<p>Returns the index of the item defined as the default item for the select. If no item was defined as the default, the default item is the first defined. This can be used to initialize variables to a default value if none is given for example.</p>

<section name="DDLParser::Select::FindItem"><code>DDLParser::SelectItem* DDLParser::Select::FindItem( const char* name )</code></section>

<p>Finds and returns an item by name. If the item does not exist, NULL is returned.</p>

<section name="DDLParser::Select::FindItem/hash"><code>DDLParser::SelectItem* DDLParser::Select::FindItem( uint32_t hash )</code></section>

<p>Finds and returns an item by the hash of its name. If the item does not exist, NULL is returned. The hash must be created with DDLParser::StringCrc32.</p>

<hr/><h1><section name="DDLParser::SelectItem"></section></h1>

<p>A select item represents an item declared in a select and has the following common methods: <code>GetName</code>, <code>GetAuthor</code>, <code>GetDescription</code>, <code>GetLabel</code>, <code>GetDisplayLabel</code>, <code>GetNameHash</code>, <code>GetTags</code>, and <code>GetTag</code>.</p>

<p>Select items doesn't have any particular method besides the common ones listed above.</p>

<hr/><h1><section name="DDLParser::Bitfield"></section></h1>

<p>A bitfield is a set, and bitfield flags are the set's items. Unlike select items, a bitfield flag has a value associated to it. This value can be an explicit zero, an unique, automatically assigned value or a set which is the union of other flags. If the value is automatically assigned, it starts with one and is incremented for each assignment.</p>

<p>In addition to the methods listed here, bitfields have the following common methods: <code>GetName</code>, <code>GetAuthor</code>, <code>GetDescription</code>, <code>GetLabel</code>, <code>GetDisplayLabel</code>, <code>GetNameHash</code>, <code>GetTags</code>, and <code>GetTag</code>.</p>

<section name="DDLParser::Bitfield::GetNumFlags"><code>uint32_t DDLParser::Bitfield::GetNumFlags() const</code></section>

<p>Returns the number of flags contained in the bitfield.</p>

<section name="DDLParser::Bitfield::GetDefaultFlag"><code>int32_t DDLParser::Bitfield::GetDefaultFlag() const</code></section>

<p>Returns the index of the flag defined as the default flag for the bitfield. If no flag was defined as the default, the default item is the empty flag. If an empty flag wasn't declared, The first flag is elected the default. This can be used to initialize variables to a default value if none is given for example.</p>

<section name="DDLParser::Bitfield::FindFlag"><code>DDLParser::BitfieldFlag* DDLParser::Bitfield::FindFlag( const char* name )</code></section>

<p>Finds and returns a flag by name. If the flag does not exist, NULL is returned.</p>

<section name="DDLParser::Bitfield::FindFlag/hash"><code>DDLParser::BitfieldFlag* DDLParser::Bitfield::FindFlag( uint32_t hash )</code></section>

<p>Finds and returns a flag by the hash of its name. If the flag does not exist, NULL is returned. The hash must be created with DDLParser::StringCrc32.</p>

<hr/><h1><section name="DDLParser::BitfieldFlag"></section></h1>

<p>A flag is a unique element in a set, or a set of other flags. A flag has the following common methods: <code>GetName</code>, <code>GetAuthor</code>, <code>GetDescription</code>, <code>GetLabel</code>, <code>GetDisplayLabel</code>, <code>GetNameHash</code>, <code>GetTags</code>, and <code>GetTag</code>.</p>

<p>Flags also have the following unique methods:</p>

<section name="DDLParser::BitfieldFlag::GetValue"><code>DDLParser::BitfieldFlagValue* DDLParser::BitfieldFlag::GetValue() const</code></section>

<p>Returns the value of the flag in case it's the empty flag or a set. If neither is true, this function returns NULL.</p>

<section name="DDLParser::BitfieldFlag::GetBit"><code>uint32_t DDLParser::BitfieldFlag::GetBit() const</code></section>

<p>If the flag is the empty flag, or if it's a set, this functions returns zero. Otherwise, it returns an automatically assigned value for the flag which is unique in the bitfield. This values starts at one and is incremented for each flag that uses it. It can be used to generate code for flags as bits in an integer, i.e. <code>1 &lt;&lt; ( flag->GetBit() - 1 )</code>.</p>

<hr/><h1><section name="DDLParser::BitfieldFlagValue"></section></h1>

<p>Defines the value of a flag when it's either the empty flag or a set.</p>

<section name="DDLParser::BitfieldFlagValue::GetCount"><code>uint32_t DDLParser::BitfieldFlagValue::GetCount() const</code></section>

<p>If the flag is the empty flag, this function returns zero. Otherwise, it returns the number of flags contained in the flag.</p>

<section name="DDLParser::BitfieldFlagValue::GetFlagIndex"><code>uint32_t DDLParser::BitfieldFlagValue::GetFlagIndex( uint32_t index ) const</code></section>

<p>Returns the index of the flag which is part of this set.</p>

<h2>Bitfield example</h2>

<p>As an example, this code generates C++ code that outputs a bitfield's flags as bits in an unsigned integer:</p>

<source>static void GenerateBitfield( DDLParser::Bitfield* bf )
{
  printf( "namespace %s\n", bf->GetName() );
  printf( "{\n" );

  for ( uint32_t i = 0; i < bf->GetNumFlags(); i++ )
  {
    DDLParser::BitfieldFlag* flag = bf->GetFlag( i );
    DDLParser::BitfieldFlagValue* val = flag->GetValue();

    printf( "  static const uint32_t %-20s = ", flag->GetName() );

    if ( val == NULL )
    {
      // The flag has an automatically assigned value.
      printf( "0x%08x;\n", 1 << ( flag->GetBit() - 1 ) );
    }
    else
    {
      if ( val->GetCount() == 0 )
      {
        // The flag is the empty flag.
        printf( "0x%08x;\n", 0 );
      }
      else
      {
        // The flag is a set containing other flags.
        DDLParser::BitfieldFlag* other = bf->GetFlag( 0 );
        printf( "%s", other->GetName() );

        for ( uint32_t j = 1; j < val->GetCount(); j++ )
        {
          other = bf->GetFlag( val->GetFlagIndex( j ) );
          printf( " | %s", other->GetName() );
        }

        printf( ";\n" );
      }
    }
  }

  printf( "}\n" );
}</source>

<hr/><h1><section name="DDLParser::Struct"></section></h1>

<p>Structures are collections of fields. Each field has its own type, which can be a native type, a select, a bitfield, another structure, a fixed or dynamic array of the previous types, or a hashmap mapping an integer, a string, a file, or a tuid to any other type.</p>

<p>In addition to the methods listed here, structures have the following common methods: <code>GetName</code>, <code>GetAuthor</code>, <code>GetDescription</code>, <code>GetLabel</code>, <code>GetDisplayLabel</code>, <code>GetNameHash</code>, <code>GetTags</code>, and <code>GetTag</code>.</p>

<section name="DDLParser::Struct::GetNumFields"><code>uint32_t DDLParser::Struct::GetNumFields() const</code></section>

<p>Returns the number of fields contained in the structure.</p>

<section name="DDLParser::Struct::GetParent"><code>DDLParser::Struct* DDLParser::Struct::GetParent() const</code></section>

<p>Returns the parent structure of this structure. If the structure doesn't inherit from another structure, it returns NULL.</p>

<section name="DDLParser::Struct::GetDefinition"><code>DDLParser::Definition* DDLParser::Struct::GetDefinition() const</code></section>

<p>Returns the definition where the structure was defined.</p>

<section name="DDLParser::Struct::FindField"><code>DDLParser::StructField* DDLParser::Struct::FindField( const char* name )</code></section>

<p>Finds and returns a field by name. If the field does not exist, NULL is returned.</p>

<section name="DDLParser::Struct::FindField/hash"><code>DDLParser::StructField* DDLParser::Struct::FindField( uint32_t hash )</code></section>

<p>Finds and returns a field by the hash of its name. If the field does not exist, NULL is returned. The hash must be created with DDLParser::StringCrc32.</p>

<section name="DDLParser::Struct::IsInherited"><code>bool DDLParser::Struct::IsInherited( DDLParser::StructField* field )</code></section>

<p>Returns <code>true</code> if the field is from a parent structure.</p>

<section name="DDLParser::Struct::GetSchemaCrc"><code>uint32_t DDLParser::Struct::GetSchemaCrc() const</code></section>

<p>The schema crc reflects the type, count, name, and order of each field. If any of those change, then the crc changes. It does not reflect default values or any tags.</p>

<hr/><h1><section name="DDLParser::StructField"></section></h1>

<p>A field is an instance of a given type. A field has these common methods: <code>GetName</code>, <code>GetAuthor</code>, <code>GetDescription</code>, <code>GetLabel</code>, <code>GetDisplayLabel</code>, <code>GetNameHash</code>, <code>GetTags</code>, and <code>GetTag</code>.</p>

<p>Fields also have the following unique method.</p>

<section name="DDLParser::StructField::GetValueInfo"><code>DDLParser::StructValueInfo* DDLParser::StructField::GetValueInfo()</code></section>

<p>Returns the value information of the field. A field always have a DDLParser::StructField::GetValueInfo so this method never returns NULL.</p>

<hr/><h1><section name="DDLParser::StructValueInfo"></section></h1>

<p>A DDLParser::StructValueInfo holds information for both a field declared in a structure and its default values if the field has a structure for its type. It has the the <code>GetTags</code> and <code>GetTag</code> common methods.</p>

<section name="DDLParser::StructValueInfo::GetNameHash"><code>uint32_t DDLParser::StructValueInfo::GetNameHash() const</code></section>

<p>Returns the hash of the field's name.</p>

<section name="DDLParser::StructValueInfo::GetType"><code>Type DDLParser::StructValueInfo::GetType() const</code></section>

<p>Returns the type of the field which is a value from the DDLParser::Type enumeration.</p>

<section name="DDLParser::StructValueInfo::GetTypeNameHash"><code>uint32_t DDLParser::StructValueInfo::GetTypeNameHash() const</code></section>

<p>Returns the hash of the field's type name. Only usefull if the field has a select, bitfield or structure type so it's possible to query the definition for the actual aggregate.</p>

<section name="DDLParser::StructValueInfo::GetTypeName"><code>const char* DDLParser::StructValueInfo::GetTypeName() const</code></section>

<p>Deprecated.</p>

<section name="DDLParser::StructValueInfo::GetArrayType"><code>ArrayType DDLParser::StructValueInfo::GetArrayType() const</code></section>

<p>Returns the array type of the field. Possible values are DDLParser::kScalar (the field is not an array), DDLParser::kFixed (the field is an array with a fixed dimension), DDLParser::kDynamic (the field is an array without an specified dimention) and DDLParser::kHashmap (the field is a hashmap), from the DDLParser::ArrayType enumeration.</p>

<section name="DDLParser::StructValueInfo::GetValue"><code>DDLParser::StructFieldValue* DDLParser::StructValueInfo::GetValue() const</code></section>

<p>Returns the array of default values for the field. If the field hasn't a default value, it returns NULL. Only scalar and fixed array types of fields can have a default value. In the former, only the first position of the array of values is valid. In the later, the array of values is valid from 0 to DDLParser::StructValueInfo::GetCount.</p>

<section name="DDLParser::StructValueInfo::GetCount">uint32_t DDLParser::StructValueInfo::GetCount() const</code></section>

<p>Returns the size of the array if the array is fixed. Always returns one for scalars and zero for dynamic arrays and hashmaps.</p>

<section name="DDLParser::StructValueInfo::GetKeyType"><code>uint32_t DDLParser::StructValueInfo::GetKeyType() const</code></section>

<p>Returns the DDL type of a hashmap key. If the field is not a hashmap, this method doesn't return anything meaningful.</p>

<section name="DDLParser::StructValueInfo::GetKeyBitSize"><code>int DDLParser::StructValueInfo::GetKeyBitSize() const</code></section>

<p>Convenience method that returns the bit size of the hashmap key. Returns 64 for DDLParser::kInt64, DDLParser::kUint64, DDLParser::kFloat64, and DDLParser::kTuid, and 32 for all other types.</p>

<section name="DDLParser::StructValueInfo::AllowSubStruct"><code>bool DDLParser::StructValueInfo::AllowSubStruct() const</code></section>

<p>Convenience method that returns <code>true</code> if the field was tagged with the <code>tag( AllowSubstruct )</code> generic tag.</p>

<section name="DDLParser::StructValueInfo::GetSchemaCrc"><code>uint32_t DDLParser::StructValueInfo::GetSchemaCrc() const</code></section>

<p>The schema crc reflects the type, count, and name of the field. If any of those change, then the crc changes. It does not reflect default values or any tags.</p>

<section name="DDLParser::StructValueInfo::GetAggregate"><code>DDLParser::Aggregate* DDLParser::StructValueInfo::GetAggregate( Definition* ddl ) const</code></section>

<p>Returns the aggregate which is the type of the field. If the field has not a select, bitfield or structure as its type, this method returns NULL.</p>

<hr/><h1><section name="DDLParser::StructFieldValue"></section></h1>

<p>DDLParser::StructFieldValue is an union which holds the values defined for fields.</p>

<source>union DDLParser::StructFieldValue
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
};</source>

<p>When a field's DDLParser::StructValueInfo has a pointer to a DDLParser::StructFieldValue (via DDLParser::StructValueInfo::GetValue), it means the field has default values. The member of the union that is to be accessed depends on DDLParser::StructValueInfo::GetType, and how many entries are valid depends on the field's array type. For DDLParser::kScalar fields, one position only is valid. For DDLParser::kFixed fields, positions from zero to DDLParser::StructValueInfo::GetCount<code> - 1</code> are valid. DDLParser::kDynamic and DDLParser::kHashmap fields don't have default values.</p>

<hr/><h1><section name="DDLParser::StructBitfieldValue"></section></h1>

<p>When a field is of a bitfield type and has a default value, this value is held in the <code>m_Bitfield</code> member of the DDLParser::StructFieldValue union. It has the following methods:</p>

<h2><section name="DDLParser::StructBitfieldValue::GetCount"><code>uint32_t DDLParser::StructBitfieldValue::GetCount() const</code></section></h2>

<p>Returns the number of flags that make the default value of the field.</p>

<h2><section name="DDLParser::StructBitfieldValue::GetHash"><code>uint32_t DDLParser::StructBitfieldValue::GetHash( uint32_t index ) const</code></section></h2>

<p>Returns the hash of the flag at the given index. This hash can be used to get the actual flag from the bitfield via DDLParser::Bitfield::FindFlag.</p>

<hr/><h1><section name="DDLParser::StructStructValue"></section></h1>

<p>When a field is of a structure type and has a default value, it's default value is held in the <code>m_Struct</code> member of the DDLParser::StructFieldValue union. For each field receiving a default value, DDLParser::StructStructValue has a DDLParser::StructValueInfo with details of which field is being initialized and what value it's receiving.</p>

<p>DDLParser::StructStructValue has the following methods:</p>

<h2><section name="DDLParser::StructStructValue::GetCount"><code>uint32_t DDLParser::StructStructValue::GetCount() const</code></section></h2>

<p>Returns the number of value infos in this structure value.</p>

<h2><section name="DDLParser::StructStructValue::GetValueInfo"><code>DDLParser::StructValueInfo* DDLParser::StructStructValue::GetValueInfo( uint32_t index ) const</code></section></h2>

<p>Returns the value info at the given index.</p>

<h1>Example</h1>

<source>struct A
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
}</source>

<p>The DDLParser::StructValueInfo for the fields declared in the DDL code above are:</p>

<p><table border="1" width="80%" cellspacing="0" cellpadding="3">
<tr><th colspan="3" bgcolor="#e0e0e0"><code>A.a</code></th></tr>
<tr><th width="15%">Method</th><th width="10%">Return Value</th><th width="55%">Meaning</th></tr>
<tr><td>GetNameHash()</td><td align="right">0x0136c985</td><td>DDLParser::StringCrc32 of "a", the field name.</td></tr>
<tr><td>GetType()</td><td align="right">2</td><td>DDLParser::kUint32.</td></tr>
<tr><td>GetTypeNameHash()</td><td align="right">0x0d5d2ca7</td><td>DDLParser::StringCrc32 of "uint32_t", but meaningless.</td></tr>
<tr><td>GetArrayType()</td><td align="right">0</td><td>DDLParser::kScalar.</td></tr>
<tr><td>GetValue()</td><td align="right">0x009d0074</td><td>The address of the DDLParser::StructFieldValue, since this field has a default value defined for it. This DDLParser::StructFieldValue has the value 1 at <code>m_Uint32[ 0 ]</code>.</td></tr>
<tr><td>GetTags()</td><td align="right">0x00000000</td><td>NULL since this field doesn't have any tag.</td></tr>
<tr><td>GetKeyType()</td><td align="right">0</td><td>Meaningless.</td></tr>
<tr><td>GetKeyBitSize()</td><td align="right">32</td><td>Meaningless.</td></tr>
<tr><td>GetAggregate()</td><td align="right">0x00000000</td><td>NULL since this field has a scalar type.</td></tr>
<tr><td>GetCount()</td><td align="right">1</td><td>This field holds one <code>uint32_t</code> value.</td></tr>
</table></p>

<p><table border="1" width="80%" cellspacing="0" cellpadding="3">
<tr><th colspan="3" bgcolor="#e0e0e0"><code>A.b</code></th></tr>
<tr><th width="15%">Method</th><th width="10%">Return Value</th><th width="55%">Meaning</th></tr>
<tr><td>GetNameHash()</td><td align="right">0x983f983f</td><td>DDLParser::StringCrc32 of "a", the field name.</td></tr>
<tr><td>GetType()</td><td align="right">2</td><td>DDLParser::kUint32.</td></tr>
<tr><td>GetTypeNameHash()</td><td align="right">0x0d5d2ca7</td><td>DDLParser::StringCrc32 of "uint32_t", but meaningless.</td></tr>
<tr><td>GetArrayType()</td><td align="right">0</td><td>DDLParser::kScalar.</td></tr>
<tr><td>GetValue()</td><td align="right">0x00000000</td><td>NULL since this field hasn't a default value defined for it.</td></tr>
<tr><td>GetTags()</td><td align="right">0x00000000</td><td>NULL since this field doesn't have any tag.</td></tr>
<tr><td>GetKeyType()</td><td align="right">0</td><td>Meaningless.</td></tr>
<tr><td>GetKeyBitSize()</td><td align="right">32</td><td>Meaningless.</td></tr>
<tr><td>GetAggregate()</td><td align="right">0x00000000</td><td>NULL since this field has a scalar type.</td></tr>
<tr><td>GetCount()</td><td align="right">1</td><td>This field holds one <code>uint32_t</code> value.</td></tr>
</table></p>

<p><table border="1" width="80%" cellspacing="0" cellpadding="3">
<tr><th colspan="3" bgcolor="#e0e0e0"><code>B.c</code></th></tr>
<tr><th width="15%">Method</th><th width="10%">Return Value</th><th width="55%">Meaning</th></tr>
<tr><td>GetNameHash()</td><td align="right">0xef38a8a9</td><td>DDLParser::StringCrc32 of "c", the field name.</td></tr>
<tr><td>GetType()</td><td align="right">2</td><td>DDLParser::kUint32.</td></tr>
<tr><td>GetTypeNameHash()</td><td align="right">0x0d5d2ca7</td><td>DDLParser::StringCrc32 of "uint32_t", but meaningless.</td></tr>
<tr><td>GetArrayType()</td><td align="right">1</td><td>DDLParser::kFixed.</td></tr>
<tr><td>GetValue()</td><td align="right">0x00780128</td><td>The address of the DDLParser::StructFieldValue, since this field has a default value defined for it. This DDLParser::StructFieldValue has the values 1 and 2 at <code>m_Uint32[ 0 ]</code> and <code>m_Uint32[ 1 ]</code>.</td></tr>
<tr><td>GetTags()</td><td align="right">0x00000000</td><td>NULL since this field doesn't have any tag.</td></tr>
<tr><td>GetKeyType()</td><td align="right">0</td><td>Meaningless.</td></tr>
<tr><td>GetKeyBitSize()</td><td align="right">32</td><td>Meaningless.</td></tr>
<tr><td>GetAggregate()</td><td align="right">0x00000000</td><td>NULL since this field has a scalar type.</td></tr>
<tr><td>GetCount()</td><td align="right">2</td><td>This field holds two <code>uint32_t</code> values.</td></tr>
</table></p>

<p><table border="1" width="80%" cellspacing="0" cellpadding="3">
<tr><th colspan="3" bgcolor="#e0e0e0"><code>B.d</code></th></tr>
<tr><th width="15%">Method</th><th width="10%">Return Value</th><th width="55%">Meaning</th></tr>
<tr><td>GetNameHash()</td><td align="right">0x715c3d0a</td><td>DDLParser::StringCrc32 of "d", the field name.</td></tr>
<tr><td>GetType()</td><td align="right">2</td><td>DDLParser::kUint32.</td></tr>
<tr><td>GetTypeNameHash()</td><td align="right">0x0d5d2ca7</td><td>DDLParser::StringCrc32 of "uint32_t", but meaningless.</td></tr>
<tr><td>GetArrayType()</td><td align="right">2</td><td>DDLParser::kDynamic.</td></tr>
<tr><td>GetValue()</td><td align="right">0x00000000</td><td>NULL since this field hasn't (and can't have) a default value defined for it.</td></tr>
<tr><td>GetTags()</td><td align="right">0x00000000</td><td>NULL since this field doesn't have any tag.</td></tr>
<tr><td>GetKeyType()</td><td align="right">0</td><td>Meaningless.</td></tr>
<tr><td>GetKeyBitSize()</td><td align="right">32</td><td>Meaningless.</td></tr>
<tr><td>GetAggregate()</td><td align="right">0x00000000</td><td>NULL since this field has a scalar type.</td></tr>
<tr><td>GetCount()</td><td align="right">0</td><td>This field is a dynamic array, which initialy has no elements.</td></tr>
</table></p>

<p><table border="1" width="80%" cellspacing="0" cellpadding="3">
<tr><th colspan="3" bgcolor="#e0e0e0"><code>B.f</code></th></tr>
<tr><th width="15%">Method</th><th width="10%">Return Value</th><th width="55%">Meaning</th></tr>
<tr><td>GetNameHash()</td><td align="right">0x9f525c26</td><td>DDLParser::StringCrc32 of "f", the field name.</td></tr>
<tr><td>GetType()</td><td align="right">10</td><td>DDLParser::kString.</td></tr>
<tr><td>GetTypeNameHash()</td><td align="right">0xa76af9f8</td><td>DDLParser::StringCrc32 of "string", but meaningless.</td></tr>
<tr><td>GetArrayType()</td><td align="right">3</td><td>DDLParser::kHashmap.</td></tr>
<tr><td>GetValue()</td><td align="right">0x00000000</td><td>NULL since this field hasn't (and can't have) a default value defined for it.</td></tr>
<tr><td>GetTags()</td><td align="right">0x00000000</td><td>NULL since this field doesn't have any tag.</td></tr>
<tr><td>GetKeyType()</td><td align="right">2</td><td>DDLParser::kUint32.</td></tr>
<tr><td>GetKeyBitSize()</td><td align="right">32</td><td>The size of the key of this hashmap, in bits.</td></tr>
<tr><td>GetAggregate()</td><td align="right">0x00000000</td><td>NULL since this field has a scalar type (strings are scalars in DDL).</td></tr>
<tr><td>GetCount()</td><td align="right">0</td><td>This field is a hashmap, which initialy has no elements.</td></tr>
</table></p>

<p><table border="1" width="80%" cellspacing="0" cellpadding="3">
<tr><th colspan="3" bgcolor="#e0e0e0"><code>B.g</code></th></tr>
<tr><th width="15%">Method</th><th width="10%">Return Value</th><th width="55%">Meaning</th></tr>
<tr><td>GetNameHash()</td><td align="right">0xe8556cb0</td><td>DDLParser::StringCrc32 of "g", the field name.</td></tr>
<tr><td>GetType()</td><td align="right">13</td><td>DDLParser::kStruct.</td></tr>
<tr><td>GetTypeNameHash()</td><td align="right">0x3a58e94d</td><td>DDLParser::StringCrc32 of "A", the name of the aggregate which is the type of this field.</td></tr>
<tr><td>GetArrayType()</td><td align="right">0</td><td>DDLParser::kScalar.</td></tr>
<tr><td>GetValue()</td><td align="right">0x007801e4</td><td>The address of the DDLParser::StructFieldValue, since this field has a default value defined for it. This DDLParser::StructFieldValue has a DDLParser::StructStructValue pointer at <code>m_Struct[ 0 ]</code>.</td></tr>
<tr><td>GetTags()</td><td align="right">0x00000000</td><td>NULL since this field doesn't have any tag.</td></tr>
<tr><td>GetKeyType()</td><td align="right">0</td><td>Meaningless.</td></tr>
<tr><td>GetKeyBitSize()</td><td align="right">32</td><td>Meaningless.</td></tr>
<tr><td>GetAggregate()</td><td align="right">0x00780014</td><td>The address of DDLParser::Struct <code>A</code> which is the type of this field.</td></tr>
<tr><td>GetCount()</td><td align="right">1</td><td>This field holds one structure <code>A</code> value.</td></tr>
</table></p>

<p>The details of the DDLParser::StructStructValue for <code>B.g</code> are:</p>

<p><table border="1" width="80%" cellspacing="0" cellpadding="3">
<tr><th colspan="3" bgcolor="#e0e0e0">Details of the DDLParser::StructStructValue for <code>B.g</code></th></tr>
<tr><th width="15%">Method</th><th width="10%">Return Value</th><th width="55%">Meaning</th></tr>
<tr><td>GetNameHash()</td><td align="right">0x0136c985</td><td>DDLParser::StringCrc32 of "a", the name of the field being initialized.</td></tr>
<tr><td>GetType()</td><td align="right">2</td><td>DDLParser::kUint32.</td></tr>
<tr><td>GetTypeNameHash()</td><td align="right">0x0d5d2ca7</td><td>DDLParser::StringCrc32 of "uint32_t", but meaningless.</td></tr>
<tr><td>GetArrayType()</td><td align="right">0</td><td>DDLParser::kScalar.</td></tr>
<tr><td>GetValue()</td><td align="right">0x00780214</td><td>The address of the DDLParser::StructFieldValue, since this field has a default value defined for it. This DDLParser::StructFieldValue has the value 2 at m_Uint32[ 0 ], which overrides the default value 1 of <code>A.a</code>.</td></tr>
<tr><td>GetTags()</td><td align="right">0x00000000</td><td>NULL as value infos of fields being initialized can't have tags.</td></tr>
<tr><td>GetKeyType()</td><td align="right">0</td><td>Meaningless.</td></tr>
<tr><td>GetKeyBitSize()</td><td align="right">32</td><td>Meaningless.</td></tr>
<tr><td>GetAggregate()</td><td align="right">0x00000000</td><td>NULL since this field has a scalar type.</td></tr>
<tr><td>GetCount()</td><td align="right">1</td><td>This field holds one <code>uint32_t</code> value.</td></tr>
</table></p>

<hr/><h1><section name="DDLParser::Tag"></section></h1>

<p>Tags are decorations that can be added to structures, fields, selects, items, bitfields, and flags. Their purpose is to pass down information to code generators to change the way code is generated for them.</p>

<p>DDLParser::Tag is the parent of all other tags, and has the following methods:</p>

<section name="DDLParser::Tag::GetType"><code>uint32_t DDLParser::Tag::GetType() const</code></section>

<p>Returns the type of the tag, which is a value from the DDLParser::TagType enum.</p>

<section name="DDLParser::Tag::GetNext"><code>DDLParser::Tag* DDLParser::Tag::GetNext() const</code></section>

<p>The tags of a DDL component form a linked list. <code>GetTags</code> returns the first tag for the component, and DDLParser::Tag::GetNext is used to return the next tag in the linked list. Returns NULL at the end of the list.</p>

<hr/><h1><section name="DDLParser::GenericTag"></section></h1>

<p>All tag types are deprecated, new code should only use generic tags that can be defined without changing the DDL parser. Generic tags have the DDLParser::kGeneric type and the following methods in addition to the methods from DDLParser::Tag:</p>

<section name="DDLParser::GenericTag::GetName"><code>const char* DDLParser::GenericTag::GetName() const</code></section>

<p>Returns the identifier used in the tag definition.</p>

<section name="DDLParser::GenericTag::GetNameHash"><code>uint32_t DDLParser::GenericTag::GetNameHash() const</code></section>

<p>Returns the hash of the identifier used in the tag definition, created with DDLParser::StringCrc32.</p>

<section name="DDLParser::GenericTag::GetNumValues"><code>uint32_t DDLParser::GenericTag::GetNumValues() const</code></section>

<p>Returns the number of values defined in the tag after its name.</p>

<section name="DDLParser::GenericTag::operator%[%]"><code>DDLParser::GenericTagValue* DDLParser::GenericTag::operator[]( unsigned int index ) const</code></section>

<p>Returns the value defined for the tag at the given index.</p>

<hr/><h1><section name="DDLParser::GenericTagValue"></section></h1>

<p>Represents a value defined in a generic tag after its name:</p>

<source>struct DDLParser::GenericTagValue
{
  uint32_t m_Type;

  union
  {
    int64_t m_Int64;
    double  m_Float64;
    String  m_String;
  };
};</source>

<p>The <code>m_Type</code> member can be <code>DDLParser::kInt64</code>, <code>DDLParser::kFloat64</code>, or <code>DDLParser::kString</code>. Use this member to know the type of the value and which of the other members to access in a value.</p>

<hr/><h1><section name="Validating Generic Tags"></section></h1>

<p>Every time the parser parses a generic tag it calls one of the following callback functions to validate the tag:</p>

<ul>
<li><code>typedef bool DDLParser::CheckStructTag( DDLParser::Definition* def, DDLParser::Struct* structure, DDLParser::GenericTag* tag, DDLParser::Str* tag_name, DDLParser::TagSet* tag_set, char* error, size_t error_size );</code></li>
<li><code>typedef bool DDLParser::CheckFieldTag( DDLParser::Definition* def, DDLParser::Struct* structure, DDLParser::StructField* field, DDLParser::GenericTag* tag, DDLParser::Str* tag_name, DDLParser::TagSet* tag_set, char* error, size_t error_size );</code></li>
<li><code>typedef bool DDLParser::CheckSelectTag( DDLParser::Definition* def, DDLParser::Select* select, DDLParser::GenericTag* tag, DDLParser::Str* tag_name, DDLParser::TagSet* tag_set, char* error, size_t error_size );</code></li>
<li><code>typedef bool DDLParser::CheckItemTag( DDLParser::Definition* def, DDLParser::Select* select, DDLParser::SelectItem* item, DDLParser::GenericTag* tag, DDLParser::Str* tag_name, DDLParser::TagSet* tag_set, char* error, size_t error_size );</code></li>
<li><code>typedef bool DDLParser::CheckBitfieldTag( DDLParser::Definition* def, DDLParser::Bitfield* bitfield, DDLParser::GenericTag* tag, DDLParser::Str* tag_name, DDLParser::TagSet* tag_set, char* error, size_t error_size );</code></li>
<li><code>typedef bool DDLParser::CheckFlagTag( DDLParser::Definition* def, DDLParser::Bitfield* bitfield, DDLParser::BitfieldFlag* flag, DDLParser::GenericTag* tag, DDLParser::Str* tag_name, DDLParser::TagSet* tag_set, char* error, size_t error_size );</code></li>
</ul>

<p>These callbacks must return <code>true</code> to inform the parser that the tag was accepted, or <code>false</code> to inform that it was rejected. In the later case, the callback must also fill the <code>error</code> buffer with an error message up to <code>error_size</code> characters.</p>

<p>If a callback isn't defined, the tag is automatically accepted.</p>

<section name="DDLParser::Str"></section>

<p>DDLParser::Str represent internal strings during the parsing. They point directly into the DDL source code being parsed so they cannot be changed. The methods that can be used to validate a generic tag are:</p>

<ul>
<li><code>bool DDLParser::Str::operator==( const DDLParser::Str::Str& other ) const</code></li>
<li><code>bool DDLParser::Str::operator==( const char* other ) const</code></li>
<li><code>const char* DDLParser::Str::GetChars() const</code></li>
<li><code>unsigned int DDLParser::Str::GetLength() const</code></li>
<li><code>uint32_t DDLParser::Str::GetHash() const</code></li>
</ul>

<section name="DDLParser::TagSet"></section>

<p>When one of the callbacks is called to validate a generic tag, a DDLParser::TagSet is provided to allow for keeping a record of which tags have already been defined and i.e. disallow duplicates. The methods that can be used during validation are:</p>

<ul>
<li><code>bool DDLParser::TagSet::Insert( uint32_t element )</code></li>
<li><code>bool DDLParser::TagSet::Exists( uint32_t element )</code></li>
</ul>

<p>Note that the tag's name hash (as returned by DDLParser::GenericTag::GetNameHash) should be used with tag sets.</p>

<h2>Note</h2>

<p>DDLParser::Compile doesn't give the programmer the chance to install callbacks in the parser. For that, duplicate the source code of DDLParser::Compile and add the callbacks as needed.</p>

<hr/><h1><section name="DDL Grammar"></section></h1>

<p>The language permits the declaration of any number of structures, bitfields and selects. At least one such declaration must exist in a valid DDL file.</p>

<code><pre>grammar ddl;

ddl
  : ( typedef | struct | bitfield | select )+
  ;</pre></code>

<p>A typedef is an alias to a data type, array type and a set of tags. After defining a typedef it can be used to declare fields, which will have everything that was defined in the typedef.</p>

<code><pre>typedef
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
  ;</pre></code>

<p>A structure begins with the <code>struct</code> keyword and with the structure name, an identifier. After the name the author, description, and label of the structure can be specified but they're not mandatory. The fields can then be specified enclosed by curly braces.</p>

<code><pre>struct
  : 'struct' ID ( ',' struct_info )* '{' field* '}'
  ;</pre></code>

<p><code>struct_info</code> allows for additional information about aggregates, namely its author, description and label. Labels are used to name UI controls, and descriptions can be used as tips for these controls. Although the grammar allows for repetitions, the parser checks for duplicates and errors accordingly.</p>

<p>The structure can also inherit fields from a parent structure, specified by the <code>base</code> tag.</p>

<code><pre>struct_info
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
  ;</pre></code>

<p>A field declaration begins with the field type of a typedef identifier. After that the field name (an identifier), is parsed and additional information about the field can be specified.</p>

<code><pre>field
  : ( field_type | ID ) ID ( ',' field_info )* ';'
  ;</pre></code>

<p>The field type is a data type. If the data type is followed by a opening square brace, it denotes that the field is an array. If an integer expression is provided within the braces the array has a fixed size, otherwise the array is dynamic and elements can added to/removed from it at runtime (depending on the code that is generated). If the data type is followed by opening curly brace, it denotes a hashmap mapping values of the data type inside the curly braces to values of the data type of the field.</p>

<code><pre>field_type
  : data_type ( '[' ( expression )? ']' | '{' data_type '}' )?
  ;</pre></code>

<p>The field data type can be one of the native types or a custom type. The native types are signed and unsigned integers with 8, 16, 32 or 64 bits, float, double, string, file, boolean, tuid and json. A custom type is just an identifier denoting a select, bitfield or structure.</p>

<code><pre>data_type
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
  ;</pre></code>

<p><code>field_info</code> allows for additional information about fields, namely its author, description, label and a default value. The default value is the value used to initialize the field.</p>

<code><pre>field_info
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
  ;</pre></code>

<p>The <code>default_value</code> is a complex rule, and which option is taken depends on the context. In other words, it's a parser decision depending on what it was parsing.</p>

<code><pre>default_value
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
  ;</pre></code>

<ul>
<li>Option #1 is taken when the field is of a native type and is not an array.</li>
<li>Option #2 is taken when the field is of a select type and is not an array.</li>
<li>Option #3 is taken when the field is of a bitfield type and is not an array.</li>
<li>Option #4 is taken when the field is of a structure type and is not an array.</li>
<li>Options #5 to #8 correspond to options #1 to #4 but are for arrays of native, select, bitfield and structure values.</li>
</ul>

<p>A bitfield is declared with the <code>bitfield</code> keyword followed by its name, an identifier, which may be followed by additional information. Its flags are declared inside enclosing braces.</p>

<code><pre>bitfield
  : 'bitfield' ID ( ',' bitfield_info )* '{' flag+ '}'
  ;

bitfield_info
  : 'author' '(' LITERAL ')'
  | 'description' '(' LITERAL ')'
  | 'label' '(' LITERAL ')'
  | generic_tag
  ;</pre></code>

<p>Declarations of bitfield flags begin with an identifier which is its name, followed by additional flag information.</p>

<code><pre>flag
  : ID ( ',' flag_info )* ';'
  ;</pre></code>

<p>Besides author, description and label, a flag can be specified as the default flag for the bitfield. Flags can also be declared as empty, meaning that their value is zero. If a value is specified, the flag is a set containing all listed flags.</p>

<code><pre>flag_info
  : 'author' '(' LITERAL ')'
  | 'description' '(' LITERAL ')'
  | 'label' '(' LITERAL ')'
  | 'default'
  | 'value' '(' ID ( '|' ID )* ')'
  | 'empty'
  | generic_tag
  ;</pre></code>

<p>A select is declared in the same way of a bitfield.</p>

<code><pre>select
  : 'select' ID ( ',' select_info )* '{' item+ '}'
  ;

select_info
  : 'author' '(' LITERAL ')'
  | 'description' '(' LITERAL ')'
  | 'label' '(' LITERAL ')'
  | generic_tag
  ;</pre></code>

<p>A select item has an implicit type of <code>uint32_t</code> so it's not possible to define a type for it. Its declaration begin with an identifier which is its name, followed by additional information. The value of a select item is the DDLParser::StringCrc32 of its name so it's not possible to define a value for it.</p>

<p>The item defined as the default item is used to initialize the default value of structure fields having the select type. As with bitfields, this default can be overridden by using the <code>value</code> tag in the field declaration. If no item is declared as the default, the first item declared in the select will be the default.</p>

<code><pre>item
  : ID ( ',' item_info )* ';'
  ;

item_info
  : 'author' '(' LITERAL ')'
  | 'description' '(' LITERAL ')'
  | 'label' '(' LITERAL ')'
  | 'default'
  | generic_tag
  ;</pre></code>

<p>The expression rule parses any valid C-like expression. It's defined as the ternary operator.</p>

<code><pre>expression
  : ternary
  ;</pre></code>

<p>The ternary operator is a <code>logic_or</code> expression optionally followed by a <code>?</code> and two expressions. If the logic_or expression is true (different from zero), the value of the ternary operator is the result of the first expression. If it's false, the value of the ternary operator is the result of the second expression. If <code>?</code> is not present in the expression, the value of the ternary operator is the value of the <code>logic_or</code> expression.</p>

<code><pre>ternary
  : logic_or ( '?' expression ':' expression )?
  ;</pre></code>

<p>The <code>logic_or</code> expression is the value of a <code>logic_and</code> expression, optionally or'ed with other <code>logic_and</code> expressions.</p>

<code><pre>logic_or
  : logic_and ( '||' logic_and )*
  ;</pre></code>

<p>The <code>logic_and</code> expression is the value of an <code>arithmetic_or</code> expression, optionally and'ed with other <code>arithmetic_or</code> expressions.</p>

<code><pre>logic_and
  : arithmetic_or ( '&&' arithmetic_or )*
  ;</pre></code>

<p>The <code>arithmetic_or</code> expression is the value of an <code>arithmetic_xor</code> expression, optionally bitwise-or'ed with other <code>arithmetic_xor</code> expressions.</p>

<code><pre>arithmetic_or
  : arithmetic_xor ( '|' arithmetic_xor )*
  ;</pre></code>

<p>The <code>arithmetic_xor</code> expression is the value of an <code>arithmetic_and</code> expression, optionally bitwise-xor'ed with other <code>arithmetic_and</code> expressions.</p>

<code><pre>arithmetic_xor
  : arithmetic_and ( '^' arithmetic_and )*
  ;</pre></code>

<p>The <code>arithmetic_and</code> expression is the value of an <code>equal</code> expression, optionally bitwise-and'ed with other <code>equal</code> expressions.</p>

<code><pre>arithmetic_and
  : equal ( '&' equal )*
  ;</pre></code>

<p>The <code>equal</code> expression is the value of a <code>conditional</code> expression, optionally compared for equality or inequality with other <code>conditional</code> expressions. It's thus possible to write <code>a > b == c > d</code> meaning <code>a > b</code> and <code>c > d</code> are both either false or true, and <code>a > b != c > d</code> meaning <code>a > b</code> is true and <code>c > d</code> is false or vice-versa.</p>

<code><pre>equal
  : conditional ( ( '==' | '!=' ) conditional )*
  ;</pre></code>

<p>The <code>conditional</code> expression is the value of a <code>shift</code> expression, optionally compared with other <code>shift</code> expressions with one of the operators <code>&lt;</code>, <code>&gt;</code>, <code>&lt;=</code> or <code>&gt;=</code>.</p>

<code><pre>conditional
  : shift ( ( '<' | '>' | '<=' | '>=' ) shift )*
  ;</pre></code>

<p>The <code>shift</code> expression is the value of a <code>term</code> expression, optionally shifted left or right by other <code>term</code> expressions.</p>

<code><pre>shift
  : term ( ( '<<' | '>>' ) term )*
  ;</pre></code>

<p>The <code>term</code> expression is the value of a <code>factor</code> expression, optionally added to or subtracted from other <code>factor</code> expressions.</p>

<code><pre>term
  : factor ( ( '+' | '-' ) factor )*
  ;</pre></code>

<p>The factor expression is the value of an <code>unary</code> expression, optionally multiplied by, divided by, mod'ed by other <code>unary</code> expressions.</p>

<code><pre>factor
  : unary ( ( '*' | '/' | '%' ) unary )*
  ;</pre></code>

<p>The <code>unary</code> expression is either the value of a <code>terminal</code> or the value of another <code>unary</code> changed by the unary plus, minus, negate or not operators.</p>

<code><pre>unary
  : ( '+' | '-' | '~' | '!' ) unary
  | terminal
  ;</pre></code>

<p>The terminal expression is an octal, hexadecimal, decimal or real (in base 10) number, a string, the constants <code>true</code>, <code>false</code>, <code>pi</code> or <code>e</code>, or another expression enclosed in parenthesis.</p>

<code><pre>terminal
  : OCTAL | BINARY | HEXADECIMAL | DECIMAL | REAL | LITERAL
  | 'true' | 'false'
  | 'pi' | 'e'
  | '(' expression ')'
  ;</pre></code>

<p>Octal constants must begin with a 0, and can only have digits from 0 to 7.</p>

<code><pre>OCTAL
  : '0' ( '0' .. '7' )+
  ;</pre></code>

<p>Binary constants must begin with <code>0b</code>, and can only have digits 0 and 1.</p>

<code><pre>BINARY
  : '0b' ( '0' | '1' )+
  ;</pre></code>

<p>Hexadecimal constants must begin with <code>0x</code>, and can have digits from 0 to 9 and from A to F (case insensitive).</p>

<code><pre>HEXADECIMAL
  : '0x' ( HEX_DIGIT )+
  ;

HEX_DIGIT:
  : '0' .. '9'
  | 'a' .. 'f'
  | 'A' .. 'f'
  ;</pre></code>
  
<p>Decimal numbers can only have digits from 0 to 9. In the lexical analyser, if a number starts with 0 it's considered to be an octal constant.</p>

<code><pre>DECIMAL
  : ( '0' .. '9' )+
  ;</pre></code>

<p>Real numbers are decimal numbers with an optional decimal period followed by another decimal number. They can have an optional exponent (an <code>e</code> or <code>E</code>) followed by the decimal exponent value which can be signed, and optionally ended with an <code>f</code>. If the real number does not have a decimal period and neither an exponent, it must end with an 'f' otherwise it will be analyzed as a decimal number. Note that this is slightly different from what is expected from C.</p>

<code><pre>REAL
  : DECIMAL ( '.' DECIMAL )? ( ( 'e' | 'E' ) ( '+' | '-' ) DECIMAL )? 'f'?
  ;</pre></code>

<p>Identifiers are case sensitive sequences of upper case letters, lower case letters, decimal digits and underscores. They cannot begin with a decimal digit.</p>

<code><pre>ID
  : ( 'a' .. 'z' | 'A' .. 'Z' | '_' ) ( 'a' .. 'z' | 'A' .. 'Z' | '_' | '0' .. '9' )*
  ;</pre></code>

<p>Literals are string constants which are sequences of any characters except the percent sign (which is the beginning of an escape sequence), the double quote (which marks the end of the string) and line feed. Strings are enclosed in double quotes.</p>

<code><pre>LITERAL
  : '"' ( '%' HEX_DIGIT HEX_DIGIT | ~( '%' | '"' ) )* '"'
  ;</pre></code>

<p>Escape sequences always start with a <code>%</code>, and must be followed by two hexadecimal digits. These three characters will be converted to one character in the resulting literal, and this character will have the code specified by the two hexadecimal digits.</p>

<p>Literals are codepage agnostic, so the meaning of characters with codes outside of the [0, 127] range are platform dependent.</p>

<hr/><h1>License</h1>

<p>Copyright (c) 2012, Insomniac Games<br/>
All rights reserved.</p>

<p>Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:</p>

<ul>
<li>Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.</li>
<li>Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.</li>
</ul>

<p>THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.</p>

</body>
</html>
]]

local function isInside( doc, i, j, a, b )
  local pattern = a .. '.-' .. ( b or a )
  local s = 1

  while true do
    local a, b = doc:find( pattern, s )

    if not a then
      return false
    end

    if a <= i and b >= j then
      return true
    end

    s = b + 1
  end
end

-- Syntax highlight.
do
  local reserved =
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
    select = 'keyword',
    bitfield = 'keyword',
    author = 'keyword',
    description = 'keyword',
    label = 'keyword',
    default = 'keyword',
    case = 'keyword',
    empty = 'keyword',
    value = 'keyword',
    static = 'keyword',
    union = 'keyword',
    [ 'true' ] = 'bool',
    [ 'false' ] = 'bool',
  }

  local function getClass( code, i )
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
      return reserved[ code:sub( s, i - 1 ) ], i
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
      return 'comment', i + 1
    else
      return 'operator', i + 1
    end
  end

  while true do
    local i, j, code = doc:find( '<source>(.-)</source>', s )

    if not i then
      break
    end

    local hl = ''
    local x, y = 1, #code

    while x < y do
      local z = x
      local class
      class, x = getClass( code, x )

      if class then
        hl = hl .. '<span class="' .. class .. '">' .. code:sub( z, x - 1 ) .. '</span>'
      else
        hl = hl .. code:sub( z, x - 1 )
      end
    end

    local len = #doc
    doc = doc:sub( 1, i - 1 ) .. '<code><pre>' .. hl .. '</pre></code>' .. doc:sub( j + 1, -1 )
    s = j + #doc - len + 1
  end
end

-- Finds and records anchors to automatically add links in the document.
local anchors = {}

for anchor in doc:gmatch( '<section%s+.-name%s-=%s-"([^"]-)"') do
  anchors[ #anchors + 1 ] = anchor
end

table.sort( anchors, function( a1, a2 ) return a1:len() > a2:len() end )

-- Adds links to sections.
for _, anchor in ipairs( anchors ) do
  local s = 1

  while true do
    local i, j = doc:find( anchor, s )

    if not i then
      break
    end

    local len = #doc

    if not doc:sub( i - 1, i - 1 ):match( '%w' ) and not doc:sub( j + 1, j + 1 ):match( '%w' ) and not isInside( doc, i, j, '"' ) and not isInside( doc, i, j, '<a', '</a>' ) then
      doc = doc:sub( 1, i - 1 ) .. '<a href="#' .. anchor .. '">' .. doc:sub( i, j ) .. '</a>' .. doc:sub( j + 1, -1 )
    end

    s = j + #doc - len + 1
  end
end

-- Convert sections to internal anchors.
do
  local s = 1

  while true do
    local i, j, anchor, title = doc:find( '<section%s+.-name%s-=%s-"([^"]-)".->([^<]-)</section>', s )
    
    if not i then
      break
    end

    if title:len() == 0 then
      title = anchor
    end

    local len = #doc
    doc = doc:sub( 1, i - 1 ) .. '<a name="' .. anchor .. '">' .. title .. '</a>' .. doc:sub( j + 1, -1 )
    s = j + #doc - len + 1
  end
end

-- Automatically adds <code></code> surrounding patterns defined in the code table.
do
  local code =
  {
    'DDLParser::%w+::%w+',
    'DDLParser::%w+',
    'NULL'
  }

  for _, pattern in ipairs( code ) do
    local s = 1

    while true do
      local i, j = doc:find( pattern, s )

      if not i then
        break
      end

      local len = #doc

      if not isInside( doc, i, j, '<code>', '</code>' ) and not isInside( doc, i, j, '"' ) then
        doc = doc:sub( 1, i - 1 ) .. '<code>' .. doc:sub( i, j ) .. '</code>' .. doc:sub( j + 1, -1 )
      end

      s = j + #doc - len + 1
    end
  end
end

io.write( doc )
