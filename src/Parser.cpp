#include <ctype.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>

#include "Parser.h"
#include "Error.h"
#include "JSONChecker.h"

namespace DDLParser
{
  // Horrible macro to unwind the stack in case of parser errors.
#define CHECK(exp) do { if (!(exp)) return false; } while (0)

  const uint32_t TypeSizes[] =
  {
    /* kUint8    */ sizeof ( uint8_t ),
    /* kUint16   */ sizeof ( uint16_t ),
    /* kUint32   */ sizeof ( uint32_t ),
    /* kUint64   */ sizeof ( uint64_t ),
    /* kInt8     */ sizeof ( int8_t ),
    /* kInt16    */ sizeof ( int16_t ),
    /* kInt32    */ sizeof ( int32_t ),
    /* kInt64    */ sizeof ( int64_t ),
    /* kFloat32  */ sizeof ( float ),
    /* kFloat64  */ sizeof ( double ),
    /* kString   */ sizeof ( char* ),
    /* kSelect   */ sizeof ( uint32_t ),
    /* kBitfield */ sizeof ( uint32_t ),
    /* kStruct   */ sizeof ( void* ),
    /* kUnknown  */ sizeof ( void* ),
    /* kBoolean  */ sizeof ( uint8_t ),
    /* kFile     */ sizeof ( char* ),
    /* kTuid     */ sizeof ( uint64_t ),
    /* kJson     */ sizeof ( void* ),
  };

  const uint32_t TypeAlignments[] =
  {
    /* kUint8    */ 1,
    /* kUint16   */ 2,
    /* kUint32   */ 4,
    /* kUint64   */ 8,
    /* kInt8     */ 1,
    /* kInt16    */ 2,
    /* Kint32    */ 4,
    /* kInt64    */ 8,
    /* kFloat32  */ 4,
    /* kFloat64  */ 8,
    /* kString   */ 4,
    /* kSelect   */ 4,
    /* kBitfield */ 4,
    /* kStruct   */ 8,
    /* kUnknown  */ 4,
    /* kBoolean  */ 1,
    /* kFile     */ 4,
    /* kTuid     */ 8,
    /* kJson     */ 4,
  };

  enum
  {
    kFilePathLenMax = 512,

#if defined( __WIN__ )
    kDirectorySeparator = '\\',
    kNotDirectorySeparator = '/'
#else
    kDirectorySeparator = '/',
    kNotDirectorySeparator = '\\'
#endif
  };

  static size_t FilePathCleanName ( const char* source_file_path, char* output_file_path, size_t output_file_path_max )
  {
    char* write = output_file_path;
    char* write_term = output_file_path + output_file_path_max - 1;
    *write_term = 0;

    while ( write < write_term )
    {
      char c = *source_file_path++;

      // convert forward slashes to back slashes
      if ( c == kNotDirectorySeparator )
      {
        c = kDirectorySeparator;
      }

      // remove double slashes (not including the initial pair of double slashes)
      if ( ( c == kDirectorySeparator ) && ( write > ( output_file_path + 1 ) ) && ( write[-1] == kDirectorySeparator ) )
      {
        write--;
        write_term--;
        *write_term = 0;
      }

      *write++ = c;

      if ( c == 0 )
      {
        break;
      }
    }

    return ( size_t ) ( write - output_file_path );
  }

  bool Parser::Init ( LinearAllocator* definition, LinearAllocator* scratch )
  {
    try
    {
      m_CheckStructTag = 0;
      m_CheckFieldTag = 0;
      m_CheckSelectTag = 0;
      m_CheckItemTag = 0;
      m_CheckBitfieldTag = 0;
      m_CheckFlagTag = 0;
      // Initialize the area manager.
      m_BinRep.Init ( definition, scratch );
      // Initialize the expression stack.
      m_Stack.Init ( scratch );
      // Initialize the typedef hashtable.
      m_Typedefs.Init ( scratch );
      // Initialize the used identifiers hashtable.
      m_UsedIdentifiers.Init ( scratch );

      if ( definition->GetCurrentOffset() == 0 )
      {
        // Create a new area in the area manager to hold the DDL definition header.
        m_DDLArea = m_BinRep.NewArea();
        // Create a new area in the area manager to hold the DDL definition aggregates.
        m_AggregatesArea = m_BinRep.NewArea();
        // Allocate the DDL header.
        Pointer<DDLParser::Definition> ddl = m_DDLArea->Allocate<DDLParser::Definition>();
        // Initialize its fields.
        ddl->m_Size  = sizeof ( DDLParser::Definition );
        ddl->m_One   = 1;
        ddl->m_NumAggregates = 0;
        m_DDL = &*ddl;
      }
      else
      {
        // Add to the already existing definition.
        m_DDL = ( DDLParser::Definition* ) definition->GetBase();
        uint32_t def_size = m_DDL->m_Size + m_DDL->m_NumAggregates * sizeof ( AggregatePtr );
        m_DDLArea = m_BinRep.NewArea ( 0, def_size );
        m_AggregatesArea = m_BinRep.NewArea ( def_size, m_DDL->m_TotalSize - def_size );
        static Token unknown;
        unknown.m_ID = tEOF;

        for ( uint32_t i = 0; i < m_DDL->m_NumAggregates; i++ )
        {
          m_DDLArea->AddRelPointer ( &m_DDL->m_Aggregates[ i ] );
          m_UsedIdentifiers.Insert ( m_DDL->m_Aggregates[ i ]->GetNameHash(), unknown );
        }
      }

      return true;
    }
    catch ( const char* the_error )
    {
      return ErrorThrown ( m_Error, sizeof ( m_Error ), the_error );
    }
  }

  void Parser::Destroy()
  {
    m_UsedIdentifiers.Destroy();
    m_Typedefs.Destroy();
    m_Stack.Destroy();
  }

  void
  Parser::SetStructTagCallback ( CheckStructTag callback )
  {
    m_CheckStructTag = callback;
  }

  void
  Parser::SetFieldTagCallback ( CheckFieldTag callback )
  {
    m_CheckFieldTag = callback;
  }

  void
  Parser::SetSelectTagCallback ( CheckSelectTag callback )
  {
    m_CheckSelectTag = callback;
  }

  void
  Parser::SetItemTagCallback ( CheckItemTag callback )
  {
    m_CheckItemTag = callback;
  }

  void
  Parser::SetBitfieldTagCallback ( CheckBitfieldTag callback )
  {
    m_CheckBitfieldTag = callback;
  }

  void
  Parser::SetFlagTagCallback ( CheckFlagTag callback )
  {
    m_CheckFlagTag = callback;
  }

  bool
  Parser::Parse ( GrowableArray< Token >& tokens, int bitfield_limit, char*& error )
  {
    try
    {
      // Set the error to our internal error buffer.
      error = m_Error;
      // Set the current token to the first token of the tokens list.
      m_TokenIterator = tokens.Iterate();
      m_Current = m_TokenIterator.Next();
      // Empty the expression stack.
      m_Stack.Reset();
      // Save the bitfield flag limit for later.
      m_BitfieldLimit = bitfield_limit;
      // Parse the source code.
      CHECK ( ParseDDL() );
      // Everything went fine, set error to NULL.
      error = NULL;
      // Success.
      return true;
    }
    catch ( const char* the_error )
    {
      return ErrorThrown ( m_Error, sizeof ( m_Error ), the_error );
    }
  }

  bool
  Parser::Parse ( LinearAllocator* definition, LinearAllocator* scratch, const void* source, uint32_t size, char* error, uint32_t error_size, bool two_us_reserved, int bitfield_limit )
  {
    // Create the tokens list.
    GrowableArray< Token > tokens;
    tokens.Init ( scratch );
    // Our error pointer.
    char* the_error;
    {
      // Create the lexer.
      Lexer lexer;

      // Tokenize the source code.
      if ( !lexer.Tokenize ( ( const uint8_t* ) source, size, tokens, two_us_reserved, the_error ) )
      {
        // Error, copy the error message to the supplied error buffer and return.
        strncpy ( error, the_error, error_size );
        return false;
      }
    }
    {
      // Create the parser.
      Parser parser;
      parser.Init ( definition, scratch );

      // Parse the tokens list.
      if ( !parser.Parse ( tokens, bitfield_limit, the_error ) )
      {
        // Error, copy the error message to the supplied buffer and return.
        strncpy ( error, the_error, error_size );
        return false;
      }

      parser.Destroy();
    }
    // Destroy the tokens list.
    tokens.Destroy();
    // Success.
    return true;
  }

  bool
  Parser::Match()
  {
    // If the current token isn't the EOF token (which is the last token in the tokens list)...
    if ( m_Current->m_ID != tEOF )
    {
      // Go to the next token.
      m_Current = m_TokenIterator.Next();
    }

    // Always succeed.
    return true;
  }

  bool
  Parser::Match ( int token )
  {
    // If the current token isn't the one we're trying to match...
    if ( m_Current->m_ID != token )
    {
      // Error out.
      return ErrorUnexpectedToken ( m_Error, sizeof ( m_Error ), *m_Current );
    }

    // Call Match to advance to the next token in the tokens list.
    return Match();
  }

  DDLParser::Aggregate*
  Parser::FindAggregate ( const Str& name ) const
  {
    // Forward the call to use the available hash.
    return FindAggregate ( name.GetHash() );
  }

  DDLParser::Aggregate*
  Parser::FindAggregate ( uint32_t hash ) const
  {
    // Get a pointer to the binary definition.
    DDLParser::Definition* ddl = DDLParser::Definition::FromBinRep ( ( uint8_t* ) m_BinRep.m_Allocator->GetBase() );
    // Try to find the aggregate in it.
    return ddl->FindAggregate ( hash );
  }

  bool
  Parser::ParseDDL()
  {
    // Parse at least one aggregate.
    do
    {
      // Save a pointer to the aggregate we're going to parse for later use.
      uint8_t* aggregate = ( uint8_t* ) m_BinRep.m_Allocator->GetBase() + m_BinRep.m_Allocator->GetCurrentOffset();

      // Parse a select, a bitfield or a structure.
      switch ( m_Current->m_ID )
      {
        case tSelect:
          CHECK ( ParseSelect ( m_BinRep ) );
          break;
        case tBitfield:
          CHECK ( ParseBitfield ( m_BinRep ) );
          break;
        case tStruct:
          CHECK ( ParseStruct ( m_BinRep ) );
          break;
        case tTypedef:
          CHECK ( ParseTypedef ( m_BinRep ) );
          continue; // Do not add an aggregate to the definition.
        default:
          return ErrorUnknownIdentifier ( m_Error, sizeof ( m_Error ), *m_Current );
      }

      // Allocate a new pointer to the aggregate in the definition header.
      m_DDLArea->Allocate<DDLParser::AggregatePtr>();
      // Register the relative pointer.
      m_DDLArea->AddRelPointer ( &m_DDL->m_Aggregates[m_DDL->m_NumAggregates] );
      // Set it to the just-parsed aggregate.
      m_DDL->m_Aggregates[m_DDL->m_NumAggregates] = ( DDLParser::Aggregate* ) ( aggregate + sizeof ( DDLParser::AggregatePtr ) );
      // Increment the number of aggregates in the definition.
      m_DDL->m_NumAggregates++;
    }

    // Repeat as long as we have more selects, bitfields and/or structures.
    while ( m_Current->m_ID == tStruct || m_Current->m_ID == tBitfield || m_Current->m_ID == tSelect || m_Current->m_ID == tTypedef );

    // Save the total size of the definition.
    m_DDL->m_TotalSize = ( uint32_t ) m_BinRep.m_Allocator->GetCurrentOffset();
    // There must be a EOF token after the last aggregate.
    return Match ( tEOF );
  }

  bool
  Parser::ParseGenericTag ( Pointer< DDLParser::GenericTag >& result, Area* type_area, StringArea& str_area, Str* tag_name )
  {
    Area* value_area = type_area->NewArea();
    // Allocate the tag.
    value_area->Align ( 8 );
    result = value_area->Allocate< DDLParser::GenericTag >();
    result->m_Size = sizeof ( DDLParser::GenericTag );
    result->m_Type = DDLParser::kGeneric;
    result->m_Next = NULL;
    // Start parsing the tag.
    CHECK ( Match ( tTag ) );
    CHECK ( Match ( '(' ) );
    // Get the name and its hash.
    result->m_Name = str_area.Add ( m_Current->m_Lexeme );
    value_area->AddRelPointer ( &result->m_Name );
    result->m_NameHash = m_Current->m_Lexeme.GetHash();
    *tag_name = m_Current->m_Lexeme;
    CHECK ( Match ( tID ) );
    // Parse the values.
    result->m_NumValues = 0;

    while ( m_Current->m_ID == ',' )
    {
      result->m_NumValues++;
      // Eat the comma.
      Match();
      // Parse an expression and get the result.
      CHECK ( ParseExpression() );
      Value value = PopValue();
      // Allocate space for a value.
      Pointer< DDLParser::GenericTagValue > tag_value = value_area->Allocate< DDLParser::GenericTagValue >();

      if ( value.GetType() == Value::kString )
      {
        tag_value->m_Type = DDLParser::kString;
        tag_value->m_String = str_area.Add ( value.GetString() );
        value_area->AddRelPointer ( &tag_value->m_String );
      }
      else if ( value.GetType() == Value::kInt )
      {
        tag_value->m_Type = DDLParser::kInt64;
        tag_value->m_Int64 = value.GetInt();
      }
      else // float
      {
        tag_value->m_Type = DDLParser::kFloat64;
        tag_value->m_Float64 = value.GetDouble();
      }
    }

    // Check for the closing parenthesis.
    return Match ( ')' );
  }

  bool
  Parser::ParseInfo ( TokenID what, DDLParser::String& string, StringArea& str_area )
  {
    // Match the token from the what parameter.
    CHECK ( Match ( what ) );
    // Match a left parenthesis.
    CHECK ( Match ( '(' ) );
    // Save and match a string.
    string = str_area.Add ( m_Current->m_Lexeme );
    CHECK ( Match ( tLiteral ) );
    // Match a right parenthesis.
    return Match ( ')' );
  }

