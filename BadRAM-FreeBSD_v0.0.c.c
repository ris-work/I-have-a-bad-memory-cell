/*
Copyright (c) 2012, Kim Øyhus
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met: 

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer. 
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution. 

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies, 
either expressed or implied, of the FreeBSD Project.
*/


#include <stdio.h>     // 
#include <fcntl.h>     // open()
#include <stdlib.h>    // perror() strtoul()
#include <sys/mman.h>  // mmap() mlock()
#include <unistd.h>    // sleep()
#include <errno.h>     // errno

main( int argc, char **argv)
{
  if( argc != 3 )  {
    fprintf( stderr,"Two arguments should be supplied. Address and mask, separated by space.\n");
    exit(0);       }

  errno = 0;
  unsigned long long start = strtoull( argv[1], NULL ,16 ); 
  if( errno )
    {
      fprintf(stderr, "Something wrong with address, which should be in hexadecimal, such as 0x15726000\n");
      exit(0);
    }
  size_t BadRAM_start  = start;  // A valid address. Nothing special

  errno = 0;
  unsigned long long mask = strtoull( argv[2], NULL ,16 ); 
  if( errno )
    {
      fprintf(stderr, "Something wrong with mask, which should be in hexadecimal, such as 0xffffe000\n");
      exit(0);
    }
  off_t BadRAM_mask  = mask;  // The mask, as user specified, which might be too short.


  printf("BadRAM for FreeBSD: 0x%llx 0x%llx\n", start, mask );
  //  printf("%d\n", argc);
  //  printf("%s\n", argv[0]);
    printf("%s\n", argv[1]);
    printf("%s\n", argv[2]);



  int ramfile = open("/dev/mem", O_RDWR | O_SYNC);
  if( ramfile == -1)
    {
      perror("Error trying to open /dev/mem for accessing physical RAM");
      exit(0);
    }

/* BadRAM test:
00015727270  Feil i en Memtest86+ test
000157272b0
000157272f0
00015727230
00015727430
00015726a70
00000001cc0  Variable bit i adresse
00000000010  Bit som er feil. Alltid denne.

Badram argument: 0x15726000, 0xffffe000

*/


/* BadRAM ARGUMENTS

The point is to block out a piece of RAM, by having its address
and an address mask. The mask tells which of the address lower bits
belong to the bad piece of RAM. Like this:

The address part inside the mask
/--------------------------\
1001 0110 1111 1101 1001 0100 0011 0100    An address
                            \---------/
                              outside the mask
1111 1111 1111 1111 1111 1110 0000 0000    The mask

This example was for 32 bit addresses with 512 addresses marked as bad, 9 bits.

All I have seen on the net for the format of BadRAM says it should be
what Memtest86+ prints out, except that is not true, because it does
not print out the mask, and the address is not in 32 bit hexadecimal,
but in 11 hex digits, which is 44 bit, giving 16 TiB addressable. The
machine I am programming this on is an old Thinkpad with 1.5 GiB RAM,
so no 64 bit FreeBSD here. I have seen BadRAM arguments with 11 digit
address and 8 digit mask, which does not make much sense.

The point of all this rambling is that I have to make this FreeBSD
BadRAM flexible. In other words, it has to expand the mask to its full
addressing range.

And by the way, the reason it only supports one bad block, is that
more is unlikely, and if you have memory THAT bad, blocking it is
quite unlikely to be good enough. Just accept that it is garbage.

My Thinkpads RAM is soldered in, so it cannot be replaced easily.
Thats why I wrote this program.
*/


//  size_t BadRAM_start  = 0x15726000;  // A valid address. Nothing special
//  size_t BadRAM_mask   = 0xffffe000;  // 32 bit might be too little, so
//  BadRAM_mask = (-1<<32) | BadRAM_mask; // sign expand it.
  off_t  BadRAM_length = 1+ ~BadRAM_mask;

  if( BadRAM_start & ~BadRAM_mask )
    printf("Last part of address truncated because it is outside mask.\n");
  BadRAM_start &= BadRAM_mask;  // Do it always anyway.

         void volatile * 
  RAM = (void volatile *)
	   mmap( 0,                      // Virtual address is irrelevant
		 BadRAM_length,          // How much to allocate
		 PROT_READ | PROT_WRITE, // So RAM can be tested
		 // PROT_NONE,  //  or this...probably not necessary...
		 MAP_SHARED,             // Real RAM, not a copy
		 ramfile,                // The RAM device file
		 BadRAM_start );         // Start of address range for my error. Should be page aligned, eve thoug doc claims it is not necessary, whatever that means.

  if( RAM == MAP_FAILED )
    {
      perror("Error trying to mmap() /dev/mem for accessing physical RAM");
      exit(0);
    }


  // Forcing real RAM to be really be mapped to virtual mem, by mlock.
  // mlock() probably fits given address inside a whole page. Hints both for and against
  // RAM mmap()ed, so should be page aligned. I hope. Havent tested.

  int locking = mlock((void *) RAM, BadRAM_length );
  if( locking == -1 )
    {
      perror("Error trying to mlock() bad RAM");
      exit(0);
    }

  // Sleeping while occupying the bad RAM. 32 years should be enough.
  sleep( 2000000000 );
}
