#include <math.h>

#define __STDC_LIMIT_MACROS
#include <stdint.h>

#include "Value.h"

namespace DDLParser
{
  Value::Value ( bool valid )
    : m_Valid ( valid )
    , m_Type ( kInt )
    , m_Int ( ( int64_t ) 0 )
  {
  }

  Value::Value ( int64_t value, bool valid )
    : m_Valid ( valid )
    , m_Type ( kInt )
    , m_Int ( value )
  {
  }

  Value::Value ( double value, bool valid )
    : m_Valid ( valid )
    , m_Type ( kDouble )
    , m_Double ( value )
  {
  }

  Value::Value ( const Str& value, bool valid )
    : m_Valid ( valid )
    , m_Type ( kString )
    , m_String ( value )
  {
  }

  Value::Value ( const Value& other )
    : m_Valid ( other.m_Valid )
    , m_Type ( other.m_Type )
    , m_Int ( other.m_Int )
    , m_Double ( other.m_Double )
    , m_String ( other.m_String )
  {
  }

  bool
  Value::IsValid() const
  {
    return m_Valid;
  }

  Value::Type
  Value::GetType() const
  {
    return m_Type;
  }

  int64_t
  Value::GetInt() const
  {
    return m_Type == kInt ? m_Int : ( int64_t ) m_Double;
  }

  double
  Value::GetDouble() const
  {
    return m_Type == kDouble ? m_Double : ( double ) m_Int;
  }

  Str
  Value::GetString() const
  {
    return m_String;
  }

  bool
  Value::IsString() const
  {
    return m_Type == kString;
  }

  bool
  Value::IsInt8() const
  {
    double unused;
    return
      ( m_Type == kInt && m_Int >= INT8_MIN && m_Int <= INT8_MAX ) ||
      ( m_Type == kDouble && m_Double >= INT8_MIN && m_Double <= INT8_MAX && modf ( m_Double, &unused ) == 0.0 );
  }

  bool
  Value::IsUint8() const
  {
    double unused;
    return
      ( m_Type == kInt && m_Int >= 0 && m_Int <= UINT8_MAX ) ||
      ( m_Type == kDouble && m_Double >= 0 && m_Double <= UINT8_MAX && modf ( m_Double, &unused ) == 0.0 );
  }

  bool
  Value::IsInt16() const
  {
    double unused;
    return
      ( m_Type == kInt && m_Int >= INT16_MIN && m_Int <= INT16_MAX ) ||
      ( m_Type == kDouble && m_Double >= INT16_MIN && m_Double <= INT16_MAX && modf ( m_Double, &unused ) == 0.0 );
  }

  bool
  Value::IsUint16() const
  {
    double unused;
    return
      ( m_Type == kInt && m_Int >= 0 && m_Int <= UINT16_MAX ) ||
      ( m_Type == kDouble && m_Double >= 0 && m_Double <= UINT16_MAX && modf ( m_Double, &unused ) == 0.0 );
  }

  bool
  Value::IsInt32() const
  {
    double unused;
    return
      ( m_Type == kInt && m_Int >= INT32_MIN && m_Int <= INT32_MAX ) ||
      ( m_Type == kDouble && m_Double >= INT32_MIN && m_Double <= INT32_MAX && modf ( m_Double, &unused ) == 0.0 );
  }

  bool
  Value::IsUint32() const
  {
    double unused;
    return
      ( m_Type == kInt && m_Int >= 0 && m_Int <= UINT32_MAX ) ||
      ( m_Type == kDouble && m_Double >= 0 && m_Double <= UINT32_MAX && modf ( m_Double, &unused ) == 0.0 );
  }

  bool
  Value::IsInt64() const
  {
    double unused;
    return
      m_Type == kInt ||
      ( m_Type == kDouble && m_Double >= INT64_MIN && m_Double <= INT64_MAX && modf ( m_Double, &unused ) == 0.0 );
  }

  bool
  Value::IsUint64() const
  {
    double unused;
    return
      m_Type == kInt ||
      ( m_Type == kDouble && m_Double >= 0 && m_Double <= UINT64_MAX && modf ( m_Double, &unused ) == 0.0 );
  }

  bool
  Value::IsFloat32() const
  {
    static union
    {
      uint32_t u;
      float    f;
    } uf = { 0x7f7fffff };
    return
      ( m_Type == kDouble && m_Double >= -uf.f && m_Double <= uf.f ) ||
      ( m_Type == kInt && m_Int >= -uf.f && m_Int <= uf.f );
  }

  bool
  Value::IsFloat64() const
  {
    static union
    {
      uint64_t u;
      double   d;
    } ud = { 0x7fefffffffffffffULL };
    return
      m_Type == kDouble ||
      ( m_Type == kInt && m_Int >= -ud.d && m_Int <= ud.d );
  }

