#pragma once

namespace DDLParser
{
  struct Area;
  class  Str;

  class StringArea
  {
    public:
      StringArea ( Area* area );

      const char* Add ( const Str& string );
      const char* AddRaw ( const Str& string );

      Area* GetArea() const;

    private:
      Area* m_Area;
  };
};