  bool
  Parser::ParseAggregateInfo ( Pointer<DDLParser::Aggregate> aggregate, StringArea& str_area )
  {
    // Create a set to avoid parsing duplicate tags.
    TokenID set_buffer[ 64 ];
    TagSet parsed ( set_buffer, sizeof ( set_buffer ) );

    // Parse as many tags as we have in the tokens list.
    for ( ;; )
    {
      // Aggregates' tags can only be autor, description and label.
      switch ( m_Current->m_ID )
      {
        case tAuthor:
        case tDescription:
        case tLabel:
          break;
        default:
          return ErrorInvalidInfo ( m_Error, sizeof ( m_Error ), *m_Current );
      }

      // If we've already parsed the tag, error out.
      if ( parsed.Exists ( m_Current->m_ID ) )
      {
        return ErrorDuplicateInfo ( m_Error, sizeof ( m_Error ), *m_Current );
      }
      else if ( m_Current->m_ID != tTag )
      {
        // Insert the tag into the set except if it's a generic tag
        // (we can have as many of those as we want)
        parsed.Insert ( m_Current->m_ID );
      }

      switch ( m_Current->m_ID )
      {
        case tAuthor:
          // Call ParseInfo to parse the author.
          CHECK ( ParseInfo ( tAuthor, aggregate->m_Author, str_area ) );
          break;
        case tDescription:
          // Call ParseInfo to parse the description.
          CHECK ( ParseInfo ( tDescription, aggregate->m_Description, str_area ) );
          break;
        case tLabel:
          // Call ParseInfo to parse the label.
          CHECK ( ParseInfo ( tLabel, aggregate->m_Label, str_area ) );
          break;
          // Never happens but we don't want VS to give a warning.
        default:
          break;
      }

      // If the next token isn't a comma, break from the loop.
      if ( m_Current->m_ID != ',' )
      {
        break;
      }

      // Otherwise, match the comma and parse another tag.
      Match();
    }

    // Success.
    return true;
  }

  bool
  Parser::ParseItemInfo ( Pointer<DDLParser::Select> select, Pointer<DDLParser::SelectItem> item, Area* value_area, StringArea& str_area )
  {
    // Create a set to avoid parsing duplicate tags.
    TokenID set_buffer[ 64 ];
    TagSet parsed ( set_buffer, sizeof ( set_buffer ) );

    // Parse as many tags as we have in the tokens list.
    for ( ;; )
    {
      // If we've already parsed the tag, error out.
      if ( parsed.Exists ( m_Current->m_ID ) )
      {
        return ErrorDuplicateInfo ( m_Error, sizeof ( m_Error ), *m_Current );
      }
      else if ( m_Current->m_ID != tTag )
      {
        // Insert the tag into the set except if it's a generic tag
        // (we can have as many of those as we want)
        parsed.Insert ( m_Current->m_ID );
      }

      switch ( m_Current->m_ID )
      {
        case tAuthor:
          // Call ParseInfo to parse the author.
          CHECK ( ParseInfo ( tAuthor, item->m_Author, str_area ) );
          break;
        case tDescription:
          // Call ParseInfo to parse the description.
          CHECK ( ParseInfo ( tDescription, item->m_Description, str_area ) );
          break;
        case tLabel:
          // Call ParseInfo to parse the label.
          CHECK ( ParseInfo ( tLabel, item->m_Label, str_area ) );
          break;
        case tDefault:

          // Check if the select already has a default item and error out if it does.
          if ( select->m_DefaultItem != -1 )
          {
            return ErrorDuplicateDefault ( m_Error, sizeof ( m_Error ), *m_Current );
          }

          // Set the default item in the select.
          select->m_DefaultItem = ( int32_t ) select->m_NumItems;
          // Match the default keyword (we know the current token is "default").
          Match();
          break;
        case tTag:
        {
          Pointer< DDLParser::GenericTag > tag = value_area->Allocate< DDLParser::GenericTag >();
          Str tag_name;
          CHECK ( ParseGenericTag ( tag, value_area, str_area, &tag_name ) );
          tag->m_Next = &*item->m_Tags;
          item->m_Tags = &*tag;
          value_area->AddRelPointer ( &tag->m_Next );

          if ( m_CheckFieldTag )
          {
            if ( ! ( *m_CheckItemTag ) ( DDLParser::Definition::FromBinRep ( m_DDL ), &*select, &*item, &*tag, &tag_name, &parsed, m_Error, sizeof ( m_Error ) ) )
            {
              return false;
            }
          }

          break;
        }
        // Invalid tag for an item.
        default:
          return ErrorInvalidInfo ( m_Error, sizeof ( m_Error ), *m_Current );
      }

      // If the next token isn't a comma, break from the loop.
      if ( m_Current->m_ID != ',' )
      {
        break;
      }

      // Otherwise, match the comma and parse another tag.
      Match();
    }

    // Success.
    return true;
  }

  bool
  Parser::ParseItem ( Pointer<DDLParser::SelectItem>& item, Pointer<DDLParser::Select> select, Area* item_area, Area* value_area, StringArea& str_area )
  {
    // Check if the item already exists and error out if it does.
    if ( select->FindItem ( m_Current->m_Lexeme.GetHash() ) != NULL )
    {
      return ErrorDuplicateIdentifier ( m_Error, sizeof ( m_Error ), *m_Current );
    }

    // Create a new item.
    item = item_area->Allocate<DDLParser::SelectItem>();
    // Register its relative pointers.
    item_area->AddRelPointer ( &item->m_Name );
    item_area->AddRelPointer ( &item->m_Author );
    item_area->AddRelPointer ( &item->m_Description );
    item_area->AddRelPointer ( &item->m_Label );
    item_area->AddRelPointer ( &item->m_Tags );
    // Initialize the item.
    item->m_Size = sizeof ( DDLParser::SelectItem );
    item->m_Name = NULL;
    item->m_Author = NULL;
    item->m_Description = NULL;
    item->m_Label = NULL;
    item->m_Tags = NULL;
    // Set its name.
    item->m_Name = str_area.Add ( m_Current->m_Lexeme );
    item->m_NameHash = m_Current->m_Lexeme.GetHash();
    CHECK ( Match ( tID ) );

    // If there's a comma, item info must follow.
    if ( m_Current->m_ID == ',' )
    {
      Match();
      CHECK ( ParseItemInfo ( select, item, value_area, str_area ) );
    }

    // Items must end with a semicolon.
    return Match ( ';' );
  }

  bool
  Parser::ParseSelectInfo ( Pointer<DDLParser::Select> select, Area* value_area, StringArea& str_area )
  {
    // Create a set to avoid parsing duplicate tags.
    TokenID set_buffer[ 64 ];
    TagSet parsed ( set_buffer, sizeof ( set_buffer ) );

    // Parse as many tags as we have in the tokens list.
    for ( ;; )
    {
      // If we've already parsed the tag, error out.
      if ( parsed.Exists ( m_Current->m_ID ) )
      {
        return ErrorDuplicateInfo ( m_Error, sizeof ( m_Error ), *m_Current );
      }
      else if ( m_Current->m_ID != tTag )
      {
        // Insert the tag into the set except if it's a generic tag
        // (we can have as many of those as we want)
        parsed.Insert ( m_Current->m_ID );
      }

      switch ( m_Current->m_ID )
      {
        case tAuthor:
          // Call ParseInfo to parse the author.
          CHECK ( ParseInfo ( tAuthor, select->m_Author, str_area ) );
          break;
        case tDescription:
          // Call ParseInfo to parse the description.
          CHECK ( ParseInfo ( tDescription, select->m_Description, str_area ) );
          break;
        case tLabel:
          // Call ParseInfo to parse the label.
          CHECK ( ParseInfo ( tLabel, select->m_Label, str_area ) );
          break;
        case tTag:
        {
          Pointer< DDLParser::GenericTag > tag = value_area->Allocate< DDLParser::GenericTag >();
          Str tag_name;
          CHECK ( ParseGenericTag ( tag, value_area, str_area, &tag_name ) );
          tag->m_Next = &*select->m_Tags;
          select->m_Tags = &*tag;
          value_area->AddRelPointer ( &tag->m_Next );

          if ( m_CheckFieldTag )
          {
            if ( ! ( *m_CheckSelectTag ) ( DDLParser::Definition::FromBinRep ( m_DDL ), &*select, &*tag, &tag_name, &parsed, m_Error, sizeof ( m_Error ) ) )
            {
              return false;
            }
          }

          break;
        }
        // Invalid tag for an item.
        default:
          return ErrorInvalidInfo ( m_Error, sizeof ( m_Error ), *m_Current );
      }

      // If the next token isn't a comma, break from the loop.
      if ( m_Current->m_ID != ',' )
      {
        break;
      }

      // Otherwise, match the comma and parse another tag.
      Match();
    }

    // Success.
    return true;
  }

  bool
  Parser::ParseSelect ( AreaManager& bin_rep )
  {
    // Create the select's header area.
    Area* header_area = bin_rep.NewArea();
    // Create the select's item area.
    Area* item_area   = bin_rep.NewArea();
    // Create the select's value area (only for tags right now.)
    Area* value_area  = bin_rep.NewArea();
    // Create the select's string area.
    StringArea str_area ( bin_rep.NewArea() );
    // Parse the select.
    Match ( tSelect );
    // Check if the aggregate already exists and error out if it does.
    const Token* duplicate = m_UsedIdentifiers.Find ( m_Current->m_Lexeme.GetHash() );

    if ( duplicate != NULL )
    {
      return ErrorDuplicateIdentifier ( m_Error, sizeof ( m_Error ), *m_Current, *duplicate );
    }

    m_UsedIdentifiers.Insert ( m_Current->m_Lexeme.GetHash(), *m_Current );
    // Create a new select.
    Pointer<DDLParser::Select> select = header_area->Allocate<DDLParser::Select>();
    // Register its relative pointers.
    header_area->AddRelPointer ( &select->m_Name );
    header_area->AddRelPointer ( &select->m_Author );
    header_area->AddRelPointer ( &select->m_Description );
    header_area->AddRelPointer ( &select->m_Label );
    header_area->AddRelPointer ( &select->m_Tags );
    // Initialize the select.
    select->m_Size = sizeof ( DDLParser::Select );
    select->m_Type = DDLParser::kSelect;
    select->m_Name = NULL;
    select->m_Author = NULL;
    select->m_Description = NULL;
    select->m_Label = NULL;
    select->m_NumItems = 0;
    select->m_DefaultItem = -1;
    select->m_Tags = NULL;
    // Set its name.
    select->m_Name = str_area.Add ( m_Current->m_Lexeme );
    select->m_NameHash = m_Current->m_Lexeme.GetHash();
    CHECK ( Match ( tID ) );

    // If there's a comma, select info must follow.
    if ( m_Current->m_ID == ',' )
    {
      Match();
      CHECK ( ParseSelectInfo ( select, value_area, str_area ) );
    }

    // Select items are enclosed in braces.
    CHECK ( Match ( '{' ) );
    // Parse one item.
    Pointer<DDLParser::SelectItem> item;
    CHECK ( ParseItem ( item, select, item_area, value_area, str_area ) );
    // Create a pointer to it in the select's header area.
    header_area->Allocate<DDLParser::SelectItemPtr>();
    // Register this relative pointer.
    header_area->AddRelPointer ( &select->m_Items[select->m_NumItems] );
    // Set the pointer.
    select->m_Items[select->m_NumItems] = &*item;
    // Increment the number of items in the select.
    select->m_NumItems++;

    // Parse the rest of the items.
    while ( m_Current->m_ID != '}' )
    {
      CHECK ( ParseItem ( item, select, item_area, value_area, str_area ) );
      header_area->Allocate<DDLParser::SelectItemPtr>();
      header_area->AddRelPointer ( &select->m_Items[select->m_NumItems] );
      select->m_Items[select->m_NumItems] = &*item;
      select->m_NumItems++;
    }

    // Match the right brace.
    Match();

    // Skip a semicolon if one is present.
    if ( m_Current->m_ID == ';' )
    {
      Match();
    }

    // If a default item was not provided in the source code...
    if ( select->m_DefaultItem == -1 )
    {
      // Set the first item as the default item.
      select->m_DefaultItem = 0;
    }

    // Align the string area (the last area) to the next 4-byte boundary.
    str_area.GetArea()->Align ( 4 );
    // Success.
    return true;
  }

  bool
  Parser::ParseFlagValue ( Pointer<DDLParser::BitfieldFlagValue>& result, Pointer<DDLParser::Bitfield> bitfield, Area* value_area )
  {
    // Match "value(".
    CHECK ( Match ( tValue ) );
    CHECK ( Match ( '(' ) );
    // Align the value area to the next 4-byte boundary.
    value_area->Align ( 4 );
    // Allocate a new BitfieldFlagValue.
    Pointer<DDLParser::BitfieldFlagValue> bv = value_area->Allocate<DDLParser::BitfieldFlagValue>();
    // Initialize the value.
    bv->m_Size = sizeof ( DDLParser::BitfieldFlagValue );
    bv->m_Count = 0;

    // We must parse at least one flag.
    for ( ;; )
    {
      // Align the value area again.
      value_area->Align ( 4 );
      // Allocate a uint32_t to hold the index of the flag.
      Pointer<uint32_t> index = value_area->Allocate<uint32_t>();
      // Try to find the flag in the bitfield.
      DDLParser::BitfieldFlag* flag = bitfield->FindFlag ( m_Current->m_Lexeme.GetHash() );

      if ( flag == NULL )
      {
        // Not found, error out.
        return ErrorUnknownIdentifier ( m_Error, sizeof ( m_Error ), *m_Current );
      }

      // Match the flag name.
      CHECK ( Match ( tID ) );
      // Set the index of the flag.
      *index = ( uint32_t ) ( &*flag - &*bitfield->m_Flags[0] );
      // Increase the number of flags in the value.
      bv->m_Count++;

      // If the next token isn't an arithmetic or, break from the loop.
      if ( m_Current->m_ID != '|' )
      {
        break;
      }

      // Otherwise match the arithmetic or and parse another flag.
      Match();
    }

    // Set the result to be the bitfield flag value.
    result = bv;
    // The value must end with a right parenthesis.
    return Match ( ')' );
  }

