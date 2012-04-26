# Index #######################################################################


1. DDL Specification  
1.1. Selects  
1.1.1. Example  
1.2. Bitfields  
1.2.1. Example  
1.3. Structures  
1.3.1. Examples  

2. Expressions  

3. Comments  

4. DDL API  
4.1. Common methods  
4.1.1. **const char\* GetName() const**  
4.1.2. **const char\* GetAuthor() const**  
4.1.3. **const char\* GetDescription() const**  
4.1.4. **const char\* GetLabel() const**  
4.1.5. **const char\* GetDisplayLabel() const**  
4.1.6. **uint32\_t GetNameHash() const**  
4.1.7. **DDLParser::Tag\* GetTags() const**  
4.1.8. **DDLParser::Tag\* GetTag( uint32\_t type ) const**  

5. DDLParser::Compile  
5.1. Parameters  
5.2. Return value  

6. DDLParser::StringCrc32  

7. DDLParser::Definition  
7.1. **uint32\_t DDLParser::Definition::GetNumAggregates() const**  
7.2. **uint32\_t DDLParser::Definition::GetTotalSize() const**  
7.3. **static DDLParser::Definition\* DDLParser::Definition::FromBinRep( void\* bin\_rep )**  
7.4. **DDLParser::Aggregate\* DDLParser::Definition::GetAggregate( unsigned int index )**  
7.5. **DDLParser::Aggregate\* DDLParser::Definition::FindAggregate( const char\* name )**  
7.6. **DDLParser::Aggregate\* DDLParser::Definition::FindAggregate( uint32\_t hash )**  

8. DDLParser::Aggregate  
8.1. **uint32\_t DDLParser::Aggregate::GetType() const**  
8.2. **DDLParser::Select\* DDLParser::Aggregate::ToSelect() const**  
8.3. **DDLParser::Bitfield\* DDLParser::Aggregate::ToBitfield() const**  
8.4. **DDLParser::Struct\* DDLParser::Aggregate::ToStruct() const**  

9. DDLParser::Select  
9.1. **uint32\_t DDLParser::Select::GetNumItems() const**  
9.2. **int32\_t DDLParser::Select::GetDefaultItem() const**  
9.3. **DDLParser::SelectItem\* DDLParser::Select::FindItem( const char\* name )**  
9.4. **DDLParser::SelectItem\* DDLParser::Select::FindItem( uint32\_t hash )**  

10. DDLParser::SelectItem  

11. DDLParser::Bitfield  
11.1. **uint32\_t DDLParser::Bitfield::GetNumFlags() const**  
11.2. **int32\_t DDLParser::Bitfield::GetDefaultFlag() const**  
11.3. **DDLParser::BitfieldFlag\* DDLParser::Bitfield::FindFlag( const char\* name )**  
11.4. **DDLParser::BitfieldFlag\* DDLParser::Bitfield::FindFlag( uint32\_t hash )**  

12. DDLParser::BitfieldFlag  
12.1. **DDLParser::BitfieldFlagValue\* DDLParser::BitfieldFlag::GetValue() const**  
12.2. **uint32\_t DDLParser::BitfieldFlag::GetBit() const**  

13. DDLParser::BitfieldFlagValue  
13.1. **uint32\_t DDLParser::BitfieldFlagValue::GetCount() const**  
13.2. **uint32\_t DDLParser::BitfieldFlagValue::GetFlagIndex( uint32\_t index ) const**  
13.3. Bitfield example  

14. DDLParser::Struct  
14.1. **uint32\_t DDLParser::Struct::GetNumFields() const**  
14.2. **DDLParser::Struct\* DDLParser::Struct::GetParent() const**  
14.3. **DDLParser::Definition\* DDLParser::Struct::GetDefinition() const**  
14.4. **DDLParser::StructField\* DDLParser::Struct::FindField( const char\* name )**  
14.5. **DDLParser::StructField\* DDLParser::Struct::FindField( uint32\_t hash )**  
14.6. **bool DDLParser::Struct::IsInherited( DDLParser::StructField\* field )**  
14.7. **uint32\_t DDLParser::Struct::GetSchemaCrc() const**  

15. DDLParser::StructField  
15.1. **DDLParser::StructValueInfo\* DDLParser::StructField::GetValueInfo()**  

16. DDLParser::StructValueInfo  
16.1. **uint32\_t DDLParser::StructValueInfo::GetNameHash() const**  
16.2. **Type DDLParser::StructValueInfo::GetType() const**  
16.3. **uint32\_t DDLParser::StructValueInfo::GetTypeNameHash() const**  
16.4. **const char\* DDLParser::StructValueInfo::GetTypeName() const**  
16.5. **ArrayType DDLParser::StructValueInfo::GetArrayType() const**  
16.6. **DDLParser::StructFieldValue\* DDLParser::StructValueInfo::GetValue() const**  
16.7. **uint32\_t DDLParser::StructValueInfo::GetCount() const**  
16.8. **uint32\_t DDLParser::StructValueInfo::GetKeyType() const**  
16.9. **int DDLParser::StructValueInfo::GetKeyBitSize() const**  
16.10. **bool DDLParser::StructValueInfo::AllowSubStruct() const**  
16.11. **uint32\_t DDLParser::StructValueInfo::GetSchemaCrc() const**  
16.12. **DDLParser::Aggregate\* DDLParser::StructValueInfo::GetAggregate( Definition\* ddl ) const**'  

17. DDLParser::StructFieldValue  

18. DDLParser::StructBitfieldValue  
18.1. **uint32\_t DDLParser::StructBitfieldValue::GetCount() const**  
18.2. **uint32\_t DDLParser::StructBitfieldValue::GetHash( uint32\_t index ) const**  

19. DDLParser::StructStructValue  
19.1. **uint32\_t DDLParser::StructStructValue::GetCount() const**  
19.2. **DDLParser::StructValueInfo\* DDLParser::StructStructValue::GetValueInfo( uint32\_t index ) const**  
19.3. Example  

20. DDLParser::Tag  
20.1. **uint32\_t DDLParser::Tag::GetType() const**  
20.2. **DDLParser::Tag\* DDLParser::Tag::GetNext() const**  

21. DDLParser::GenericTag  
21.1. **const char\* DDLParser::GenericTag::GetName() const**  
21.2. **uint32\_t DDLParser::GenericTag::GetNameHash() const**  
21.3. **uint32\_t DDLParser::GenericTag::GetNumValues() const**  
21.4. **DDLParser::GenericTagValue\* DDLParser::GenericTag::operator[]( unsigned int index ) const**  

22. DDLParser::GenericTagValue  
22.1. Validating Generic Tags  

23. DDLParser::Str  

24. DDLParser::TagSet  

25. DDL Grammar  

26. License  

# 1. DDL Specification ########################################################

