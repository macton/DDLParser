#pragma once

#include <stdint.h>
#include <stdlib.h>

// A relative pointer. The final pointer is computed as the sum of
// (uint8_t*)this and m_Offset. The overloaded operators makes it easy
// to use the pointer as a real pointer.

// The intention was to use a template, but Visual C++ didn't like it
// in the Struct::Value union.
#define DEF_POINTER(name, type)            \
  struct name                              \
  {                                        \
    type* operator->() const;              \
    type& operator*() const;               \
    type* Get() const;                     \
    void  operator=(const type* pointer);  \
    bool  operator==(void* pointer) const; \
    bool  operator!=(void* pointer) const; \
    Offset m_Offset;                       \
  }

namespace DDLParser
{
  class LinearAllocator
  {
  public:
    virtual bool   Init( size_t max_size_in_bytes ) = 0;
    virtual void   Destroy() = 0;
    virtual void*  Allocate( size_t size_in_bytes, size_t alignment_in_bytes = 1 ) = 0;
    virtual void*  GetBase() const = 0;
    virtual size_t GetCurrentOffset() const = 0;

    inline void* SafeAllocate( size_t size_in_bytes, size_t alignment_in_bytes = 1 )
    {
      void* ptr = Allocate( size_in_bytes, alignment_in_bytes );

      if ( ptr )
      {
        return ptr;
      }

      throw "Out of memory";
    }
  };
  
#define kCrcSeedDefaultU32 0xEDB88320

  uint32_t Crc32WithSeed(const void* data, int len, uint32_t crc);

  inline uint32_t Crc32(const void* data, int len)
  {
    return Crc32WithSeed(data, len, kCrcSeedDefaultU32);
  }

  uint32_t StringCrc32WithSeed(const char* data, uint32_t crc);

  inline uint32_t StringCrc32(const char* data)
  {
    return StringCrc32WithSeed(data, kCrcSeedDefaultU32);
  }

  // The DDL types. New types only allowed appended at the end.
  enum Type
  {
    kUint8 = 0,
    kUint16,   // 1
    kUint32,   // 2
    kUint64,   // 3
    kInt8,     // 4
    kInt16,    // 5
    kInt32,    // 6
    kInt64,    // 7
    kFloat32,  // 8
    kFloat64,  // 9
    kString,   // 10
    kSelect,   // 11
    kBitfield, // 12
    kStruct,   // 13
    kUnknown,  // 14
    kBoolean,  // 15
    kFile,     // 16
    kTuid,     // 17
    kJson,     // 18
    
    kTypeMax
  };

  // The different array types.
  enum ArrayType
  {
    kScalar = 0, // Not an array.
    kFixed,      // Fixed-sized array.
    kDynamic,
    kHashmap,    // Hashmap.

    kArrayTypeMax
  };

  // The type of relative pointer offsets.
  typedef int32_t Offset;

  // The types of tag in the m_Tags members.
  enum TagType
  {
    kExtensions = 0,
    kVaultHints,
    kUIRange,
    kUIRender,
    kParallel,
    kVersion,
    kCallback,
    kKey,
    kUnits,
    kAbstract,
    kGeneric
  };

  // The default string size if no size is given.
  const int kDefaultStrSize = 64;

  // A string is just a relative pointer to the first character and is
  // null-terminated.
  DEF_POINTER(String, char);

  // Relative void pointer. We can't define a void relative pointer with
  // "void" as the underlining type because pointer deferencing in
  // DEF_POINTER and IMP_POINTER. This is not a problem since it will be
  // cast to another type anyway.
  typedef String VoidPtr;

  // Other relative pointers.
  struct Definition;
  DEF_POINTER(DefinitionPtr, Definition);
  struct Tag;
  DEF_POINTER(TagPtr, Tag);
  struct GenericTagValue;
  DEF_POINTER( GenericTagValuePtr, GenericTagValue );
  struct GenericTag;
  DEF_POINTER( GenericTagPtr, GenericTag );
  struct Aggregate;
  DEF_POINTER(AggregatePtr, Aggregate);
  struct SelectItem;
  DEF_POINTER(SelectItemPtr, SelectItem);
  struct BitfieldFlagValue;
  DEF_POINTER(BitfieldFlagValuePtr, BitfieldFlagValue);
  struct BitfieldFlag;
  DEF_POINTER(BitfieldFlagPtr, BitfieldFlag);
  struct Struct;
  DEF_POINTER(StructPtr, Struct);
  struct StructBitfieldValue;
  DEF_POINTER(StructBitfieldValuePtr, StructBitfieldValue);
  union StructFieldValue;
  DEF_POINTER(StructFieldValuePtr, StructFieldValue);
  struct StructValueInfo;
  DEF_POINTER(StructValueInfoPtr, StructValueInfo);
  struct StructStructValue;
  DEF_POINTER(StructStructValuePtr, StructStructValue);
  struct StructUnknownValue;
  DEF_POINTER(StructUnknownValuePtr, StructUnknownValue);
  struct StructField;
  DEF_POINTER(StructFieldPtr, StructField);
  struct StructFieldRange;
  DEF_POINTER(StructFieldRangePtr, StructFieldRange);

