#pragma once
#include "memory_basic.h"

class MemoryBlock
{
private:
	u8* pMemoryBlock = nullptr;
	u8* apBaseAndCap[2] = { nullptr, nullptr };
	u8* apFrame[2] = { nullptr, nullptr };
	S_MemoryFrame MemoryFrame;
public:
	MemoryBlock() {}

	void init(const U64 memory_size, const short heap_num) {
		MemoryFrame = _GetMemoryFrame(heap_num);
		pMemoryBlock = (u8*)_AllocFrameMemory(memory_size, heap_num);
		if (_pMemoryBlock == 0)
			error_exit("MemoryBlock: memory alloc failed!");

		apBaseAndCap[0] = (u8*)ALIGNUP(pMemoryBlock, _nByteAlignment);
		apBaseAndCap[1] = (u8*)ALIGNUP(pMemoryBlock + memory_size, _nByteAlignment);

		apFrame[0] = apBaseAndCap[0];
		apFrame[1] = apBaseAndCap[1];
	}

	~MemoryBlock() {
		_ReleaseMemoryFrame(&MemoryFrame);
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

