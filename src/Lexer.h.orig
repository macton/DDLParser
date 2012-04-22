#pragma once

#include <stdint.h>

#include "Str.h"
#include "GrowableArray.h"

namespace DDLParser
{
  // The possible complex tokens in a DDL source file. Starts at 256 because simple tokens
  // composed of just one character have the value of the character code.
  enum TokenID
  {
    tEOF = 256,
    tID,
    tLiteral,
    tDecimal,
    tHexadecimal,
    tOctal,
    tBinary,
    tReal,
    tTrue,
    tFalse,
    tPi,
    tE,
    tInt8,
    tUint8,
    tInt16,
    tUint16,
    tInt32,
    tUint32,
    tInt64,
    tUint64,
    tFloat32,
    tFloat64,
    tString,
    tSelect,
    tBitfield,
    tStruct,
    tBoolean,
    tFile,
    tAuthor,
    tDescription,
    tLabel,
    tValue,
    tDefault,
    tExtensions,
    tVaultHints,
    tLogicOr,
    tLogicAnd,
    tEqual,
    tNotEqual,
    tLessEqual,
    tGreaterEqual,
    tShiftLeft,
    tShiftRight,
    tEmpty,
    tBase,
    tUIRange,
    tUIRender,
    tSize,
    tComponentPostJob,
    tTargets,
    tParallel,
    tVersion,
    tCallback,
    tKey,
    tUnits,
    tTuid,
    tJson,
    tTag,
    tTypedef
  };

  // A token has the token ID, the lexeme (string identified by the lexer as having that ID) and
  // the line in the source code where the token were found for error messages.
  struct Token
  {
    TokenID  m_ID;
    Str   m_Lexeme;
    Str   m_FileName;
    uint32_t m_Line;
  };

  // The lexer transforms a memory buffer with a DDL source code into a GrowableAlloc of Token
  // with all tokens found in the source code.
  class Lexer
  {
    public:
      // Tokenizes buffer into the GrowableAlloc tokes. If two_us_reserved is true, any identifier
      // starting with two underlines generate an error because those are reserved for DDL
      // configuration files where __FILE_EXTENSIONS and __TARGETS are defined. Returns true if
      // everything was correctly tokenized or false otherwise. In this case, the error parameter
      // points to the error message.
      bool Tokenize(const uint8_t* buffer, size_t size, GrowableArray< Token >& tokens, bool two_us_reserved, char*& error);

    private:
      // Gets the next token from the source code.
      bool           GetNext(Token& token);
      // True if the current character in the source code is a space (according to isspace from ctype.h).
      bool           IsSpace() const;
      // Ditto for alphabetic characters.
      bool           IsAlpha() const;
      // Ditto for decimal digits.
      bool           IsDigit() const;
      // Ditto for hexadecimal digits.
      bool           IsHexa() const;
      // True if the current character is alphanumeric (according to isalnum) or an underline.
      bool           IsAlNum() const;
      // True if '0' <= current character <= '7'
      bool           IsOctal() const;
      // Returns the current character as an unsigned byte of -1 if we're past of the end of the source code.
      int            Get() const;
      // Goes to the next character and updates the current source line if necessary.
      void           Skip();

      // The source line counter.
      uint32_t       m_Line;
      // The pointer to the current character in the source code.
      const uint8_t* m_Buffer;
      // The pointer to the end + 1 of the source code.
      const uint8_t* m_BufferEnd;
      // True if we're at the first character of a line.
      bool            m_AtFirstChar;
      // The current file name.
      Str          m_FileName;
      // True if identifiers beginning with two underlines are reserved.
      bool           m_TwoUsReserved;
      // Buffer to hold error messages.
      char           m_Error[256];
  };

  const char* GetTokenStr( TokenID token );
}
