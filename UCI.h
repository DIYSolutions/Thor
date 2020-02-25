#pragma once
#include "hash.h"
#include "ChessThreadManager.h"

class UCI
{
public:
	UCI() {
		InitSearchinfo();
		pTaskManager = new ChessThreadManager();
		pChessboard = new Chessboard();
		init();
	}

	void uci_loop(void) {
		while (true) {


			break;
		}
	}

private:
	short ThreadNum = STD_THREAD;
	U64 MemorySize = getMemorySize(STD_HASHTABLE_MB, STD_THREAD);
	ChessThreadManager* pTaskManager = nullptr;
	Chessboard* pChessboard = nullptr;
	short SearchMode = 0;

	inline void StartSearch(void) {
		_pSearchInfo->fh = 0;
		_pSearchInfo->fhf = 0;
		_pSearchInfo->nodes = 0;
		_pSearchInfo->nullCut = 0;
		pTaskManager->putNewMessage(
			pChessboard->GenThreadMessage(SearchMode)
		);
	}

	void SetupThreadsAndMemory(const U64 _MemorySize, const short _ThreadNum) {
		pTaskManager->Shutdown();
		destroyHashTable();
		_ShutdownFrameMemorySystem();

		ThreadNum = _ThreadNum;
		MemorySize = _MemorySize;

		init();
	}

	void init(void) {
		_InitFrameMemorySystem(MemorySize, 4);
		// init threads
		pTaskManager->Startup(ThreadNum);
		// init hash
		InitHashTable(_GetFreeMemory());
		if (ThreadNum > 1)
			SearchMode = ThreadMainSearch;
		else
			SearchMode = ThreadAloneSearch;
	}
};