  Value
  Value::operator|| ( const Value& other )
  {
    int left = m_Type == kInt ? GetInt() != 0 : GetDouble() != 0.0;
    int right = other.GetType() == kInt ? other.GetInt() != 0 : other.GetDouble() != 0.0;
    return Value ( ( int64_t ) ( left || right ), m_Valid && other.IsValid() );
  }

  Value
  Value::operator&& ( const Value& other )
  {
    int left = GetType() == kInt ? GetInt() != 0 : GetDouble() != 0.0;
    int right = other.GetType() == kInt ? other.GetInt() != 0 : other.GetDouble() != 0.0;
    return Value ( ( int64_t ) ( left && right ), m_Valid && other.IsValid() );
  }

  Value
  Value::operator| ( const Value& other )
  {
    if ( m_Type == kInt && other.GetType() == kInt )
    {
      return Value ( GetInt() | other.GetInt(), m_Valid && other.IsValid() );
    }

    return Value();
  }

  Value
  Value::operator^ ( const Value& other )
  {
    if ( m_Type == kInt && other.GetType() == kInt )
    {
      return Value ( GetInt() ^ other.GetInt(), m_Valid && other.IsValid() );
    }

    return Value();
  }

  Value
  Value::operator& ( const Value& other )
  {
    if ( m_Type == kInt && other.GetType() == kInt )
    {
      return Value ( GetInt() & other.GetInt(), m_Valid && other.IsValid() );
    }

    return Value();
  }

  Value
  Value::operator== ( const Value& other )
  {
    if ( GetType() == kInt && other.GetType() == kInt )
    {
      return Value ( ( int64_t ) ( GetInt() == other.GetInt() ), m_Valid && other.IsValid() );
    }
    else if ( GetType() == kDouble && other.GetType() == kDouble )
    {
      return Value ( ( int64_t ) ( GetDouble() == other.GetDouble() ), m_Valid && other.IsValid() );
    }
    else if ( GetType() == kString && other.GetType() == kString )
    {
      return Value ( ( int64_t ) ( GetString() == other.GetString() ), m_Valid && other.IsValid() );
    }
    else if ( GetType() == kInt && other.GetType() == kDouble )
    {
      return Value ( ( int64_t ) ( GetInt() == other.GetDouble() ), m_Valid && other.IsValid() );
    }
    else if ( GetType() == kDouble && other.GetType() == kInt )
    {
      return Value ( ( int64_t ) ( GetDouble() == other.GetInt() ), m_Valid && other.IsValid() );
    }

    return Value();
  }

  Value
  Value::operator!= ( const Value& other )
  {
    return ! ( *this == other );
  }

  Value
  Value::operator< ( const Value& other )
  {
    if ( GetType() == kInt && other.GetType() == kInt )
    {
      return Value ( ( int64_t ) ( GetInt() < other.GetInt() ), m_Valid && other.IsValid() );
    }
    else if ( GetType() == kDouble && other.GetType() == kDouble )
    {
      return Value ( ( int64_t ) ( GetDouble() < other.GetDouble() ), m_Valid && other.IsValid() );
    }
    /*else if (GetType() == kString && other.GetType() == kString)
    {
      return Value((int64_t)(GetString() < other.GetString()), m_Valid && other.IsValid());
    }*/
    else if ( GetType() == kInt && other.GetType() == kDouble )
    {
      return Value ( ( int64_t ) ( GetInt() < other.GetDouble() ), m_Valid && other.IsValid() );
    }
    else if ( GetType() == kDouble && other.GetType() == kInt )
    {
      return Value ( ( int64_t ) ( GetDouble() < other.GetInt() ), m_Valid && other.IsValid() );
    }

    return Value();
  }

  Value
  Value::operator> ( const Value& other )
  {
    return ! ( *this < other || *this == other );
  }

  Value
  Value::operator<= ( const Value& other )
  {
    return *this < other || *this == other;
  }

  Value
  Value::operator>= ( const Value& other )
  {
    return ! ( *this < other );
  }

  Value
  Value::operator<< ( const Value& other )
  {
    if ( GetType() == kInt && other.GetType() == kInt )
    {
      if ( other.GetInt() >= 64 )
      {
        return Value ( ( int64_t ) 0, m_Valid && other.IsValid() );
      }

      return Value ( ( int64_t ) GetInt() << other.GetInt(), m_Valid && other.IsValid() );
    }

    return Value();
  }

  Value
  Value::operator>> ( const Value& other )
  {
    if ( GetType() == kInt && other.GetType() == kInt )
    {
      if ( other.GetInt() >= 64 )
      {
        return Value ( ( int64_t ) 0, m_Valid && other.IsValid() );
      }

      return Value ( GetInt() >> other.GetInt(), m_Valid && other.IsValid() );
    }

    return Value();
  }