  // BasicInfo is used in all aggregates and aggregate items.
  struct BasicInfo
  {
    uint32_t m_Size;
    String   m_Name;
    String   m_Author;
    String   m_Description;
    String   m_Label;

    const char* GetName() const;
    const char* GetAuthor() const;
    const char* GetDescription() const;
    const char* GetLabel() const;
    const char* GetDisplayLabel() const;
  };

  struct Tag
  {
    uint32_t m_Size;
    uint32_t m_Type;
    TagPtr   m_Next;

    uint32_t GetType() const;
    Tag*     GetNext() const;
  };

  // The extensions accepted by a file.
  struct Extensions: public Tag
  {
    uint32_t m_Count;
    String   m_Extension[ 0 ];

    uint32_t    GetCount() const;
    const char* GetExtension( uint32_t index ) const;
  };

  // The vaulthints accepted by a file.
  struct VaultHints: public Tag
  {
    uint32_t m_Count;
    String   m_VaultHint[ 0 ];

    uint32_t    GetCount() const;
    const char* GetVaultHint( uint32_t index ) const;
  };

  // Possible values of the minimum and maximum values of a range.
  union UIRangeValue
  {
    uint8_t  m_Uint8;
    uint16_t m_Uint16;
    uint32_t m_Uint32;
    uint64_t m_Uint64;
    int8_t   m_Int8;
    int16_t  m_Int16;
    int32_t  m_Int32;
    int64_t  m_Int64;
    float    m_Float32;
    double   m_Float64;
  };

  // The range of accepted values for numeric fields.
  struct UIRange: public Tag
  {
    UIRangeValue m_SoftMin;
    UIRangeValue m_SoftMax;
    UIRangeValue m_HardMin;
    UIRangeValue m_HardMax;
    UIRangeValue m_Step;

    UIRangeValue* GetSoftMin();
    UIRangeValue* GetSoftMax();
    UIRangeValue* GetHardMin();
    UIRangeValue* GetHardMax();
    UIRangeValue* GetStep();
  };

  // The string specifying how to render a structure or field.
  struct UIRender: public Tag
  {
    String m_Render;

    const char* GetRender() const;
  };

  // The dynamic array controlling this array.
  struct Parallel: public Tag
  {
    StructFieldPtr m_Array;

    StructField* GetArray() const;
  };

  // The version of the structure.
  struct Version: public Tag
  {
    String m_Version;

    const char* GetVersion() const;
  };

  // Callback function name
  struct Callback: public Tag
  {
    String m_Callback;

    const char* GetCallback() const;
  };

  // Key function name
  struct Key: public Tag
  {
    String m_Key;

    const char* GetKey() const;
  };

  // Attach units to a value
  struct Units: public Tag
  {
    String m_Units;

    const char* GetUnits() const;
  };

  // Generic tag
  struct GenericTagValue
  {
    uint32_t m_Type;

    union
    {
      int64_t m_Int64;
      double  m_Float64;
      String  m_String;
    };
  };

  struct GenericTag: public Tag
  {
    String   m_Name;
    uint32_t m_NameHash;
    uint32_t m_NumValues;

    GenericTagValue m_Values[ 0 ];

    const char*      GetName() const;
    uint32_t         GetNameHash() const;
    uint32_t         GetNumValues() const;
    GenericTagValue* operator[]( unsigned int index ) const;
  };

  // Forward declare.
  struct Select;
  struct Bitfield;
  struct Struct;

  // Aggregate is the super class of selects, bitfields and structures.
  struct Aggregate
  {
    // The size of the aggregate structure.
    uint32_t m_Size;
    // Can be only kSelect, kBitfield and kStruct.
    uint32_t m_Type;
    // The name of the aggregate.
    String   m_Name;
    // Its author.
    String   m_Author;
    // Its description; can be used as a tool-tip in the UI.
    String   m_Description;
    // Its label; also to be used in the UI.
    String   m_Label;
    // The name's hash.
    uint32_t m_NameHash;

    // These methods are safe ways to get information from the aggregate
    // because they take into account the size of the structure.
    // uint32_t GetSize() const; m_Size is guarateed to be always present.
    uint32_t    GetType() const;
    const char* GetName() const;
    const char* GetAuthor() const;
    const char* GetDescription() const;
    const char* GetLabel() const;
    const char* GetDisplayLabel() const;
    uint32_t    GetNameHash() const;