  bool
  Parser::ParseFlagInfo ( Pointer<DDLParser::Bitfield> bitfield, Pointer<DDLParser::BitfieldFlag> flag, Area* value_area, StringArea& str_area )
  {
    // Create a set to avoid parsing duplicate tags.
    TokenID set_buffer[ 64 ];
    TagSet parsed ( set_buffer, sizeof ( set_buffer ) );

    // Parse as many tags as we have in the tokens list.
    for ( ;; )
    {
      // If we've already parsed the tag, error out.
      if ( parsed.Exists ( m_Current->m_ID ) )
      {
        return ErrorDuplicateInfo ( m_Error, sizeof ( m_Error ), *m_Current );
      }
      else if ( m_Current->m_ID != tTag )
      {
        // Insert the tag into the set except if it's a generic tag
        // (we can have as many of those as we want)
        parsed.Insert ( m_Current->m_ID );
      }

      switch ( m_Current->m_ID )
      {
        case tAuthor:
          // Call ParseInfo to parse the author.
          CHECK ( ParseInfo ( tAuthor, flag->m_Author, str_area ) );
          break;
        case tDescription:
          // Call ParseInfo to parse the description.
          CHECK ( ParseInfo ( tDescription, flag->m_Description, str_area ) );
          break;
        case tLabel:
          // Call ParseInfo to parse the label.
          CHECK ( ParseInfo ( tLabel, flag->m_Label, str_area ) );
          break;
        case tDefault:

          // Check if the bitfield already has a default item and error out if it does.
          if ( bitfield->m_DefaultFlag != -1 )
          {
            return ErrorDuplicateDefault ( m_Error, sizeof ( m_Error ), *m_Current );
          }

          // Set the default item in the select.
          bitfield->m_DefaultFlag = ( int32_t ) bitfield->m_NumFlags;
          Match();
          break;
        case tValue:
        {
          // Parse a flag value.
          Pointer<DDLParser::BitfieldFlagValue> result;
          CHECK ( ParseFlagValue ( result, bitfield, value_area ) );
          // Set the flag's value to point to the just-parsed value.
          flag->m_Value = &*result;
        }
        break;
        case tEmpty:
        {
          // Match "empty".
          CHECK ( Match ( tEmpty ) );
          // Create an empty flag value to mark the flag as being empty.
          Pointer<DDLParser::BitfieldFlagValue> bv = value_area->Allocate<DDLParser::BitfieldFlagValue>();
          bv->m_Size = sizeof ( DDLParser::BitfieldFlagValue );
          bv->m_Count = 0;
          // Set the flag's value to point to the empty value.
          flag->m_Value = &*bv;
        }
        break;
        case tTag:
        {
          Pointer< DDLParser::GenericTag > tag = value_area->Allocate< DDLParser::GenericTag >();
          Str tag_name;
          CHECK ( ParseGenericTag ( tag, value_area, str_area, &tag_name ) );
          tag->m_Next = &*flag->m_Tags;
          flag->m_Tags = &*tag;
          value_area->AddRelPointer ( &tag->m_Next );

          if ( m_CheckFieldTag )
          {
            if ( ! ( *m_CheckFlagTag ) ( DDLParser::Definition::FromBinRep ( m_DDL ), &*bitfield, &*flag, &*tag, &tag_name, &parsed, m_Error, sizeof ( m_Error ) ) )
            {
              return false;
            }
          }

          break;
        }
        // Invalid tag for a flag.
        default:
          return ErrorInvalidInfo ( m_Error, sizeof ( m_Error ), *m_Current );
      }

      // If the next token isn't a comma, break from the loop.
      if ( m_Current->m_ID != ',' )
      {
        break;
      }

      // Otherwise, match the comma and parse another tag.
      Match();
    }

    // Success.
    return true;
  }

  bool
  Parser::ParseFlag ( Pointer<DDLParser::BitfieldFlag>& flag, Pointer<DDLParser::Bitfield> bitfield, Area* flag_area, Area* value_area, StringArea& str_area )
  {
    // If there's a limit on the number of flags in a bitfield...
    if ( m_BitfieldLimit != 0 )
    {
      // Count the number of flags that actually take a bit in a bitfield.
      uint32_t count = 0;

      for ( uint32_t i = 0; i < bitfield->m_NumFlags; i++ )
      {
        // Only flags with m_Value == NULL take a bit, if m_Value is != NULL then it is a list of
        // flag indexes which are or'ed together to make the flag value.
        if ( bitfield->m_Flags[i]->m_Value == NULL )
        {
          count++;
        }
      }

      // If the limit has already been reached, error out.
      if ( count == ( uint32_t ) m_BitfieldLimit )
      {
        return ErrorMaximumNumberOfFlagsExceeded ( m_Error, sizeof ( m_Error ), *m_Current );
      }
    }

    // Check if the flag already exists and error out if it does.
    if ( bitfield->FindFlag ( m_Current->m_Lexeme.GetHash() ) != NULL )
    {
      return ErrorDuplicateIdentifier ( m_Error, sizeof ( m_Error ), *m_Current );
    }

    // Create a new flag.
    flag = flag_area->Allocate<DDLParser::BitfieldFlag>();
    // Register its relative pointers.
    flag_area->AddRelPointer ( &flag->m_Name );
    flag_area->AddRelPointer ( &flag->m_Author );
    flag_area->AddRelPointer ( &flag->m_Description );
    flag_area->AddRelPointer ( &flag->m_Label );
    flag_area->AddRelPointer ( &flag->m_Value );
    flag_area->AddRelPointer ( &flag->m_Tags );
    // Initialize the flag.
    flag->m_Size = sizeof ( DDLParser::BitfieldFlag );
    flag->m_Name = NULL;
    flag->m_Author = NULL;
    flag->m_Description = NULL;
    flag->m_Label = NULL;
    flag->m_Value = NULL;
    flag->m_Tags = NULL;
    // Set its name.
    flag->m_Name = str_area.Add ( m_Current->m_Lexeme );
    flag->m_NameHash = m_Current->m_Lexeme.GetHash();
    CHECK ( Match ( tID ) );

    // If there's a comma, flag info must follow.
    if ( m_Current->m_ID == ',' )
    {
      Match();
      CHECK ( ParseFlagInfo ( bitfield, flag, value_area, str_area ) );
    }

    // End of flag declaration.
    return Match ( ';' );
  }

  bool
  Parser::ParseBitfieldInfo ( Pointer<DDLParser::Bitfield> bitfield, Area* value_area, StringArea& str_area )
  {
    // Create a set to avoid parsing duplicate tags.
    TokenID set_buffer[ 64 ];
    TagSet parsed ( set_buffer, sizeof ( set_buffer ) );

    // Parse as many tags as we have in the tokens list.
    for ( ;; )
    {
      // If we've already parsed the tag, error out.
      if ( parsed.Exists ( m_Current->m_ID ) )
      {
        return ErrorDuplicateInfo ( m_Error, sizeof ( m_Error ), *m_Current );
      }
      else if ( m_Current->m_ID != tTag )
      {
        // Insert the tag into the set except if it's a generic tag
        // (we can have as many of those as we want)
        parsed.Insert ( m_Current->m_ID );
      }

      switch ( m_Current->m_ID )
      {
        case tAuthor:
          // Call ParseInfo to parse the author.
          CHECK ( ParseInfo ( tAuthor, bitfield->m_Author, str_area ) );
          break;
        case tDescription:
          // Call ParseInfo to parse the description.
          CHECK ( ParseInfo ( tDescription, bitfield->m_Description, str_area ) );
          break;
        case tLabel:
          // Call ParseInfo to parse the label.
          CHECK ( ParseInfo ( tLabel, bitfield->m_Label, str_area ) );
          break;
        case tTag:
        {
          Pointer< DDLParser::GenericTag > tag = value_area->Allocate< DDLParser::GenericTag >();
          Str tag_name;
          CHECK ( ParseGenericTag ( tag, value_area, str_area, &tag_name ) );
          tag->m_Next = &*bitfield->m_Tags;
          bitfield->m_Tags = &*tag;
          value_area->AddRelPointer ( &tag->m_Next );

          if ( m_CheckFieldTag )
          {
            if ( ! ( *m_CheckBitfieldTag ) ( DDLParser::Definition::FromBinRep ( m_DDL ), &*bitfield, &*tag, &tag_name, &parsed, m_Error, sizeof ( m_Error ) ) )
            {
              return false;
            }
          }

          break;
        }
        // Invalid tag for an item.
        default:
          return ErrorInvalidInfo ( m_Error, sizeof ( m_Error ), *m_Current );
      }

      // If the next token isn't a comma, break from the loop.
      if ( m_Current->m_ID != ',' )
      {
        break;
      }

      // Otherwise, match the comma and parse another tag.
      Match();
    }

    // Success.
    return true;
  }

  bool
  Parser::ParseBitfield ( AreaManager& bin_rep )
  {
    // Create the bitfield's header area.
    Area* header_area = bin_rep.NewArea();
    // Create the bitfield's flag area.
    Area* flag_area   = bin_rep.NewArea();
    // Create the flag's value area.
    Area* value_area  = bin_rep.NewArea();
    // Create the bitfield's string area.
    StringArea str_area ( bin_rep.NewArea() );
    // Parse the bitfield.
    CHECK ( Match ( tBitfield ) );
    // Check if the aggregate already exists and error out if it does.
    const Token* duplicate = m_UsedIdentifiers.Find ( m_Current->m_Lexeme.GetHash() );

    if ( duplicate != NULL )
    {
      return ErrorDuplicateIdentifier ( m_Error, sizeof ( m_Error ), *m_Current, *duplicate );
    }

    m_UsedIdentifiers.Insert ( m_Current->m_Lexeme.GetHash(), *m_Current );
    // Create a new bitfield.
    Pointer<DDLParser::Bitfield> bitfield = header_area->Allocate<DDLParser::Bitfield>();
    // Register its relative pointers.
    header_area->AddRelPointer ( &bitfield->m_Name );
    header_area->AddRelPointer ( &bitfield->m_Author );
    header_area->AddRelPointer ( &bitfield->m_Description );
    header_area->AddRelPointer ( &bitfield->m_Label );
    header_area->AddRelPointer ( &bitfield->m_Tags );
    // Initialize the bitfield.
    bitfield->m_Size = sizeof ( DDLParser::Bitfield );
    bitfield->m_Type = DDLParser::kBitfield;
    bitfield->m_Name = NULL;
    bitfield->m_Author = NULL;
    bitfield->m_Description = NULL;
    bitfield->m_Label = NULL;
    bitfield->m_NumFlags = 0;
    bitfield->m_DefaultFlag = -1;
    bitfield->m_Tags = NULL;
    // Set its name.
    bitfield->m_Name = str_area.Add ( m_Current->m_Lexeme );
    bitfield->m_NameHash = m_Current->m_Lexeme.GetHash();
    CHECK ( Match ( tID ) );

    // If there's a comma, bitfield info must follow.
    if ( m_Current->m_ID == ',' )
    {
      Match();
      CHECK ( ParseBitfieldInfo ( bitfield, value_area, str_area ) );
    }

    // Select items are enclosed in braces.
    CHECK ( Match ( '{' ) );
    // Parse one flag.
    Pointer<DDLParser::BitfieldFlag> flag;
    CHECK ( ParseFlag ( flag, bitfield, flag_area, value_area, str_area ) );
    // Create a pointer to it in the select's header area.
    header_area->Allocate<DDLParser::BitfieldFlagPtr>();
    // Register this relative pointer.
    header_area->AddRelPointer ( &bitfield->m_Flags[bitfield->m_NumFlags] );
    // Set the pointer.
    bitfield->m_Flags[bitfield->m_NumFlags] = &*flag;
    // Increment the number of items in the select.
    bitfield->m_NumFlags++;

    // Parse the rest of the flags.
    while ( m_Current->m_ID != '}' )
    {
      CHECK ( ParseFlag ( flag, bitfield, flag_area, value_area, str_area ) );
      header_area->Allocate<DDLParser::BitfieldFlagPtr>();
      header_area->AddRelPointer ( &bitfield->m_Flags[bitfield->m_NumFlags] );
      bitfield->m_Flags[bitfield->m_NumFlags] = &*flag;
      bitfield->m_NumFlags++;
    }

    // Match the right brace.
    Match();

    // Skip a semicolon if one is present.
    if ( m_Current->m_ID == ';' )
    {
      Match();
    }

    // If a default flag was not provided in the source code...
    if ( bitfield->m_DefaultFlag == -1 )
    {
      // Set the first flag as the default flag.
      bitfield->m_DefaultFlag = 0;
      // Look for an empty flag to be the default.
      uint32_t j;

      for ( j = 0; j < bitfield->m_NumFlags; j++ )
      {
        DDLParser::BitfieldFlag* flag = &* ( *bitfield ) [j];
        DDLParser::BitfieldFlagValue* value = &*flag->m_Value;

        if ( value != NULL && value->m_Count == 0 )
        {
          // Found an empty flag, set it as the default flag.
          bitfield->m_DefaultFlag = ( int32_t ) j;
        }
      }
    }

    // Align the string area (the last area) to the next 4-byte boundary.
    str_area.GetArea()->Align ( 4 );
    // Evaluate the 1-based bit index of the flag.
    uint32_t bit = 1;

    for ( uint32_t i = 0; i < bitfield->m_NumFlags; i++ )
    {
      DDLParser::BitfieldFlag* flag = ( *bitfield ) [i];

      if ( flag->m_Value == NULL )
      {
        // Set the bit index if the flag takes a bit.
        flag->m_Bit = bit++;
      }
      else
      {
        // Set it to zero if the flag's value is a list of other flags or'ed together.
        flag->m_Bit = 0;
      }
    }

    // Success.
    return true;
  }