  Value
  Value::operator+ ( const Value& other )
  {
    if ( GetType() == kInt && other.GetType() == kInt )
    {
      return Value ( GetInt() + other.GetInt(), m_Valid && other.IsValid() );
    }
    else if ( GetType() == kDouble && other.GetType() == kDouble )
    {
      return Value ( GetDouble() + other.GetDouble(), m_Valid && other.IsValid() );
    }
    /*else if (GetType() == kString && other.GetType() == kString)
    {
      return Value(GetString() + other.GetString(), m_Valid && other.IsValid());
    }*/
    else if ( GetType() == kInt && other.GetType() == kDouble )
    {
      return Value ( GetInt() + other.GetDouble(), m_Valid && other.IsValid() );
    }
    else if ( GetType() == kDouble && other.GetType() == kInt )
    {
      return Value ( GetDouble() + other.GetInt(), m_Valid && other.IsValid() );
    }

    return Value();
  }

  Value
  Value::operator- ( const Value& other )
  {
    if ( GetType() == kInt && other.GetType() == kInt )
    {
      return Value ( GetInt() - other.GetInt(), m_Valid && other.IsValid() );
    }
    else if ( GetType() == kDouble && other.GetType() == kDouble )
    {
      return Value ( GetDouble() - other.GetDouble(), m_Valid && other.IsValid() );
    }
    else if ( GetType() == kInt && other.GetType() == kDouble )
    {
      return Value ( GetInt() - other.GetDouble(), m_Valid && other.IsValid() );
    }
    else if ( GetType() == kDouble && other.GetType() == kInt )
    {
      return Value ( GetDouble() - other.GetInt(), m_Valid && other.IsValid() );
    }

    return Value();
  }

  Value
  Value::operator* ( const Value& other )
  {
    if ( GetType() == kInt && other.GetType() == kInt )
    {
      return Value ( GetInt() * other.GetInt(), m_Valid && other.IsValid() );
    }
    else if ( GetType() == kDouble && other.GetType() == kDouble )
    {
      return Value ( GetDouble() * other.GetDouble(), m_Valid && other.IsValid() );
    }
    else if ( GetType() == kInt && other.GetType() == kDouble )
    {
      return Value ( GetInt() * other.GetDouble(), m_Valid && other.IsValid() );
    }
    else if ( GetType() == kDouble && other.GetType() == kInt )
    {
      return Value ( GetDouble() * other.GetInt(), m_Valid && other.IsValid() );
    }

    return Value();
  }

  Value
  Value::operator/ ( const Value& other )
  {
    if ( GetType() == kInt && other.GetType() == kInt )
    {
      return Value ( GetInt() / other.GetInt(), m_Valid && other.IsValid() );
    }
    else if ( GetType() == kDouble && other.GetType() == kDouble )
    {
      return Value ( GetDouble() / other.GetDouble(), m_Valid && other.IsValid() );
    }
    else if ( GetType() == kInt && other.GetType() == kDouble )
    {
      return Value ( GetInt() / other.GetDouble(), m_Valid && other.IsValid() );
    }
    else if ( GetType() == kDouble && other.GetType() == kInt )
    {
      return Value ( GetDouble() / other.GetInt(), m_Valid && other.IsValid() );
    }

    return Value();
  }

  Value
  Value::operator% ( const Value& other )
  {
    if ( GetType() == kInt && other.GetType() == kInt )
    {
      return Value ( GetInt() % other.GetInt(), m_Valid && other.IsValid() );
    }
    else if ( GetType() == kDouble && other.GetType() == kDouble )
    {
      return Value ( fmod ( GetDouble(), other.GetDouble() ), m_Valid && other.IsValid() );
    }
    else if ( GetType() == kInt && other.GetType() == kDouble )
    {
      return Value ( fmod ( ( double ) GetInt(), other.GetDouble() ), m_Valid && other.IsValid() );
    }
    else if ( GetType() == kDouble && other.GetType() == kInt )
    {
      return Value ( fmod ( GetDouble(), ( double ) other.GetInt() ), m_Valid && other.IsValid() );
    }

    return Value();
  }

  Value
  Value::operator-()
  {
    if ( GetType() == kInt )
    {
      return Value ( -GetInt(), m_Valid );
    }
    else if ( GetType() == kDouble )
    {
      return Value ( -GetDouble(), m_Valid );
    }

    return Value();
  }

  Value
  Value::operator~()
  {
    if ( GetType() == kInt )
    {
      return Value ( ~GetInt(), m_Valid );
    }

    return Value();
  }

  Value
  Value::operator!()
  {
    if ( GetType() == kInt )
    {
      return Value ( ( int64_t ) ( !GetInt() ), m_Valid );
    }
    else if ( GetType() == kDouble )
    {
      return Value ( ( int64_t ) ( !GetDouble() ), m_Valid );
    }

    return Value();
  }
};
