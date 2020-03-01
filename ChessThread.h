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
	inline void deactivate() {
		delete pChessboard;
		pMemory->ReleaseMemoryFrame(&MemoryFrame);
		delete pMemory;
	}
	inline void init(short threadID) {
		_tid = threadID;
		pMemory = new MemoryBlock();
		pMemory->init(THREAD_MEMORY_SIZE, ThreadHeap);
		MemoryFrame = pMemory->GetMemoryFrame(ThreadHeap);
		pChessboard = newChessboard(pMemory);
	}

	void loop(void) {
		S_ThreadMessage* msg = nullptr;
		BoardValue Score = 0, Alpha = 0, Beta = 0;
		ThreadStopped = false;
		ThreadEnable = true;
		std::chrono::milliseconds timespan(50); // 50ms	
		ChessThreadMessenger* Message = MESSENGER;
		Message->ThreadRunning();
		short MaxDepth = 0;
		U64 pv_moves[SEARCH_MAX_MOVES];
		short pv_count = 0;
	ThreadLoopBegin:
		ThreadIdle = true;
		Score = 0;
		Depth = 0;

		Message->ThreadWaiting();
		msg = Message->getNewMessage();
		while (!msg) {
			if (!ThreadEnable || _pSearchInfo->stopped)
				goto ThreadLoopEnd;
			ThreadSleep(timespan);
			msg = Message->getNewMessage();
		}
		Message->ThreadWorking();
		ThreadIdle = false;
		// set up chessboard by S_ThreadMessage
		pChessboard->SetThreadMessage(msg);
		Mode = msg->Mode;
		MaxDepth = msg->Depth;
		Alpha = msg->Alpha;
		Beta = msg->Beta;
		releaseThreadMessage(msg);

		switch (Mode) {
		case ThreadPerftTest:
			ThreadSleep(timespan * 10);
			print_console("ThreadPerftTest %d\n", _tid);
			break;
		case ThreadMainSearch:
		case ThreadAloneSearch:
			// do iterative deepening alphabeta search - MaxDepth == SEARCH_MAX_MOVES
			while(Depth < MaxDepth) {

				if (_pSearchInfo->stopped)
					break;

				Score = AlphaBeta(++Depth, Alpha, Beta);

				if (_pSearchInfo->stopped)
					break;

				// get pv line
				pv_count = pChessboard->getPvLine(pv_moves, SEARCH_MAX_MOVES) - pv_moves;
				// print pv line 
				// start Thread - Mode == ThreadPVSearch
				// for each move in pv line
				printing_console_start();
				std::cout << "info score cp " << Score << " depth " << Depth << " nodes " << _pSearchInfo->nodes << " time " << (GetMilliTime() - _pSearchInfo->starttime);

				std::cout << "pv";
				for (short i = 0; i < pv_count; i++) {
					std::cout << " " << PrMove(pv_moves[i]);
					pChessboard->doMove(&pv_moves[i]);
					msg = getThreadMessage();
					if (msg) {
						// while MessageContainer available
						pChessboard->GenThreadMessage(msg);
						msg->Depth = Depth - 1;
						if (msg->Depth == 0)
							msg->Depth = 1;
						else if (msg->Depth > 10)
							msg->Depth = 10;
						msg->Alpha = Score - 150;
						msg->Beta = Score + 150;
						msg->Mode = ThreadPVSearch;
						Message->putNewMessage(msg);
					}

				}
				while (pChessboard->SearchPly() > 0)
					pChessboard->undoMove();

				std::cout << std::endl;
				printing_console_end();

				_pSearchInfo->depth = Depth;					
				
			}
			//cleanup if Mode == ThreadMainSearch
			if (Mode == ThreadMainSearch)
				while (Message->getNewMessage());
			
			break;
		case ThreadPVSearch:
			// do iterative deepening alphabeta search - MaxDepth == 10
			while (Depth < MaxDepth) {
				// manage all other threads if Mode == ThreadMainSearch
				Score = AlphaBeta(++Depth, Alpha, Beta);
				if (_pSearchInfo->stopped)
					break;
			}
			break;
		case ThreadMinMaxSearch:
			// do minmax search - MaxDepth == 4
			MinMax(MaxDepth);
			break;
		default:
			break;
		}
		if (ThreadEnable)
			goto ThreadLoopBegin;
	ThreadLoopEnd:
		ThreadStopped = true;
		Message->ThreadStopped();
	}
	
	inline void StopThread(void) { ThreadEnable = false; _pSearchInfo->stopped = true; }

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
	S_MemoryFrame MemoryFrame;
	Chessboard* pChessboard = nullptr;
	MemoryBlock* pMemory = nullptr;
	std::atomic<bool> ThreadEnable = true;
	std::atomic<bool> ThreadIdle = true;
	std::atomic<bool> ThreadStopped = false;
	short Mode = ThreadPerftTest;
	char fen[255] = "                                                                                                                                                                                                                                                    ";
	short fen_len = 0;
	BoardValue Score = 0;
	U64 BestMove = 0ULL;
	short Depth = 0;
	short _tid = 0;

	void MinMax(const short Depth) { 

	}
	BoardValue AlphaBeta(const short Depth, BoardValue Alpha, BoardValue Beta) {
		return 0;
	}
	BoardValue Quiescence(const short Depth) {
		return 0;
	}
};