DDL files can contain three constructs: selects, bitfields and structures. All
constructs must have a name, and they can have an author, a description and a
label assigned to them. These fields can be used by tools when rendering the
constructs as UI elements.

There is a annotated DDL grammar at the end of this document for further
reference.

## 1.1. Selects ###############################################################

A select is just a list of options much like an C enum. They represent
exclusive values, like a drop-down where just one value can be selected. But
unlike C enums, numeric values cannot be assigned to select fields; their
values are just the hash of their names. This way new fields can be added
without constraints while maintaining compatibility with cold that uses the
select.

Each select item can have an author, a description and a label.

### 1.1.1. Example ############################################################

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
    }

An item can be marked as default (see item **kPistol**) and it will be the item
used in fields declared with the select type that don't have an explicit value
to override the default.

## 1.2. Bitfields #############################################################

Bitfields represent a set of related flags. Their flags can be or'ed to form
the value of another flag. Bitfields should be used where more than one value
can be selected, like a checkbox group.

As with select items, each bitfield flag can have an author, a description and
a label.

### 1.2.1. Example ############################################################

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
    }

Flags without a value will get an unique bit index that can be queried at
runtime. Flags can be marked as default. If there isn't a default flag in the
bitfield the empty flag will be used, or the first declared flag will be used
if there isn't an empty flag. To get a zeroed flag, tag it with **empty** like
the **kNone** flag above.

## 1.3. Structures ############################################################

While selects and bitfields only define custom types and take no memory,
structures are aggregates of fields which actually take up memory to hold their
fields' values.

Structure fields can be of the following types:

* **u8** (or **uint8\_t**)
* **u16** (or **uint16\_t**)
* **u32** (or **uint32\_t**)
* **u64** (or **uint64\_t**)
* **i8** (or **int8\_t**)
* **i16** (or **int16\_t**)
* **i32** (or **int32\_t**)
* **i64** (or **int64\_t**)
* **f32** (or **float**)
* **f64** (or **double**)
* **boolean**
* **string**
* **file**
* **tuid**
* **json**
* Any previously declared select, bitfield or structure

Fields can also be arrays of any of these types by just adding **[]** right
after the type name. These arrays cannot take default values, and can grow as
needed at runtime. Adding a **[ x ]**, where **x** is an integer expression,
makes the array fixed and up to **x** default values can be specified for the
field. Just like select items and bitfield flags, structure fields can have an
author, a description and a label.

To inform the default value of a field, use the **value** keyword and write any
expression inside the parenthesis. The expression must evaluate to a value
which fits in the data type of the field.

### 1.3.1. Examples ###########################################################

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
    }

Array values must be written inside braces with commas separating each
expression. Values of structured fields, like **m\_Position** above, must also
appear inside braces. It's not mandatory that all fields are present in the
value.

# 2. Expressions ##############################################################

DDL expressions are very similar to C expressions, and care has been taken to
implement the same operators with the same precedence order. The operators, in
decreasing order of precedence, are:

* **?** (ternary operator)
* **||** (logic or)
* **&&** (logic and)
* **|** (arithmetic or)
* **^** (arithmetic xor)
* **&** (arithmetic and)
* **==** and **!=** (equality)
* **<**, **<=**, **>** and **>=** (comparison)
* **<<** and **>>** (shift)
* **+** and **-** (add and subtract)
* **\***, **/** and **%** (multiply, divide and modulus)
* **+**, **-**, **~** and **!** (unary positive, negative, complement and logic
  not)

Terminals can be binary, octal, decimal and hexadecimal integer constants,
floating point constants, string literals, the **true** and **false**
constants, **pi**, the natural number **e**, and expressions between
parenthesis.

Binary constants are written similarly to hexadecimal constants but using the
**0b** prefix instead of **0x**. String literals are specified inside single or
double quotes, and any occurrence of **%xx** in the string is substituted by
the character with hexadecimal value **xx**.

The constants **true** and **false** evaluates to the integers 1 and 0
respectively.

# 3. Comments #################################################################

DDL files can have C and C++ style comments:

* **/\* this is a comment \*/**
* **// this is a comment that ends at the end of the line**

# 4. DDL API ##################################################################

The DDL API are a collection of functions and structures that allows for DDL
source code compilation and subsequent querying of the components found in the
source file for i.e. code generation.

## 4.1. Common methods ########################################################

Many structures present a set of common methods that are explained here.

### 4.1.1. **const char\* GetName() const** ###################################

Returns the name of the component, i.e. a select's name or a structure's
field's name.

### 4.1.2. **const char\* GetAuthor() const** #################################

If the component has been tagged with the **author** tag, **GetAuthor** returns
the contents of the tag, otherwise it returns NULL. This tag can be used to
document the person who authored the component.

### 4.1.3. **const char\* GetDescription() const** ############################

If the component has been tagged with the **description** tag,
**GetDescription** returns the contents of the tag, otherwise it returns NULL.
This tag can be used to give the component a verbose description.

### 4.1.4. **const char\* GetLabel() const** ##################################

If the component has been tagged with the **label** tag, **GetLabel** returns
the contents of the tag, otherwise it returns NULL. This tag can be used to
give the component a label to be used in the user interface, for example.

### 4.1.5. **const char\* GetDisplayLabel() const** ###########################

Returns the contents of the **label** tag if it has been defined and is not the
empty string. Otherwise it returns the name of the component.

### 4.1.6. **uint32\_t GetNameHash() const** ##################################

Returns the hash of the components's name. The hash is created with
DDLParser::StringCrc32.

### 4.1.7. **DDLParser::Tag\* GetTags() const** ###############################

Returns the first tag defined for the component, or NULL if the component has
no tags defined.

### 4.1.8. **DDLParser::Tag\* GetTag( uint32\_t type ) const** ################

Finds and returns a tag by its type. Alternatively, type can be the hash of the
generic tag to be searched for. Either way, if the tag isn't found NULL is
returned.

# 5. DDLParser::Compile #######################################################

**DDLParser::Definition\* DDLParser::Compile( DDLParser::LinearAllocator\* definition, DDLParser::LinearAllocator\* scratch, const void\* source, size\_t source\_size, char\* error, size\_t error\_size, bool two\_us\_reserved, int bitfield\_limit )**
The DDLParser::Compile function compiles a DDL source file into a
DDLParser::Definition ready to be used to generate code.

## 5.1. Parameters ############################################################

* **definition**: A DDLParser::LinearAllocator where the definition is
  assembled. When DDLParser::Compile returns, this allocator will have the
  complete definition.
* **scratch**: A DDLParser::LinearAllocator used for temporary work. Can be
  discarded after DDLParser::Compile returns.
* **source**: A pointer to the DDL source code. This buffer doesn't have to be
  null-terminated and cannot be NULL.
* **source\_size**: The size of the **source** buffer.
* **error**: A pointer to a buffer where the error message, if there's one,
  will be written to. Cannot be NULL.