  bool
  Parser::ParseFieldNativeValue ( Pointer<DDLParser::StructFieldValue>& result, DDLParser::Type type, Area* value_area )
  {
    // Parse an expression.
    CHECK ( ParseExpression() );
    // Get the resulting value of the expression.
    Value value = PopValue();
    // Align the value area according to the alignment of the field type.
    value_area->Align ( TypeAlignments[type] );
    // Allocate suficient space in the value area to hold the value.
    Pointer<uint8_t> pointer = value_area->Allocate<uint8_t> ( TypeSizes[type] );
    // ok will hold true if the resulting value fits in the field type.
    bool ok;

    switch ( type )
    {
      case DDLParser::kUint8:
        // Value must fit in an unsigned 8-bit integer.
        ok = value.IsUint8();
        // Set the value.
        * ( uint8_t* ) &*pointer = ( uint8_t ) value.GetInt();
        break;
      case DDLParser::kUint16:
        // Value must fit in an unsigned 16-bit integer.
        ok = value.IsUint16();
        // Set the value.
        * ( uint16_t* ) &*pointer = ( uint16_t ) value.GetInt();
        break;
      case DDLParser::kUint32:
        // Value must fit in an unsigned 32-bit integer.
        ok = value.IsUint32();
        // Set the value.
        * ( uint32_t* ) &*pointer = ( uint32_t ) value.GetInt();
        break;
      case DDLParser::kUint64:
        // Value must fit in an unsigned 64-bit integer.
        ok = value.IsUint64();
        // Set the value.
        * ( uint64_t* ) &*pointer = ( uint64_t ) value.GetInt();
        break;
      case DDLParser::kInt8:
        // Value must fit in a signed 8-bit integer.
        ok = value.IsInt8();
        // Set the value.
        * ( int8_t* ) &*pointer = ( int8_t ) value.GetInt();
        break;
      case DDLParser::kInt16:
        // Value must fit in a signed 16-bit integer.
        ok = value.IsInt16();
        // Set the value.
        * ( int16_t* ) &*pointer = ( int16_t ) value.GetInt();
        break;
      case DDLParser::kInt32:
        // Value must fit in a signed 32-bit integer.
        ok = value.IsInt32();
        // Set the value.
        * ( int32_t* ) &*pointer = ( int32_t ) value.GetInt();
        break;
      case DDLParser::kInt64:
        // Value must fit in a signed 64-bit integer.
        ok = value.IsInt64();
        // Set the value.
        * ( int64_t* ) &*pointer = ( int64_t ) value.GetInt();
        break;
      case DDLParser::kFloat32:
        // Value must fit in a 32-bit floating point number.
        ok = value.IsFloat32();
        // Set the value.
        * ( float* ) &*pointer = ( float ) value.GetDouble();
        break;
      case DDLParser::kFloat64:
        // Value must fit in a 64-bit floating point number.
        ok = value.IsFloat64();
        // Set the value.
        * ( double* ) &*pointer = ( double ) value.GetDouble();
        break;
      case DDLParser::kBoolean:
        // Value must be an integer.
        ok = value.IsInt64();
        // Set the unsigned 8-bit integer to 0 or 1.
        * ( uint8_t* ) &*pointer = value.GetInt() == 0 ? 0U : 1U;
        break;
      case DDLParser::kTuid:
        // Value must be an uint64.
        ok = value.IsUint64();
        // Set the value.
        * ( uint64_t* ) &*pointer = ( uint64_t ) value.GetInt();
        break;
      default:
        // All other value types aren't native types.
        ok = false;
    }

    // If something went wrong, error out.
    if ( !ok )
    {
      return ErrorTypeMismatch ( m_Error, sizeof ( m_Error ), *m_Current );
    }

    // Set the result pointer.
    pointer.CastTo< DDLParser::StructFieldValue > ( result );
    // Success.
    return true;
  }

  bool
  Parser::ParseFieldStrValue ( Pointer<DDLParser::StructFieldValue>& result, Area* value_area, StringArea& str_area )
  {
    // Parse an expression.
    CHECK ( ParseExpression() );
    // Get the resulting value of the expression.
    Value value = PopValue();

    // If the resulting value is not a string, error out.
    if ( !value.IsString() )
    {
      return ErrorTypeMismatch ( m_Error, sizeof ( m_Error ), *m_Current );
    }

    // Get the string.
    Str value_string = value.GetString();
    // Align the value area to the next 4-byte boundary.
    value_area->Align ( 4 );
    // Allocate a pointer to a string in the value area.
    Pointer<DDLParser::String> pointer = value_area->Allocate<DDLParser::String>();
    // Register the relative pointer.
    value_area->AddRelPointer ( &*pointer );
    // Set the pointer to the place where the string was added in the string area.
    *pointer = str_area.Add ( value_string );
    // Set the result pointer.
    pointer.CastTo< DDLParser::StructFieldValue > ( result );
    // Success.
    return true;
  }

  bool
  Parser::ParseFieldFileValue ( Pointer<DDLParser::StructFieldValue>& result, Area* value_area, StringArea& str_area )
  {
    // Parse an expression.
    CHECK ( ParseExpression() );
    // Get the resulting value of the expression.
    Value value = PopValue();

    // If the resulting value is not a string, error out.
    if ( !value.IsString() )
    {
      return ErrorTypeMismatch ( m_Error, sizeof ( m_Error ), *m_Current );
    }

    // Get the string.
    Str source_string   = value.GetString();
    // Allocate space to the unescaped string.
    char*  clean_path      = ( char* ) alloca ( source_string.GetLength() + 1 );
    // Unescape the string and get its unescaped length.
    size_t clean_path_len  = FilePathCleanName ( source_string.GetChars(), clean_path, source_string.GetLength() + 1 );
    // Create a string object with the unescaped string.
    Str clean_string    = Str ( clean_path, ( uint32_t ) clean_path_len );
    // Align the value area to the next 4-byte boundary.
    value_area->Align ( 4 );
    // Allocate a pointer to a string in the value area.
    Pointer<DDLParser::String> pointer = value_area->Allocate<DDLParser::String>();
    // Register the relative pointer.
    value_area->AddRelPointer ( &*pointer );
    // Set the pointer to the place where the unescaped string was added in the string area.
    *pointer = str_area.Add ( clean_string );
    // Set the result pointer.
    pointer.CastTo< DDLParser::StructFieldValue > ( result );
    // Success.
    return true;
  }

  bool
  Parser::ParseFieldJsonValue ( Pointer<DDLParser::StructFieldValue>& result, Area* value_area, StringArea& str_area )
  {
    // Parse an expression.
    CHECK ( ParseExpression() );
    // Get the resulting value of the expression.
    Value value = PopValue();

    // If the resulting value is not a string, error out.
    if ( !value.IsString() )
    {
      return ErrorTypeMismatch ( m_Error, sizeof ( m_Error ), *m_Current );
    }

    // Validate that the string si a valid JSON value.
    const char*  json        = value.GetString().GetChars();
    unsigned int json_length = value.GetString().GetLength();
    JSONChecker jc;
    JSONCheckerInit ( &jc );

    while ( json_length-- > 0 )
    {
      if ( !JSONCheckerChar ( &jc, *json++ ) )
      {
        return ErrorInvalidJsonValue ( m_Error, sizeof ( m_Error ), *m_Current );
      }
    }

    if ( !JSONCheckerDone ( &jc ) )
    {
      return ErrorInvalidJsonValue ( m_Error, sizeof ( m_Error ), *m_Current );
    }

    // Get the string.
    Str value_string = value.GetString();
    // Align the value area to the next 4-byte boundary.
    value_area->Align ( 4 );
    // Allocate a pointer to a string in the value area.
    Pointer<DDLParser::String> pointer = value_area->Allocate<DDLParser::String>();
    // Register the relative pointer.
    value_area->AddRelPointer ( &*pointer );
    // Set the pointer to the place where the string was added in the string area.
    *pointer = str_area.Add ( value_string );
    // Set the result pointer.
    pointer.CastTo< DDLParser::StructFieldValue > ( result );
    // Success.
    return true;
  }

  bool
  Parser::ParseFieldSelectValue ( Pointer<DDLParser::StructFieldValue>& result, DDLParser::Select* select, Area* value_area )
  {
    // Check if the item exists in the select and error out if it doesn't.
    if ( select->FindItem ( m_Current->m_Lexeme.GetHash() ) == NULL )
    {
      return ErrorInvalidSelectValue ( m_Error, sizeof ( m_Error ), *m_Current );
    }

    // Align the value area to the next 4-byte boundary.
    value_area->Align ( 4 );
    // Allocate an uint32_t in the value area.
    Pointer<uint32_t> pointer = value_area->Allocate<uint32_t>();
    // Set it to the select item's hash.
    *pointer = m_Current->m_Lexeme.GetHash();
    // Set the resulting pointer.
    pointer.CastTo< DDLParser::StructFieldValue > ( result );
    // Match the identifier of the item and return.
    return Match ( tID );
  }

  bool
  Parser::ParseFieldBitfieldValue ( Pointer<DDLParser::StructFieldValue>& result, DDLParser::Bitfield* bitfield, Area* value_area, Area* bv_area )
  {
    // Align the bitfield value area to the next 4-byte boundary.
    bv_area->Align ( 4 );
    // Allocate a bitfield value in the bitfield value area.
    Pointer<DDLParser::StructBitfieldValue> bv = bv_area->Allocate<DDLParser::StructBitfieldValue>();
    // Initialize the bitfield value.
    bv->m_Size = sizeof ( DDLParser::StructBitfieldValue );
    bv->m_Count = 0;
    // Align the value area to the next 4-byte boundary.
    value_area->Align ( 4 );
    // Allocate a pointer to a bitfield value.
    Pointer<DDLParser::StructBitfieldValuePtr> pointer = value_area->Allocate<DDLParser::StructBitfieldValuePtr>();
    // Register the relative pointer.
    value_area->AddRelPointer ( &*pointer );
    // Set the pointer to the bitfield value we allocated earlier.
    *pointer = &*bv;

    // Parse flags.
    for ( int i = 0;; i++ )
    {
      // Check if the flag exists in the bitfield and error out if it doesn't.
      if ( bitfield->FindFlag ( m_Current->m_Lexeme.GetHash() ) == NULL )
      {
        return ErrorUnknownIdentifier ( m_Error, sizeof ( m_Error ), *m_Current );
      }

      // Allocate an uint32_t in the bitfield value area.
      Pointer<uint32_t> hash = bv_area->Allocate<uint32_t>();
      // Set it to the flag's hash.
      *hash = m_Current->m_Lexeme.GetHash();
      // Match the identifier of the flag.
      CHECK ( Match ( tID ) );
      // Increment the counter of the bitfield value.
      bv->m_Count++;

      // If the next token isn't a '|', break from the loop.
      if ( m_Current->m_ID != '|' )
      {
        break;
      }

      // Otherwise, match the '|' and parse another flag.
      Match();
    }

    // Set the result pointer.
    pointer.CastTo< DDLParser::StructFieldValue > ( result );
    // Success.
    return true;
  }

  bool
  Parser::ParseFieldUnknownValue ( Pointer<DDLParser::StructFieldValue>& result, Area* value_area, Area* uv_area, StringArea& str_area )
  {
    uv_area->Align ( 4 );
    Pointer<DDLParser::StructUnknownValue> uv = uv_area->Allocate<DDLParser::StructUnknownValue>();
    uv->m_Size = sizeof ( DDLParser::StructUnknownValue );
    uv->m_Count = 0;
    value_area->Align ( 4 );
    Pointer<DDLParser::StructUnknownValuePtr> pointer = value_area->Allocate<DDLParser::StructUnknownValuePtr>();
    value_area->AddRelPointer ( &*pointer );
    *pointer = &*uv;

    for ( int i = 0;; i++ )
    {
      Pointer<DDLParser::String> id = uv_area->Allocate<DDLParser::String>();
      uv_area->AddRelPointer ( &*id );
      *id = str_area.Add ( m_Current->m_Lexeme );
      CHECK ( Match ( tID ) );
      uv->m_Count++;

      if ( m_Current->m_ID != '|' )
      {
        break;
      }

      Match();
    }

    pointer.CastTo< DDLParser::StructFieldValue > ( result );
    return true;
  }

  bool
  Parser::ParseFieldStructValue ( Pointer<DDLParser::StructFieldValue>& result, Pointer<DDLParser::StructValueInfo> value_info, Area* value_area, Area* sv_area, StringArea& str_area )
  {
    // Align the structure value area to the next 4-byte boundary.
    sv_area->Align ( 4 );
    // Allocate a structure value in the structure value area.
    Pointer<DDLParser::StructStructValue> sv = sv_area->Allocate<DDLParser::StructStructValue>();
    // Initialize the structure value.
    sv->m_Size = sizeof ( DDLParser::StructStructValue );
    sv->m_Count = 0;
    // Align the value area to the next 4-byte boundary.
    value_area->Align ( 4 );
    // Allocate a pointer to a structure value.
    Pointer<DDLParser::StructStructValuePtr> pointer = value_area->Allocate<DDLParser::StructStructValuePtr>();
    // Register the relative pointer.
    value_area->AddRelPointer ( &*pointer );
    // Set the pointer to the structure value we allocated earlier.
    *pointer = &*sv;
    // Structure values begin with a left brace.
    CHECK ( Match ( '{' ) );
    // Create two new areas to call ParseFieldValue recursively.
    Area* newvalue_area = sv_area->NewArea();
    Area* newsv_area = newvalue_area->NewArea();

    // Parse ID = field_value pairs.
    for ( ;; )
    {
      // Get the aggregate corresponding to the field type.
      DDLParser::Aggregate* aggregate = FindAggregate ( value_info->m_TypeNameHash );
      // Get the field from the aggregate.
      const DDLParser::StructField* field = aggregate->ToStruct()->FindField ( m_Current->m_Lexeme.GetHash() );

      // If the field doesn't exist, error out.
      if ( field == NULL )
      {
        return ErrorUnknownIdentifier ( m_Error, sizeof ( m_Error ), *m_Current );
      }

      // Allocate a new StructValueInfo in the structure value area to represent the value.
      Pointer<DDLParser::StructValueInfo> vi = sv_area->Allocate<DDLParser::StructValueInfo>();
      // Register the relative pointer.
      sv_area->AddRelPointer ( &vi->m_Value );
      // Make it equal to the value info of the field representing the type we're parsing.
      *vi = * ( DDLParser::StructValueInfo* ) &field->m_ValueInfo;
      // Clean up tags for the copied value.
      vi->m_Tags = NULL;
      // Set its value to NULL.
      vi->m_Value = NULL;
      // Match the field name.
      CHECK ( Match ( tID ) );
      // Match the equal sign.
      CHECK ( Match ( '=' ) );
      // Parse the field value for the new value info.
      CHECK ( ParseFieldValue ( vi, newvalue_area, newsv_area, str_area ) );
      // Increment the counter of the structure value.
      sv->m_Count++;

      // If the next token isn't a comma, break the loop.
      if ( m_Current->m_ID != ',' )
      {
        break;
      }

      // Otherwise, match the comma and parse another ID = field_value pair.
      Match();
    }

    // Merge back the two new areas.
    newvalue_area->Merge();
    sv_area->Merge();
    // Set the result pointer.
    pointer.CastTo< DDLParser::StructFieldValue > ( result );
    // Match the right brace and return.
    return Match ( '}' );
  }

