#include <string.h>

#include <DDLParser.h>

#include "Parser.h"

#define IMP_POINTER(name, type)                               \
  type* name::operator->() const                              \
  {                                                           \
    return Get();                                             \
  }                                                           \
  type& name::operator*() const                               \
  {                                                           \
    return *Get();                                            \
  }                                                           \
  type* name::Get() const                                     \
  {                                                           \
    if (m_Offset != 0)                                        \
    {                                                         \
      return (type*)((uint8_t*)this + m_Offset);              \
    }                                                         \
    return 0;                                                 \
  }                                                           \
  void name::operator=(const type* pointer)                   \
  {                                                           \
    if (pointer == 0)                                         \
    {                                                         \
      m_Offset = 0;                                           \
      return;                                                 \
    }                                                         \
    m_Offset = (int32_t)((uint8_t*)pointer - (uint8_t*)this); \
  }                                                           \
  bool name::operator==(void* pointer) const                  \
  {                                                           \
    return Get() == (type*)pointer;                           \
  }                                                           \
  bool name::operator!=(void* pointer) const                  \
  {                                                           \
    return !operator==(pointer);                              \
  }

namespace DDLParser
{
  IMP_POINTER( String,                 char );
  IMP_POINTER( DefinitionPtr,          Definition );
  IMP_POINTER( TagPtr,                 Tag );
  IMP_POINTER( GenericTagValuePtr,     GenericTagValue );
  IMP_POINTER( GenericTagPtr,          GenericTag );
  IMP_POINTER( AggregatePtr,           Aggregate );
  IMP_POINTER( SelectItemPtr,          SelectItem );
  IMP_POINTER( BitfieldFlagValuePtr,   BitfieldFlagValue );
  IMP_POINTER( BitfieldFlagPtr,        BitfieldFlag );
  IMP_POINTER( StructPtr,              Struct );
  IMP_POINTER( StructBitfieldValuePtr, StructBitfieldValue );
  IMP_POINTER( StructFieldValuePtr,    StructFieldValue );
  IMP_POINTER( StructStructValuePtr,   StructStructValue );
  IMP_POINTER( StructUnknownValuePtr,  StructUnknownValue );
  IMP_POINTER( StructFieldPtr,         StructField );
  IMP_POINTER( StructFieldRangePtr,    StructFieldRange );

#define ASSERT( x )
#define OFFSET_OF(s, m) (((size_t)&(((s *)0x10)->m))-0x10)
#define CHECK_OFFSET(structure, member, size) (OFFSET_OF(structure, member) < size)

  const char*
  BasicInfo::GetName() const
  {
    if (CHECK_OFFSET(BasicInfo, m_Name, m_Size))
    {
      return &*m_Name;
    }
    return 0;
  }

  const char*
  BasicInfo::GetAuthor() const
  {
    if (CHECK_OFFSET(BasicInfo, m_Author, m_Size))
    {
      return &*m_Author;
    }
    return 0;
  }

  const char*
  BasicInfo::GetDescription() const
  {
    if (CHECK_OFFSET(BasicInfo, m_Description, m_Size))
    {
      return &*m_Description;
    }
    return 0;
  }

  const char*
  BasicInfo::GetLabel() const
  {
    if (CHECK_OFFSET(BasicInfo, m_Label, m_Size))
    {
      return &*m_Label;
    }
    return 0;
  }

  const char*
  BasicInfo::GetDisplayLabel() const
  {
    if (CHECK_OFFSET(BasicInfo, m_Label, m_Size))
    {
      const char* label = &*m_Label;
      if( label && label[0] )
      {
        return label;
      }
    }
    if (CHECK_OFFSET(BasicInfo, m_Name, m_Size))
    {
      return &*m_Name;
    }
    return 0;
  }

  uint32_t
  Tag::GetType() const
  {
    if (CHECK_OFFSET(Tag, m_Type, m_Size))
    {
      return m_Type;
    }
    return 0xffffffff;
  }

  Tag*
  Tag::GetNext() const
  {
    if (CHECK_OFFSET(Tag, m_Next, m_Size))
    {
      return &*m_Next;
    }
    return 0;
  }

