#include <ctype.h>

#include "Lexer.h"
#include "Error.h"

namespace DDLParser
{
// The hash table of DDL reserved keywords generated from Tokens.gperf.
#include "Tokens.inc"

  bool
  Lexer::Tokenize(const uint8_t* buffer, size_t size, GrowableArray< Token >& tokens, bool two_us_reserved, char*& error)
  {
    try
    {
      // Initializes the error parameter to the internal error buffer in Lexer.
      error           = m_Error;
      // Start line counter at 1.
      m_Line          = 1;
      // m_Buffer points to the first character in the source code.
      m_Buffer        = buffer;
      // Evaluate the end of the buffer + 1.
      m_BufferEnd     = buffer + size;
      // The first character of the input is also the first character of the first line.
      m_AtFirstChar   = true;
      // Set the current file name to nothing.
      m_FileName      = Str();
      // Save the two_us_reserved for later.
      m_TwoUsReserved = two_us_reserved;
      // Loop to tokenize the source code.
      Token token;
      do
      {
        // Try to get the next token.
        if (!GetNext(token))
        {
          // False if we had problems, the error message is generated inside GetNext.
          return false;
        }
        // Append the token to the tokens array.
        *tokens.Append() = token;
      }
      // Repeat until we reach the end of the source code.
      while (token.m_ID != tEOF);
      // No errors reported.
      error = NULL;
      // Successfull tokenization.
      return true;
    }
    catch( const char* the_error )
    {
      return ErrorThrown( m_Error, sizeof( m_Error ), the_error );
    }
  }

