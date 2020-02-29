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

static int _nByteAlignment;
static u8* _pMemoryBlock = nullptr;
static u8* _apBaseAndCap[2];

static u8* _apFrame[2];

enum FrameInitStatus {FrameInit, FrameReInit};

template <FrameInitStatus FIS>
inline bool _InitFrameMemorySystem(U64 nSizeInBytes, int nByteAlignment) {
	nSizeInBytes = ALIGNUP(nSizeInBytes, nByteAlignment);

	if(FIS == FrameInit)
		_pMemoryBlock = (u8*)malloc(nSizeInBytes + nByteAlignment);
	else
		_pMemoryBlock = (u8*)realloc(_pMemoryBlock, nSizeInBytes + nByteAlignment);
	
	if (_pMemoryBlock == 0)
		return false;

	_nByteAlignment = nByteAlignment;

	_apBaseAndCap[0] = (u8*)ALIGNUP(_pMemoryBlock, _nByteAlignment);
	_apBaseAndCap[1] = (u8*)ALIGNUP(_pMemoryBlock + nSizeInBytes, _nByteAlignment);

	_apFrame[0] = _apBaseAndCap[0];
	_apFrame[1] = _apBaseAndCap[1];

	return true;
}


inline void _ShutdownFrameMemorySystem(void) {
	if (_pMemoryBlock)
		free(_pMemoryBlock);
}


template <HEAPNUM nHeapNum>
inline void* _AllocFrameMemory(U64 nBytes) {
}

template <>
inline void* _AllocFrameMemory<ThreadHeap>(U64 nBytes) {
	nBytes = ALIGNUP(nBytes, _nByteAlignment);

	if (_apFrame[HashHeap] + nBytes > _apFrame[ThreadHeap])
		return nullptr;

	_apFrame[ThreadHeap] -= nBytes;
	return (void*)(_apFrame[ThreadHeap]);
}

template <>
inline void* _AllocFrameMemory<HashHeap>(U64 nBytes) {
	u8* pMem;

	nBytes = ALIGNUP(nBytes, _nByteAlignment);

	if (_apFrame[HashHeap] + nBytes > _apFrame[ThreadHeap])
		return nullptr;

	pMem = _apFrame[HashHeap];
	_apFrame[HashHeap] += nBytes;

	return (void*)pMem;
}

inline void* _AllocFrameMemory(U64 nBytes, int nHeapNum) {
	if (nHeapNum)
		return _AllocFrameMemory<ThreadHeap>(nBytes);
	return _AllocFrameMemory<HashHeap>(nBytes);
}

inline S_MemoryFrame _GetMemoryFrame(short nHeapNum) {
	S_MemoryFrame Frame;

	Frame.pFrame = _apFrame[nHeapNum];
	Frame.nHeapNum = nHeapNum;

	return Frame;
}


void _ReleaseMemoryFrame(S_MemoryFrame * Frame) {
	_apFrame[Frame->nHeapNum] = Frame->pFrame;
}


U64 _GetFreeMemory(void) {
	return _apFrame[ThreadHeap] - _apFrame[HashHeap];
}