#pragma once

#include <DDLParser.h>

#include "Lexer.h"
#include "BlockAllocator.h"
#include "Value.h"
#include "AreaManager.h"
#include "StringArea.h"
#include "GrowableArray.h"
#include "Set.h"
#include "Stack.h"

namespace DDLParser
{
  // Class wrapping a SetLight32 used to call SetLight32.Destroy when the instance goes out of scope.
  class TagSet
  {
  private:
      uint32_t* m_Elements;
      size_t    m_MaxSizeInElements;
      size_t    m_Count;

  public:
    // Initialize the set with the given buffer and debug name.
    inline TagSet( void* buffer, uint32_t buffer_size_in_bytes ): m_Elements( (uint32_t*)buffer ), m_MaxSizeInElements( buffer_size_in_bytes / 4 ), m_Count( 0 ) {}

    bool Insert( uint32_t element )
    {
      if ( Exists( element ) || m_Count == m_MaxSizeInElements )
      {
        return false;
      }

      m_Elements[ m_Count++ ] = element;
      return true;
    }

    bool Exists( uint32_t element )
    {
      for ( size_t i = 0; i < m_Count; i++ )
      {
        if ( m_Elements[ i ] == element )
        {
          return true;
        }
      }

      return false;
    }
  };

  typedef bool CheckStructTag( Definition* def, Struct* structure, GenericTag* tag, Str* tag_name, TagSet* tag_set, char* error, size_t error_size );
  typedef bool CheckFieldTag( Definition* def, Struct* structure, StructField* field, GenericTag* tag, Str* tag_name, TagSet* tag_set, char* error, size_t error_size );
  typedef bool CheckSelectTag( Definition* def, Select* select, GenericTag* tag, Str* tag_name, TagSet* tag_set, char* error, size_t error_size );
  typedef bool CheckItemTag( Definition* def, Select* select, SelectItem* item, GenericTag* tag, Str* tag_name, TagSet* tag_set, char* error, size_t error_size );
  typedef bool CheckBitfieldTag( Definition* def, Bitfield* bitfield, GenericTag* tag, Str* tag_name, TagSet* tag_set, char* error, size_t error_size );
  typedef bool CheckFlagTag( Definition* def, Bitfield* bitfield, BitfieldFlag* flag, GenericTag* tag, Str* tag_name, TagSet* tag_set, char* error, size_t error_size );

  class Parser
  {
    public:
      bool Init( LinearAllocator* definition, LinearAllocator* scratch );
      void Destroy();

      // Sets callbacks to validate generic tags. Callbacks must return true if the tag is validated,
      // or false if there's a problem, and in this case the callback must fill the error field with
      // the error message.
      void SetStructTagCallback( CheckStructTag callback );
      void SetFieldTagCallback( CheckFieldTag callback );
      void SetSelectTagCallback( CheckSelectTag callback );
      void SetItemTagCallback( CheckItemTag callback );
      void SetBitfieldTagCallback( CheckBitfieldTag callback );
      void SetFlagTagCallback( CheckFlagTag callback );

      // Parse the tokens array. If bitfield_limit is not 0, bitfields can have at most bitfield_limit
      // flags. If there's an error during the parsing, the error parameter is set to point to the
      // error message.
      bool Parse(GrowableArray< Token >& tokens, int bitfield_limit, char*& error);

      // Helper method to directly translate a source code into a Definition. The two_us_reserved
      // parameter is explained in Lexer.h.
      static bool Parse( LinearAllocator* definition, LinearAllocator* scratch, const void* source, uint32_t size, char* error, uint32_t error_size, bool two_us_reserved, int bitfield_limit );

    private:
      // Matches the current token in the tokens array, whatever it is.
      bool     Match();
      // Matches the current token in the tokens array if it has the token_id ID.
      bool     Match(int token_id);

      // Find an aggregate in the definition being constructed. Good to find double declarations of the
      // same aggregates.
      Aggregate* FindAggregate(const Str& name) const;
      // Ditto but takes a hash instead of a string.
      Aggregate* FindAggregate(uint32_t hash) const;

