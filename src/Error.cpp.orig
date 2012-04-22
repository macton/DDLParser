#include <stdio.h>
#include <malloc.h>

#include "Lexer.h"

#define ERROR_HEADER "DDLC ERROR : "
#define ARRAY_SIZE( x ) ( sizeof( x ) / sizeof( x[ 0 ] ) )

namespace DDLParser
{
  static bool BuildErrorMsg( char* out, uint32_t out_size, const char* format, const Token& token, const char* more = NULL )
  {
    char* file_name = (char*)alloca( token.m_FileName.GetLength() + 1 );
    token.m_FileName.ToCStr( file_name );

    char* lexeme = NULL;
    
    if ( more == NULL )
    {
      lexeme = (char*)alloca( token.m_Lexeme.GetLength() + 1 );
      token.m_Lexeme.ToCStr( lexeme );
    }
    
    int num_written = snprintf( out, out_size, "%s(%u) : " ERROR_HEADER, file_name, token.m_Line );
    snprintf( out + num_written, out_size - (uint32_t)num_written, format, more == NULL ? lexeme : more );

    return false;
  }

  bool
  ErrorUnterminatedLiteral(char* out, uint32_t out_size, const Token& token)
  {
    return BuildErrorMsg( out, out_size, "Unterminated literal", token );
  }

  bool
  ErrorInvalidEscapeSequence(char* out, uint32_t out_size, const Token& token, const char* value_string )
  {
    return BuildErrorMsg( out, out_size, "Invalid escape sequence in: \"%s\"", token, value_string );
  }

  bool
  ErrorUnterminatedComment(char* out, uint32_t out_size, const Token& token)
  {
    return BuildErrorMsg( out, out_size, "Unterminated comment", token );
  }

  bool
  ErrorInvalidCharacter(char* out, uint32_t out_size, const Token& token)
  {
    char invalid = token.m_Lexeme.GetChars()[0];
    char more[ 8 ];

    if (invalid >= 32 || invalid <= 126)
    {
      snprintf( more, ARRAY_SIZE( more ), "%c", invalid );
    }
    else
    {
      snprintf( more, ARRAY_SIZE( more ), "\\%x02x", invalid );
    }
    return BuildErrorMsg( out, out_size, "Invalid character: '%s'", token, more );
  }

  bool
  ErrorUnexpectedToken(char* out, uint32_t out_size, const Token& token)
  {
    return BuildErrorMsg( out, out_size, "Unexpected \"%s\"", token );
  }

  bool
  ErrorDuplicateIdentifier(char* out, uint32_t out_size, const Token& token)
  {
    return BuildErrorMsg( out, out_size, "Duplicate identifier \"%s\"", token );
  }

  bool
  ErrorDuplicateIdentifier(char* out, uint32_t out_size, const Token& token, const Token& duplicate)
  {
    char* file_name = (char*)alloca( token.m_FileName.GetLength() + 1 );
    token.m_FileName.ToCStr( file_name );
    char* identifier = (char*)alloca( token.m_Lexeme.GetLength() + 1 );
    token.m_Lexeme.ToCStr( identifier );

    if ( duplicate.m_ID != tEOF )
    {
      char* dup_file_name = (char*)alloca( duplicate.m_FileName.GetLength() + 1 );
      duplicate.m_FileName.ToCStr( dup_file_name );

      snprintf( out, out_size, "%s(%u) : " ERROR_HEADER "Duplicate identifier \"%s\", first seen at %s(%u)", file_name, token.m_Line, identifier, dup_file_name, duplicate.m_Line );
    }
    else
    {
      snprintf( out, out_size, "%s(%u) : " ERROR_HEADER "Duplicate identifier \"%s\", first seen in a dependent file", file_name, token.m_Line, identifier );
    }
    return false;
  }

  bool
  ErrorDuplicateInfo(char* out, uint32_t out_size, const Token& token)
  {
    return BuildErrorMsg( out, out_size, "Duplicate tag \"%s\"", token );
  }