  bool
  Lexer::GetNext(Token& token)
  {
    // Skip spaces.
    for (;;)
    {
      if (Get() == -1)
      {
        // Return tEOF if we've reached the end of the input.
        token.m_ID = tEOF;
        token.m_Lexeme = Str("EOF");
        token.m_Line = m_Line;
        return true;
      }
      else if (!IsSpace())
      {
        break;
      }
      Skip();
    }
    // Save the current line and filename into the token structure.
    token.m_Line     = m_Line;
    token.m_FileName = m_FileName;
    // Save the start of the lexeme.
    const uint8_t* start = m_Buffer;
    // If the character is a digit, the token is a number.
    if (IsDigit())
    {
      // Check for octal, hexadecimal and binary numbers.
      if (Get() == '0')
      {
        // Skip the leading zero...
        Skip();
        if (Get() == 'x')
        {
          // Grab the hexadecimal number.
          Skip();
          while (IsHexa())
          {
            Skip();
          }
          // Finish the token structure.
          token.m_ID = tHexadecimal;
          token.m_Lexeme = Str((const char*)start, (unsigned int)(m_Buffer - start));
          // Success.
          return true;
        }
        else if (Get() == 'b')
        {
          // Grab the binary number.
          Skip();
          while (Get() == '0' || Get() == '1')
          {
            Skip();
          }
          // Finish the token structure.
          token.m_ID = tBinary;
          token.m_Lexeme = Str((const char*)start, (unsigned int)(m_Buffer - start));
          // Success.
          return true;
        }
        else if (IsOctal())
        {
          // Grab the octal number.
          while (IsOctal())
          {
            Skip();
          }
          // Finish the token structure.
          token.m_ID = tOctal;
          token.m_Lexeme = Str((const char*)start, (unsigned int)(m_Buffer - start));
          // Success.
          return true;
        }
        // Else fall through and read a regular decimal number.
      }
      // Assume the number is an integer for now.
      token.m_ID = tDecimal;
      // Get all digits.
      while (IsDigit())
      {
        Skip();
      }
      // Check for decimals.
      if (Get() == '.')
      {
        // The number is a real number if it has a decimal dot.
        token.m_ID = tReal;
        Skip();
        // Get any digits after the decimal digit.
        while (IsDigit())
        {
          Skip();
        }
      }
      // Check for exponent.
      if (tolower(Get()) == 'e')
      {
        // The number is a real number if it has an exponent.
        token.m_ID = tReal;
        Skip();
        // Check optional exponent sign.
        if (Get() == '+' || Get() == '-')
        {
          Skip();
        }
        // At least one digit must follow.
        if (!IsDigit())
        {
          return ErrorUnterminatedNumber( m_Error, sizeof( m_Error ), token );
        }
        // Get exponent.
        while (IsDigit())
        {
          Skip();
        }
      }
      // Check for optional 'f' after number.
      if (Get() == 'f')
      {
        // The number is a real number if it is succeeded by an 'f'.
        token.m_ID = tReal;
        Skip();
      }
      token.m_Lexeme = Str((const char*)start, (unsigned int)(m_Buffer - start));
      // Success.
      return true;
    }
    // If the character is alphabetic or '_', the token is an identifier.
    if (IsAlpha())
    {
      // Get all alphanumeric and '_' characters.
      while (IsAlNum())
      {
        Skip();
      }
      token.m_Lexeme = Str((const char*)start, (unsigned int)(m_Buffer - start));
      // Check the hash table for the reserved keywords.
      const Keyword* keyword = Perfect_Hash::in_word_set(token.m_Lexeme.GetChars(), (unsigned int)token.m_Lexeme.GetLength());
      if (keyword != NULL)
      {
        token.m_ID = keyword->m_Token;
      }
      else
      {
        // It's not a reserved keyword, check if the identifier starts with two underlines.
        if (m_TwoUsReserved && token.m_Lexeme.GetLength() > 1 && start[0] == '_' && start[1] == '_')
        {
          // It does and it's not allowed, error.
          return ErrorReservedKeyword(m_Error, sizeof(m_Error), token);
        }
        token.m_ID = tID;
      }
      // Success.
      return true;
    }
    // If the character is a single quote or a double quote, it's a string.
    if ( Get() == '"' || Get() == '\'' )
    {
      int quote = Get();
      // Note that we don't process escape sequences here since all lexemes point directly
      // into the source code.
      token.m_ID = tLiteral;
      Skip();
      // Get anything until another double quote.
      while ( Get() != quote )
      {
        if ( Get() == -1 )
        {
          // Error if the source code ended before the string was closed.
          return ErrorUnterminatedLiteral(m_Error, sizeof(m_Error), token);
        }
        Skip();
      }
      // Skip the ending double quote.
      Skip();
      token.m_Lexeme = Str((const char*)start + 1, (unsigned int)(m_Buffer - start - 2));
      // Success.
      return true;
    }
    // Otherwise the token is one of the bellow. If the token is composed of just one character,
    // its ID is the character itself. We initialize the token ID here and overwrite it if
    // necessary.
    token.m_ID = (TokenID)Get();
    switch (Get())
    {
    case ',':
    case '{':
    case '}':
    case '[':
    case ']':
    case '(':
    case ')':
    case ';':
    case '?':
    case ':':
    case '.':
    case '^':
    case '+':
    case '-':
    case '*':
    case '%':
    case '~':
      // Skip the character.
      Skip();
      token.m_Lexeme = Str((const char*)start, (unsigned int)(m_Buffer - start));
      // Success.
      return true;
    case '/':
      Skip();
      // Two slashes are the beginning of a comment.
      if (Get() == '/')
      {
        // Skip everything until the end of the current line.
        while (Get() != '\n' && Get() != -1)
        {
          Skip();
        }
        // Call GetNext recursively to get the next token. This will be a tail call on smart
        // compiler, otherwise there's a cost in the call. We could "goto" to the start of the
        // method...
        return GetNext(token);
      }
      // A slash followed by an '*' is the beginning of a comment.
      else if (Get() == '*')
      {
        // Skip the '*'.
        Skip();
        do
        {
          // Skip everything until we find another '*'.
          while (Get() != '*')
          {
            // Error if the source code ended before the comment was closed.
            if (Get() == -1)
            {
              return ErrorUnterminatedComment(m_Error, sizeof(m_Error), token);
            }
            Skip();
          }
          // Skip the '*'.
          Skip();
        }
        // If the next character isn't a slash to close the comment, start looking for another '*'.
        while (Get() != '/');
        // Skip the slash.
        Skip();
        // Call GetNext recursively to get the next token.
        return GetNext(token);
      }
      else
      {
        // Otherwise the token is just a single slash.
        token.m_Lexeme = Str((const char*)start, (unsigned int)(m_Buffer - start));
      }
      // Success.
      return true;
    case '|':
      Skip();
      // If there's a '|' after the first '|', it's a logical or.
      if (Get() == '|')
      {
        token.m_ID = tLogicOr;
        Skip();
      }
      // Otherwise it's an arithmetic or.
      token.m_Lexeme = Str((const char*)start, (unsigned int)(m_Buffer - start));
      // Success.
      return true;
    case '&':
      Skip();
      // If there's a '&' after the first '&', it's a logical and.
      if (Get() == '&')
      {
        token.m_ID = tLogicAnd;
        Skip();
      }
      // Otherwise it's an arithmetic and.
      token.m_Lexeme = Str((const char*)start, (unsigned int)(m_Buffer - start));
      // Success.
      return true;
    case '=':
      Skip();
      // If there's a '=' after the first '=', it's an equality operator.
      if (Get() == '=')
      {
        token.m_ID = tEqual;
        Skip();
      }
      // Otherwise it's an equal sign.
      token.m_Lexeme = Str((const char*)start, (unsigned int)(m_Buffer - start));
      // Success.
      return true;
    case '!':
      Skip();
      // If there's a '=' after the '!', it's an inequality operator.
      if (Get() == '=')
      {
        token.m_ID = tNotEqual;
        Skip();
      }
      // Otherwise it's the unary logical not operator.
      token.m_Lexeme = Str((const char*)start, (unsigned int)(m_Buffer - start));
      // Success.
      return true;
    case '<':
      Skip();
      // If there's a '=' after the '<', it's the less than or equal to relational operator.
      if (Get() == '=')
      {
        token.m_ID = tLessEqual;
        Skip();
      }
      // Otherwise, if there's a '<' it's the shift left operator.
      else if (Get() == '<')
      {
        token.m_ID = tShiftLeft;
        Skip();
      }
      // Otherwise it's the less than relational operator.
      token.m_Lexeme = Str((const char*)start, (unsigned int)(m_Buffer - start));
      // Success.
      return true;
    case '>':
      Skip();
      // If there's a '=' after the '>', it's the greater than or equal to relational operator.
      if (Get() == '=')
      {
        token.m_ID = tGreaterEqual;
        Skip();
      }
      // Otherwise, if there's a '>' it's the shift right operator.
      else if (Get() == '>')
      {
        token.m_ID = tShiftRight;
        Skip();
      }
      // Otherwise it's the greater than relational operator.
      token.m_Lexeme = Str((const char*)start, (unsigned int)(m_Buffer - start));
      // Success.
      return true;
    case '#':
      // Process a #line directive from the preprocessor so we keep track of
      // the current file name and line.
      {
        if ( !m_AtFirstChar )
        {
          break;
        }
        Skip();

        const uint8_t* beginning = m_Buffer;

        while ( !IsSpace() )
        {
          Skip();
        }
        // Check for "line".
        if ( ( m_Buffer - start ) != 5 || start[ 1 ] != 'l' || start[ 2 ] != 'i' || start[ 3 ] != 'n' || start[ 4 ] != 'e' )
        {
          Str directive = Str( (const char*)beginning, (unsigned int)( m_Buffer - beginning ) );
          return ErrorInvalidDirective( m_Error, sizeof( m_Error ), token, directive );
        }
        // Skip spaces.
        while ( IsSpace() )
        {
          Skip();
        }
        // Get the line number.
        m_Line = 0;

        while ( IsDigit() )
        {
          m_Line = m_Line * 10 + Get() - '0';
          Skip();
        }
        // Decrement because the end of the directive line will increment it.
        m_Line--;
        // Skip spaces.
        while ( IsSpace() )
        {
          Skip();
        }
        // Skip the opening double quote.
        Skip();
        // Get the file name.
        start = m_Buffer;

        while ( Get() != '"' )
        {
          Skip();
        }

        m_FileName = Str( (const char*)start, (unsigned int)( m_Buffer - start ) );
        // Skip the closing double quote
        Skip();
        // Get the next token.
        return GetNext( token );
      }
    }
    // Set the lexeme to be the invalid character.
    token.m_Lexeme = Str((const char*)m_Buffer, 1);
    // Set the error message and return false.
    return ErrorInvalidCharacter(m_Error, sizeof(m_Error), token);
  }

