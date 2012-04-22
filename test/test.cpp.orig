#include <stdio.h>
#include <string.h>

#ifndef __WIN__
#include <unistd.h>
#endif

#include "test_ddl.h"

int main()
{
  Mariner m1, m2;

  m1.Init();
  FILE* fp = fopen( "mariner.dat", "wb" );
  if ( !m1.Save( fp ) )
  {
    unlink( "mariner.dat" );
    fprintf( stderr, "Failed to save mariner\n" );
    return -1;
  }
  fclose( fp );

  memset( &m2, 0, sizeof( Mariner ) );
  fp = fopen( "mariner.dat", "rb" );
  if ( !m2.Load( fp ) )
  {
    unlink( "mariner.dat" );
    fprintf( stderr, "Failed to load mariner\n" );
    return -1;
  }
  fclose( fp );

  if ( memcmp( &m1, &m1, sizeof( Mariner ) ) != 0 )
  {
    unlink( "mariner.dat" );
    fprintf( stderr, "Data does not match\n" );
    return -1;
  }

  unlink( "mariner.dat" );

  fprintf( stderr, "Success!\n" );
  return 0;
}