  bool
  ErrorInvalidInfo(char* out, uint32_t out_size, const Token& token)
  {
    return BuildErrorMsg( out, out_size, "Invalid tag \"%s\"", token );
  }

  bool
  ErrorUnknownIdentifier(char* out, uint32_t out_size, const Token& token)
  {
    return BuildErrorMsg( out, out_size, "Unknown identifier \"%s\"", token );
  }

  bool
  ErrorValueDoesNotFit(char* out, uint32_t out_size, const Token& token)
  {
    return BuildErrorMsg( out, out_size, "Value does not fit in destination", token );
  }

  bool
  ErrorInvalidBitCombination(char* out, uint32_t out_size, const Token& token)
  {
    return BuildErrorMsg( out, out_size, "Invalid combination of flags in bitfield value", token );
  }

  bool
  ErrorInvalidSelectValue(char* out, uint32_t out_size, const Token& token)
  {
    return BuildErrorMsg( out, out_size, "Invalid select value \"%s\"", token );
  }

  bool
  ErrorTypeMismatch(char* out, uint32_t out_size, const Token& token)
  {
    return BuildErrorMsg( out, out_size, "Type mismatch", token );
  }

  bool
  ErrorUnspecifiedDefaultValue(char* out, uint32_t out_size, const Token& token)
  {
    return BuildErrorMsg( out, out_size, "Default value unspecified", token );
  }

  bool
  ErrorInvalidBitfieldValue(char* out, uint32_t out_size, const Token& token)
  {
    return BuildErrorMsg( out, out_size, "Invalid bitfield value", token );
  }

  bool
  ErrorDuplicateBitfieldValue(char* out, uint32_t out_size, const Token& token)
  {
    return BuildErrorMsg( out, out_size, "Duplicate bitfield value", token );
  }

  bool
  ErrorTerminalExpected(char* out, uint32_t out_size, const Token& token)
  {
    return BuildErrorMsg( out, out_size, "Terminal expected", token );
  }

  bool
  ErrorMaximumNumberOfFlagsExceeded(char* out, uint32_t out_size, const Token& token)
  {
    return BuildErrorMsg( out, out_size, "Maximum number of flags in a bitfield exceeded", token );
  }

  bool
  ErrorDuplicateDefault(char* out, uint32_t out_size, const Token& token)
  {
    return BuildErrorMsg( out, out_size, "Duplicate default value in select or bitfield", token );
  }

  bool
  ErrorIntegerExpressionExpected(char* out, uint32_t out_size, const Token& token)
  {
    return BuildErrorMsg( out, out_size, "Integer expression expected", token );
  }

  bool
  ErrorArraySizeIsZero(char* out, uint32_t out_size, const Token& token)
  {
    return BuildErrorMsg( out, out_size, "Array size must be greater than zero", token );
  }

  bool
  ErrorVariableSizedArrayCannotHaveDefaultValue(char* out, uint32_t out_size, const Token& token)
  {
    return BuildErrorMsg( out, out_size, "Variable-sized arrays cannot have default values", token );
  }

  bool
  ErrorHashmapCannotHaveDefaultValue(char* out, uint32_t out_size, const Token& token)
  {
    return BuildErrorMsg( out, out_size, "Hashmaps cannot have default values", token );
  }

  bool
  ErrorWrongNumberOfDefaultValues(char* out, uint32_t out_size, const Token& token, uint32_t expected_count, uint32_t found_count)
  {
    char more[ 48 ];
    snprintf( more, ARRAY_SIZE( more ), "%u but found %u", expected_count, found_count );

    return BuildErrorMsg( out, out_size, "Wrong number of default values in array: expected %s", token, more );
  }

  bool
  ErrorInvalidTypeForExtensions(char* out, uint32_t out_size, const Token& token)
  {
    return BuildErrorMsg( out, out_size, "Extensions cannot be used with this data type", token );
  }