  bool
  Lexer::IsSpace() const
  {
    // Forward to isspace from ctype.h.
    return isspace(Get()) != 0;
  }

  bool
  Lexer::IsAlpha() const
  {
    int k = Get();
    // Forward to isalpha from ctype.h but also test for an underline.
    return isalpha(k) || k == '_';
  }

  bool
  Lexer::IsDigit() const
  {
    // Forward to isdigit from ctype.h.
    return isdigit(Get()) != 0;
  }

  bool
  Lexer::IsHexa() const
  {
    // Forward to isxdigit from ctype.h.
    return isxdigit(Get()) != 0;
  }

  bool
  Lexer::IsAlNum() const
  {
    int k = Get();
    // Forward to isalnum from ctype.h but also test for an underline.
    return isalnum(k) || k == '_';
  }

  bool
  Lexer::IsOctal() const
  {
    int k = Get();
    // Just check if the digit is a valid octal digit.
    return k >= '0' && k <= '7';
  }

  int
  Lexer::Get() const
  {
    // Returns -1 if the current character is past the end of the buffer.
    if (m_Buffer >= m_BufferEnd)
    {
      return -1;
    }
    // Return the unsigned character as an int otherwise.
    return *m_Buffer;
  }

  void
  Lexer::Skip()
  {
    // If the current character is a new line, increment the source line counter.
    if (Get() == '\n')
    {
      m_Line++;
      m_AtFirstChar = true;
    }
    // Increment the current character pointer.
    m_Buffer++;
  }