* **error\_size**: The size of the **error** buffer.
* **two\_us\_reserved**: If true, identifiers starting with two underlines are
  reserved and disallowed in the source code.
* **bitfield\_limit**: The maximum number of flags allowed in a bitfield. If
  this value is zero, there is no limit on the number of flags per bitfield.

## 5.2. Return value ##########################################################

If the parse succeeds it returns the definition assembled in **definition**
casted to a DDLParser::Definition pointer. If there was an error,
DDLParser::Compile returns NULL and **error** is filled with the error message.

# 6. DDLParser::StringCrc32 ###################################################

**uint32\_t DDLParser::StringCrc32( const char\* data )**

Returns the CRC32 of the string.

# 7. DDLParser::Definition ####################################################

After a successful parsing, DDLParser::Compile returns a DDLParser::Definition,
which is used as the starting point to query all selects, bitfields, and
structures, defined in the source code.

DDLParser::Definition has the following methods:

## 7.1. **uint32\_t DDLParser::Definition::GetNumAggregates() const** #########

Returns the number of aggregates (selects, bitfields and structures) in the
definition.

## 7.2. **uint32\_t DDLParser::Definition::GetTotalSize() const** #############

Returns the total size in bytes of the definition. This can be used if you want
to write it into a file for example.

## 7.3. **static DDLParser::Definition\* DDLParser::Definition::FromBinRep( void\* bin\_rep )** 

Just returns the **bin\_rep** pointer casted to a DDLParser::Definition.

## 7.4. **DDLParser::Aggregate\* DDLParser::Definition::GetAggregate( unsigned int index )** 

Returns an aggregate given its index. Aggregates are saved into the definition
in the order they appear in the source code. No bounds check is done with
**index**.

## 7.5. **DDLParser::Aggregate\* DDLParser::Definition::FindAggregate( const char\* name )** 

Finds and returns an aggregate by name. If the aggregate does not exist, NULL
is returned.

## 7.6. **DDLParser::Aggregate\* DDLParser::Definition::FindAggregate( uint32\_t hash )** 

Finds and returns an aggregate by the hash of its name. If the aggregate does
not exist, NULL is returned. The hash must be created with
DDLParser::StringCrc32.

# 8. DDLParser::Aggregate #####################################################

An aggregate holds information common to selects, bitfields, and structures. It
has the following common methods: **GetName**, **GetAuthor**,
**GetDescription**, **GetLabel**, **GetDisplayLabel**, and **GetNameHash**.

## 8.1. **uint32\_t DDLParser::Aggregate::GetType() const** ###################

Returns the type of the aggregate, DDLParser::kSelect, DDLParser::kBitfield, or
DDLParser::kStruct.

## 8.2. **DDLParser::Select\* DDLParser::Aggregate::ToSelect() const** ########

Returns the aggregate casted to DDLParser::Select without checking for the
actual aggregate type.

## 8.3. **DDLParser::Bitfield\* DDLParser::Aggregate::ToBitfield() const** ####

Returns the aggregate casted to DDLParser::Bitfield without checking for the
actual aggregate type.

## 8.4. **DDLParser::Struct\* DDLParser::Aggregate::ToStruct() const** ########

Returns the aggregate casted to DDLParser::Struct without checking for the
actual aggregate type.

# 9. DDLParser::Select ########################################################

A select is like a C enum, but you can't define the value of the items. Their
value is always the hash code of their identifiers. A select holds information
about its items.

In addition to the methods listed here, selects have the following common
methods: **GetName**, **GetAuthor**, **GetDescription**, **GetLabel**,
**GetDisplayLabel**, **GetNameHash**, **GetTags**, and **GetTag**.

## 9.1. **uint32\_t DDLParser::Select::GetNumItems() const** ##################

Returns the number of items contained in the select.

## 9.2. **int32\_t DDLParser::Select::GetDefaultItem() const** ################

Returns the index of the item defined as the default item for the select. If no
item was defined as the default, the default item is the first defined. This
can be used to initialize variables to a default value if none is given for
example.

## 9.3. **DDLParser::SelectItem\* DDLParser::Select::FindItem( const char\* name )** 

Finds and returns an item by name. If the item does not exist, NULL is
returned.

## 9.4. **DDLParser::SelectItem\* DDLParser::Select::FindItem( uint32\_t hash )** 

Finds and returns an item by the hash of its name. If the item does not exist,
NULL is returned. The hash must be created with DDLParser::StringCrc32.

# 10. DDLParser::SelectItem ###################################################

A select item represents an item declared in a select and has the following
common methods: **GetName**, **GetAuthor**, **GetDescription**, **GetLabel**,
**GetDisplayLabel**, **GetNameHash**, **GetTags**, and **GetTag**.

Select items doesn't have any particular method besides the common ones listed
above.

# 11. DDLParser::Bitfield #####################################################

A bitfield is a set, and bitfield flags are the set's items. Unlike select
items, a bitfield flag has a value associated to it. This value can be an
explicit zero, an unique, automatically assigned value or a set which is the
union of other flags. If the value is automatically assigned, it starts with
one and is incremented for each assignment.

In addition to the methods listed here, bitfields have the following common
methods: **GetName**, **GetAuthor**, **GetDescription**, **GetLabel**,
**GetDisplayLabel**, **GetNameHash**, **GetTags**, and **GetTag**.

## 11.1. **uint32\_t DDLParser::Bitfield::GetNumFlags() const** ###############

Returns the number of flags contained in the bitfield.

## 11.2. **int32\_t DDLParser::Bitfield::GetDefaultFlag() const** #############

Returns the index of the flag defined as the default flag for the bitfield. If
no flag was defined as the default, the default item is the empty flag. If an
empty flag wasn't declared, The first flag is elected the default. This can be
used to initialize variables to a default value if none is given for example.

## 11.3. **DDLParser::BitfieldFlag\* DDLParser::Bitfield::FindFlag( const char\* name )** 

Finds and returns a flag by name. If the flag does not exist, NULL is returned.

## 11.4. **DDLParser::BitfieldFlag\* DDLParser::Bitfield::FindFlag( uint32\_t hash )** 

Finds and returns a flag by the hash of its name. If the flag does not exist,
NULL is returned. The hash must be created with DDLParser::StringCrc32.

# 12. DDLParser::BitfieldFlag #################################################

A flag is a unique element in a set, or a set of other flags. A flag has the
following common methods: **GetName**, **GetAuthor**, **GetDescription**,
**GetLabel**, **GetDisplayLabel**, **GetNameHash**, **GetTags**, and
**GetTag**.

Flags also have the following unique methods:

## 12.1. **DDLParser::BitfieldFlagValue\* DDLParser::BitfieldFlag::GetValue() const** 

Returns the value of the flag in case it's the empty flag or a set. If neither
is true, this function returns NULL.

## 12.2. **uint32\_t DDLParser::BitfieldFlag::GetBit() const** ################

