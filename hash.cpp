
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
const int numSPAs = 4;
const int numSPUs = 7;
const int totalSPUs = (numSPAs * numSPUs);

ipAddress_t srcIP[totalSPUs];
ipAddress_t dstIP[totalSPUs];
unsigned short srcUDP[totalSPUs];
unsigned short dstUDP[totalSPUs];
unsigned short xorResult[totalSPUs];
unsigned short xorCnt[numSlaves];

//
// doL3L4Xor
//
void doL3L4Xor()
{
  printf("\n");
  printf("(srcIP    ^   dstIP) ^  (sUDP ^ dUDP) modulo %d\n", numSlaves);  
  printf("------------------------------------------------\n");  

  for (int i=0; i<totalSPUs; i++) {

	xorResult[i] = (((unsigned int)srcIP[i].b[3] ^ (unsigned int)dstIP[i].b[3]) ^  
					(srcUDP[i] ^ dstUDP[i])) % numSlaves;
	
	xorCnt[xorResult[i]]++;

	printf("(%d.%d.%d.%d ^ %d.%d.%d.%d) ^ (%d ^ %d) mod %d = %d\n",  
		   (unsigned int)srcIP[i].b[0], (unsigned int)srcIP[i].b[1], (unsigned int)srcIP[i].b[2], (unsigned int)srcIP[i].b[3],
		   (unsigned int)dstIP[i].b[0], (unsigned int)dstIP[i].b[1], (unsigned int)dstIP[i].b[2], (unsigned int)dstIP[i].b[3],
		   srcUDP[i], dstUDP[i], numSlaves, xorResult[i]);
  }

  printf("Interface distribution: ");
  for (int i=0; i<numSlaves; i++) {
	printf("%d=>%d, ", i, xorCnt[i]);
  }
  printf("\n");
  
}

