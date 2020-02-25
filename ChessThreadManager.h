#pragma once
#include "ChessThread.h"
#include "ChessThreadMessenger.h"


void SearchThread(void * _Thread) {
	ChessThread* Thread = (ChessThread*)_Thread;
	Thread->loop();
}

class ChessThreadManager: public ChessThreadMessenger
{
public:
	ChessThreadManager(void): ChessThreadMessenger() {
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
		Threads[0] = newChessThread((ChessThreadMessenger*)this);// first thread is the main thread
		for (short i = 1; i < _ThreadNum; i++) {
			Threads[i] = newChessThread((ChessThreadMessenger*)this);
			// start Thread
			_beginthread(SearchThread, 0, (void*)&Threads[i]);
		}
	}


private:
	ChessThread * Threads[MAX_THREAD];
	short ThreadNum = 0;
};