      // Parses the source code. Returns true on success. All Parse* methods return true on success.
      bool     ParseDDL();

      // Parses a generic tag.
      bool     ParseGenericTag( Pointer< GenericTag >& result, Area* value_area, StringArea& str_area, Str* tag_name );
      // Parses any phrase in the form of "keyword(string)".
      bool     ParseInfo(TokenID what, DDLParser::String& string, StringArea& str_area);
      // Parses the tags of an aggregate.
      bool     ParseAggregateInfo(Pointer<Aggregate> aggregate, StringArea& str_area);

      // Parses the tags of a select item.
      bool     ParseItemInfo(Pointer<Select> select, Pointer<SelectItem> item, Area* value_area, StringArea& str_area);
      // parses a select item.
      bool     ParseItem(Pointer<SelectItem>& item, Pointer<Select> select, Area* item_area, Area* value_area, StringArea& str_area);
      // Parses the tags of a select.
      bool     ParseSelectInfo(Pointer<Select> select, Area* value_area, StringArea& str_area);
      // Parses a select.
      bool     ParseSelect(AreaManager& bin_rep);

      // Parses the "value" tag of a bitfield flag.
      bool     ParseFlagValue(Pointer<BitfieldFlagValue>& result, Pointer<Bitfield> bitfield, Area* value_area);
      // Parses the tags of a bitfield flag.
      bool     ParseFlagInfo(Pointer<Bitfield> bitfield, Pointer<BitfieldFlag> flag, Area* value_area, StringArea& str_area);
      // Parses a bitfield flag.
      bool     ParseFlag(Pointer<BitfieldFlag>& flag, Pointer<Bitfield> bitfield, Area* flag_area, Area* value_area, StringArea& str_area);
      // Parses the tags of a bitfield.
      bool     ParseBitfieldInfo(Pointer<Bitfield> bitfield, Area* value_area, StringArea& str_area);
      // Parses a bitfield.
      bool     ParseBitfield(AreaManager& bin_rep);

      // Parses the native value of a structure field.
      bool     ParseFieldNativeValue(Pointer<StructFieldValue>& result, Type type, Area* value_area);
      // Parses the string value of a field.
      bool     ParseFieldStrValue(Pointer<StructFieldValue>& result, Area* value_area, StringArea& str_area);
      // Parses the file value of a field. The difference from this method to ParseFieldStrValue is that
      // ParseFieldFileValue calls FilePathCleanName before storing the string in the StringArea.
      bool     ParseFieldFileValue(Pointer<StructFieldValue>& result, Area* value_area, StringArea& str_area);
      // Parses the JSON value of a field.
      bool     ParseFieldJsonValue(Pointer<StructFieldValue>& result, Area* value_area, StringArea& str_area);
      // Parses the select value of a field.
      bool     ParseFieldSelectValue(Pointer<StructFieldValue>& result, Select* select, Area* value_area);
      // Parses the bitfield value of a field.
      bool     ParseFieldBitfieldValue(Pointer<StructFieldValue>& result, Bitfield* bitfield, Area* value_area, Area* bv_area);
      // Parses the unknown value of a field.
      bool     ParseFieldUnknownValue(Pointer<StructFieldValue>& result, Area* value_area, Area* uv_area, StringArea& str_area);
      // Parses the structure value of a field.
      bool     ParseFieldStructValue(Pointer<StructFieldValue>& result, Pointer<StructValueInfo> value_info, Area* value_area, Area* sv_area, StringArea& str_area);
      // Parses the "value" tag of a field.
      bool     ParseFieldValue(Pointer<StructValueInfo> value_info, Area* value_area, Area* valueptr_area, StringArea& str_area);
      // Parses the "extensions" tag of a file field.
      bool     ParseFileExtensions(Pointer<Extensions>& result, Area* value_area, StringArea& str_area);
      // Parses the "vaulthints" tag of a file field.
      bool     ParseFileVaultHints(Pointer<VaultHints>& result, Area* value_area, StringArea& str_area);
      // Parses the "range" tag of a numeric field.
      bool     ParseFieldRange(Type type, Pointer<UIRange>& result, Area* value_area);

