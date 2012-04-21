#include <malloc.h>

#include "StringArea.h"
#include "AreaManager.h"
#include "Str.h"

namespace DDLParser
{
  static size_t StrUrlDecode(char* dst, size_t dst_max_len, const char* src, size_t src_size)
  {
    size_t src_char_ndx;
    size_t dst_char_ndx;

    for (src_char_ndx=0,dst_char_ndx=0;dst_char_ndx<dst_max_len;src_char_ndx++,dst_char_ndx++)
    {
      if ( src[src_char_ndx] == '\0' || src_char_ndx == src_size )
      {
        break;
      }

      if ( ( src[src_char_ndx] == '%' ) && src[src_char_ndx+1] && src[src_char_ndx+2] )
      {
        char hex_0 = src[src_char_ndx+1];
        char hex_1 = src[src_char_ndx+2];

        if (hex_0 >= 'A' && hex_0 <= 'F')
        {
          hex_0 = 0x0A + ( hex_0 - 'A' );
        }
        else if (hex_0 >= 'a' && hex_0 <= 'f')
        {
          hex_0 = 0x0A + ( hex_0 - 'a' );
        }
        else if (hex_0 >= '0' && hex_0 <= '9')
        {
          hex_0 = 0x00 + ( hex_0 - '0' );
        }
        else
        {
          dst_char_ndx -= 1;
          src_char_ndx += 2;
          continue;
        }

        if (hex_1 >= 'A' && hex_1 <= 'F')
        {
          hex_1 = 0x0A + ( hex_1 - 'A' );
        }
        else if (hex_1 >= 'a' && hex_1 <= 'f')
        {
          hex_1 = 0x0A + ( hex_1 - 'a' );
        }
        else if (hex_1 >= '0' && hex_1 <= '9')
        {
          hex_1 = 0x00 + ( hex_1 - '0' );
        }
        else
        {
          dst_char_ndx -= 1;
          src_char_ndx += 2;
          continue;
        }

        dst[ dst_char_ndx ]  = (hex_0 << 4)|(hex_1);
        src_char_ndx        += 2;
      }
      // There are a couple of apps that don't encode spaces as plus signs so we don't handle the decoding here.
      /*else if ( src[ src_char_ndx ] == '+' )
      {
        dst[ dst_char_ndx ] = ' ';
      }*/
      else
      {
        dst[ dst_char_ndx ] = src[ src_char_ndx ];
      }
    }
    dst[ dst_char_ndx ] = '\0';

    return (dst_char_ndx);
  }

  StringArea::StringArea(Area *area)
    : m_Area(area)
  {
  }

  const char*
  StringArea::Add(const Str& string)
  {
    char*  decoded     = (char*)alloca(string.GetLength() + 1);
    size_t decoded_len = StrUrlDecode( decoded, string.GetLength(), string.GetChars(), string.GetLength() );

    // Check if it already exists in the string area.
    Str test(decoded, (uint32_t)decoded_len);
    char* str = (char*)m_Area->GetBase();
    char* end = str + m_Area->m_Size;
    while (str < end)
    {
      if (test == str)
      {
        return str;
      }
      str += strlen(str) + 1;
    }
    // Add it to the string area.
    str = &*m_Area->Allocate<char>((uint32_t)decoded_len+1);
    memcpy(str, decoded, decoded_len+1);
    return str;
  }

  const char*
  StringArea::AddRaw(const Str& string)
  {
    size_t raw_size = string.GetLength() + 1;
    char*  str      = (char*)m_Area->GetBase();
    char*  end      = str + m_Area->m_Size;

    while (str < end)
    {
      if (string == str)
      {
        return str;
      }
      str += strlen(str) + 1;
    }
    // Add it to the string area.
    str = &*m_Area->Allocate<char>( (uint32_t)raw_size );
    //strcpy(str, unescaped);
    memcpy(str, string.GetChars(), raw_size);
    return str;
  }

  Area*
  StringArea::GetArea() const
  {
    return m_Area;
  }
};
