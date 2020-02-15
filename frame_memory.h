#pragma once
#include "basic_data.h"
#include <malloc.h>

typedef unsigned char u8;
typedef enum HEAPNUM { lowerHeap, upperHeap };

typedef struct {
	u8 *pFrame;
	short nHeapNum;
} S_MemoryFrame;

#define ALIGNUP( nAddress, nBytes ) ( (((U64)nAddress) + (nBytes)-1)&(~((nBytes)-1)) )

static int _nByteAlignment;
static u8* _pMemoryBlock;
static u8* _apBaseAndCap[2];

static u8* _apFrame[2];

inline bool InitFrameMemorySystem(U64 nSizeInBytes, int nByteAlignment) {
	nSizeInBytes = ALIGNUP(nSizeInBytes, nByteAlignment);

	_pMemoryBlock = (u8*) malloc(nSizeInBytes + nByteAlignment);

	if (_pMemoryBlock == 0)
		return false;

	_nByteAlignment = nByteAlignment;

	_apBaseAndCap[0] = (u8*)ALIGNUP(_pMemoryBlock, _nByteAlignment);
	_apBaseAndCap[1] = (u8*)ALIGNUP(_pMemoryBlock + nSizeInBytes, _nByteAlignment);

	_apFrame[0] = _apBaseAndCap[0];
	_apFrame[1] = _apBaseAndCap[1];

	return true;
}

inline void ShutdownFrameMemorySystem(void) {
	free(_pMemoryBlock);
}

template <HEAPNUM nHeapNum>
inline void* AllocFrameMemory(int nBytes) {
}

template <>
inline void* AllocFrameMemory<upperHeap>(int nBytes) {
	nBytes = ALIGNUP(nBytes, _nByteAlignment);

	if (_apFrame[lowerHeap] + nBytes > _apFrame[upperHeap])
		return nullptr;

	_apFrame[upperHeap] -= nBytes;
	return (void*)(_apFrame[upperHeap]);
}
#define MemoryFrameAlloc(nBytes) AllocFrameMemory<upperHeap>(nBytes)

template <>
inline void* AllocFrameMemory<lowerHeap>(int nBytes) {
	u8* pMem;

	nBytes = ALIGNUP(nBytes, _nByteAlignment);

	if (_apFrame[lowerHeap] + nBytes > _apFrame[upperHeap])
		return nullptr;

	pMem = _apFrame[lowerHeap];
	_apFrame[lowerHeap] += nBytes;

	return (void*)pMem;
}

inline void* AllocFrameMemory(int nBytes, int nHeapNum) {
	if (nHeapNum)
		return AllocFrameMemory<upperHeap>(nBytes);
	return AllocFrameMemory<lowerHeap>(nBytes);
}

inline S_MemoryFrame GetMemoryFrame(short nHeapNum) {
	S_MemoryFrame Frame;

	Frame.pFrame = _apFrame[nHeapNum];
	Frame.nHeapNum = nHeapNum;

	return Frame;
}
#define newMemoryFrame GetMemoryFrame(upperHeap)

void ReleaseMemoryFrame(S_MemoryFrame Frame) {
	_apFrame[Frame.nHeapNum] = Frame.pFrame;
}


U64 GetFreeMemory(void) {
	return _apFrame[upperHeap] - _apFrame[lowerHeap];
}