If the flag is the empty flag, or if it's a set, this functions returns zero.
Otherwise, it returns an automatically assigned value for the flag which is
unique in the bitfield. This values starts at one and is incremented for each
flag that uses it. It can be used to generate code for flags as bits in an
integer, i.e. **1 << ( flag->GetBit() - 1 )**.

# 13. DDLParser::BitfieldFlagValue ############################################

Defines the value of a flag when it's either the empty flag or a set.

## 13.1. **uint32\_t DDLParser::BitfieldFlagValue::GetCount() const** #########

If the flag is the empty flag, this function returns zero. Otherwise, it
returns the number of flags contained in the flag.

## 13.2. **uint32\_t DDLParser::BitfieldFlagValue::GetFlagIndex( uint32\_t index ) const** 

Returns the index of the flag which is part of this set.

## 13.3. Bitfield example #####################################################

As an example, this code generates C++ code that outputs a bitfield's flags as
bits in an unsigned integer:

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
    }

# 14. DDLParser::Struct #######################################################

Structures are collections of fields. Each field has its own type, which can be
a native type, a select, a bitfield, another structure, a fixed or dynamic
array of the previous types, or a hashmap mapping an integer, a string, a file,
or a tuid to any other type.

In addition to the methods listed here, structures have the following common
methods: **GetName**, **GetAuthor**, **GetDescription**, **GetLabel**,
**GetDisplayLabel**, **GetNameHash**, **GetTags**, and **GetTag**.

## 14.1. **uint32\_t DDLParser::Struct::GetNumFields() const** ################

Returns the number of fields contained in the structure.

## 14.2. **DDLParser::Struct\* DDLParser::Struct::GetParent() const** #########

Returns the parent structure of this structure. If the structure doesn't
inherit from another structure, it returns NULL.

## 14.3. **DDLParser::Definition\* DDLParser::Struct::GetDefinition() const** #

Returns the definition where the structure was defined.

## 14.4. **DDLParser::StructField\* DDLParser::Struct::FindField( const char\* name )** 

Finds and returns a field by name. If the field does not exist, NULL is
returned.

## 14.5. **DDLParser::StructField\* DDLParser::Struct::FindField( uint32\_t hash )** 

Finds and returns a field by the hash of its name. If the field does not exist,
NULL is returned. The hash must be created with DDLParser::StringCrc32.

## 14.6. **bool DDLParser::Struct::IsInherited( DDLParser::StructField\* field )** 

Returns **true** if the field is from a parent structure.

## 14.7. **uint32\_t DDLParser::Struct::GetSchemaCrc() const** ################

The schema crc reflects the type, count, name, and order of each field. If any
of those change, then the crc changes. It does not reflect default values or
any tags.

# 15. DDLParser::StructField ##################################################

A field is an instance of a given type. A field has these common methods:
**GetName**, **GetAuthor**, **GetDescription**, **GetLabel**,
**GetDisplayLabel**, **GetNameHash**, **GetTags**, and **GetTag**.

Fields also have the following unique method.

## 15.1. **DDLParser::StructValueInfo\* DDLParser::StructField::GetValueInfo()** 

Returns the value information of the field. A field always have a
DDLParser::StructField::GetValueInfo so this method never returns NULL.

# 16. DDLParser::StructValueInfo ##############################################

A DDLParser::StructValueInfo holds information for both a field declared in a
structure and its default values if the field has a structure for its type. It
has the the **GetTags** and **GetTag** common methods.

## 16.1. **uint32\_t DDLParser::StructValueInfo::GetNameHash() const** ########

Returns the hash of the field's name.

## 16.2. **Type DDLParser::StructValueInfo::GetType() const** #################

Returns the type of the field which is a value from the DDLParser::Type
enumeration.

## 16.3. **uint32\_t DDLParser::StructValueInfo::GetTypeNameHash() const** ####

Returns the hash of the field's type name. Only usefull if the field has a
select, bitfield or structure type so it's possible to query the definition for
the actual aggregate.

## 16.4. **const char\* DDLParser::StructValueInfo::GetTypeName() const** #####

Deprecated.

## 16.5. **ArrayType DDLParser::StructValueInfo::GetArrayType() const** #######

Returns the array type of the field. Possible values are DDLParser::kScalar
(the field is not an array), DDLParser::kFixed (the field is an array with a
fixed dimension), DDLParser::kDynamic (the field is an array without an
specified dimention) and DDLParser::kHashmap (the field is a hashmap), from the
DDLParser::ArrayType enumeration.

## 16.6. **DDLParser::StructFieldValue\* DDLParser::StructValueInfo::GetValue() const** 

Returns the array of default values for the field. If the field hasn't a
default value, it returns NULL. Only scalar and fixed array types of fields can
have a default value. In the former, only the first position of the array of
values is valid. In the later, the array of values is valid from 0 to
DDLParser::StructValueInfo::GetCount.

## 16.7. **uint32\_t DDLParser::StructValueInfo::GetCount() const** ###########

Returns the size of the array if the array is fixed. Always returns one for
scalars and zero for dynamic arrays and hashmaps.

## 16.8. **uint32\_t DDLParser::StructValueInfo::GetKeyType() const** #########

Returns the DDL type of a hashmap key. If the field is not a hashmap, this
method doesn't return anything meaningful.

## 16.9. **int DDLParser::StructValueInfo::GetKeyBitSize() const** ############

Convenience method that returns the bit size of the hashmap key. Returns 64 for
DDLParser::kInt64, DDLParser::kUint64, DDLParser::kFloat64, and
DDLParser::kTuid, and 32 for all other types.

## 16.10. **bool DDLParser::StructValueInfo::AllowSubStruct() const** #########

Convenience method that returns **true** if the field was tagged with the
**tag( AllowSubstruct )**' generic tag.

## 16.11. **uint32\_t DDLParser::StructValueInfo::GetSchemaCrc() const** ######

The schema crc reflects the type, count, and name of the field. If any of those
change, then the crc changes. It does not reflect default values or any tags.

## 16.12. **DDLParser::Aggregate\* DDLParser::StructValueInfo::GetAggregate( Definition\* ddl ) const**' 

Returns the aggregate which is the type of the field. If the field has not a
select, bitfield or structure as its type, this method returns NULL.

# 17. DDLParser::StructFieldValue #############################################

DDLParser::StructFieldValue is an union which holds the values defined for
fields.

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
    };

When a field's DDLParser::StructValueInfo has a pointer to a
DDLParser::StructFieldValue (via DDLParser::StructValueInfo::GetValue), it
means the field has default values. The member of the union that is to be
accessed depends on DDLParser::StructValueInfo::GetType, and how many entries
are valid depends on the field's array type. For DDLParser::kScalar fields, one
position only is valid. For DDLParser::kFixed fields, positions from zero to
DDLParser::StructValueInfo::GetCount** - 1** are valid. DDLParser::kDynamic and
DDLParser::kHashmap fields don't have default values.

