#include <stdio.h>
#include <stdint.h>

int main( int argc, const char* argv[] )
{
  FILE* in = fopen( argv[ 1 ], "r" );

  if ( argc != 3 )
  {
    return -1;
  }
  
  printf( "unsigned char %s[] =\n", argv[ 2 ] );
  printf( "{\n" );

  int total = 0;
  
  for ( ;; )
  {
    uint8_t buffer[ 16 ];
    
    int num_read = fread( buffer, 1, 16, in );
    
    if ( num_read <= 0 )
    {
      break;
    }
    
    printf( "  " );
    
    for ( int i = 0; i < num_read; i++ )
    {
      printf( "0x%02x,", buffer[ i ] );
    }
    
    total += num_read;
    printf( "\n" );
  }

  printf( "\n" );
  printf( "};\n\n" );
  printf( "int _%s_size = %d;\n", argv[ 2 ], total );
  
  return 0;
}
