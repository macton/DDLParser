#pragma once

#include <stdint.h>

#include "Str.h"

namespace DDLParser
{
  class Value
  {
    public:
      enum Type
      {
        kInt, kDouble, kString
      };

      Value(bool valid = false);
      Value(int64_t value, bool valid = true);
      Value(double value, bool valid = true);
      Value(const Str& value, bool valid = true);
      Value(const Value& other);

      bool    IsValid() const;
      Type    GetType() const;
      int64_t GetInt() const;
      double  GetDouble() const;
      Str     GetString() const;

      bool IsString() const;
      bool IsInt8() const;
      bool IsUint8() const;
      bool IsInt16() const;
      bool IsUint16() const;
      bool IsInt32() const;
      bool IsUint32() const;
      bool IsInt64() const;
      bool IsUint64() const;
      bool IsFloat32() const;
      bool IsFloat64() const;

      Value operator||(const Value& other);
      Value operator&&(const Value& other);
      Value operator|(const Value& other);
      Value operator^(const Value& other);
      Value operator&(const Value& other);
      Value operator==(const Value& other);
      Value operator!=(const Value& other);
      Value operator<(const Value& other);
      Value operator>(const Value& other);
      Value operator<=(const Value& other);
      Value operator>=(const Value& other);
      Value operator<<(const Value& other);
      Value operator>>(const Value& other);
      Value operator+(const Value& other);
      Value operator-(const Value& other);
      Value operator*(const Value& other);
      Value operator/(const Value& other);
      Value operator%(const Value& other);
      Value operator-();
      Value operator~();
      Value operator!();

    private:
      bool    m_Valid;
      Type    m_Type;
      int64_t m_Int;
      double  m_Double;
      Str     m_String;
  };
};
