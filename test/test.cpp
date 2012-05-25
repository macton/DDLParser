#include <stdio.h>
#include <string.h>

#ifndef __WIN__
#include <unistd.h>
#endif

#include "test_ddl.h"

int main()
{
  Mariner m1, m2;
  
  class CleanUp { public: Mariner *m1, *m2; ~CleanUp() { m1->Destroy(); m2->Destroy(); } } cleanup;
  cleanup.m1 = &m1;
  cleanup.m2 = &m2;

  m1.Init();
  
  Position p;
  p.SetX( 1 );
  p.SetY( 2 );
  p.SetAngle( 3 );
  m1.AppendDeaths( p );
  m1.InsertCompleted( "E1M1", 100 );
  
  FILE* fp = fopen( "mariner.dat", "wb" );
  if ( !m1.Save( fp ) )
  {
    unlink( "mariner.dat" );
    fprintf( stderr, "Failed to save mariner\n" );
    return -1;
  }
  fclose( fp );

  fp = fopen( "mariner.dat", "rb" );
  if ( !m2.Load( fp ) )
  {
    unlink( "mariner.dat" );
    fprintf( stderr, "Failed to load mariner\n" );
    return -1;
  }
  fclose( fp );

  unlink( "mariner.dat" );
  
  bool ok = true;
  
  ok = ok && m1.GetHealth() == m2.GetHealth();
  ok = ok && m1.GetWeapon().Get() == m2.GetWeapon().Get();
  ok = ok && m1.GetPowerup().Get() == m2.GetPowerup().Get();
  ok = ok && m1.GetAmmunitionCount() == m2.GetAmmunitionCount();
  
  if ( ok )
  {
    for ( size_t i = 0; i < m1.GetAmmunitionCount(); i++ )
    {
      ok = ok && m1.GetAmmunition( i ) == m2.GetAmmunition( i );
    }
  }
  
  ok = ok && m1.GetName() == m2.GetName();
  ok = ok && m1.GetPosition().GetX() == m2.GetPosition().GetX();
  ok = ok && m1.GetPosition().GetY() == m2.GetPosition().GetY();
  ok = ok && m1.GetPosition().GetAngle() == m2.GetPosition().GetAngle();
  ok = ok && m1.GetDeathsCount() == m1.GetDeathsCount();
  
  if ( ok )
  {
    for ( size_t i = 0; i < m1.GetDeathsCount(); i++ )
    {
      ok = ok && m1.GetDeaths( i ).GetX() == m2.GetDeaths( i ).GetX();
      ok = ok && m1.GetDeaths( i ).GetY() == m2.GetDeaths( i ).GetY();
      ok = ok && m1.GetDeaths( i ).GetAngle() == m2.GetDeaths( i ).GetAngle();
    }
  }
  
  if ( ok )
  {
    int percent1 = m1.GetCompleted( "E1M1" );
    int percent2 = m2.GetCompleted( "E1M1" );
    ok = ok && percent1 == percent2;
  }
  
  if ( !ok )
  {
    fprintf( stderr, "Data does not match\n" );
    return -1;
  }

  fprintf( stderr, "Success!\n" );
  return 0;
}
