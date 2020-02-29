#pragma once
#include "ChessThread.h"
#include "ChessThreadMessenger.h"


void SearchThread(void * _Thread) {
	ChessThread* Thread = (ChessThread*)_Thread;
	Thread->loop();
}

class ChessThreadManager
{
public:
	ChessThreadManager(void) {	}

	inline bool ThreadRunning(void) {
		return !MESSENGER->ThreadsStopped();
	}
	void StopSearch(void) {
		_pSearchInfo->stopped = true;
	}
	void Shutdown(void) {
		_pSearchInfo->stopped = true;
		
		while (ThreadRunning());

		for (short i = ThreadNum - 1; i >= 0; i--) {
			Threads[i].deactivate();
		}

		_ReleaseMemoryFrame(&MemoryFrame);
	}
	void Startup(const short _ThreadNum) {
		ThreadNum = _ThreadNum;
		MemoryFrame = _GetMemoryFrame(ThreadHeap);
		for (short i = 0; i < _ThreadNum; i++) {
			Threads[i].init(i);
			// start Thread
			_beginthread(SearchThread, 0, (void*)&Threads[i]);
		}
	}

	inline void putNewMessage(S_ThreadMessage* newMessage) {
		MESSENGER->putNewMessage(newMessage);
	}


private:
	ChessThread Threads[MAX_THREAD];
	short ThreadNum = 0;
	S_MemoryFrame MemoryFrame;
	
};