# 18. DDLParser::StructBitfieldValue ##########################################

When a field is of a bitfield type and has a default value, this value is held
in the **m\_Bitfield** member of the DDLParser::StructFieldValue union. It has
the following methods:

## 18.1. **uint32\_t DDLParser::StructBitfieldValue::GetCount() const** #######

Returns the number of flags that make the default value of the field.

## 18.2. **uint32\_t DDLParser::StructBitfieldValue::GetHash( uint32\_t index ) const** 

Returns the hash of the flag at the given index. This hash can be used to get
the actual flag from the bitfield via DDLParser::Bitfield::FindFlag.

# 19. DDLParser::StructStructValue ############################################

When a field is of a structure type and has a default value, it's default value
is held in the **m\_Struct** member of the DDLParser::StructFieldValue union.
For each field receiving a default value, DDLParser::StructStructValue has a
DDLParser::StructValueInfo with details of which field is being initialized and
what value it's receiving.

DDLParser::StructStructValue has the following methods:

## 19.1. **uint32\_t DDLParser::StructStructValue::GetCount() const** #########

Returns the number of value infos in this structure value.

## 19.2. **DDLParser::StructValueInfo\* DDLParser::StructStructValue::GetValueInfo( uint32\_t index ) const** 

Returns the value info at the given index.

## 19.3. Example ##############################################################

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
    }

The DDLParser::StructValueInfo for the fields declared in the DDL code above
are:

#### **A.a**
* **Method**: **GetNameHash()** **Return Value**: 0x0136c985 **Meaning**: DDLParser::StringCrc32 of **a**, the field name. 
* **Method**: **GetType()** **Return Value**: 2 **Meaning**: DDLParser::kUint32. 
* **Method**: **GetTypeNameHash()** **Return Value**: 0x0d5d2ca7 **Meaning**: DDLParser::StringCrc32 of **uint32\_t**, but meaningless. 
* **Method**: **GetArrayType()** **Return Value**: 0 **Meaning**: DDLParser::kScalar. 
* **Method**: **GetValue()** **Return Value**: 0x009d0074 **Meaning**: The address of the DDLParser::StructFieldValue, since this field has a default value defined for it. This DDLParser::StructFieldValue has the value 1 at **m\_Uint32[ 0 ]**. 
* **Method**: **GetTags()** **Return Value**: 0x00000000 **Meaning**: NULL since this field doesn't have any tag. 
* **Method**: **GetKeyType()** **Return Value**: 0 **Meaning**: Meaningless. 
* **Method**: **GetKeyBitSize()** **Return Value**: 32 **Meaning**: Meaningless. 
* **Method**: **GetAggregate()** **Return Value**: 0x00000000 **Meaning**: NULL since this field has a scalar type. 
* **Method**: **GetCount()** **Return Value**: 1 **Meaning**: This field holds one **uint32\_t** value. 

#### **A.b**
* **Method**: **GetNameHash()** **Return Value**: 0x983f983f **Meaning**: DDLParser::StringCrc32 of **a**, the field name. 
* **Method**: **GetType()** **Return Value**: 2 **Meaning**: DDLParser::kUint32. 
* **Method**: **GetTypeNameHash()** **Return Value**: 0x0d5d2ca7 **Meaning**: DDLParser::StringCrc32 of **uint32\_t**, but meaningless. 
* **Method**: **GetArrayType()** **Return Value**: 0 **Meaning**: DDLParser::kScalar. 
* **Method**: **GetValue()** **Return Value**: 0x00000000 **Meaning**: NULL since this field hasn't a default value defined for it. 
* **Method**: **GetTags()** **Return Value**: 0x00000000 **Meaning**: NULL since this field doesn't have any tag. 
* **Method**: **GetKeyType()** **Return Value**: 0 **Meaning**: Meaningless. 
* **Method**: **GetKeyBitSize()** **Return Value**: 32 **Meaning**: Meaningless. 
* **Method**: **GetAggregate()** **Return Value**: 0x00000000 **Meaning**: NULL since this field has a scalar type. 
* **Method**: **GetCount()** **Return Value**: 1 **Meaning**: This field holds one **uint32\_t** value. 

#### **B.c**
* **Method**: **GetNameHash()** **Return Value**: 0xef38a8a9 **Meaning**: DDLParser::StringCrc32 of **c**, the field name. 
* **Method**: **GetType()** **Return Value**: 2 **Meaning**: DDLParser::kUint32. 
* **Method**: **GetTypeNameHash()** **Return Value**: 0x0d5d2ca7 **Meaning**: DDLParser::StringCrc32 of **uint32\_t**, but meaningless. 
* **Method**: **GetArrayType()** **Return Value**: 1 **Meaning**: DDLParser::kFixed. 
* **Method**: **GetValue()** **Return Value**: 0x00780128 **Meaning**: The address of the DDLParser::StructFieldValue, since this field has a default value defined for it. This DDLParser::StructFieldValue has the values 1 and 2 at **m\_Uint32[ 0 ]** and **m\_Uint32[ 1 ]**. 
* **Method**: **GetTags()** **Return Value**: 0x00000000 **Meaning**: NULL since this field doesn't have any tag. 
* **Method**: **GetKeyType()** **Return Value**: 0 **Meaning**: Meaningless. 
* **Method**: **GetKeyBitSize()** **Return Value**: 32 **Meaning**: Meaningless. 
* **Method**: **GetAggregate()** **Return Value**: 0x00000000 **Meaning**: NULL since this field has a scalar type. 
* **Method**: **GetCount()** **Return Value**: 2 **Meaning**: This field holds two **uint32\_t** values. 

#### **B.d**
* **Method**: **GetNameHash()** **Return Value**: 0x715c3d0a **Meaning**: DDLParser::StringCrc32 of **d**, the field name. 
* **Method**: **GetType()** **Return Value**: 2 **Meaning**: DDLParser::kUint32. 
* **Method**: **GetTypeNameHash()** **Return Value**: 0x0d5d2ca7 **Meaning**: DDLParser::StringCrc32 of **uint32\_t**, but meaningless. 
* **Method**: **GetArrayType()** **Return Value**: 2 **Meaning**: DDLParser::kDynamic. 
* **Method**: **GetValue()** **Return Value**: 0x00000000 **Meaning**: NULL since this field hasn't (and can't have) a default value defined for it. 
* **Method**: **GetTags()** **Return Value**: 0x00000000 **Meaning**: NULL since this field doesn't have any tag. 
* **Method**: **GetKeyType()** **Return Value**: 0 **Meaning**: Meaningless. 
* **Method**: **GetKeyBitSize()** **Return Value**: 32 **Meaning**: Meaningless. 
* **Method**: **GetAggregate()** **Return Value**: 0x00000000 **Meaning**: NULL since this field has a scalar type. 
* **Method**: **GetCount()** **Return Value**: 0 **Meaning**: This field is a dynamic array, which initialy has no elements. 

