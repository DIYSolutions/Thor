#pragma once
#include "memory_basic.h"

class MemoryBlock
{
private:
	u8* pMemoryBlock = nullptr;
	u8* apBaseAndCap[2] = { nullptr, nullptr };
	u8* apFrame[2] = { nullptr, nullptr };
public:
	MemoryBlock(const U64 memory_size) {
		pMemoryBlock = (u8*)malloc(memory_size + _nByteAlignment);
		if (pMemoryBlock == 0)
			error_exit("MemoryBlock: memory alloc failed!");

		apBaseAndCap[0] = (u8*)ALIGNUP(pMemoryBlock, _nByteAlignment);
		apBaseAndCap[1] = (u8*)ALIGNUP(pMemoryBlock + memory_size, _nByteAlignment);

		apFrame[0] = apBaseAndCap[0];
		apFrame[1] = apBaseAndCap[1];
	}

	~MemoryBlock() {
		free(pMemoryBlock);
	}


	template <HEAPNUM nHeapNum>
	inline void* AllocFrameMemory(int nBytes) {
	}

	template <>
	inline void* AllocFrameMemory<ThreadHeap>(int nBytes) {
		nBytes = ALIGNUP(nBytes, _nByteAlignment);

		if (apFrame[HashHeap] + nBytes > apFrame[ThreadHeap])
			return nullptr;

		apFrame[ThreadHeap] -= nBytes;
		return (void*)(apFrame[ThreadHeap]);
	}

	template <>
	inline void* AllocFrameMemory<HashHeap>(int nBytes) {
		u8* pMem;

		nBytes = ALIGNUP(nBytes, _nByteAlignment);

		if (apFrame[HashHeap] + nBytes > apFrame[ThreadHeap])
			return nullptr;

		pMem = apFrame[HashHeap];
		apFrame[HashHeap] += nBytes;

		return (void*)pMem;
	}

	inline void* AllocFrameMemory(int nBytes) {
		return AllocFrameMemory<ThreadHeap>(nBytes);
	}

	inline S_MemoryFrame GetMemoryFrame(short nHeapNum) {
		S_MemoryFrame Frame;

		Frame.pFrame = apFrame[nHeapNum];
		Frame.nHeapNum = nHeapNum;

		return Frame;
	}

	void ReleaseMemoryFrame(S_MemoryFrame * Frame) {
		apFrame[Frame->nHeapNum] = Frame->pFrame;
	}


	U64 GetFreeMemory(void) {
		return apFrame[ThreadHeap] - apFrame[HashHeap];
	}


};

