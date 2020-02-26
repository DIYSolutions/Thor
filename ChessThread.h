#pragma once
#include "MemoryBlock.h"
#include "Chessboard.h"
#include "ChessThreadMessenger.h"
#include <atomic>
#include <process.h>
#include <windows.h>
#include <thread>

enum ThreadTask { ThreadPerftTest, ThreadAloneSearch, ThreadMainSearch, ThreadSubSearch, ThreadPVSearch, ThreadMinMaxSearch };

inline void ThreadSleep(std::chrono::milliseconds timespan) {
	std::this_thread::sleep_for(timespan);
}

class ChessThread
{
public:
	ChessThread(void) { }

	inline void init(ChessThreadMessenger * _ManagerMessage) {
		pMemory = (MemoryBlock*)_AllocFrameMemory<ThreadHeap>(sizeof(MemoryBlock));
		pMemory->init(THREAD_MEMORY_SIZE, ThreadHeap);
		pChessboard = newChessboard(pMemory);
		pManagerMessage = _ManagerMessage;
	}

	void loop(void) {
		S_ThreadMessage* msg = nullptr;
		BoardValue Score = 0;
		ThreadStopped = false;
		ThreadEnable = true;
		std::chrono::milliseconds timespan(50); // or whatever		
		
	ThreadLoopBegin:
		ThreadIdle = true;
		Score = 0;
		Depth = 0;
		while(_pSearchInfo->stopped)
			ThreadSleep(timespan);

		pManagerMessage->ThreadWaiting();
		msg = pManagerMessage->getNewMessage();
		while (!msg) {
			ThreadSleep(timespan);
			msg = pManagerMessage->getNewMessage();
		}
		pManagerMessage->ThreadWorking();
		ThreadIdle = false;
		// set up chessboard by S_ThreadMessage
		pChessboard->SetThreadMessage(msg);
		Mode = msg->Mode;
		releaseThreadMessage(msg);

		switch (Mode) {
		case ThreadPerftTest:
			break;
		case ThreadSubSearch:
			_pSearchInfo->SubSearchNum++;
		case ThreadMainSearch:
			pManagerMessage->putNewMessage(
				pChessboard->GenThreadMessage(ThreadMinMaxSearch)
			);
		case ThreadAloneSearch:
			// do iterative deepening alphabeta search - MaxDepth == SEARCH_MAX_MOVES
			while(Depth < SEARCH_MAX_MOVES) {

				if (_pSearchInfo->stopped)
					break;

				Score = AlphaBeta(++Depth, MIN_INFINTE, MAX_INFINTE);

				if (_pSearchInfo->stopped)
					break;

				// get pv line
				// print pv line 

				// no other threads available if Mode != ThreadMainSearch
				while (pManagerMessage->ThreadAvailable()) {// while threads available
					// start Thread - Mode == ThreadPVSearch
					// for each move in pv line
					// break if no pv line or last move reached
				}
				// now for each move in starting position
				while (pManagerMessage->ThreadAvailable()) {// while threads available
					// start Thread - Mode == ThreadPVSearch
					// break if last move reached
				}
				if (Mode == ThreadSubSearch) {
					if (Depth >= _pSearchInfo->depth) {
						_pSearchInfo->SubSearchNum--;
						break;
					}
				}
				else {
					_pSearchInfo->depth = Depth;
					
				}
			}
			//cleanup if Mode == ThreadMainSearch
			if (Mode == ThreadMainSearch)
				while (pManagerMessage->getNewMessage());
			
			break;
		case ThreadPVSearch:
			// do iterative deepening alphabeta search - MaxDepth == 10
			while (Depth < 10) {
				// manage all other threads if Mode == ThreadMainSearch
				Score = AlphaBeta(++Depth, MIN_INFINTE, MAX_INFINTE);
				if (_pSearchInfo->stopped)
					break;
			}
			break;
		case ThreadMinMaxSearch:
			// do minmax search - MaxDepth == 4
			MinMax(4);
			break;
		default:
			break;
		}

		if (ThreadEnable)
			goto ThreadLoopBegin;
		ThreadStopped = true;
	}
	
	inline void StopThread(void) { ThreadEnable = false; }

	inline void SetFEN(char* fen) {
		fen_len = 0;
		while (*fen != ';') {
			fen[fen_len++] = *(fen++);
		}
	}

	inline void SetMode(const short _Mode) {
		Mode = _Mode;
	}
	inline BoardValue getMode(void) { return Mode; }
	inline BoardValue getScore(void) { return Score; }
	inline U64 getBestMove(void) { return BestMove; }

	inline bool Stopped(void) { return ThreadStopped; }
	inline bool Idle(void) { return ThreadIdle; }
private:
	Chessboard* pChessboard = nullptr;
	MemoryBlock* pMemory = nullptr;
	ChessThreadMessenger* pManagerMessage = nullptr;
	std::atomic<bool> ThreadEnable = true;
	std::atomic<bool> ThreadIdle = true;
	std::atomic<bool> ThreadStopped = false;
	short Mode = ThreadPerftTest;
	char fen[255] = "                                                                                                                                                                                                                                                    ";
	short fen_len = 0;
	BoardValue Score = 0;
	U64 BestMove = 0ULL;
	short Depth = 0;


	void MinMax(const short Depth) { 

	}
	BoardValue AlphaBeta(const short Depth, BoardValue Alpha, BoardValue Beta) {
		return 0;
	}
	BoardValue Quiescence(const short Depth) {
		return 0;
	}
};

inline ChessThread* newChessThread(ChessThreadMessenger* _ManagerMessage) {
	ChessThread* pNewChessThread = (ChessThread*)_AllocFrameMemory<ThreadHeap>(sizeof(ChessThread));
	if (!pNewChessThread)
		error_exit("ChessThread: memory alloc failed!");

	pNewChessThread->init(_ManagerMessage);
	return pNewChessThread;
}