  bool
  Parser::ParseFieldValue ( Pointer<DDLParser::StructValueInfo> value_info, Area* value_area, Area* valueptr_area, StringArea& str_area )
  {
    // Create a new value area to use in recursive functions.
    Area* newvalue_area = value_area->NewArea();

    // If it's an array, it must begin with a left brace.
    if ( value_info->m_ArrayType != DDLParser::kScalar )
    {
      CHECK ( Match ( '{' ) );
    }

    // Count the number of values in the field value.
    uint32_t count = 0;

    // If it's not an array or it is and the list is not empty...
    if ( value_info->m_ArrayType == DDLParser::kScalar || m_Current->m_ID != '}' )
    {
      // Declare a pointer to hold the resulting field value.
      Pointer<DDLParser::StructFieldValue> pointer;

      // Parse as many field values as there are in the list.
      for ( int i = 0;; i++ )
      {
        // Parse according to the field type.
        switch ( value_info->m_Type )
        {
          case DDLParser::kUint8:
          case DDLParser::kUint16:
          case DDLParser::kUint32:
          case DDLParser::kUint64:
          case DDLParser::kInt8:
          case DDLParser::kInt16:
          case DDLParser::kInt32:
          case DDLParser::kInt64:
          case DDLParser::kFloat32:
          case DDLParser::kFloat64:
          case DDLParser::kBoolean:
          case DDLParser::kTuid:
            // Parse a native value.
            CHECK ( ParseFieldNativeValue ( pointer, ( DDLParser::Type ) value_info->m_Type, value_area ) );
            break;
          case DDLParser::kString:
            // Parse a string value.
            CHECK ( ParseFieldStrValue ( pointer, value_area, str_area ) );
            break;
          case DDLParser::kSelect:
            // Parse a select value.
            CHECK ( ParseFieldSelectValue ( pointer, FindAggregate ( value_info->m_TypeNameHash )->ToSelect(), value_area ) );
            break;
          case DDLParser::kBitfield:
            // Parse a bitfield value.
            CHECK ( ParseFieldBitfieldValue ( pointer, FindAggregate ( value_info->m_TypeNameHash )->ToBitfield(), value_area, newvalue_area ) );
            break;
          case DDLParser::kStruct:
            // Parse a structure value.
            CHECK ( ParseFieldStructValue ( pointer, value_info, value_area, newvalue_area, str_area ) );
            break;
          case DDLParser::kUnknown:
            CHECK ( ParseFieldUnknownValue ( pointer, value_area, newvalue_area, str_area ) );
            break;
          case DDLParser::kFile:
            // Parse a file value.
            CHECK ( ParseFieldFileValue ( pointer, value_area, str_area ) );
            break;
          case DDLParser::kJson:
            // Parse a JSON value.
            CHECK ( ParseFieldJsonValue ( pointer, value_area, str_area ) );
            break;
        }

        // If it's the first element of an array (or if it's a scalar)...
        if ( i == 0 )
        {
          // Set the value to be the resulting value of one of the switch cases above.
          value_info->m_Value = &*pointer;
        }

        // Increment the counter.
        count++;

        // If it's not an array or the next token is not a comma, break the loop.
        if ( value_info->m_ArrayType == DDLParser::kScalar || m_Current->m_ID != ',' )
        {
          break;
        }

        // Otherwise, match the comma and parse another value.
        Match();
      }
    }

    // If it's an array, match the right brace.
    if ( value_info->m_ArrayType != DDLParser::kScalar )
    {
      CHECK ( Match ( '}' ) );
    }

    // If the number of parsed elements is different from the array size, error out.
    if ( count != value_info->GetCount() )
    {
      return ErrorWrongNumberOfDefaultValues ( m_Error, sizeof ( m_Error ), *m_Current, value_info->GetCount(), count );
    }

    // Merge back the value area.
    value_area->Merge();
    // Success.
    return true;
  }

  bool
  Parser::ParseFileExtensions ( Pointer<DDLParser::Extensions>& result, Area* value_area, StringArea& str_area )
  {
    CHECK ( Match ( tExtensions ) );
    CHECK ( Match ( '(' ) );
    result->m_Count = 0;
    value_area->Allocate< DDLParser::String >();
    result->m_Extension[ result->m_Count ] = str_area.Add ( m_Current->m_Lexeme );
    value_area->AddRelPointer ( &result->m_Extension[ result->m_Count ] );
    CHECK ( Match ( tLiteral ) );
    result->m_Count++;

    while ( m_Current->m_ID == ',' )
    {
      Match();
      value_area->Allocate< DDLParser::String >();
      result->m_Extension[ result->m_Count ] = str_area.Add ( m_Current->m_Lexeme );
      value_area->AddRelPointer ( &result->m_Extension[ result->m_Count ] );
      CHECK ( Match ( tLiteral ) );
      result->m_Count++;
    }

    return Match ( ')' );
  }

  bool
  Parser::ParseFileVaultHints ( Pointer<DDLParser::VaultHints>& result, Area* value_area, StringArea& str_area )
  {
    CHECK ( Match ( tVaultHints ) );
    CHECK ( Match ( '(' ) );
    result->m_Count = 0;
    value_area->Allocate< DDLParser::String >();
    result->m_VaultHint[ result->m_Count ] = str_area.Add ( m_Current->m_Lexeme );
    value_area->AddRelPointer ( &result->m_VaultHint[ result->m_Count ] );
    CHECK ( Match ( tLiteral ) );
    result->m_Count++;

    while ( m_Current->m_ID == ',' )
    {
      Match();
      value_area->Allocate< DDLParser::String >();
      result->m_VaultHint[ result->m_Count ] = str_area.Add ( m_Current->m_Lexeme );
      value_area->AddRelPointer ( &result->m_VaultHint[ result->m_Count ] );
      CHECK ( Match ( tLiteral ) );
      result->m_Count++;
    }

    return Match ( ')' );
  }

  bool
  Parser::ParseFieldRange ( DDLParser::Type type, Pointer<DDLParser::UIRange>& result, Area* value_area )
  {
    CHECK ( Match ( tUIRange ) );
    CHECK ( Match ( '(' ) );
    // Get soft min and max.
    CHECK ( ParseExpression() );
    Value soft_min = PopValue();
    CHECK ( Match ( ',' ) );
    CHECK ( ParseExpression() );
    Value soft_max = PopValue();
    // Set defaults for hard limits and step.
    Value hard_min = soft_min;
    Value hard_max = soft_max;
    Value step ( ( int64_t ) 1 );

    if ( m_Current->m_ID == ',' )
    {
      // Get the hard limits.
      Match();
      CHECK ( ParseExpression() );
      hard_min = PopValue();
      CHECK ( Match ( ',' ) );
      CHECK ( ParseExpression() );
      hard_max = PopValue();

      if ( m_Current->m_ID == ',' )
      {
        // Get the step.
        Match();
        CHECK ( ParseExpression() );
        step = PopValue();
      }
    }

    value_area->Align ( 4 );
    result = value_area->Allocate<DDLParser::UIRange>();
    result->m_Size = sizeof ( DDLParser::UIRange );
    result->m_Type = DDLParser::kUIRange;
    result->m_Next = NULL;

    if ( type == DDLParser::kFloat32 || type == DDLParser::kFloat64 )
    {
      if ( soft_min.GetDouble() >= soft_max.GetDouble() || hard_min.GetDouble() >= hard_max.GetDouble() )
      {
        return ErrorEmptyRange ( m_Error, sizeof ( m_Error ), *m_Current );
      }

      if ( step.GetDouble() <= 0 )
      {
        return ErrorInvalidRangeStep ( m_Error, sizeof ( m_Error ), *m_Current );
      }
    }
    else
    {
      if ( soft_min.GetInt() >= soft_max.GetInt() || hard_min.GetInt() >= hard_max.GetInt() )
      {
        return ErrorEmptyRange ( m_Error, sizeof ( m_Error ), *m_Current );
      }

      if ( step.GetInt() <= 0 )
      {
        return ErrorInvalidRangeStep ( m_Error, sizeof ( m_Error ), *m_Current );
      }
    }

    switch ( type )
    {
      case DDLParser::kUint8:

        if ( !soft_min.IsUint8() || !soft_max.IsUint8() || !hard_min.IsUint8() || !hard_max.IsUint8() || !step.IsUint8() )
        {
          return ErrorValueDoesNotFit ( m_Error, sizeof ( m_Error ), *m_Current );
        }

        result->GetSoftMin()->m_Uint8 = ( uint8_t ) soft_min.GetInt();
        result->GetSoftMax()->m_Uint8 = ( uint8_t ) soft_max.GetInt();
        result->GetHardMin()->m_Uint8 = ( uint8_t ) hard_min.GetInt();
        result->GetHardMax()->m_Uint8 = ( uint8_t ) hard_max.GetInt();
        result->GetStep()->m_Uint8 = ( uint8_t ) step.GetInt();
        break;
      case DDLParser::kUint16:

        if ( !soft_min.IsUint16() || !soft_max.IsUint16() || !hard_min.IsUint16() || !hard_max.IsUint16() || !step.IsUint16() )
        {
          return ErrorValueDoesNotFit ( m_Error, sizeof ( m_Error ), *m_Current );
        }

        result->GetSoftMin()->m_Uint16 = ( uint16_t ) soft_min.GetInt();
        result->GetSoftMax()->m_Uint16 = ( uint16_t ) soft_max.GetInt();
        result->GetHardMin()->m_Uint16 = ( uint16_t ) hard_min.GetInt();
        result->GetHardMax()->m_Uint16 = ( uint16_t ) hard_max.GetInt();
        result->GetStep()->m_Uint16 = ( uint16_t ) step.GetInt();
        break;
      case DDLParser::kUint32:

        if ( !soft_min.IsUint32() || !soft_max.IsUint32() || !hard_min.IsUint32() || !hard_max.IsUint32() || !step.IsUint32() )
        {
          return ErrorValueDoesNotFit ( m_Error, sizeof ( m_Error ), *m_Current );
        }

        result->GetSoftMin()->m_Uint32 = ( uint32_t ) soft_min.GetInt();
        result->GetSoftMax()->m_Uint32 = ( uint32_t ) soft_max.GetInt();
        result->GetHardMin()->m_Uint32 = ( uint32_t ) hard_min.GetInt();
        result->GetHardMax()->m_Uint32 = ( uint32_t ) hard_max.GetInt();
        result->GetStep()->m_Uint32 = ( uint32_t ) step.GetInt();
        break;
      case DDLParser::kUint64:

        if ( !soft_min.IsUint64() || !soft_max.IsUint64() || !hard_min.IsUint64() || !hard_max.IsUint64() || !step.IsUint64() )
        {
          return ErrorValueDoesNotFit ( m_Error, sizeof ( m_Error ), *m_Current );
        }

        result->GetSoftMin()->m_Uint64 = ( uint64_t ) soft_min.GetInt();
        result->GetSoftMax()->m_Uint64 = ( uint64_t ) soft_max.GetInt();
        result->GetHardMin()->m_Uint64 = ( uint64_t ) hard_min.GetInt();
        result->GetHardMax()->m_Uint64 = ( uint64_t ) hard_max.GetInt();
        result->GetStep()->m_Uint64 = ( uint64_t ) step.GetInt();
        break;
      case DDLParser::kInt8:

        if ( !soft_min.IsInt8() || !soft_max.IsInt8() || !hard_min.IsInt8() || !hard_max.IsInt8() || !step.IsInt8() )
        {
          return ErrorValueDoesNotFit ( m_Error, sizeof ( m_Error ), *m_Current );
        }

        result->GetSoftMin()->m_Int8 = ( int8_t ) soft_min.GetInt();
        result->GetSoftMax()->m_Int8 = ( int8_t ) soft_max.GetInt();
        result->GetHardMin()->m_Int8 = ( int8_t ) hard_min.GetInt();
        result->GetHardMax()->m_Int8 = ( int8_t ) hard_max.GetInt();
        result->GetStep()->m_Int8 = ( int8_t ) step.GetInt();
        break;
      case DDLParser::kInt16:

        if ( !soft_min.IsInt16() || !soft_max.IsInt16() || !hard_min.IsInt16() || !hard_max.IsInt16() || !step.IsInt16() )
        {
          return ErrorValueDoesNotFit ( m_Error, sizeof ( m_Error ), *m_Current );
        }

        result->GetSoftMin()->m_Int16 = ( int16_t ) soft_min.GetInt();
        result->GetSoftMax()->m_Int16 = ( int16_t ) soft_max.GetInt();
        result->GetHardMin()->m_Int16 = ( int16_t ) hard_min.GetInt();
        result->GetHardMax()->m_Int16 = ( int16_t ) hard_max.GetInt();
        result->GetStep()->m_Int16 = ( int16_t ) step.GetInt();
        break;
      case DDLParser::kInt32:

        if ( !soft_min.IsInt32() || !soft_max.IsInt32() || !hard_min.IsInt32() || !hard_max.IsInt32() || !step.IsInt32() )
        {
          return ErrorValueDoesNotFit ( m_Error, sizeof ( m_Error ), *m_Current );
        }

        result->GetSoftMin()->m_Int32 = ( int32_t ) soft_min.GetInt();
        result->GetSoftMax()->m_Int32 = ( int32_t ) soft_max.GetInt();
        result->GetHardMin()->m_Int32 = ( int32_t ) hard_min.GetInt();
        result->GetHardMax()->m_Int32 = ( int32_t ) hard_max.GetInt();
        result->GetStep()->m_Int32 = ( int32_t ) step.GetInt();
        break;
      case DDLParser::kInt64:

        if ( !soft_min.IsInt64() || !soft_max.IsInt64() || !hard_min.IsInt64() || !hard_max.IsInt64() || !step.IsInt64() )
        {
          return ErrorValueDoesNotFit ( m_Error, sizeof ( m_Error ), *m_Current );
        }

        result->GetSoftMin()->m_Int64 = ( int64_t ) soft_min.GetInt();
        result->GetSoftMax()->m_Int64 = ( int64_t ) soft_max.GetInt();
        result->GetHardMin()->m_Int64 = ( int64_t ) hard_min.GetInt();
        result->GetHardMax()->m_Int64 = ( int64_t ) hard_max.GetInt();
        result->GetStep()->m_Int64 = ( int64_t ) step.GetInt();
        break;
      case DDLParser::kFloat32:

        if ( !soft_min.IsFloat32() || !soft_max.IsFloat32() || !hard_min.IsFloat32() || !hard_max.IsFloat32() || !step.IsFloat32() )
        {
          return ErrorValueDoesNotFit ( m_Error, sizeof ( m_Error ), *m_Current );
        }

        result->GetSoftMin()->m_Float32 = ( float ) soft_min.GetDouble();
        result->GetSoftMax()->m_Float32 = ( float ) soft_max.GetDouble();
        result->GetHardMin()->m_Float32 = ( float ) hard_min.GetDouble();
        result->GetHardMax()->m_Float32 = ( float ) hard_max.GetDouble();
        result->GetStep()->m_Float32 = ( float ) step.GetDouble();
        break;
      case DDLParser::kFloat64:

        if ( !soft_min.IsFloat64() || !soft_max.IsFloat64() || !hard_min.IsFloat64() || !hard_max.IsFloat64() || !step.IsFloat64() )
        {
          return ErrorValueDoesNotFit ( m_Error, sizeof ( m_Error ), *m_Current );
        }

        result->GetSoftMin()->m_Float64 = ( double ) soft_min.GetDouble();
        result->GetSoftMax()->m_Float64 = ( double ) soft_max.GetDouble();
        result->GetHardMin()->m_Float64 = ( double ) hard_min.GetDouble();
        result->GetHardMax()->m_Float64 = ( double ) hard_max.GetDouble();
        result->GetStep()->m_Float64 = ( double ) step.GetDouble();
        break;
      default:
        return ErrorInvalidTypeForRange ( m_Error, sizeof ( m_Error ), *m_Current );
    }

    return Match ( ')' );
  }