      // Parses the tags of a structure field.
      bool     ParseFieldInfo(Pointer<Struct> structure, Pointer<StructField> field, Area* value_area, Area* valueptr_area, StringArea& str_area);
      // Parses the type of a field.
      bool     ParseFieldType(Pointer<StructValueInfo> value_info, Area* vi_area, StringArea& str_area);
      // Parses a structure field.
      bool     ParseField(Pointer<StructField>& field, Pointer<Struct> structure, Area* field_area, Area* value_area, Area* valueptr_area, StringArea& str_area);
      // Parses the tags of a structure.
      bool     ParseStructInfo(Pointer<Struct> structure, Area* value_area, StringArea& str_area);
      // Parses a structure.
      bool     ParseStruct(AreaManager& bin_rep);

      // Parses a typedef.
      bool     ParseTypedef( AreaManager& bin_rep );

      // Parses an expression. This just calls ParseTernary. The methods that parse an expression appear
      // in order of precedence, from highest to lowest.
      bool     ParseExpression();
      // Parses the ternary (?) operator.
      bool     ParseTernary();
      // Parses a logical or (||)
      bool     ParseLogicOr();
      // Parses a logical and (&&)
      bool     ParseLogicAnd();
      // Parses an arithmetic or (|)
      bool     ParseArithmeticOr();
      // Parses an arithmetic xor (^)
      bool     ParseArithmeticXor();
      // Parses an arithmentic and (&)
      bool     ParseArithmeticAnd();
      // Parses (in)equality operators (== and !=)
      bool     ParseEquality();
      // Parses conditional operators (<, <=, > and >=)
      bool     ParseConditional();
      // Parses shift operators (<< and >>)
      bool     ParseShift();
      // Parses additions and subtractions.
      bool     ParseTerm();
      // Parses multiplications, divisions and modulus.
      bool     ParseFactor();
      // Parses unary operators (+, -, ~ and !)
      bool     ParseUnary();
      // Parses terminals (number, string and boolean constants, or an expression
      // inside parenthesis.
      bool     ParseTerminal();

      // Gets the integer value of a token. The token ID must be tBinary, tOctal, tDecimal
      // or tHexadecimal. The function does not test for errors.
      int64_t  ToInt(const Token& token);

      // Pushes a value into the expression stack.
      bool     PushValue(const Value& value);
      // Pops a value from the expression stack.
      Value    PopValue();

      // The AreaManager used to build the Definition of the source code.
      AreaManager      m_BinRep;
      // The Area representing the definition header.
      Area*            m_DDLArea;
      // The Area representing the aggregates area.
      Area*            m_AggregatesArea;
      // The proper Definition of the m_BinRep AreaManager.
      Definition* m_DDL;
      // The current token in the tokens array.
      const Token*     m_Current;
      GrowableArray< Token >::Iterator m_TokenIterator;
      // The expression parser stack.
      Stack< Value >   m_Stack;
      // The bitfield flag limit.
      int               m_BitfieldLimit;
      // Buffer to hold error messages.
      char m_Error[256];
      // Callbacks to validate tags.
      CheckStructTag*   m_CheckStructTag;
      CheckFieldTag*    m_CheckFieldTag;
      CheckSelectTag*   m_CheckSelectTag;
      CheckItemTag*     m_CheckItemTag;
      CheckBitfieldTag* m_CheckBitfieldTag;
      CheckFlagTag*     m_CheckFlagTag;

      // A typedef.
      struct Typedef
      {
        GrowableArray< Token >::Iterator m_Iterator;
      };

      // The set that holds defined typedefs.
      Set< Typedef > m_Typedefs;

      // The set that holds used identifiers and the places where they were declared.
      Set< Token > m_UsedIdentifiers;
  };
};