    // These methods return the aggregate cast to a specialized class. They
    // assert that the aggregate is convertible to the desired class.
    Select*   ToSelect() const;
    Bitfield* ToBitfield() const;
    Struct*   ToStruct() const;
  };

  // Select items only contain information available in BasicInfo.
  struct SelectItem: public BasicInfo
  {
    uint32_t m_NameHash;
    TagPtr   m_Tags;

    uint32_t GetNameHash() const;
    // Returns a pointer for the first tag.
    Tag*     GetTags() const;
    // Returns the first tag with the given type.
    Tag*     GetTag( uint32_t type ) const;
  };

  struct Select: public Aggregate
  {
    // Number of items in the select.
    uint32_t      m_NumItems;
    // The index of the default item, or -1 if no default was assigned.
    int32_t       m_DefaultItem;
    // Tags.
    TagPtr        m_Tags;
    // The items.
    SelectItemPtr m_Items[0];

    uint32_t GetNumItems() const;
    int32_t  GetDefaultItem() const;

    // Returns an item given its index.
    SelectItem* operator[](unsigned int index);
    // Returns an item given its name.
    SelectItem* FindItem(const char* name);
    // Returns an item given its hash. All return NULL if item wasn't found.
    SelectItem* FindItem(uint32_t hash);
    // Returns a pointer for the first tag.
    Tag*        GetTags() const;
    // Returns the first tag with the given type.
    Tag*        GetTag( uint32_t type ) const;
  };

  // Flag values are sets of zero or more flags or'ed toghether.
  struct BitfieldFlagValue
  {
    uint32_t m_Size;
    uint32_t m_Count;
    uint32_t m_FlagIndex[0];

    uint32_t GetCount() const;
    uint32_t GetFlagIndex(uint32_t index) const;
  };

  // A flag is the BasicInfo plus a relative pointer to its value.
  struct BitfieldFlag: public BasicInfo
  {
    uint32_t             m_NameHash;
    BitfieldFlagValuePtr m_Value;
    uint32_t             m_Bit;
    TagPtr               m_Tags;

    uint32_t           GetNameHash() const;
    BitfieldFlagValue* GetValue() const;
    uint32_t           GetBit() const;
    // Returns a pointer for the first tag.
    Tag*               GetTags() const;
    // Returns the first tag with the given type.
    Tag*               GetTag( uint32_t type ) const;
  };

  struct Bitfield: public Aggregate
  {
    // Number of flags in the bitfield.
    uint32_t     m_NumFlags;
    // The index of the default flag, or -1 if no default was assigned.
    int32_t      m_DefaultFlag;
    // Tags.
    TagPtr       m_Tags;
    // The flags.
    BitfieldFlagPtr m_Flags[0];

    uint32_t GetNumFlags() const;
    int32_t  GetDefaultFlag() const;

    // Returns a flag given its index.
    BitfieldFlag* operator[](unsigned int index);
    // Returns a flag given its name.
    BitfieldFlag* FindFlag(const char* name);
    // Returns a flag given its hash. All return NULL if the flag wasn't found.
    BitfieldFlag* FindFlag(uint32_t hash);
    // Returns a pointer for the first tag.
    Tag*          GetTags() const;
    // Returns the first tag with the given type.
    Tag*          GetTag( uint32_t type ) const;
  };

  // Forward declare.
  struct Definition;

  // The type of a default bitfield value.
  struct StructBitfieldValue
  {
    uint32_t m_Size;
    uint32_t m_Count;
    uint32_t m_Hashes[0];

    uint32_t GetCount() const;
    uint32_t GetHash(uint32_t index) const;
  };

  // Forward declare.
  union StructFieldValue;

  // Helper struct to provide information for both fields and default field
  // values.
  struct StructValueInfo
  {
    uint32_t            m_Size;
    // Hash of the field name.
    uint32_t            m_NameHash;
    // Type of the field as declared in DDL::Type.
    uint32_t            m_Type;
    union
    {
      uint32_t          m_TypeNameHash; // for selects, bitfields and structures
      String            m_TypeName;     // for unknown types
    };
    // The type of the array.
    uint32_t            m_ArrayType;
    // Number of values in the array, 0 = variable-sized array, >0 = fixed-sized
    // array.
    uint32_t            m_Count;
    // Relative pointer to the array of values.
    StructFieldValuePtr m_Value;
    // Relative pointer to the tags.
    TagPtr              m_Tags;
    // Key type of the hashmap.
    uint32_t            m_KeyType;

