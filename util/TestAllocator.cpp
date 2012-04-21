// Run and watch process memory usage.

#include <stdio.h>
#include <string.h>

#if defined( __WIN__ )

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define sleep( x ) Sleep( x * 1000 )

#include <WinLinearAllocator.h>
typedef WinLinearAllocator LinearAllocator;

#else

#include <unistd.h>

#include <LinuxLinearAllocator.h>
typedef LinuxLinearAllocator LinearAllocator;

#endif

int main()
{
  LinearAllocator alloc;
  alloc.Init( 64 * 1024 * 1024 );

  for ( int i = 0; i < 64; i++ )
  {
    memset( alloc.Allocate( 1024 * 1024 ), 0xaa, 1024 * 1024 );
    sleep( 1 );
  }

  alloc.Destroy();
  return 0;
}