  bool
  ErrorInvalidTypeForVaultHints(char* out, uint32_t out_size, const Token& token)
  {
    return BuildErrorMsg( out, out_size, "VaultHints cannot be used with this data type", token );
  }

  bool
  ErrorReservedKeyword(char* out, uint32_t out_size, const Token& token)
  {
    return BuildErrorMsg( out, out_size, "Reserved keyword \"%s\"", token );
  }

  bool
  ErrorEmptyRange(char* out, uint32_t out_size, const Token& token)
  {
    return BuildErrorMsg( out, out_size, "Numeric range is empty (minimum value is greater than maximum value)", token );
  }

  bool
  ErrorInvalidTypeForRange(char* out, uint32_t out_size, const Token& token)
  {
    return BuildErrorMsg( out, out_size, "Only numeric fields can have ranges", token );
  }

  bool
  ErrorDefaultValueOutOfRange(char* out, uint32_t out_size, const Token& token)
  {
    return BuildErrorMsg( out, out_size, "Default value is out of the specified range", token );
  }

  bool
  ErrorInvalidRangeStep(char* out, uint32_t out_size, const Token& token)
  {
    return BuildErrorMsg( out, out_size, "Range step must be greater than zero", token );
  }

  bool
  ErrorInvalidTypeForSize(char* out, uint32_t out_size, const Token& token)
  {
    return BuildErrorMsg( out, out_size, "Only string fields can have size", token );
  }

  bool
  ErrorInvalidSize(char* out, uint32_t out_size, const Token& token)
  {
    return BuildErrorMsg( out, out_size, "Invalid size", token );
  }

  bool
  ErrorFileExtensionsBitfieldNotFound(char* out, uint32_t out_size, const Token& token)
  {
    return BuildErrorMsg( out, out_size, "__FILE_EXTENSIONS bitfield not declared", token );
  }

  bool
  ErrorTargetsBitfieldNotFound(char* out, uint32_t out_size, const Token& token)
  {
    return BuildErrorMsg( out, out_size, "__TARGETS bitfield not declared", token );
  }

  bool
  ErrorArrayNotDynamic(char* out, uint32_t out_size, const Token& token)
  {
    return BuildErrorMsg( out, out_size, "\"%s\" is not a dynamic array", token );
  }

  bool
  ErrorArrayTypeNotNative(char* out, uint32_t out_size, const Token& token)
  {
    return BuildErrorMsg( out, out_size, "\"%s\" is not of a native type", token );
  }

  bool
  ErrorSelectExpected( char* out, uint32_t out_size, const Token& token )
  {
    return BuildErrorMsg( out, out_size, "\"%s\" is not a select", token );
  }

  bool
  ErrorItemNotFound( char* out, uint32_t out_size, const Token& token )
  {
    return BuildErrorMsg( out, out_size, "\"%s\" is not a valid item in the select", token );
  }

  bool
  ErrorInvalidHashmapKey( char* out, uint32_t out_size, const Token& token )
  {
    return BuildErrorMsg( out, out_size, "\"%s\" is not a valid key for a hashmap", token );
  }

  bool
  ErrorInvalidDirective( char* out, uint32_t out_size, const Token& token, const Str& directive )
  {
    char* directive_str = (char*)alloca( directive.GetLength() + 1 );
    directive.ToCStr( directive_str );

    return BuildErrorMsg( out, out_size, "\"%s\" is not a valid directive", token, directive_str );
  }

  bool
  ErrorInvalidJsonValue( char* out, uint32_t out_size, const Token& token )
  {
    return BuildErrorMsg( out, out_size, "Default JSON value is not valid", token );
  }

  bool
  ErrorUnterminatedNumber( char* out, uint32_t out_size, const Token& token )
  {
    return BuildErrorMsg( out, out_size, "Unterminated number constant", token );
  }

  bool
  ErrorThrown( char* out, uint32_t out_size, const char* the_error )
  {
    snprintf( out, out_size, ERROR_HEADER "%s", the_error );
    return false;
  }
};