  uint32_t
  Extensions::GetCount() const
  {
    if (CHECK_OFFSET(Extensions, m_Count, m_Size))
    {
      return m_Count;
    }
    return 0;
  }

  const char*
  Extensions::GetExtension( uint32_t index ) const
  {
    ASSERT( index < GetCount() );
    String* exts_indices = (String*)( (uint8_t*)this + m_Size );
    return &*exts_indices[ index ];
  }

  uint32_t
  VaultHints::GetCount() const
  {
    if (CHECK_OFFSET(VaultHints, m_Count, m_Size))
    {
      return m_Count;
    }
    return 0;
  }

  const char*
  VaultHints::GetVaultHint( uint32_t index ) const
  {
    ASSERT( index < GetCount() );
    String* exts_indices = (String*)( (uint8_t*)this + m_Size );
    return &*exts_indices[ index ];
  }

  UIRangeValue*
  UIRange::GetSoftMin()
  {
    if (CHECK_OFFSET(UIRange, m_SoftMin, m_Size))
    {
      return &m_SoftMin;
    }
    return 0;
  }

  UIRangeValue*
  UIRange::GetSoftMax()
  {
    if (CHECK_OFFSET(UIRange, m_SoftMax, m_Size))
    {
      return &m_SoftMax;
    }
    return 0;
  }

  UIRangeValue*
  UIRange::GetHardMin()
  {
    if (CHECK_OFFSET(UIRange, m_HardMin, m_Size))
    {
      return &m_HardMin;
    }
    return 0;
  }

  UIRangeValue*
  UIRange::GetHardMax()
  {
    if (CHECK_OFFSET(UIRange, m_HardMax, m_Size))
    {
      return &m_HardMax;
    }
    return 0;
  }

  UIRangeValue*
  UIRange::GetStep()
  {
    if (CHECK_OFFSET(UIRange, m_Step, m_Size))
    {
      return &m_Step;
    }
    return 0;
  }

  const char*
  UIRender::GetRender() const
  {
    if (CHECK_OFFSET(UIRender, m_Render, m_Size))
    {
      return &*m_Render;
    }
    return 0;
  }

  StructField*
  Parallel::GetArray() const
  {
    if (CHECK_OFFSET(Parallel, m_Array, m_Size))
    {
      return &*m_Array;
    }
    return 0;
  }

  const char*
  Version::GetVersion() const
  {
    if (CHECK_OFFSET(Version, m_Version, m_Size))
    {
      return &*m_Version;
    }
    return 0;
  }

  const char*
  Callback::GetCallback() const
  {
    if (CHECK_OFFSET(Callback, m_Callback, m_Size))
    {
      return &*m_Callback;
    }
    return 0;
  }

  const char*
  Key::GetKey() const
  {
    if (CHECK_OFFSET(Key, m_Key, m_Size))
    {
      return &*m_Key;
    }
    return 0;
  }

  const char*
  Units::GetUnits() const
  {
    if ( CHECK_OFFSET( Units, m_Units, m_Size ) )
    {
      return &*m_Units;
    }
    return 0;
  }

  const char*
  GenericTag::GetName() const
  {
    if( CHECK_OFFSET( GenericTag, m_Name, m_Size ) )
    {
      return &*m_Name;
    }
    return 0;
  }

  uint32_t
  GenericTag::GetNameHash() const
  {
    if ( CHECK_OFFSET( GenericTag, m_NameHash, m_Size ) )
    {
      return m_NameHash;
    }
    return 0;
  }

  uint32_t
  GenericTag::GetNumValues() const
  {
    if ( CHECK_OFFSET( GenericTag, m_NumValues, m_Size ) )
    {
      return m_NumValues;
    }
    return 0;
  }

  GenericTagValue*
  GenericTag::operator[]( unsigned int index ) const
  {
    ASSERT( index < GetNumValues() );
    GenericTagValue* values = (GenericTagValue*)( (uint8_t*)this + m_Size );
    return &values[ index ];
  }

  static Tag*
  FindTag( Tag* tag, uint32_t type )
  {
    while ( tag != 0 )
    {
      if ( tag->GetType() == type || ( tag->GetType() == kGeneric && ( (GenericTag*)tag )->GetNameHash() == type ) )
      {
        return tag;
      }
      tag = tag->GetNext();
    }
    return 0;
  }

