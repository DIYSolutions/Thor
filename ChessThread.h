#pragma once
#include "MemoryBlock.h"
#include "Chessboard.h"
#include <atomic>

typedef enum ThreadTask { ThreadNeedWork, ThreadAloneSearch, ThreadMainSearch, ThreadPVSearch, ThreadMinMaxSearch };

class ChessThread
{
public:
	ChessThread(void) {}

	inline void init(void) {
		pMemory = (MemoryBlock*)_AllocFrameMemory<ThreadHeap>(sizeof(MemoryBlock));
		pMemory->init(THREAD_MEMORY_SIZE, ThreadHeap);
		pChessboard = newChessboard(pMemory);
	}

	void loop(void) {
		ThreadStopped = false;
		ThreadEnable = true;
	ThreadLoopBegin:
		ThreadIdle = true;
		switch (Mode) {
		case ThreadNeedWork:
			// add sleep!!
			break;
		case ThreadAloneSearch:
			ThreadIdle = false;
			// set up chessboard by FEN
			// do iterative deepening alphabeta search - MaxDepth == SEARCH_MAX_MOVES
			break;
		case ThreadMainSearch:
			ThreadIdle = false;
			// set up chessboard by FEN
			// do iterative deepening alphabeta search - MaxDepth == SEARCH_MAX_MOVES
			// manage all other threads
			break;
		case ThreadPVSearch:
			ThreadIdle = false;
			// set up chessboard by FEN
			// do iterative deepening alphabeta search - MaxDepth == 8
			break;
		case ThreadMinMaxSearch:
			ThreadIdle = false;
			// set up chessboard by FEN
			// do minmax search - MaxDepth == 4
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
	std::atomic<bool> ThreadEnable = true;
	std::atomic<bool> ThreadIdle = true;
	std::atomic<bool> ThreadStopped = false;
	short Mode = ThreadNeedWork;
	char fen[255] = "                                                                                                                                                                                                                                                    ";
	short fen_len = 0;
	BoardValue Score = 0;
	U64 BestMove = 0ULL;
};

inline ChessThread* newChessThread(void) {
	ChessThread* pNewChessThread = (ChessThread*)_AllocFrameMemory<ThreadHeap>(sizeof(ChessThread));
	if (!pNewChessThread)
		error_exit("ChessThread: memory alloc failed!");

	pNewChessThread->init();
	return pNewChessThread;
}