    uint32_t          GetNameHash() const;
    Type              GetType() const;
    uint32_t          GetTypeNameHash() const;
    const char*       GetTypeName() const;
    ArrayType         GetArrayType() const;
    StructFieldValue* GetValue() const;
    Tag*              GetTags() const;
    Tag*              GetTag(uint32_t type) const;
    uint32_t          GetKeyType() const;
    int               GetKeyBitSize() const;
    bool              AllowSubStruct() const;
    uint32_t          GetSchemaCrc() const;

    Aggregate*  GetAggregate(Definition* ddl) const;
    uint32_t    GetCount() const;
  };

  // The type of a default field value.
  struct StructStructValue
  {
    uint32_t        m_Size;
    // Number of fields with default values.
    uint32_t        m_Count;
    // The information about each field.
    StructValueInfo m_Values[0];

    uint32_t         GetCount() const;
    StructValueInfo* GetValueInfo(uint32_t index) const;
  };

  // The type of a default unknown value.
  struct StructUnknownValue
  {
    uint32_t m_Size;
    // The number of strings in the value.
    uint32_t m_Count;
    // Relative pointers to the strings.
    String   m_Ids[0];

    uint32_t    GetCount() const;
    const char* GetId(uint32_t index) const;
  };

  // Possible values of a field. If the field is *not* an array, only index
  // zero is valid.
  union StructFieldValue
  {
    uint8_t                m_Uint8[0];
    uint16_t               m_Uint16[0];
    uint32_t               m_Uint32[0];
    uint64_t               m_Uint64[0];
    int8_t                 m_Int8[0];
    int16_t                m_Int16[0];
    int32_t                m_Int32[0];
    int64_t                m_Int64[0];
    float                  m_Float32[0];
    double                 m_Float64[0];
    String                 m_String[0];
    uint32_t               m_Select[0];
    StructBitfieldValuePtr m_Bitfield[0];
    StructStructValuePtr   m_Struct[0];
    StructUnknownValuePtr  m_Unknown[0];
    uint8_t                m_Boolean[0];
    String                 m_File[0];
    uint64_t               m_Tuid[0];
    String                 m_Json[0];
  };

  // A field is BasicInfo plus ValueInfo.
  struct StructField: public BasicInfo
  {
    StructValueInfo m_ValueInfo;
    StructValueInfo* GetValueInfo();
  };

  struct Struct: public Aggregate
  {
    // Number of fields in the structure.
    uint32_t       m_NumFields;
    // The hash of the parent structure, 0 for no parent.
    StructPtr      m_Parent;
    // Relative pointer to the tags.
    TagPtr         m_Tags;
    // The runtime definition containing this struct
    DefinitionPtr  m_Definition;
    // The fields.
    StructFieldPtr m_Fields[0];

    uint32_t    GetNumFields() const;
    Struct*     GetParent() const;
    Tag*        GetTags() const;
    Tag*        GetTag(uint32_t type) const;
    Definition* GetDefinition() const;

    // Returns a field given its index.
    StructField* operator[](unsigned int index);

    const StructField* operator[](unsigned int index) const;
    
    // Returns a field given its name.
    StructField* FindField(const char* name);
    // Returns a field given its hash. All return NULL if the field wasn't
    // found.
    StructField* FindField(uint32_t hash);
    // Returns true if the field in inherited from the parent.
    bool         IsInherited(StructField* field);

    // The schema crc reflects the type, count, name, and order of each field.  If any of those change, then the crc changes.  It does not reflect default values or any tags.
    uint32_t     GetSchemaCrc() const;
  };

  // The header of DDL binary data.
  struct Definition
  {
    // Size of the structure.
    uint32_t     m_Size;
    // One, so we can test if the endianess is correct and swap everything if
    // it isn't.
    uint32_t     m_One;
    // Number of aggregates in the DDL.
    uint32_t     m_NumAggregates;
    // The total size of the definition in bytes.
    uint32_t     m_TotalSize;
    // The aggregates.
    AggregatePtr m_Aggregates[0];

    uint32_t GetNumAggregates() const;
    uint32_t GetTotalSize() const;

    // Returns a DDL from a buffer. No check is done, it just checks for m_One
    // and swaps everything if necessary and returns bin_rep cast to DDL.
    static Definition* FromBinRep(void* bin_rep);

    // Returns an aggregate given its index.
    Aggregate* operator[](unsigned int index);
    Aggregate* GetAggregate(unsigned int index);
    // Returns an aggregate given its name.
    Aggregate* FindAggregate(const char* name);
    // Returns an aggregate given its hash. All return NULL if the aggregate
    // wasn't found.
    Aggregate* FindAggregate(uint32_t hash);
  };

  Definition* Compile( LinearAllocator* definition, LinearAllocator* scratch, const void* source, size_t source_size, char* error, size_t error_size, bool two_us_reserved, int bitfield_limit );
};