#### **B.f**
* **Method**: **GetNameHash()** **Return Value**: 0x9f525c26 **Meaning**: DDLParser::StringCrc32 of "f", the field name. 
* **Method**: **GetType()** **Return Value**: 10 **Meaning**: DDLParser::kString. 
* **Method**: **GetTypeNameHash()** **Return Value**: 0xa76af9f8 **Meaning**: DDLParser::StringCrc32 of "string", but meaningless. 
* **Method**: **GetArrayType()** **Return Value**: 3 **Meaning**: DDLParser::kHashmap. 
* **Method**: **GetValue()** **Return Value**: 0x00000000 **Meaning**: NULL since this field hasn't (and can't have) a default value defined for it. 
* **Method**: **GetTags()** **Return Value**: 0x00000000 **Meaning**: NULL since this field doesn't have any tag. 
* **Method**: **GetKeyType()** **Return Value**: 2 **Meaning**: DDLParser::kUint32. 
* **Method**: **GetKeyBitSize()** **Return Value**: 32 **Meaning**: The size of the key of this hashmap, in bits. 
* **Method**: **GetAggregate()** **Return Value**: 0x00000000 **Meaning**: NULL since this field has a scalar type (strings are scalars in DDL). 
* **Method**: **GetCount()** **Return Value**: 0 **Meaning**: This field is a hashmap, which initialy has no elements. 

#### **B.g**
* **Method**: **GetNameHash()** **Return Value**: 0xe8556cb0 **Meaning**: DDLParser::StringCrc32 of "g", the field name. 
* **Method**: **GetType()** **Return Value**: 13 **Meaning**: DDLParser::kStruct. 
* **Method**: **GetTypeNameHash()** **Return Value**: 0x3a58e94d **Meaning**: DDLParser::StringCrc32 of "A", the name of the aggregate which is the type of this field. 
* **Method**: **GetArrayType()** **Return Value**: 0 **Meaning**: DDLParser::kScalar. 
* **Method**: **GetValue()** **Return Value**: 0x007801e4 **Meaning**: The address of the DDLParser::StructFieldValue, since this field has a default value defined for it. This DDLParser::StructFieldValue has a DDLParser::StructStructValue pointer at **m\_Struct[ 0 ]**. 
* **Method**: **GetTags()** **Return Value**: 0x00000000 **Meaning**: NULL since this field doesn't have any tag. 
* **Method**: **GetKeyType()** **Return Value**: 0 **Meaning**: Meaningless. 
* **Method**: **GetKeyBitSize()** **Return Value**: 32 **Meaning**: Meaningless. 
* **Method**: **GetAggregate()** **Return Value**: 0x00780014 **Meaning**: The address of DDLParser::Struct **A** which is the type of this field. 
* **Method**: **GetCount()** **Return Value**: 1 **Meaning**: This field holds one structure **A** value. 

The details of the DDLParser::StructStructValue for **B.g** are:

#### Details of the DDLParser::StructStructValue for **B.g**
* **Method**: **GetNameHash()** **Return Value**: 0x0136c985 **Meaning**: DDLParser::StringCrc32 of "a", the name of the field being initialized. 
* **Method**: **GetType()** **Return Value**: 2 **Meaning**: DDLParser::kUint32. 
* **Method**: **GetTypeNameHash()** **Return Value**: 0x0d5d2ca7 **Meaning**: DDLParser::StringCrc32 of "uint32_t", but meaningless. 
* **Method**: **GetArrayType()** **Return Value**: 0 **Meaning**: DDLParser::kScalar. 
* **Method**: **GetValue()** **Return Value**: 0x00780214 **Meaning**: The address of the DDLParser::StructFieldValue, since this field has a default value defined for it. This DDLParser::StructFieldValue has the value 2 at **m\_Uint32[ 0 ]**, which overrides the default value 1 of **A.a**. 
* **Method**: **GetTags()** **Return Value**: 0x00000000 **Meaning**: NULL as value infos of fields being initialized can't have tags. 
* **Method**: **GetKeyType()** **Return Value**: 0 **Meaning**: Meaningless. 
* **Method**: **GetKeyBitSize()** **Return Value**: 32 **Meaning**: Meaningless. 
* **Method**: **GetAggregate()** **Return Value**: 0x00000000 **Meaning**: NULL since this field has a scalar type. 
* **Method**: **GetCount()** **Return Value**: 1 **Meaning**: This field holds one **uint32\_t** value. 

# 20. DDLParser::Tag ##########################################################

Tags are decorations that can be added to structures, fields, selects, items,
bitfields, and flags. Their purpose is to pass down information to code
generators to change the way code is generated for them.

DDLParser::Tag is the parent of all other tags, and has the following methods:

## 20.1. **uint32\_t DDLParser::Tag::GetType() const** ########################

Returns the type of the tag, which is a value from the DDLParser::TagType enum.

## 20.2. **DDLParser::Tag\* DDLParser::Tag::GetNext() const** #################

The tags of a DDL component form a linked list. **GetTags** returns the first
tag for the component, and DDLParser::Tag::GetNext is used to return the next
tag in the linked list. Returns NULL at the end of the list.

# 21. DDLParser::GenericTag ###################################################

All tag types are deprecated, new code should only use generic tags that can be
defined without changing the DDL parser. Generic tags have the
DDLParser::kGeneric type and the following methods in addition to the methods
from DDLParser::Tag:

## 21.1. **const char\* DDLParser::GenericTag::GetName() const** ##############

Returns the identifier used in the tag definition.

## 21.2. **uint32\_t DDLParser::GenericTag::GetNameHash() const** #############

Returns the hash of the identifier used in the tag definition, created with
DDLParser::StringCrc32.

## 21.3. **uint32\_t DDLParser::GenericTag::GetNumValues() const** ############

Returns the number of values defined in the tag after its name.

## 21.4. **DDLParser::GenericTagValue\* DDLParser::GenericTag::operator[]( unsigned int index ) const** 

Returns the value defined for the tag at the given index.

# 22. DDLParser::GenericTagValue ##############################################

Represents a value defined in a generic tag after its name:

    struct DDLParser::GenericTagValue
    {
      uint32_t m_Type;
      union
      {
        int64_t m_Int64;
        double  m_Float64;
        String  m_String;
      };
    };

The **m\_Type** member can be **DDLParser::kInt64**, **DDLParser::kFloat64**,
or **DDLParser::kString**. Use this member to know the type of the value and
which of the other members to access in a value.

## 22.1. Validating Generic Tags ##############################################

Every time the parser parses a generic tag it calls one of the following
callback functions to validate the tag:

