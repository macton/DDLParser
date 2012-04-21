#include <string.h>

#include <DDLParser.h>

#include "Str.h"

namespace DDLParser
{
  Str::Str()
    : m_Chars(0)
    , m_Length(0)
    , m_Hash(Crc32(0, 0))
  {
  }

  Str::Str(const char* cstr, unsigned int length)
    : m_Chars(cstr)
    , m_Length(length)
    , m_Hash(Crc32(cstr, length))
  {
  }

  Str::Str(const char* cstr)
    : m_Chars(cstr)
    , m_Length((unsigned int)strlen(cstr))
    , m_Hash(StringCrc32(cstr))
  {
  }

  Str::Str(const Str& other)
    : m_Chars(other.m_Chars)
    , m_Length(other.m_Length)
    , m_Hash(other.m_Hash)
  {
  }

  bool
  Str::operator==(const Str& other) const
  {
    return m_Hash == other.m_Hash;
  }

  bool
  Str::operator==(const char* other) const
  {
    return m_Hash == StringCrc32(other);
  }

  const char*
  Str::GetChars() const
  {
    return m_Chars;
  }

  unsigned int
  Str::GetLength() const
  {
    return m_Length;
  }

  uint32_t
  Str::GetHash() const
  {
    return m_Hash;
  }

  void
  Str::ToCStr(char* cstr) const
  {
    strncpy(cstr, m_Chars, m_Length);
    cstr[m_Length] = 0;
  }

  bool
  Str::ValidateEscapeSequences() const
  {
    for (uint32_t i=0;i<m_Length;i++)
    {
      if ( m_Chars[i] == '\\' )
      {
        int is_another_char_avail = (i+1) < m_Length;

        if ( is_another_char_avail )
        {
          i++;
          switch ( m_Chars[i] )
          {
            case 'n':
            case 't':
            case '"':
            case '\\':
              break; /* Valid sequences */
            default:
              return (false);
          }
        }
      }
    }
    return (true);
  }
};
