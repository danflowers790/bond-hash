
#include <iostream>
#include <cstdio>
#include <iomanip>
#include <vector>
#include <string.h>

using namespace std;

typedef union {
    unsigned char b[4];
    unsigned int  packedBytes;
} ipAddress_t;

const int numSlaves = 4;
const int numSPUs = 16;

ipAddress_t srcIP[numSPUs];
ipAddress_t dstIP[numSPUs];
unsigned short srcUDP[numSPUs];
unsigned short dstUDP[numSPUs];
unsigned short xorResult[numSPUs];
unsigned short xorCnt[numSlaves];

//
// doXor
//
void doXor()
{
  printf("\n");
  printf("(a    ^   b) \n");  
  printf("------------------------------------------------\n");  

  for (int i=0; i<numSPUs; i++) {
	xorResult[i] = (srcUDP[i] ^ dstUDP[i]) % numSlaves;
	
	xorCnt[xorResult[i]]++;

	printf("(%d ^ %d) mod %d = %d\n",  
		   srcUDP[i], dstUDP[i], numSlaves, xorResult[i]);
  }

  printf("distribution: ");
  for (int i=0; i<numSlaves; i++) {
	printf("%d=>%d, ", i, xorCnt[i]);
  }
  printf("\n");
  
}


//
// main()
//
// simulate bond hashing for L3 + L4
//
// (srcIP xor dstIP) xor (srcPort xor dstPort)
// module number of slaves in Bond
//
int main()
{
  
  // initialize arrays
  for (int i=0; i<numSPUs; i++) {

	srcUDP[i] = i;
	dstUDP[i] = i + 1;
	xorCnt[i]=0;
  }

  doXor();

  dstUDP[0]=5490;
  srcUDP[0]=dstUDP[0]-16;
  printf("%d(0x%x) ^ %d(0x%x) = %d (0x%x)\n", 
		 srcUDP[0], srcUDP[0], dstUDP[0], dstUDP[0], 
		 (srcUDP[0] ^ dstUDP[0]), (srcUDP[0] ^ dstUDP[0]));

  printf("----\n");
  for (int i=0; i<4; i++) {

	unsigned short sUDP = 5486 + (i%4);
	
	printf("%d(0x%x) ^ %d(0x%x) = %d\n", 
		   sUDP, sUDP, 
		   dstUDP[0], dstUDP[0], 
		   (sUDP ^ dstUDP[0])%4);
  }


  return 1;
}
