#pragma once
#include "basic.h"
#include <malloc.h>

typedef unsigned char u8;
enum HEAPNUM { HashHeap, ThreadHeap };

typedef struct S_MemoryFrame {
	u8* pFrame = nullptr;
	short nHeapNum = HashHeap;
} S_MemoryFrame;

#define ALIGNUP( nAddress, nBytes ) ( (((U64)nAddress) + (nBytes)-1)&(~((nBytes)-1)) )

static int _nByteAlignment = 4;