* **typedef bool DDLParser::CheckStructTag( DDLParser::Definition\* def,
  DDLParser::Struct\* structure, DDLParser::GenericTag\* tag, DDLParser::Str\*
  tag\_name, DDLParser::TagSet\* tag\_set, char\* error, size\_t error\_size
  );**
* **typedef bool DDLParser::CheckFieldTag( DDLParser::Definition\* def,
  DDLParser::Struct\* structure, DDLParser::StructField\* field,
  DDLParser::GenericTag\* tag, DDLParser::Str\* tag\_name, DDLParser::TagSet\*
  tag\_set, char\* error, size\_t error\_size );**
* **typedef bool DDLParser::CheckSelectTag( DDLParser::Definition\* def,
  DDLParser::Select\* select, DDLParser::GenericTag\* tag, DDLParser::Str\*
  tag\_name, DDLParser::TagSet\* tag\_set, char\* error, size\_t error\_size
  );**
* **typedef bool DDLParser::CheckItemTag( DDLParser::Definition\* def,
  DDLParser::Select\* select, DDLParser::SelectItem\* item,
  DDLParser::GenericTag\* tag, DDLParser::Str\* tag\_name, DDLParser::TagSet\*
  tag\_set, char\* error, size\_t error\_size );**
* **typedef bool DDLParser::CheckBitfieldTag( DDLParser::Definition\* def,
  DDLParser::Bitfield\* bitfield, DDLParser::GenericTag\* tag, DDLParser::Str\*
  tag\_name, DDLParser::TagSet\* tag\_set, char\* error, size\_t error\_size
  );**
* **typedef bool DDLParser::CheckFlagTag( DDLParser::Definition\* def,
  DDLParser::Bitfield\* bitfield, DDLParser::BitfieldFlag\* flag,
  DDLParser::GenericTag\* tag, DDLParser::Str\* tag\_name, DDLParser::TagSet\*
  tag\_set, char\* error, size\_t error\_size );**

These callbacks must return **true** to inform the parser that the tag was
accepted, or **false** to inform that it was rejected. In the later case, the
callback must also fill the **error** buffer with an error message up to
**error\_size** characters.

If a callback isn't defined, the tag is automatically accepted.

Note: DDLParser::Compile doesn't give the programmer the chance to install
callbacks in the parser. For that, duplicate the source code of
DDLParser::Compile and add the callbacks as needed.

# 23. DDLParser::Str ##########################################################

DDLParser::Str represent internal strings during the parsing. They point
directly into the DDL source code being parsed so they cannot be changed. The
methods that can be used to validate a generic tag are:

* **bool DDLParser::Str::operator==( const DDLParser::Str::Str& other ) const**
* **bool DDLParser::Str::operator==( const char\* other ) const**
* **const char\* DDLParser::Str::GetChars() const**
* **unsigned int DDLParser::Str::GetLength() const**
* **uint32\_t DDLParser::Str::GetHash() const**

# 24. DDLParser::TagSet #######################################################

When one of the callbacks is called to validate a generic tag, a
DDLParser::TagSet is provided to allow for keeping a record of which tags have
already been defined and i.e. disallow duplicates. The methods that can be used
during validation are:

* **bool DDLParser::TagSet::Insert( uint32\_t element )**
* **bool DDLParser::TagSet::Exists( uint32\_t element )**

Note that the tag's name hash (as returned by
DDLParser::GenericTag::GetNameHash) should be used with tag sets.

# 25. DDL Grammar #############################################################

The language permits the declaration of any number of structures, bitfields and
selects. At least one such declaration must exist in a valid DDL file.

    grammar ddl;
    ddl
      : ( typedef | struct | bitfield | select )+
      ;

A typedef is an alias to a data type, array type and a set of tags. After
defining a typedef it can be used to declare fields, which will have everything
that was defined in the typedef.

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
      ;

A structure begins with the **struct** keyword and with the structure name, an
identifier. After the name the author, description, and label of the structure
can be specified but they're not mandatory. The fields can then be specified
enclosed by curly braces.

    struct
      : 'struct' ID ( ',' struct_info )* '{' field* '}'
      ;

**struct\_info** allows for additional information about aggregates, namely its
author, description and label. Labels are used to name UI controls, and
descriptions can be used as tips for these controls. Although the grammar
allows for repetitions, the parser checks for duplicates and errors
accordingly.

The structure can also inherit fields from a parent structure, specified by the
**base** tag.

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
      ;

A field declaration begins with the field type of a typedef identifier. After
that the field name (an identifier), is parsed and additional information about
the field can be specified.

    field
      : ( field_type | ID ) ID ( ',' field_info )* ';'
      ;

The field type is a data type. If the data type is followed by a opening square
brace, it denotes that the field is an array. If an integer expression is
provided within the braces the array has a fixed size, otherwise the array is
dynamic and elements can added to/removed from it at runtime (depending on the
code that is generated). If the data type is followed by opening curly brace,
it denotes a hashmap mapping values of the data type inside the curly braces to
values of the data type of the field.

    field_type
      : data_type ( '[' ( expression )? ']' | '{' data_type '}' )?
      ;

The field data type can be one of the native types or a custom type. The native
types are signed and unsigned integers with 8, 16, 32 or 64 bits, float,
double, string, file, boolean, tuid and json. A custom type is just an
identifier denoting a select, bitfield or structure.

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
      ;

**field\_info** allows for additional information about fields, namely its
author, description, label and a default value. The default value is the value
used to initialize the field.

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
      ;

The **default\_value** is a complex rule, and which option is taken depends on
the context. In other words, it's a parser decision depending on what it was
parsing.

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
      ;

* Option #1 is taken when the field is of a native type and is not an array.
* Option #2 is taken when the field is of a select type and is not an array.
* Option #3 is taken when the field is of a bitfield type and is not an array.
* Option #4 is taken when the field is of a structure type and is not an array.
* Options #5 to #8 correspond to options #1 to #4 but are for arrays of native,
  select, bitfield and structure values.

A bitfield is declared with the **bitfield** keyword followed by its name, an
identifier, which may be followed by additional information. Its flags are
declared inside enclosing braces.

    bitfield
      : 'bitfield' ID ( ',' bitfield_info )* '{' flag+ '}'
      ;
    bitfield_info
      : 'author' '(' LITERAL ')'
      | 'description' '(' LITERAL ')'
      | 'label' '(' LITERAL ')'
      | generic_tag
      ;

Declarations of bitfield flags begin with an identifier which is its name,
followed by additional flag information.

    flag
      : ID ( ',' flag_info )* ';'
      ;

Besides author, description and label, a flag can be specified as the default
flag for the bitfield. Flags can also be declared as empty, meaning that their
value is zero. If a value is specified, the flag is a set containing all listed
flags.

    flag_info
      : 'author' '(' LITERAL ')'
      | 'description' '(' LITERAL ')'
      | 'label' '(' LITERAL ')'
      | 'default'
      | 'value' '(' ID ( '|' ID )* ')'
      | 'empty'
      | generic_tag
      ;

