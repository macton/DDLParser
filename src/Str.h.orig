#pragma once

#include <stdint.h>

namespace DDLParser
{
  class Str
  {
    public:
      Str();
      Str(const char* cstr, unsigned int length);
      Str(const char* cstr);
      Str(const Str& other);

      bool operator==(const Str& other) const;
      bool operator==(const char* other) const;

      const char*  GetChars() const;
      unsigned int GetLength() const;
      uint32_t     GetHash() const;

      void         ToCStr(char* cstr) const;

      bool         ValidateEscapeSequences() const;

    private:
      const char*  m_Chars;
      uint32_t     m_Length;
      uint32_t     m_Hash;
  };
};