  bool
  Parser::ParseFieldInfo ( Pointer<DDLParser::Struct> structure, Pointer<DDLParser::StructField> field, Area* value_area, Area* valueptr_area, StringArea& str_area )
  {
    // Create a set to avoid parsing duplicate tags.
    TokenID set_buffer[ 64 ];
    TagSet parsed ( set_buffer, sizeof ( set_buffer ) );

    for ( ;; )
    {
      // Check for duplicate tag.
      if ( parsed.Exists ( m_Current->m_ID ) )
      {
        return ErrorDuplicateInfo ( m_Error, sizeof ( m_Error ), *m_Current );
      }
      else if ( m_Current->m_ID != tTag )
      {
        // Insert the tag into the set except if it's a generic tag
        // (we can have as many of those as we want)
        parsed.Insert ( m_Current->m_ID );
      }

      switch ( m_Current->m_ID )
      {
        case tAuthor:
          CHECK ( ParseInfo ( tAuthor, field->m_Author, str_area ) );
          break;
        case tDescription:
          CHECK ( ParseInfo ( tDescription, field->m_Description, str_area ) );
          break;
        case tLabel:
          CHECK ( ParseInfo ( tLabel, field->m_Label, str_area ) );
          break;
        case tValue:
        {
          if ( field->m_ValueInfo.m_ArrayType == DDLParser::kDynamic )
          {
            return ErrorVariableSizedArrayCannotHaveDefaultValue ( m_Error, sizeof ( m_Error ), *m_Current );
          }
          else if ( field->m_ValueInfo.m_ArrayType == DDLParser::kHashmap )
          {
            return ErrorHashmapCannotHaveDefaultValue ( m_Error, sizeof ( m_Error ), *m_Current );
          }

          CHECK ( Match ( tValue ) );
          CHECK ( Match ( '(' ) );
          Pointer< DDLParser::StructValueInfo > value_info ( field.m_Area );
          value_info = &field->m_ValueInfo;
          CHECK ( ParseFieldValue ( value_info, value_area, valueptr_area, str_area ) );
          CHECK ( Match ( ')' ) );
          break;
        }
        case tExtensions:
        {
          if ( field->m_ValueInfo.m_Type != DDLParser::kFile )
          {
            return ErrorInvalidTypeForExtensions ( m_Error, sizeof ( m_Error ), *m_Current );
          }

          Pointer< DDLParser::Extensions > extensions = value_area->Allocate< DDLParser::Extensions >();
          CHECK ( ParseFileExtensions ( extensions, value_area, str_area ) );
          extensions->m_Size = sizeof ( DDLParser::Extensions );
          extensions->m_Type = DDLParser::kExtensions;
          extensions->m_Next = &*field->m_ValueInfo.m_Tags;
          field->m_ValueInfo.m_Tags = &*extensions;
          value_area->AddRelPointer ( &extensions->m_Next );
        }
        break;
        case tVaultHints:
        {
          if ( field->m_ValueInfo.m_Type != DDLParser::kFile )
          {
            return ErrorInvalidTypeForVaultHints ( m_Error, sizeof ( m_Error ), *m_Current );
          }

          Pointer< DDLParser::VaultHints > VaultHints = value_area->Allocate< DDLParser::VaultHints >();
          CHECK ( ParseFileVaultHints ( VaultHints, value_area, str_area ) );
          VaultHints->m_Size = sizeof ( DDLParser::VaultHints );
          VaultHints->m_Type = DDLParser::kVaultHints;
          VaultHints->m_Next = &*field->m_ValueInfo.m_Tags;
          field->m_ValueInfo.m_Tags = &*VaultHints;
          value_area->AddRelPointer ( &VaultHints->m_Next );
        }
        break;
        case tUIRange:
        {
          Pointer< DDLParser::UIRange > result;
          CHECK ( ParseFieldRange ( ( DDLParser::Type ) field->m_ValueInfo.m_Type, result, value_area ) );
          result->m_Next = &*field->m_ValueInfo.m_Tags;
          field->m_ValueInfo.m_Tags = &*result;
          value_area->AddRelPointer ( &result->m_Next );
        }
        break;
        case tUIRender:
        {
          Pointer< DDLParser::UIRender > render = value_area->Allocate< DDLParser::UIRender >();
          render->m_Size = sizeof ( DDLParser::UIRender );
          render->m_Type = DDLParser::kUIRender;
          CHECK ( ParseInfo ( tUIRender, render->m_Render, str_area ) );
          render->m_Next = &*field->m_ValueInfo.m_Tags;
          field->m_ValueInfo.m_Tags = &*render;
          value_area->AddRelPointer ( &render->m_Render );
          value_area->AddRelPointer ( &render->m_Next );
        }
        break;
        case tParallel:
        {
          Match();
          CHECK ( Match ( '(' ) );
          DDLParser::StructField* array = structure->FindField ( m_Current->m_Lexeme.GetHash() );

          if ( array == NULL )
          {
            return ErrorUnknownIdentifier ( m_Error, sizeof ( m_Error ), *m_Current );
          }

          if ( array->GetValueInfo()->GetArrayType() != DDLParser::kDynamic )
          {
            return ErrorArrayNotDynamic ( m_Error, sizeof ( m_Error ), *m_Current );
          }

          if ( field->GetValueInfo()->GetArrayType() != DDLParser::kDynamic )
          {
            Token token = *m_Current;
            token.m_Lexeme = Str ( field->GetName() );
            return ErrorArrayNotDynamic ( m_Error, sizeof ( m_Error ), token );
          }

          Pointer< DDLParser::Parallel > parallel = value_area->Allocate< DDLParser::Parallel >();
          parallel->m_Size = sizeof ( DDLParser::Parallel );
          parallel->m_Type = DDLParser::kParallel;
          parallel->m_Array = array;
          CHECK ( Match ( tID ) );
          CHECK ( Match ( ')' ) );
          parallel->m_Next = &*field->m_ValueInfo.m_Tags;
          field->m_ValueInfo.m_Tags = &*parallel;
          value_area->AddRelPointer ( &parallel->m_Array );
          value_area->AddRelPointer ( &parallel->m_Next );
        }
        break;
        case tUnits:
        {
          Pointer< DDLParser::Units > units = value_area->Allocate< DDLParser::Units >();
          units->m_Size = sizeof ( DDLParser::Units );
          units->m_Type = DDLParser::kUnits;
          CHECK ( ParseInfo ( tUnits, units->m_Units, str_area ) );
          units->m_Next = &*field->m_ValueInfo.m_Tags;
          field->m_ValueInfo.m_Tags = &*units;
          value_area->AddRelPointer ( &units->m_Units );
          value_area->AddRelPointer ( &units->m_Next );
        }
        break;
        case tTag:
        {
          Pointer< DDLParser::GenericTag > tag = value_area->Allocate< DDLParser::GenericTag >();
          Str tag_name;
          CHECK ( ParseGenericTag ( tag, value_area, str_area, &tag_name ) );
          tag->m_Next = &*field->m_ValueInfo.m_Tags;
          field->m_ValueInfo.m_Tags = &*tag;
          value_area->AddRelPointer ( &tag->m_Next );

          if ( m_CheckFieldTag )
          {
            if ( ! ( *m_CheckFieldTag ) ( DDLParser::Definition::FromBinRep ( m_DDL ), &*structure, &*field, &*tag, &tag_name, &parsed, m_Error, sizeof ( m_Error ) ) )
            {
              return false;
            }
          }

          break;
        }
        default:
          return ErrorInvalidInfo ( m_Error, sizeof ( m_Error ), *m_Current );
      }

      if ( m_Current->m_ID != ',' )
      {
        break;
      }

      Match();
    }

    // Check if default values are in the specified range.
    if ( field->m_ValueInfo.m_ArrayType != DDLParser::kDynamic && parsed.Exists ( tUIRange ) )
    {
      DDLParser::UIRange* range = ( DDLParser::UIRange* ) &*field->m_ValueInfo.m_Tags;

      for ( uint32_t i = 0; i < field->m_ValueInfo.GetCount(); i++ )
      {
        DDLParser::StructFieldValue* value = &*field->m_ValueInfo.m_Value;

        if ( value != NULL )
        {
          bool ok;

          switch ( field->m_ValueInfo.m_Type )
          {
            case DDLParser::kUint8:
              ok = value->m_Uint8[0] >= range->GetHardMin()->m_Uint8 && value->m_Uint8[0] <= range->GetHardMax()->m_Uint8;
              break;
            case DDLParser::kUint16:
              ok = value->m_Uint16[0] >= range->GetHardMin()->m_Uint16 && value->m_Uint16[0] <= range->GetHardMax()->m_Uint16;
              break;
            case DDLParser::kUint32:
              ok = value->m_Uint32[0] >= range->GetHardMin()->m_Uint32 && value->m_Uint32[0] <= range->GetHardMax()->m_Uint32;
              break;
            case DDLParser::kUint64:
              ok = value->m_Uint64[0] >= range->GetHardMin()->m_Uint64 && value->m_Uint64[0] <= range->GetHardMax()->m_Uint64;
              break;
            case DDLParser::kInt8:
              ok = value->m_Int8[0] >= range->GetHardMin()->m_Int8 && value->m_Int8[0] <= range->GetHardMax()->m_Int8;
              break;
            case DDLParser::kInt16:
              ok = value->m_Int16[0] >= range->GetHardMin()->m_Int16 && value->m_Int16[0] <= range->GetHardMax()->m_Int16;
              break;
            case DDLParser::kInt32:
              ok = value->m_Int32[0] >= range->GetHardMin()->m_Int32 && value->m_Int32[0] <= range->GetHardMax()->m_Int32;
              break;
            case DDLParser::kInt64:
              ok = value->m_Int64[0] >= range->GetHardMin()->m_Int64 && value->m_Int64[0] <= range->GetHardMax()->m_Int64;
              break;
            case DDLParser::kFloat32:
              ok = value->m_Float32[0] >= range->GetHardMin()->m_Float32 && value->m_Float32[0] <= range->GetHardMax()->m_Float32;
              break;
            case DDLParser::kFloat64:
              ok = value->m_Float64[0] >= range->GetHardMin()->m_Float64 && value->m_Float64[0] <= range->GetHardMax()->m_Float64;
              break;
          }

          if ( !ok )
          {
            return ErrorDefaultValueOutOfRange ( m_Error, sizeof ( m_Error ), *m_Current );
          }
        }
      }
    }

    return true;
  }

