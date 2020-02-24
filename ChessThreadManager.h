#pragma once
#include "ChessThread.h"

#include <process.h>
#include <windows.h>

void SearchThread(void * _Thread) {
	ChessThread* Thread = (ChessThread*)_Thread;
	Thread->loop();
}

class ChessThreadManager
{
public:
	ChessThreadManager() {
	}

	bool ThreadRunning(void) {
		for (short i = 0; i < ThreadNum; i++)
			if (Threads[i]->Stopped())
				return true;
		return false;
	}
	void StopSearch(void) {
		_pSearchInfo->stopped = true;
	}
	void Shutdown(void) {
		_pSearchInfo->stopped = true;
		for (short i = 0; i < ThreadNum; i++)
			Threads[i]->StopThread();
		
		while (ThreadRunning());
	}
	void Startup(const short _ThreadNum) {
		ThreadNum = 0;
		for (short i = 0; i < _ThreadNum; i++) {
			Threads[i] = newChessThread();
			// start Thread
			_beginthread(SearchThread, 0, (void*)&Threads[i]);
		}
	}


private:
	ChessThread * Threads[MAX_THREAD];
	short ThreadNum = 0;
	std::atomic<bool> ThreadEnable = true;
	std::atomic<bool> ThreadStopped = false;
};