  uint32_t
  Aggregate::GetType() const
  {
    if (CHECK_OFFSET(Aggregate, m_Type, m_Size))
    {
      return m_Type;
    }
    return 0xffffffff; // A value different from all valid types.
  }

  const char*
  Aggregate::GetName() const
  {
    if (CHECK_OFFSET(Aggregate, m_Name, m_Size))
    {
      return &*m_Name;
    }
    return 0;
  }

  const char*
  Aggregate::GetAuthor() const
  {
    if (CHECK_OFFSET(Aggregate, m_Author, m_Size))
    {
      return &*m_Author;
    }
    return 0;
  }

  const char*
  Aggregate::GetDescription() const
  {
    if (CHECK_OFFSET(Aggregate, m_Description, m_Size))
    {
      return &*m_Description;
    }
    return 0;
  }

  const char*
  Aggregate::GetLabel() const
  {
    if (CHECK_OFFSET(Aggregate, m_Label, m_Size))
    {
      return &*m_Label;
    }
    return 0;
  }

  const char*
  Aggregate::GetDisplayLabel() const
  {
    if (CHECK_OFFSET(Aggregate, m_Label, m_Size))
    {
      const char* label = &*m_Label;
      if( label && label[0] )
      {
        return label;
      }
    }
    if (CHECK_OFFSET(Aggregate, m_Name, m_Size))
    {
      return &*m_Name;
    }
    return 0;
  }

  uint32_t
  Aggregate::GetNameHash() const
  {
    if (CHECK_OFFSET(Aggregate, m_NameHash, m_Size))
    {
      return m_NameHash;
    }
    return 0;
  }

  Select*
  Aggregate::ToSelect() const
  {
    ASSERT(m_Type == kSelect);
    return (Select*)this;
  }

  Bitfield*
  Aggregate::ToBitfield() const
  {
    ASSERT(m_Type == kBitfield);
    return (Bitfield*)this;
  }

  Struct*
  Aggregate::ToStruct() const
  {
    ASSERT(m_Type == kStruct);
    return (Struct*)this;
  }

  uint32_t
  SelectItem::GetNameHash() const
  {
    if (CHECK_OFFSET(SelectItem, m_NameHash, m_Size))
    {
      return m_NameHash;
    }
    return 0;
  }

  Tag*
  SelectItem::GetTags() const
  {
    if ( CHECK_OFFSET( SelectItem, m_Tags, m_Size ) )
    {
      return &*m_Tags;
    }
    return 0;
  }

  Tag*
  SelectItem::GetTag( uint32_t type ) const
  {
    return FindTag( GetTags(), type );
  }

  uint32_t
  Select::GetNumItems() const
  {
    if (CHECK_OFFSET(Select, m_NumItems, m_Size))
    {
      return m_NumItems;
    }
    return 0;
  }

  int32_t
  Select::GetDefaultItem() const
  {
    if (CHECK_OFFSET(Select, m_DefaultItem, m_Size))
    {
      return m_DefaultItem;
    }
    return -1;
  }


  SelectItem*
  Select::operator[](unsigned int index)
  {
    ASSERT(index < GetNumItems());
    SelectItemPtr* items = (SelectItemPtr*)((uint8_t*)this + m_Size);
    return &*items[index];
  }

  SelectItem*
  Select::FindItem(const char* name)
  {
    return FindItem(StringCrc32(name));
  }

  SelectItem*
  Select::FindItem(uint32_t hash)
  {
    for (uint32_t i = 0; i < GetNumItems(); i++)
    {
      SelectItem* item = (*this)[i];
      if (item->GetNameHash() == hash)
      {
        return item;
      }
    }
    return 0;
  }

  Tag*
  Select::GetTags() const
  {
    if ( CHECK_OFFSET( Select, m_Tags, m_Size ) )
    {
      return &*m_Tags;
    }
    return 0;
  }

  Tag*
  Select::GetTag( uint32_t type ) const
  {
    return FindTag( GetTags(), type );
  }

  uint32_t
  BitfieldFlagValue::GetCount() const
  {
    if (CHECK_OFFSET(BitfieldFlagValue, m_Count, m_Size))
    {
      return m_Count;
    }
    return 0;
  }