//
// doL3Xor
//
void doL3Xor()
{
  printf("\n");
  printf("(srcIP    ^   dstIP) modulo %d\n", numSlaves);  
  printf("----------------------------\n");  

  for (int i=0; i<totalSPUs; i++) {

	xorResult[i] = (((unsigned int)srcIP[i].b[3] ^ (unsigned int)dstIP[i].b[3])) % numSlaves;
	
	xorCnt[xorResult[i]]++;

	printf("(%d.%d.%d.%d ^ %d.%d.%d.%d) mod %d = %d\n",  
		   (unsigned int)srcIP[i].b[0], (unsigned int)srcIP[i].b[1], (unsigned int)srcIP[i].b[2], (unsigned int)srcIP[i].b[3],
		   (unsigned int)dstIP[i].b[0], (unsigned int)dstIP[i].b[1], (unsigned int)dstIP[i].b[2], (unsigned int)dstIP[i].b[3],
		   numSlaves, xorResult[i]);
  }

  printf("Interface distribution: ");
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
  
  // Host-Dist Use case 1: Standard FComm with slot-based IP
  //   host              -> SPU
  //   srcIP=10.0.0.2       dstIP=10.0.2.slot (slot + odd/even)
  //   srcPort=5489         dstPort=5490
  
  srcIP[0].b[0]=10; srcIP[0].b[1]=0; srcIP[0].b[2]=0; srcIP[0].b[3]=1;
  dstIP[0].b[0]=10; dstIP[0].b[1]=0; dstIP[0].b[2]=2; dstIP[0].b[3]=2;
  srcUDP[0]=5489;
  dstUDP[0]=5490;

  // initialize arrays
  for (int i=0; i<totalSPUs; i++) {

	// All srcIPs, srcUDP, and dstUDP are the same.
	srcIP[i] = srcIP[0];
	srcUDP[i] = srcUDP[0];
	dstUDP[i] = dstUDP[0];

	// odd/even SPU IPs (based on slot)
	dstIP[i] = dstIP[0];
	dstIP[i].b[3] = (i*2+1)*2 + i%2;

	xorCnt[i]=0;
  }

  printf("Host-Dist use case 1: Standard FComm settings.");
  doL3L4Xor();

  ///////////////////////////////////////////////////////////////

  // Host-Dist: Use case 2: Sequential IPs
  //   host              -> SPU
  //   srcIP=10.0.0.2       dstIP=10.0.2.1 (odd/even)
  //   srcPort=5489         dstPort=5490
  
  srcIP[0].b[0]=10; srcIP[0].b[1]=0; srcIP[0].b[2]=0; srcIP[0].b[3]=2;
  dstIP[0].b[0]=10; dstIP[0].b[1]=0; dstIP[0].b[2]=2; dstIP[0].b[3]=1;
  srcUDP[0]=5489;
  dstUDP[0]=5490;

  // initialize arrays
  for (int i=0; i<totalSPUs; i++) 
	{
	  // All srcIPs, srcUDP, and dstUDP are the same.
	  srcIP[i] = srcIP[0];
	  srcUDP[i] = srcUDP[0];
	  dstUDP[i] = dstUDP[0];
	  
	  // odd/even SPU IPs
	  dstIP[i] = dstIP[0];
	  dstIP[i].b[3] = i+1;

	  xorCnt[i]=0;
	}

  // Use case 2: Standard FComm
  printf("\nHost-Dist Use Case 2: Sequential IPs (L3+L4).");
  doL3L4Xor();

  for (int i=0; i<totalSPUs; i++) 
	{
	  xorCnt[i]=0;
	}
  printf("\nHost-Dist Use Case 2: Sequential IPs (L3).");
  doL3Xor();



  ///////////////////////////////////////////////////////////////
  // Host-Dist: Use case 3: Sequential IPs and Adapted S-UDP
  //   host              -> SPU
  //   srcIP=10.0.0.2       dstIP=10.0.2.[1..28]
  //   srcPort=[5486-5489]  dstPort=5490
  
  srcIP[0].b[0]=10; srcIP[0].b[1]=0; srcIP[0].b[2]=0; srcIP[0].b[3]=2;
  dstIP[0].b[0]=10; dstIP[0].b[1]=0; dstIP[0].b[2]=2; dstIP[0].b[3]=1;
  srcUDP[0]=5486;
  dstUDP[0]=5490;

  // initialize arrays
  for (int i=0; i<totalSPUs; i++) 
	{
	  // All srcIPs and dstUDP are the same.
	  srcIP[i] = srcIP[0];
	  dstUDP[i] = dstUDP[0];
	  
	  // odd/even SPU IPs
	  dstIP[i] = dstIP[0];
	  dstIP[i].b[3] = i+1;

	  // Src-UDP is function of SIP/DIP Xor
	  // To assure fragmented packets (without UDP hdr) take same path

	  //	  srcUDP[i] = srcUDP[0] + (i%numSlaves);
	  srcUDP[i] = 5490;
	  
	  xorCnt[i]=0;
	}

  // Use case 2: Standard FComm
  printf("\nHost-Dist Use Case 3: Sequential IPs (L3+L4) and Adapted S-UDP.");
  doL3L4Xor();

  for (int i=0; i<totalSPUs; i++) 
	{
	  xorCnt[i]=0;
	}
  printf("\nHost-Dist Use Case 2: Sequential IPs (L3) and Adapted S-UDP.");
  doL3Xor();


  ///////////////////////////////////////////////////////////////

  // SPU-2-SPU: Use case 1: Standard FComm (slot-based IP addresses)
  //   SPU              -> SPU
  //   srcIP=10.0.2.slot    dstIP=10.0.2.slot (slot*2 + odd/even)
  //   srcPort=5492         dstPort=5492
#if 0  
  srcIP[0].b[0]=10; srcIP[0].b[1]=0; srcIP[0].b[2]=2; srcIP[0].b[3]=1;
  dstIP[0].b[0]=10; dstIP[0].b[1]=0; dstIP[0].b[2]=2; dstIP[0].b[3]=1;
  srcUDP[0]=5489;
  dstUDP[0]=5490;

  // initialize arrays
  for (int i=0; i<totalSPUs; i++) {

	// All srcIPs, srcUDP, and dstUDP are the same.
	srcIP[i] = srcIP[0];
	srcUDP[i] = srcUDP[0];
	dstUDP[i] = dstUDP[0];

	// odd/even SPU IPs
	dstIP[i] = dstIP[0];
	dstIP[i].b[3] = dstIP[0].b[3] + i;

	xorCnt[i]=0;
  }

  // Use case 2: Standard FComm
  printf("\nHost-Dist Use Case 2: Standard FComm settings - but simple odd/even IPs.");
  doL3L4Xor();
#endif
  ////////////////////////////////////


  return 1;
}
