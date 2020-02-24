#pragma once
#include "hash.h"
#include "ChessThreadManager.h"

class UCI
{
private:
	short ThreadNum = STD_THREAD;
	U64 MemorySize = getMemorySize(STD_HASHTABLE_MB, STD_THREAD);
	ChessThreadManager* TaskManager = nullptr;

public:
	UCI() {
		InitSearchinfo();
		TaskManager = new ChessThreadManager();
		init();
	}
	void init(void) {
		_InitFrameMemorySystem(MemorySize, 4);
		// init threads
		TaskManager->Startup(ThreadNum);
		// init hash
		InitHashTable(_GetFreeMemory());
	}

	void uci_loop(void) {
		while (true) {


			break;
		}
	}

	void SetupThreadsAndMemory(const U64 _MemorySize, const short _ThreadNum) {
		TaskManager->Shutdown();
		destroyHashTable();
		_ShutdownFrameMemorySystem();

		ThreadNum = _ThreadNum;
		MemorySize = _MemorySize;

		init();
	}
};