  uint32_t
  BitfieldFlagValue::GetFlagIndex(uint32_t index) const
  {
    ASSERT(index < GetCount());
    uint32_t* flag_indices = (uint32_t*)((uint8_t*)this + m_Size);
    return flag_indices[index];
  }

  uint32_t
  BitfieldFlag::GetNameHash() const
  {
    if (CHECK_OFFSET(BitfieldFlag, m_NameHash, m_Size))
    {
      return m_NameHash;
    }
    return 0;
  }
  
  BitfieldFlagValue*
  BitfieldFlag::GetValue() const
  {
    if (CHECK_OFFSET(BitfieldFlag, m_Value, m_Size))
    {
      return &*m_Value;
    }
    return 0;
  }

  uint32_t
  BitfieldFlag::GetBit() const
  {
    if (CHECK_OFFSET(BitfieldFlag, m_Bit, m_Size))
    {
      return m_Bit;
    }
    return 0;
  }

  Tag*
  BitfieldFlag::GetTags() const
  {
    if ( CHECK_OFFSET( BitfieldFlag, m_Tags, m_Size ) )
    {
      return &*m_Tags;
    }
    return 0;
  }

  Tag*
  BitfieldFlag::GetTag( uint32_t type ) const
  {
    return FindTag( GetTags(), type );
  }

  uint32_t
  Bitfield::GetNumFlags() const
  {
    if (CHECK_OFFSET(Bitfield, m_NumFlags, m_Size))
    {
      return m_NumFlags;
    }
    return 0;
  }

  int32_t
  Bitfield::GetDefaultFlag() const
  {
    if (CHECK_OFFSET(Bitfield, m_DefaultFlag, m_Size))
    {
      return m_DefaultFlag;
    }
    return -1;
  }

  BitfieldFlag*
  Bitfield::operator[](unsigned int index)
  {
    ASSERT(index < GetNumFlags());
    BitfieldFlagPtr* flags = (BitfieldFlagPtr*)((uint8_t*)this + m_Size);
    return &*flags[index];
  }

  BitfieldFlag*
  Bitfield::FindFlag(const char* name)
  {
    return FindFlag(StringCrc32(name));
  }

  BitfieldFlag*
  Bitfield::FindFlag(uint32_t hash)
  {
    for (uint32_t i = 0; i < GetNumFlags(); i++)
    {
      BitfieldFlag* flag = (*this)[i];
      if (flag->GetNameHash() == hash)
      {
        return flag;
      }
    }
    return 0;
  }

  Tag*
  Bitfield::GetTags() const
  {
    if ( CHECK_OFFSET( Bitfield, m_Tags, m_Size ) )
    {
      return &*m_Tags;
    }
    return 0;
  }

  Tag*
  Bitfield::GetTag( uint32_t type ) const
  {
    return FindTag( GetTags(), type );
  }

  uint32_t
  StructBitfieldValue::GetCount() const
  {
    if (CHECK_OFFSET(StructBitfieldValue, m_Count, m_Size))
    {
      return m_Count;
    }
    return 0;
  }

  uint32_t
  StructBitfieldValue::GetHash(uint32_t index) const
  {
    ASSERT(index < GetCount());
    uint32_t* hashes = (uint32_t*)((uint8_t*)this + m_Size);
    return hashes[index];
  }

  uint32_t
  StructValueInfo::GetNameHash() const
  {
    if (CHECK_OFFSET(StructValueInfo, m_NameHash, m_Size))
    {
      return m_NameHash;
    }
    return 0;
  }

  Type
  StructValueInfo::GetType() const
  {
    if (CHECK_OFFSET(StructValueInfo, m_Type, m_Size))
    {
      return (Type)m_Type;
    }
    return (Type)0xffffffff;
  }

  uint32_t
  StructValueInfo::GetTypeNameHash() const
  {
    if (CHECK_OFFSET(StructValueInfo, m_TypeNameHash, m_Size))
    {
      return m_TypeNameHash;
    }
    return 0;
  }

  const char*
  StructValueInfo::GetTypeName() const
  {
    if (CHECK_OFFSET(StructValueInfo, m_TypeName, m_Size))
    {
      return &*m_TypeName;
    }
    return 0;
  }