A select is declared in the same way of a bitfield.

    select
      : 'select' ID ( ',' select_info )* '{' item+ '}'
      ;
    select_info
      : 'author' '(' LITERAL ')'
      | 'description' '(' LITERAL ')'
      | 'label' '(' LITERAL ')'
      | generic_tag
      ;

A select item has an implicit type of **uint32\_t** so it's not possible to
define a type for it. Its declaration begin with an identifier which is its
name, followed by additional information. The value of a select item is the
DDLParser::StringCrc32 of its name so it's not possible to define a value for
it.

The item defined as the default item is used to initialize the default value of
structure fields having the select type. As with bitfields, this default can be
overridden by using the **value** tag in the field declaration. If no item is
declared as the default, the first item declared in the select will be the
default.

    item
      : ID ( ',' item_info )* ';'
      ;
    item_info
      : 'author' '(' LITERAL ')'
      | 'description' '(' LITERAL ')'
      | 'label' '(' LITERAL ')'
      | 'default'
      | generic_tag
      ;

The expression rule parses any valid C-like expression. It's defined as the
ternary operator.

    expression
      : ternary
      ;

The ternary operator is a **logic\_or** expression optionally followed by a
**?** and two expressions. If the logic_or expression is true (different from
zero), the value of the ternary operator is the result of the first expression.
If it's false, the value of the ternary operator is the result of the second
expression. If **?** is not present in the expression, the value of the ternary
operator is the value of the **logic\_or** expression.

    ternary
      : logic_or ( '?' expression ':' expression )?
      ;

The **logic\_or** expression is the value of a **logic\_and** expression,
optionally or'ed with other **logic\_and** expressions.

    logic_or
      : logic_and ( '||' logic_and )*
      ;

The **logic\_and** expression is the value of an **arithmetic\_or** expression,
optionally and'ed with other **arithmetic\_or** expressions.

    logic_and
      : arithmetic_or ( '&&' arithmetic_or )*
      ;

The **arithmetic\_or** expression is the value of an **arithmetic\_xor**
expression, optionally bitwise-or'ed with other **arithmetic\_xor**
expressions.

    arithmetic_or
      : arithmetic_xor ( '|' arithmetic_xor )*
      ;

The **arithmetic\_xor** expression is the value of an **arithmetic\_and**
expression, optionally bitwise-xor'ed with other **arithmetic\_and**
expressions.

    arithmetic_xor
      : arithmetic_and ( '^' arithmetic_and )*
      ;

The **arithmetic\_and** expression is the value of an **equal** expression,
optionally bitwise-and'ed with other **equal** expressions.

    arithmetic_and
      : equal ( '&' equal )*
      ;

The **equal** expression is the value of a **conditional** expression,
optionally compared for equality or inequality with other **conditional**
expressions. It's thus possible to write **a > b == c > d** meaning **a > b**
and **c > d** are both either false or true, and **a > b != c > d** meaning **a
> b** is true and **c > d** is false or vice-versa.

    equal
      : conditional ( ( '==' | '!=' ) conditional )*
      ;

The **conditional** expression is the value of a **shift** expression,
optionally compared with other **shift** expressions with one of the operators
**<**, **>**, **<=** or **>=**.

    conditional
      : shift ( ( '<' | '>' | '<=' | '>=' ) shift )*
      ;

The **shift** expression is the value of a **term** expression, optionally
shifted left or right by other **term** expressions.

    shift
      : term ( ( '<<' | '>>' ) term )*
      ;

The **term** expression is the value of a **factor** expression, optionally
added to or subtracted from other **factor** expressions.

    term
      : factor ( ( '+' | '-' ) factor )*
      ;

The factor expression is the value of an **unary** expression, optionally
multiplied by, divided by, mod'ed by other **unary** expressions.

    factor
      : unary ( ( '*' | '/' | '%' ) unary )*
      ;

The **unary** expression is either the value of a **terminal** or the value of
another **unary** changed by the unary plus, minus, negate or not operators.

    unary
      : ( '+' | '-' | '~' | '!' ) unary
      | terminal
      ;

The terminal expression is an octal, hexadecimal, decimal or real (in base 10)
number, a string, the constants **true**, **false**, **pi** or **e**, or
another expression enclosed in parenthesis.

    terminal
      : OCTAL | BINARY | HEXADECIMAL | DECIMAL | REAL | LITERAL
      | 'true' | 'false'
      | 'pi' | 'e'
      | '(' expression ')'
      ;

Octal constants must begin with a 0, and can only have digits from 0 to 7.

    OCTAL
      : '0' ( '0' .. '7' )+
      ;

Binary constants must begin with **0b**, and can only have digits 0 and 1.

    BINARY
      : '0b' ( '0' | '1' )+
      ;

Hexadecimal constants must begin with **0x**, and can have digits from 0 to 9
and from A to F (case insensitive).

    HEXADECIMAL
      : '0x' ( HEX_DIGIT )+
      ;
    HEX_DIGIT:
      : '0' .. '9'
      | 'a' .. 'f'
      | 'A' .. 'f'
      ;

  
Decimal numbers can only have digits from 0 to 9. In the lexical analyser, if a
number starts with 0 it's considered to be an octal constant.

    DECIMAL
      : ( '0' .. '9' )+
      ;

Real numbers are decimal numbers with an optional decimal period followed by
another decimal number. They can have an optional exponent (an **e** or **E**)
followed by the decimal exponent value which can be signed, and optionally
ended with an **f**. If the real number does not have a decimal period and
neither an exponent, it must end with an **f** otherwise it will be analyzed as
a decimal number. Note that this is different from what is expected from C.

    REAL
      : DECIMAL ( '.' DECIMAL )? ( ( 'e' | 'E' ) ( '+' | '-' ) DECIMAL )? 'f'?
      ;

Identifiers are case sensitive sequences of upper case letters, lower case
letters, decimal digits and underscores. They cannot begin with a decimal
digit.

    ID
      : ( 'a' .. 'z' | 'A' .. 'Z' | '_' ) ( 'a' .. 'z' | 'A' .. 'Z' | '_' | '0' .. '9' )*
      ;

Literals are string constants which are sequences of any characters except the
percent sign (which is the beginning of an escape sequence), the double quote
(which marks the end of the string) and line feed. Strings are enclosed in
double quotes.

    LITERAL
      : '"' ( '%' HEX_DIGIT HEX_DIGIT | ~( '%' | '"' ) )* '"'
      ;

Escape sequences always start with a **%**, and must be followed by two
hexadecimal digits. These three characters will be converted to one character
in the resulting literal, and this character will have the code specified by
the two hexadecimal digits.

Literals are codepage agnostic, so the meaning of characters with codes outside
of the [0, 127] range are platform dependent.

# 26. License #################################################################

Copyright (c) 2012, Insomniac Games  
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