  bool
  Parser::ParseFieldType ( Pointer<DDLParser::StructValueInfo> value_info, Area* vi_area, StringArea& str_area )
  {
    ( void ) vi_area;
    ( void ) str_area;
    value_info->m_Size = sizeof ( DDLParser::StructValueInfo );
    value_info->m_TypeNameHash = m_Current->m_Lexeme.GetHash();

    switch ( m_Current->m_ID )
    {
      case tUint8:
        value_info->m_Type = DDLParser::kUint8;
        break;
      case tUint16:
        value_info->m_Type = DDLParser::kUint16;
        break;
      case tUint32:
        value_info->m_Type = DDLParser::kUint32;
        break;
      case tUint64:
        value_info->m_Type = DDLParser::kUint64;
        break;
      case tInt8:
        value_info->m_Type = DDLParser::kInt8;
        break;
      case tInt16:
        value_info->m_Type = DDLParser::kInt16;
        break;
      case tInt32:
        value_info->m_Type = DDLParser::kInt32;
        break;
      case tInt64:
        value_info->m_Type = DDLParser::kInt64;
        break;
      case tFloat32:
        value_info->m_Type = DDLParser::kFloat32;
        break;
      case tFloat64:
        value_info->m_Type = DDLParser::kFloat64;
        break;
      case tString:
        value_info->m_Type = DDLParser::kString;
        break;
      case tBoolean:
        value_info->m_Type = DDLParser::kBoolean;
        break;
      case tFile:
        value_info->m_Type = DDLParser::kFile;
        break;
      case tTuid:
        value_info->m_Type = DDLParser::kTuid;
        break;
      case tJson:
        value_info->m_Type = DDLParser::kJson;
        break;
      case tID:
      {
        DDLParser::Aggregate* aggregate = FindAggregate ( m_Current->m_Lexeme );

        if ( aggregate == NULL )
        {
          /*value_info->m_Type     = DDLParser::kUnknown;
          value_info->m_TypeName = str_area.Add(m_Current->m_Lexeme);
          vi_area->AddRelPointer(&value_info->m_TypeName);*/
          return ErrorUnknownIdentifier ( m_Error, sizeof ( m_Error ), *m_Current );
        }
        else
        {
          value_info->m_Type = aggregate->m_Type;
        }

        break;
      }
      default:
        CHECK ( Match ( tID ) );
        break;
    }

    Match();

    if ( m_Current->m_ID == '[' )
    {
      Match();

      if ( m_Current->m_ID == ']' )
      {
        value_info->m_ArrayType = DDLParser::kDynamic;
        value_info->m_Count = 0;
      }
      else
      {
        CHECK ( ParseExpression() );
        Value value = PopValue();

        if ( !value.IsUint32() )
        {
          return ErrorIntegerExpressionExpected ( m_Error, sizeof ( m_Error ), *m_Current );
        }

        if ( value.GetInt() == 0 )
        {
          return ErrorArraySizeIsZero ( m_Error, sizeof ( m_Error ), *m_Current );
        }

        value_info->m_ArrayType = DDLParser::kFixed;
        value_info->m_Count = ( uint32_t ) value.GetInt();
      }

      CHECK ( Match ( ']' ) );
    }
    else if ( m_Current->m_ID == '{' )
    {
      Match();

      switch ( m_Current->m_ID )
      {
        case tUint8:
          value_info->m_KeyType = DDLParser::kUint8;
          break;
        case tUint16:
          value_info->m_KeyType = DDLParser::kUint16;
          break;
        case tUint32:
          value_info->m_KeyType = DDLParser::kUint32;
          break;
        case tUint64:
          value_info->m_KeyType = DDLParser::kUint64;
          break;
        case tInt8:
          value_info->m_KeyType = DDLParser::kInt8;
          break;
        case tInt16:
          value_info->m_KeyType = DDLParser::kInt16;
          break;
        case tInt32:
          value_info->m_KeyType = DDLParser::kInt32;
          break;
        case tInt64:
          value_info->m_KeyType = DDLParser::kInt64;
          break;
        case tString:
          value_info->m_KeyType = DDLParser::kString;
          break;
        case tFile:
          value_info->m_KeyType = DDLParser::kFile;
          break;
        case tTuid:
          value_info->m_KeyType = DDLParser::kTuid;
          break;
        default:
          return ErrorInvalidHashmapKey ( m_Error, sizeof ( m_Error ), *m_Current );
      }

      Match();
      Match ( '}' );
      value_info->m_ArrayType = DDLParser::kHashmap;
      value_info->m_Count = 0;
    }
    else
    {
      value_info->m_ArrayType = DDLParser::kScalar;
      value_info->m_Count = 0;
    }

    return true;
  }

  bool
  Parser::ParseField ( Pointer<DDLParser::StructField>& field, Pointer<DDLParser::Struct> structure, Area* field_area, Area* value_area, Area* valueptr_area, StringArea& str_area )
  {
    // Create a new field.
    field = field_area->Allocate<DDLParser::StructField>();
    field_area->AddRelPointer ( &field->m_ValueInfo.m_Value );
    field_area->AddRelPointer ( &field->m_ValueInfo.m_Tags );
    field_area->AddRelPointer ( &field->m_Name );
    field_area->AddRelPointer ( &field->m_Author );
    field_area->AddRelPointer ( &field->m_Description );
    field_area->AddRelPointer ( &field->m_Label );
    field->m_Size = sizeof ( DDLParser::StructField );
    field->m_Name = NULL;
    field->m_Author = NULL;
    field->m_Description = NULL;
    field->m_Label = NULL;
    field->m_ValueInfo.m_Size = sizeof ( DDLParser::StructValueInfo );
    field->m_ValueInfo.m_NameHash = 0;
    field->m_ValueInfo.m_Type = 0;
    field->m_ValueInfo.m_TypeNameHash = 0;
    field->m_ValueInfo.m_ArrayType = DDLParser::kScalar;
    field->m_ValueInfo.m_Count = 0;
    field->m_ValueInfo.m_Value = NULL;
    field->m_ValueInfo.m_Tags = NULL;
    // Parse its type.
    Pointer<DDLParser::StructValueInfo> value_info ( field.m_Area );
    value_info = &field->m_ValueInfo;
    const Typedef* td = m_Typedefs.Find ( m_Current->m_Lexeme.GetHash() );
    GrowableArray< Token >::Iterator save;

    // If the type is a typedef, make the parsing continue at the typedef token stream.
    if ( td != NULL )
    {
      save = m_TokenIterator;
      CHECK ( Match() ); // Skip the typedef name
      m_TokenIterator = td->m_Iterator;
      m_Current = m_TokenIterator.Next();
    }

    CHECK ( ParseFieldType ( value_info, field_area, str_area ) );

    // Make the parsing continue back on the regular token stream to get the field id.
    if ( td != NULL )
    {
      GrowableArray< Token >::Iterator temp = m_TokenIterator;
      m_TokenIterator = save;
      save = temp;
      m_Current = m_TokenIterator.Next();
    }

    // Check if the field already exists.
    if ( structure->FindField ( m_Current->m_Lexeme.GetHash() ) != NULL )
    {
      return ErrorDuplicateIdentifier ( m_Error, sizeof ( m_Error ), *m_Current );
    }

    // Set its name.
    field->m_Name = str_area.Add ( m_Current->m_Lexeme );
    field->m_ValueInfo.m_NameHash = m_Current->m_Lexeme.GetHash();
    CHECK ( Match ( tID ) );

    // If the typedef includes tags, parse those tags.
    if ( td != NULL )
    {
      if ( save.Next()->m_ID == ',' )
      {
        GrowableArray< Token >::Iterator temp = m_TokenIterator;
        m_TokenIterator = save;
        save = temp;
        CHECK ( ParseFieldInfo ( structure, field, value_area, valueptr_area, str_area ) );
        m_TokenIterator = save;
      }
    }

    // If there's a comma, field info must follow.
    if ( m_Current->m_ID == ',' )
    {
      Match();
      CHECK ( ParseFieldInfo ( structure, field, value_area, valueptr_area, str_area ) );
    }

    // End of field declaration.
    return Match ( ';' );
  }

  bool
  Parser::ParseStructInfo ( Pointer<DDLParser::Struct> structure, Area* value_area, StringArea& str_area )
  {
    // Create a set to avoid parsing duplicate tags.
    TokenID set_buffer[ 64 ];
    TagSet parsed ( set_buffer, sizeof ( set_buffer ) );

    for ( ;; )
    {
      if ( parsed.Exists ( m_Current->m_ID ) )
      {
        return ErrorDuplicateInfo ( m_Error, sizeof ( m_Error ), *m_Current );
      }
      else if ( m_Current->m_ID != tTag )
      {
        // Insert the tag into the set except if it's a generic tag
        // (we can have as many of those as we want)
        parsed.Insert ( m_Current->m_ID );
      }

      switch ( m_Current->m_ID )
      {
        case tAuthor:
          CHECK ( ParseInfo ( tAuthor, structure->m_Author, str_area ) );
          break;
        case tDescription:
          CHECK ( ParseInfo ( tDescription, structure->m_Description, str_area ) );
          break;
        case tLabel:
          CHECK ( ParseInfo ( tLabel, structure->m_Label, str_area ) );
          break;
        case tBase:
        {
          Match();
          CHECK ( Match ( '(' ) );
          DDLParser::Aggregate* aggregate = FindAggregate ( m_Current->m_Lexeme.GetHash() );

          if ( aggregate == NULL || aggregate->GetType() != DDLParser::kStruct )
          {
            return ErrorUnknownIdentifier ( m_Error, sizeof ( m_Error ), *m_Current );
          }

          structure->m_Parent = aggregate->ToStruct();
          CHECK ( Match ( tID ) );
          CHECK ( Match ( ')' ) );
        }
        break;
        case tUIRender:
        {
          Pointer<DDLParser::UIRender> render = value_area->Allocate<DDLParser::UIRender>();
          render->m_Size = sizeof ( DDLParser::UIRender );
          render->m_Type = DDLParser::kUIRender;
          CHECK ( ParseInfo ( tUIRender, render->m_Render, str_area ) );
          render->m_Next = &*structure->m_Tags;
          structure->m_Tags = &*render;
          value_area->AddRelPointer ( &render->m_Render );
          value_area->AddRelPointer ( &render->m_Next );
        }
        break;
        case tVersion:
        {
          Pointer<DDLParser::Version> version = value_area->Allocate<DDLParser::Version>();
          version->m_Size = sizeof ( DDLParser::Version );
          version->m_Type = DDLParser::kVersion;
          CHECK ( ParseInfo ( tVersion, version->m_Version, str_area ) );
          version->m_Next = &*structure->m_Tags;
          structure->m_Tags = &*version;
          value_area->AddRelPointer ( &version->m_Version );
          value_area->AddRelPointer ( &version->m_Next );
        }
        break;
        case tCallback:
        {
          Pointer<DDLParser::Callback> callback = value_area->Allocate<DDLParser::Callback>();
          callback->m_Size = sizeof ( DDLParser::Callback );
          callback->m_Type = DDLParser::kCallback;
          CHECK ( ParseInfo ( tCallback, callback->m_Callback, str_area ) );
          callback->m_Next = &*structure->m_Tags;
          structure->m_Tags = &*callback;
          value_area->AddRelPointer ( &callback->m_Callback );
          value_area->AddRelPointer ( &callback->m_Next );
        }
        break;
        case tKey:
        {
          Pointer<DDLParser::Key> Key = value_area->Allocate<DDLParser::Key>();
          Key->m_Size = sizeof ( DDLParser::Key );
          Key->m_Type = DDLParser::kKey;
          CHECK ( ParseInfo ( tKey, Key->m_Key, str_area ) );
          Key->m_Next = &*structure->m_Tags;
          structure->m_Tags = &*Key;
          value_area->AddRelPointer ( &Key->m_Key );
          value_area->AddRelPointer ( &Key->m_Next );
        }
        break;
        case tTag:
        {
          Pointer< DDLParser::GenericTag > tag = value_area->Allocate< DDLParser::GenericTag >();
          Str tag_name;
          CHECK ( ParseGenericTag ( tag, value_area, str_area, &tag_name ) );
          tag->m_Next = &*structure->m_Tags;
          structure->m_Tags = &*tag;
          value_area->AddRelPointer ( &tag->m_Next );

          if ( m_CheckStructTag )
          {
            if ( ! ( *m_CheckStructTag ) ( DDLParser::Definition::FromBinRep ( m_DDL ), &*structure, &*tag, &tag_name, &parsed, m_Error, sizeof ( m_Error ) ) )
            {
              return false;
            }
          }

          break;
        }
        default:
          return ErrorInvalidInfo ( m_Error, sizeof ( m_Error ), *m_Current );
      }

      if ( m_Current->m_ID != ',' )
      {
        break;
      }

      Match();
    }

    return true;
  }

  bool
  Parser::ParseStruct ( AreaManager& bin_rep )
  {
    // Create the areas.
    Area* header_area   = bin_rep.NewArea();
    Area* field_area    = bin_rep.NewArea();
    Area* value_area    = bin_rep.NewArea();
    Area* valueptr_area = bin_rep.NewArea();
    StringArea str_area ( bin_rep.NewArea() );
    // Parse the struct.
    CHECK ( Match ( tStruct ) );
    // Check if the aggregate already exists.
    const Token* duplicate = m_UsedIdentifiers.Find ( m_Current->m_Lexeme.GetHash() );

    if ( duplicate != NULL )
    {
      return ErrorDuplicateIdentifier ( m_Error, sizeof ( m_Error ), *m_Current, *duplicate );
    }

    m_UsedIdentifiers.Insert ( m_Current->m_Lexeme.GetHash(), *m_Current );
    // Create a new struct.
    Pointer<DDLParser::Struct> structure = header_area->Allocate<DDLParser::Struct>();
    header_area->AddRelPointer ( &structure->m_Name );
    header_area->AddRelPointer ( &structure->m_Author );
    header_area->AddRelPointer ( &structure->m_Description );
    header_area->AddRelPointer ( &structure->m_Label );
    header_area->AddRelPointer ( &structure->m_Parent );
    header_area->AddRelPointer ( &structure->m_Tags );
    header_area->AddRelPointer ( &structure->m_Definition );
    structure->m_Size = sizeof ( DDLParser::Struct );
    structure->m_Type = DDLParser::kStruct;
    structure->m_Name = NULL;
    structure->m_Author = NULL;
    structure->m_Description = NULL;
    structure->m_Label = NULL;
    structure->m_NumFields = 0;
    structure->m_NameHash = 0;
    structure->m_Parent = NULL;
    structure->m_Tags = NULL;
    structure->m_Definition = m_DDL;
    // Set its name.
    structure->m_Name = str_area.Add ( m_Current->m_Lexeme );
    structure->m_NameHash = m_Current->m_Lexeme.GetHash();
    CHECK ( Match ( tID ) );

    // If there's a comma, struct info must follow.
    if ( m_Current->m_ID == ',' )
    {
      Match();
      CHECK ( ParseStructInfo ( structure, value_area, str_area ) );
    }

    // Parse at least one item.
    CHECK ( Match ( '{' ) );
    Pointer<DDLParser::StructField> field;

    /*CHECK(ParseField(field, structure, field_area, value_area, valueptr_area, str_area));
    header_area->Allocate<DDLParser::StructFieldPtr>();
    header_area->AddRelPointer(&structure->m_Fields[structure->m_NumFields]);
    structure->m_Fields[structure->m_NumFields] = &*field;
    structure->m_NumFields++;*/
    // Parse the rest of the items.
    while ( m_Current->m_ID != '}' )
    {
      CHECK ( ParseField ( field, structure, field_area, value_area, valueptr_area, str_area ) );
      header_area->Allocate<DDLParser::StructFieldPtr>();
      header_area->AddRelPointer ( &structure->m_Fields[structure->m_NumFields] );
      structure->m_Fields[structure->m_NumFields] = &*field;
      structure->m_NumFields++;
    }

    str_area.GetArea()->Align ( 4 );
    CHECK ( Match ( '}' ) );

    // Skip a semicolon if one is present.
    if ( m_Current->m_ID == ';' )
    {
      Match();
    }

    return true;
  }