  ArrayType
  StructValueInfo::GetArrayType() const
  {
    if (CHECK_OFFSET(StructValueInfo, m_ArrayType, m_Size))
    {
      return (ArrayType)m_ArrayType;
    }
    return (ArrayType)0xffffffff;
  }

  StructFieldValue*
  StructValueInfo::GetValue() const
  {
    if (CHECK_OFFSET(StructValueInfo, m_Value, m_Size))
    {
      return &*m_Value;
    }
    return 0;
  }

  Tag*
  StructValueInfo::GetTags() const
  {
    if (CHECK_OFFSET(StructValueInfo, m_Tags, m_Size))
    {
      return &*m_Tags;
    }
    return 0;
  }

  Tag*
  StructValueInfo::GetTag( uint32_t type ) const
  {
    return FindTag( GetTags(), type );
  }

  uint32_t
  StructValueInfo::GetKeyType() const
  {
    if ( CHECK_OFFSET( StructValueInfo, m_KeyType, m_Size ) )
    {
      return m_KeyType;
    }
    return 0xffffffff;
  }

  int
  StructValueInfo::GetKeyBitSize() const
  {
    if ( CHECK_OFFSET( StructValueInfo, m_KeyType, m_Size ) )
    {
      switch ( m_KeyType )
      {
      case kFloat64:
      case kInt64:
      case kTuid:
      case kUint64:
        return 64;

      default:
        return 32;
      }
    }

    return 64;
  }

  bool
  StructValueInfo::AllowSubStruct() const
  {
    for(Tag* tag = m_Tags.Get(); tag != 0; tag = tag->GetNext())
    {
      if(tag->GetType() == kGeneric)
      {
        GenericTag * gentag = (GenericTag *)tag;
        if(gentag->GetNumValues() == 0 && !strcmp(gentag->m_Name.Get(), "AllowSubstruct"))
        {
          return true;
        }
      }
    }

    return false;
  }

  Aggregate*
  StructValueInfo::GetAggregate(Definition* ddl) const
  {
    return ddl->FindAggregate(m_TypeNameHash);
  }

  uint32_t
  StructValueInfo::GetCount() const
  {
    return m_ArrayType == kScalar ? 1 : m_ArrayType == kFixed ? m_Count : 0;
  }

  uint32_t
  StructValueInfo::GetSchemaCrc() const
  {
    uint32_t crc = 0;

    crc = GetNameHash();
    crc ^= GetTypeNameHash();
    crc += GetArrayType();

    return crc;
  }

  uint32_t
  StructStructValue::GetCount() const
  {
    if (CHECK_OFFSET(StructStructValue, m_Count, m_Size))
    {
      return m_Count;
    }
    return 0;
  }

  StructValueInfo*
  StructStructValue::GetValueInfo(uint32_t index) const
  {
    ASSERT(index < GetCount());
    StructValueInfo* values = (StructValueInfo*)((uint8_t*)this + m_Size);
    return &values[index];
  }

  uint32_t
  StructUnknownValue::GetCount() const
  {
    if (CHECK_OFFSET(StructUnknownValue, m_Count, m_Size))
    {
      return m_Count;
    }
    return 0;
  }

  const char*
  StructUnknownValue::GetId(uint32_t index) const
  {
    ASSERT(index < GetCount());
    String* ids = (String*)((uint8_t*)this + m_Size);
    return &*ids[index];
  }

  StructValueInfo*
  StructField::GetValueInfo()
  {
    if (CHECK_OFFSET(StructField, m_ValueInfo, m_Size))
    {
      return &m_ValueInfo;
    }
    return 0;
  }

  uint32_t
  Struct::GetNumFields() const
  {
    uint32_t num_fields = 0;
    if (CHECK_OFFSET(Struct, m_NumFields, m_Size))
    {
      num_fields = m_NumFields;
    }
    Struct* parent = GetParent();
    if (parent != 0)
    {
      num_fields += parent->GetNumFields();
    }
    return num_fields;
  }

  Struct*
  Struct::GetParent() const
  {
    if (CHECK_OFFSET(Struct, m_Parent, m_Size))
    {
      return &*m_Parent;
    }
    return 0;
  }