  const char* g_TokenNames[] =
  {
    "tEOF",
    "tID",
    "tLiteral",
    "tDecimal",
    "tHexadecimal",
    "tOctal",
    "tBinary",
    "tReal",
    "tTrue",
    "tFalse",
    "tPi",
    "tE",
    "tInt8",
    "tUint8",
    "tInt16",
    "tUint16",
    "tInt32",
    "tUint32",
    "tInt64",
    "tUint64",
    "tFloat32",
    "tFloat64",
    "tStr",
    "tSelect",
    "tBitfield",
    "tStruct",
    "tBoolean",
    "tFile",
    "tAuthor",
    "tDescription",
    "tLabel",
    "tValue",
    "tDefault",
    "tExtensions",
    "tVaultHints",
    "tLogicOr",
    "tLogicAnd",
    "tEqual",
    "tNotEqual",
    "tLessEqual",
    "tGreaterEqual",
    "tShiftLeft",
    "tShiftRight",
    "tEmpty",
    "tBase",
    "tUIRange",
    "tUIRender",
    "tSize",
    "tComponentPostJob",
    "tTargets",
    "tParallel",
    "tVersion",
    "tCallback",
    "tKey",
    "tUnits",
    "tJson"
  };

  const char* GetTokenStr( TokenID token )
  {
    int ndx = (int)token - DDLParser::tEOF;
    return g_TokenNames[ndx];
  }
}