  bool
  Parser::ParseTypedef ( AreaManager& bin_rep )
  {
    ( void ) bin_rep;
    // Save the start of the typedef token stream.
    GrowableArray< Token >::Iterator typedef_start = m_TokenIterator;
    CHECK ( Match ( tTypedef ) );

    // Check for a valid DDL type.
    switch ( m_Current->m_ID )
    {
      case tUint8:
      case tUint16:
      case tUint32:
      case tUint64:
      case tInt8:
      case tInt16:
      case tInt32:
      case tInt64:
      case tFloat32:
      case tFloat64:
      case tString:
      case tBoolean:
      case tFile:
      case tTuid:
      case tJson:
      case tID:
        CHECK ( Match() );
        break;
      default:
        CHECK ( Match ( tID ) );
        break;
    }

    // Check for arrays and hashmaps.
    if ( m_Current->m_ID == '[' )
    {
      CHECK ( Match() );

      if ( m_Current->m_ID != ']' )
      {
        CHECK ( ParseExpression() );
        PopValue();
      }

      CHECK ( Match ( ']' ) );
    }
    else if ( m_Current->m_ID == '{' )
    {
      CHECK ( Match() );

      // Check for a valid hashmap key type.
      switch ( m_Current->m_ID )
      {
        case tUint8:
        case tUint16:
        case tUint32:
        case tUint64:
        case tInt8:
        case tInt16:
        case tInt32:
        case tInt64:
        case tString:
        case tFile:
        case tTuid:
          CHECK ( Match() );
          break;
        default:
          return ErrorInvalidHashmapKey ( m_Error, sizeof ( m_Error ), *m_Current );
      }

      CHECK ( Match ( '}' ) );
    }

    // Parse tags present after the type.
    while ( m_Current->m_ID == ',' )
    {
      CHECK ( Match() );

      switch ( m_Current->m_ID )
      {
        case tAuthor:
        case tDescription:
        case tLabel:
        case tUIRender:
        case tCallback:
        case tKey:
        case tUnits:
          CHECK ( Match() );
          CHECK ( Match ( '(' ) );
          CHECK ( Match ( tLiteral ) );
          CHECK ( Match ( ')' ) );
          break;
        case tExtensions:
        case tVaultHints:
          CHECK ( Match() );
          CHECK ( Match ( '(' ) );
          CHECK ( Match ( tLiteral ) );

          while ( m_Current->m_ID == ',' )
          {
            CHECK ( Match() );
            CHECK ( Match ( tLiteral ) );
          }

          CHECK ( Match ( ')' ) );
          break;
        case tUIRange:
          CHECK ( Match() );
          CHECK ( Match ( '(' ) );
          CHECK ( ParseExpression() );
          PopValue();
          CHECK ( Match ( ',' ) );
          CHECK ( ParseExpression() );
          PopValue();

          if ( m_Current->m_ID == ',' )
          {
            CHECK ( Match() );
            CHECK ( ParseExpression() );
            PopValue();
            CHECK ( Match ( ',' ) );
            CHECK ( ParseExpression() );
            PopValue();

            if ( m_Current->m_ID == ',' )
            {
              CHECK ( Match() );
              CHECK ( ParseExpression() );
              PopValue();
            }
          }

          break;
        case tTag:
          CHECK ( Match() );
          CHECK ( Match ( '(' ) );
          CHECK ( Match ( tID ) );

          while ( m_Current->m_ID == ',' )
          {
            CHECK ( Match() );
            CHECK ( ParseExpression() );
            PopValue();
          }

          CHECK ( Match ( ')' ) );
          break;
        default:
          return ErrorInvalidInfo ( m_Error, sizeof ( m_Error ), *m_Current );
      }
    }

    const Typedef* duplicate = m_Typedefs.Find ( m_Current->m_Lexeme.GetHash() );

    if ( duplicate != NULL )
    {
      GrowableArray< Token >::Iterator it = duplicate->m_Iterator;
      Token* token;
      Token* previous = 0;

      while ( ( token = it.Next() ) != 0 && token->m_ID != ';' )
      {
        previous = token;
      }

      return ErrorDuplicateIdentifier ( m_Error, sizeof ( m_Error ), *m_Current, *previous );
    }

    // Get the typedef name.
    Str typedef_name = m_Current->m_Lexeme;
    CHECK ( Match ( tID ) );
    // End of the typedef, save it to the typedef hashmap.
    CHECK ( Match ( ';' ) );
    Typedef td;
    td.m_Iterator = typedef_start;
    m_Typedefs.Insert ( typedef_name.GetHash(), td );
    return true;
  }

  bool
  Parser::ParseExpression()
  {
    return ParseTernary();
  }

  bool
  Parser::ParseTernary()
  {
    CHECK ( ParseLogicOr() );

    if ( m_Current->m_ID == '?' )
    {
      Value condition = PopValue();

      if ( condition.GetType() == Value::kString )
      {
        return ErrorTypeMismatch ( m_Error, sizeof ( m_Error ), *m_Current );
      }

      Match();
      CHECK ( ParseExpression() );
      Value first = PopValue();
      Match ( ':' );
      CHECK ( ParseExpression() );
      Value second = PopValue();

      if ( ( condition.GetType() == Value::kInt && condition.GetInt() != 0 ) || ( condition.GetType() == Value::kDouble && condition.GetDouble() != 0.0 ) )
      {
        PushValue ( first );
      }
      else
      {
        PushValue ( second );
      }
    }

    return true;
  }

  bool
  Parser::ParseLogicOr()
  {
    CHECK ( ParseLogicAnd() );

    while ( m_Current->m_ID == tLogicOr )
    {
      Match();
      CHECK ( ParseLogicAnd() );
      Value right = PopValue();
      Value left = PopValue();
      PushValue ( left || right );
    }

    return true;
  }

  bool
  Parser::ParseLogicAnd()
  {
    CHECK ( ParseArithmeticOr() );

    while ( m_Current->m_ID == tLogicAnd )
    {
      Match();
      CHECK ( ParseArithmeticOr() );
      Value right = PopValue();
      Value left = PopValue();
      PushValue ( left && right );
    }

    return true;
  }

  bool
  Parser::ParseArithmeticOr()
  {
    CHECK ( ParseArithmeticXor() );

    while ( m_Current->m_ID == '|' )
    {
      Match();
      CHECK ( ParseArithmeticXor() );
      Value right = PopValue();
      Value left = PopValue();
      PushValue ( left | right );
    }

    return true;
  }

  bool
  Parser::ParseArithmeticXor()
  {
    CHECK ( ParseArithmeticAnd() );

    while ( m_Current->m_ID == '^' )
    {
      Match();
      CHECK ( ParseArithmeticAnd() );
      Value right = PopValue();
      Value left = PopValue();
      PushValue ( left ^ right );
    }

    return true;
  }

  bool
  Parser::ParseArithmeticAnd()
  {
    CHECK ( ParseEquality() );

    while ( m_Current->m_ID == '&' )
    {
      Match();
      CHECK ( ParseEquality() );
      Value right = PopValue();
      Value left = PopValue();
      PushValue ( left & right );
    }

    return true;
  }

  bool
  Parser::ParseEquality()
  {
    CHECK ( ParseConditional() );

    while ( m_Current->m_ID == tEqual || m_Current->m_ID == tNotEqual )
    {
      TokenID op = m_Current->m_ID;
      Match();
      CHECK ( ParseConditional() );
      Value right = PopValue();
      Value left = PopValue();

      if ( op == tEqual )
      {
        PushValue ( left == right );
      }
      else
      {
        PushValue ( left != right );
      }
    }

    return true;
  }

  bool
  Parser::ParseConditional()
  {
    CHECK ( ParseShift() );

    while ( m_Current->m_ID == '<' || m_Current->m_ID == tLessEqual || m_Current->m_ID == '>' || m_Current->m_ID == tGreaterEqual )
    {
      TokenID op = m_Current->m_ID;
      Match();
      CHECK ( ParseShift() );
      Value right = PopValue();
      Value left = PopValue();

      switch ( ( int ) op )
      {
        case '<':
          PushValue ( left < right );
          break;
        case tLessEqual:
          PushValue ( left <= right );
          break;
        case '>':
          PushValue ( left > right );
          break;
        case tGreaterEqual:
          PushValue ( left >= right );
          break;
        default: // Never happens
          break;
      }
    }

    return true;
  }

  bool
  Parser::ParseShift()
  {
    CHECK ( ParseTerm() );

    while ( m_Current->m_ID == tShiftLeft || m_Current->m_ID == tShiftRight )
    {
      TokenID op = m_Current->m_ID;
      Match();
      CHECK ( ParseTerm() );
      Value right = PopValue();
      Value left = PopValue();

      if ( op == tShiftLeft )
      {
        PushValue ( left << right );
      }
      else
      {
        PushValue ( left >> right );
      }
    }

    return true;
  }

  bool
  Parser::ParseTerm()
  {
    CHECK ( ParseFactor() );

    while ( m_Current->m_ID == '+' || m_Current->m_ID == '-' )
    {
      TokenID op = m_Current->m_ID;
      Match();
      CHECK ( ParseFactor() );
      Value right = PopValue();
      Value left = PopValue();

      if ( op == '+' )
      {
        PushValue ( left + right );
      }
      else
      {
        PushValue ( left - right );
      }
    }

    return true;
  }

  bool
  Parser::ParseFactor()
  {
    CHECK ( ParseUnary() );

    while ( m_Current->m_ID == '*' || m_Current->m_ID == '/' || m_Current->m_ID == '%' )
    {
      TokenID op = m_Current->m_ID;
      Match();
      CHECK ( ParseUnary() );
      Value right = PopValue();
      Value left = PopValue();

      if ( op == '*' )
      {
        PushValue ( left * right );
      }
      else if ( op == '/' )
      {
        PushValue ( left / right );
      }
      else
      {
        PushValue ( left % right );
      }
    }

    return true;
  }

  bool
  Parser::ParseUnary()
  {
    TokenID op = m_Current->m_ID;

    if ( op == '+' || op == '-' || op == '~' || op == '!' )
    {
      Match();
      CHECK ( ParseUnary() );
      Value value = PopValue();

      switch ( ( int ) op )
      {
        case '+':
          PushValue ( value );
          break;
        case '-':
          PushValue ( -value );
          break;
        case '~':
          PushValue ( ~value );
          break;
        case '!':
          PushValue ( !value );
          break;
        default: // Never happens
          break;
      }
    }
    else
    {
      CHECK ( ParseTerminal() );
    }

    return true;
  }

  bool
  Parser::ParseTerminal()
  {
    switch ( ( int ) m_Current->m_ID )
    {
      case tOctal:
      case tHexadecimal:
      case tBinary:
      case tDecimal:
        PushValue ( Value ( ToInt ( *m_Current ) ) );
        Match();
        break;
      case tReal:
        PushValue ( Value ( strtod ( m_Current->m_Lexeme.GetChars(), NULL ) ) );
        Match();
        break;
      case tLiteral:
        PushValue ( Value ( m_Current->m_Lexeme ) );
        Match();
        break;
      case tTrue:
        PushValue ( Value ( ( int64_t ) 1 ) );
        Match();
        break;
      case tFalse:
        PushValue ( Value ( ( int64_t ) 0 ) );
        Match();
        break;
      case tPi:
        PushValue ( Value ( ( double ) M_PI ) );
        Match();
        break;
      case tE:
        PushValue ( Value ( ( double ) M_E ) );
        Match();
        break;
      case tID:
      {
        DDLParser::Aggregate* aggregate = FindAggregate ( m_Current->m_Lexeme.GetHash() );

        if ( aggregate == NULL )
        {
          return ErrorUnknownIdentifier ( m_Error, sizeof ( m_Error ), *m_Current );
        }

        if ( aggregate->GetType() != DDLParser::kSelect )
        {
          return ErrorSelectExpected ( m_Error, sizeof ( m_Error ), *m_Current );
        }

        Match();
        CHECK ( Match ( '.' ) );

        if ( m_Current->m_ID != tID )
        {
          CHECK ( Match ( tID ) );
        }

        DDLParser::Select* select = aggregate->ToSelect();
        DDLParser::SelectItem* item = select->FindItem ( m_Current->m_Lexeme.GetHash() );

        if ( item == NULL )
        {
          return ErrorItemNotFound ( m_Error, sizeof ( m_Error ), *m_Current );
        }

        Match();
        PushValue ( Value ( ( int64_t ) item->GetNameHash() ) );
        break;
      }
      break;
      case '(':
        Match();
        CHECK ( ParseExpression() );
        Match ( ')' );
        break;
      default:
        return ErrorTerminalExpected ( m_Error, sizeof ( m_Error ), *m_Current );
    }

    return true;
  }

  int64_t
  Parser::ToInt ( const Token& token )
  {
    TokenID id         = token.m_ID;
    const char* lexeme = token.m_Lexeme.GetChars();
    int first_digit    = ( id == tHexadecimal || id == tBinary ) ? 2 : 0;
    int base           = ( id == tDecimal ) ? 10 : ( id == tOctal ) ? 8 : ( id == tHexadecimal ) ? 16 : 2;
    int64_t value      = 0;

    for ( size_t i = first_digit; i < token.m_Lexeme.GetLength(); i++ )
    {
      value *= base;
      char k = lexeme[i];

      if ( isdigit ( k ) )
      {
        value += k - '0';
      }
      else
      {
        value += tolower ( k ) - 'a' + 10;
      }
    }

    return value;
  }

  bool
  Parser::PushValue ( const Value& value )
  {
    if ( !value.IsValid() )
    {
      return ErrorTypeMismatch ( m_Error, sizeof ( m_Error ), *m_Current );
    }

    m_Stack.Push ( value );
    return true;
  }

  Value
  Parser::PopValue()
  {
    Value value;
    m_Stack.Pop ( value );
    return value;
  }
};
