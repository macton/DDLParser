#pragma once

// Scope-based utility for writing JSON data to character buffer
// Designed to be super simple to use and not carry any actual JSON parsing overhead.

// Requires: size_t
// Requires: sprintf

class JsonWriteScope
{
  public:
    JsonWriteScope( char*& output, size_t& output_size )
      : m_Output(output), m_OutputSize(output_size), m_FirstElement(true)
    {
    }
    ~JsonWriteScope()
    {
    }

    void WriteComma()
    {
      if (!m_FirstElement)
      {
        m_Output[0] = ',';
        m_OutputSize++;
        m_Output++;
      }
      else
      {
        m_FirstElement = false;
      }
    }
    void WriteDouble( double value, const char* name = NULL )
    {
      WriteComma();

      if ( name )
      {
        int out_size = sprintf( m_Output, "\"%s\": %g", name, value );
        m_Output     += out_size;
        m_OutputSize += out_size;
      }
      else
      {
        int out_size = sprintf( m_Output, "%g", value );
        m_Output     += out_size;
        m_OutputSize += out_size;
      }
    }
    void WriteFloat( float value, const char* name = NULL )
    {
      WriteDouble( value, name );
    }
    void WriteUInt( unsigned int value, const char* name = NULL )
    {
      WriteComma();

      if ( name )
      {
        int out_size = sprintf( m_Output, "\"%s\": %u", name, value );
        m_Output     += out_size;
        m_OutputSize += out_size;
      }
      else
      {
        int out_size = sprintf( m_Output, "%u", value );
        m_Output     += out_size;
        m_OutputSize += out_size;
      }
    }
    void WriteInt( unsigned int value, const char* name = NULL )
    {
      WriteComma();

      if ( name )
      {
        int out_size = sprintf( m_Output, "\"%s\": %d", name, value );
        m_Output     += out_size;
        m_OutputSize += out_size;
      }
      else
      {
        int out_size = sprintf( m_Output, "%d", value );
        m_Output     += out_size;
        m_OutputSize += out_size;
      }
    }
    void WriteString( const char* value, const char* name = NULL )
    {
      WriteComma();

      if ( name )
      {
        int out_size = sprintf( m_Output, "\"%s\": \"%s\"", name, value );
        m_Output     += out_size;
        m_OutputSize += out_size;
      }
      else
      {
        int out_size = sprintf( m_Output, "\"%s\"", value );
        m_Output     += out_size;
        m_OutputSize += out_size;
      }
    }
    void WriteBool( bool value, const char* name = NULL )
    {
      WriteComma();

      if ( name )
      {
        int out_size = sprintf( m_Output, "\"%s\": %s", name, (value)?"true":"false" );
        m_Output     += out_size;
        m_OutputSize += out_size;
      }
      else
      {
        int out_size = sprintf( m_Output, "%s", (value)?"true":"false" );
        m_Output     += out_size;
        m_OutputSize += out_size;
      }
    }
    char*&  m_Output;
    size_t& m_OutputSize;
    bool    m_FirstElement;
};

class JsonWriteScopeObject : public JsonWriteScope
{
  public:
    JsonWriteScopeObject( JsonWriteScope* parent_scope, const char* name = NULL )
      : JsonWriteScope( parent_scope->m_Output, parent_scope->m_OutputSize )
    {
      parent_scope->WriteComma();

      if ( name )
      {
        int out_size = sprintf( m_Output, "\"%s\": {", name );
        m_Output     += out_size;
        m_OutputSize += out_size;
      }
      else
      {
        m_Output[0] = '{';
        m_OutputSize++;
        m_Output++;
      }

    }
    ~JsonWriteScopeObject()
    {
      m_Output[0] = '}';
      m_OutputSize++;
      m_Output++;
    }
};

class JsonWriteScopeArray : public JsonWriteScope
{
  public:
    JsonWriteScopeArray( JsonWriteScope* parent_scope, const char* name = NULL )
      : JsonWriteScope( parent_scope->m_Output, parent_scope->m_OutputSize )
    {
      parent_scope->WriteComma();

      if ( name )
      {
        int out_size = sprintf( m_Output, "\"%s\": [", name );
        m_Output     += out_size;
        m_OutputSize += out_size;
      }
      else
      {
        m_Output[0] = '{';
        m_OutputSize++;
        m_Output++;
      }

    }
    ~JsonWriteScopeArray()
    {
      m_Output[0] = ']';
      m_OutputSize++;
      m_Output++;
    }
};