  Tag*
  Struct::GetTags() const
  {
    if (CHECK_OFFSET(Struct, m_Tags, m_Size))
    {
      return &*m_Tags;
    }
    return 0;
  }

  Tag*
  Struct::GetTag(uint32_t type) const
  {
    return FindTag( GetTags(), type );
  }

  Definition*
  Struct::GetDefinition() const
  {
    if (CHECK_OFFSET(Struct, m_Definition, m_Size))
    {
      return &*m_Definition;
    }
    return 0;
  }

  StructField*
  Struct::operator[](unsigned int index)
  {
    Struct* parent = GetParent();
    if (parent != 0)
    {
      uint32_t parent_num_fields = parent->GetNumFields();
      if (index < parent_num_fields)
      {
        return (*parent)[index];
      }
      index -= parent_num_fields;
    }
    ASSERT(CHECK_OFFSET(Struct, m_NumFields, m_Size) && index < m_NumFields);
    StructFieldPtr* fields = (StructFieldPtr*)((uint8_t*)this + m_Size);
    return &*fields[index];
  }

  const StructField*
  Struct::operator[](unsigned int index) const
  {
    return (const StructField*) const_cast<Struct*>(this)->operator[](index);
  }
  
  StructField*
  Struct::FindField(const char* name)
  {
    return FindField(StringCrc32(name));
  }

  StructField*
  Struct::FindField(uint32_t hash)
  {
    for (uint32_t i = 0; i < GetNumFields(); i++)
    {
      StructField* field = (*this)[i];
      if (field->m_ValueInfo.GetNameHash() == hash)
      {
        return field;
      }
    }
    return 0;
  }

  bool
  Struct::IsInherited(StructField* field)
  {
    Struct* parent = GetParent();
    if (parent != 0 && parent->FindField(field->GetValueInfo()->GetNameHash()) != 0)
    {
      return true;
    }
    return false;
  }

  uint32_t
  Struct::GetSchemaCrc() const
  {
    uint32_t crc = 0;

    for ( uint32_t i = 0; i < GetNumFields(); ++i )
    {
      const StructField* field = (*this)[ i ];
      const uint32_t field_crc = field->m_ValueInfo.GetSchemaCrc();

      crc = Crc32WithSeed( &field_crc, 4, crc );
      crc += i;
    }

    return crc;
  }

  uint32_t
  Definition::GetNumAggregates() const
  {
    if (CHECK_OFFSET(Definition, m_NumAggregates, m_Size))
    {
      return m_NumAggregates;
    }
    return 0;
  }

  uint32_t
  Definition::GetTotalSize() const
  {
    if (CHECK_OFFSET(Definition, m_TotalSize, m_Size))
    {
      return m_TotalSize;
    }
    return 0;
  }

  Definition*
  Definition::FromBinRep(void* bin_rep)
  {
    return (Definition*)bin_rep;
  }

  Aggregate*
  Definition::operator[](unsigned int index)
  {
    ASSERT(index < m_NumAggregates);
    AggregatePtr* aggregates = (AggregatePtr*)((uint8_t*)this + m_Size);
    return &*aggregates[index];
  }

  Aggregate*
  Definition::GetAggregate(unsigned int index)
  {
    ASSERT(index < m_NumAggregates);
    AggregatePtr* aggregates = (AggregatePtr*)((uint8_t*)this + m_Size);
    return &*aggregates[index];
  }

  Aggregate*
  Definition::FindAggregate(const char* name)
  {
    return FindAggregate(StringCrc32(name));
  }

  Aggregate*
  Definition::FindAggregate(uint32_t hash)
  {
    for (uint32_t i = 0; i < m_NumAggregates; i++)
    {
      Aggregate* aggregate = (*this)[i];
      if (aggregate->GetNameHash() == hash)
      {
        return aggregate;
      }
    }
    return 0;
  }

  Definition* Compile( LinearAllocator* definition, LinearAllocator* scratch, const void* source, size_t source_size, char* error, size_t error_size, bool two_us_reserved, int bitfield_limit )
  {
    bool ok = Parser::Parse( definition, scratch, source, (uint32_t)source_size, error, (uint32_t)error_size, two_us_reserved, bitfield_limit );

    if ( ok )
    {
      return Definition::FromBinRep( definition->GetBase() );
    }

    return 0;
  }
};
