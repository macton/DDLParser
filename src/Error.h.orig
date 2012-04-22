#pragma once

#include <stdint.h>

#include "Str.h"

namespace DDLParser
{
  struct Token;

  bool ErrorUnterminatedLiteral(char* out, uint32_t out_size, const Token& token);
  bool ErrorInvalidEscapeSequence(char* out, uint32_t out_size, const Token& token, const char* value_string );
  bool ErrorUnterminatedComment(char* out, uint32_t out_size, const Token& token);
  bool ErrorInvalidCharacter(char* out, uint32_t out_size, const Token& token);
  bool ErrorUnexpectedToken(char* out, uint32_t out_size, const Token& token);
  bool ErrorDuplicateIdentifier(char* out, uint32_t out_size, const Token& token);
  bool ErrorDuplicateIdentifier(char* out, uint32_t out_size, const Token& token, const Token& duplicate);
  bool ErrorDuplicateInfo(char* out, uint32_t out_size, const Token& token);
  bool ErrorInvalidInfo(char* out, uint32_t out_size, const Token& token);
  bool ErrorUnknownIdentifier(char* out, uint32_t out_size, const Token& token);
  bool ErrorValueDoesNotFit(char* out, uint32_t out_size, const Token& token);
  bool ErrorInvalidBitCombination(char* out, uint32_t out_size, const Token& token);
  bool ErrorInvalidSelectValue(char* out, uint32_t out_size, const Token& token);
  bool ErrorTypeMismatch(char* out, uint32_t out_size, const Token& token);
  bool ErrorUnspecifiedDefaultValue(char* out, uint32_t out_size, const Token& token);
  bool ErrorInvalidBitfieldValue(char* out, uint32_t out_size, const Token& token);
  bool ErrorDuplicateBitfieldValue(char* out, uint32_t out_size, const Token& token);
  bool ErrorTerminalExpected(char* out, uint32_t out_size, const Token& token);
  bool ErrorMaximumNumberOfFlagsExceeded(char* out, uint32_t out_size, const Token& token);
  bool ErrorDuplicateDefault(char* out, uint32_t out_size, const Token& token);
  bool ErrorIntegerExpressionExpected(char* out, uint32_t out_size, const Token& token);
  bool ErrorArraySizeIsZero(char* out, uint32_t out_size, const Token& token);
  bool ErrorVariableSizedArrayCannotHaveDefaultValue(char* out, uint32_t out_size, const Token& token);
  bool ErrorHashmapCannotHaveDefaultValue(char* out, uint32_t out_size, const Token& token);
  bool ErrorWrongNumberOfDefaultValues(char* out, uint32_t out_size, const Token& token, uint32_t expected_count, uint32_t found_count);
  bool ErrorInvalidTypeForExtensions(char* out, uint32_t out_size, const Token& token);
  bool ErrorInvalidTypeForVaultHints(char* out, uint32_t out_size, const Token& token);
  bool ErrorReservedKeyword(char* out, uint32_t out_size, const Token& token);
  bool ErrorEmptyRange(char* out, uint32_t out_size, const Token& token);
  bool ErrorInvalidTypeForRange(char* out, uint32_t out_size, const Token& token);
  bool ErrorDefaultValueOutOfRange(char* out, uint32_t out_size, const Token& token);
  bool ErrorInvalidRangeStep(char* out, uint32_t out_size, const Token& token);
  bool ErrorInvalidTypeForSize(char* out, uint32_t out_size, const Token& token);
  bool ErrorInvalidSize(char* out, uint32_t out_size, const Token& token);
  bool ErrorFileExtensionsBitfieldNotFound(char* out, uint32_t out_size, const Token& token);
  bool ErrorTargetsBitfieldNotFound(char* out, uint32_t out_size, const Token& token);
  bool ErrorArrayNotDynamic(char* out, uint32_t out_size, const Token& token);
  bool ErrorArrayTypeNotNative(char* out, uint32_t out_size, const Token& token);
  bool ErrorSelectExpected( char* out, uint32_t out_size, const Token& token );
  bool ErrorItemNotFound( char* out, uint32_t out_size, const Token& token );
  bool ErrorInvalidHashmapKey( char* out, uint32_t out_size, const Token& token );
  bool ErrorInvalidDirective( char* out, uint32_t out_size, const Token& token, const Str& directive );
  bool ErrorInvalidJsonValue( char* out, uint32_t out_size, const Token& token );
  bool ErrorUnterminatedNumber( char* out, uint32_t out_size, const Token& token );
  bool ErrorThrown( char* out, uint32_t out_size, const char* the_error );
};
