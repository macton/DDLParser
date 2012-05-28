#include <stdio.h>
#include <string.h>
#include <stdint.h>

#ifndef __WIN__
#include <unistd.h>
#endif

#include "test_nacl_ddl.h"

int main()
{
  Mariner m1;

  m1.Init();

  char   output_buffer[ 4096 ];
  char*  output_start  = output_buffer;
  char*  output_cursor = output_start;
  size_t output_size   = 0;

  m1.WriteJson( output_cursor, output_size );

  fwrite( output_start, output_size, 1, stdout );
  
  printf("\n");
  printf("Complete.\n");
  
  m1.Destroy();

  return (0);
}
