#pragma once
#include "hash.h"
#include "perfttest.h"
#include "ThreadManager.h"

#define INPUTBUFFER 400 * 6

class UCI
{
public:
	UCI() {
		InitOutputLock();
		InitSearchinfo();		
		init();
	}
	void uci_loop(void) {
		setvbuf(stdin, NULL, _IOFBF , INPUTBUFFER);
		setvbuf(stdout, NULL, _IOFBF , INPUTBUFFER);

		std::cout << "id name " << ENGINE_NAME << std::endl;
		std::cout << "id author " << ENGINE_AUTHOR << std::endl;
		std::cout << "option name Threads type spin default 1 min 1 max " << MAX_THREAD << std::endl;
		std::cout << "option name Hash type spin default 64 min 4 max " << MAX_HASHTABLE_MB << std::endl;
		std::cout << "option name Book type check default false" << std::endl;
		std::cout << "uciok" << std::endl;

		while (uci_loop(MemoryHashMB, ThreadNum));
	}

	bool uci_loop(int _MB, int _T) {
		char line[INPUTBUFFER];
		int MB = _MB;
		int T = _T;
		int D = 0;
		while (true) {
			memset(&line[0], 0, sizeof(line));
			fflush(stdout);
			if (!fgets(line, INPUTBUFFER, stdin))
				continue;

			if (line[0] == '\n')
				continue;

			if (!strncmp(line, "isready", 7)) {
				print_console("readyok");
				continue;
			}
			else if (!strncmp(line, "position", 8)) {
				ParsePosition(line);
			}
			else if (!strncmp(line, "ucinewgame", 10)) {
				ParsePosition((char*)"position startpos");
			}
			else if (!strncmp(line, "go", 2)) {
				print_console("Seen Go..");
				ParseGo(line);
			}
			else if (!strncmp(line, "quit", 4)) {
				_pSearchInfo->quit = true;
				break;
			}
			else if (!strncmp(line, "uci", 3)) {
				printing_console_start();
				std::cout << "id name " << ENGINE_NAME << std::endl;
				std::cout << "id author " << ENGINE_AUTHOR << std::endl;
				std::cout << "uciok" << std::endl;
				printing_console_end();
			}
			else if (!strncmp(line, "perfttest ", 10)) {
				sscanf_s(line, "%*s %d", &D);
				print_console("starting perft test(depth=%d)", D);
				perfttest(pMemory, pChessboard, D, false);
			}
			else if (!strncmp(line, "multiperft ", 10)) {
				sscanf_s(line, "%*s %d", &D);
				print_console("starting perft test(depth=%d)", D);
				perfttest(pMemory, pChessboard, D, true);
			}
			else if (!strncmp(line, "setoption name Threads value ", 29)) {
				sscanf_s(line, "%*s %*s %*s %*s %d", &T);
				if (T < 1) T = 1;
				if (T > MAX_THREAD) T = MAX_THREAD;

				print_console("Set Threads to %d\n", T);
				SetupThreadsAndMemory(getMemorySize(MB, T), T);
				goto wait4threads;
			}
			else if (!strncmp(line, "setoption name Hash value ", 26)) {
				sscanf_s(line, "%*s %*s %*s %*s %d", &MB);
				if (MB < 4) MB = 4;
				if (MB > MAX_HASHTABLE_MB) MB = MAX_HASHTABLE_MB;
				print_console("Set Hash to %d MB\n", MB);
				SetupThreadsAndMemory(getMemorySize(MB, ThreadNum), ThreadNum);
				goto wait4threads;
			}
			else if (!strncmp(line, "setoption name Book value ", 26)) {
				char* ptrTrue = nullptr;
				ptrTrue = strstr(line, "true");
				if (ptrTrue != nullptr) {
					_pSearchInfo->useBook = true;
				}
				else {
					_pSearchInfo->useBook = false;
				}
			}

			if (_pSearchInfo->quit) {
				shutdown();
				std::cout << "Bye." << std::endl;
				break;
			}

		}
		return false;
	wait4threads:
		return true;
	}

private:
	int ThreadNum = STD_THREAD;
	int MemoryHashMB = STD_HASHTABLE_MB;
	U64 MemorySize = getMemorySize(STD_HASHTABLE_MB, STD_THREAD);
	Chessboard* pChessboard = nullptr;
	MemoryBlock* pMemory = nullptr;
	short SearchMode = 0;
	ThreadManager* pTaskManager;

	inline void StartSearch(void) {
		_pSearchInfo->fh = 0;
		_pSearchInfo->fhf = 0;
		_pSearchInfo->nodes = 0;
		_pSearchInfo->nullCut = 0;
	}

	void SetupThreadsAndMemory(const U64 _MemorySize, const short _ThreadNum) {
		shutdown();

		ThreadNum = _ThreadNum;
		MemorySize = _MemorySize;
		
		init();
	}

	void shutdown(void) {
		delete pTaskManager;
		destroyHashTable();		
		delete pChessboard;
		delete pMemory;
	}
	
	void init(void) {
		// init threads
		pTaskManager = new ThreadManager(ThreadNum);
		thread_manager = pTaskManager;
		pMemory = new MemoryBlock(THREAD_MEMORY_SIZE);
		pChessboard = new Chessboard();
		//pTaskManager->Startup(ThreadNum);
		// init hash
		InitHashTable(MemorySize - ((ThreadNum + 1) * THREAD_MEMORY_SIZE));
	}


	// go depth 6 wtime 180000 btime 100000 binc 1000 winc 1000 movetime 1000 movestogo 40
	void ParseGo(char* line) {

		int depth = -1, movestogo = -1, movetime = -1;
		int time = -1, inc = 0;
		char* ptr = NULL;
		_pSearchInfo->timeset = false;

		if ((ptr = strstr(line, "infinite"))) {
			;
		}

		if ((ptr = strstr(line, "binc")) && pChessboard->SideToMove() == BLACK) {
			inc = atoi(ptr + 5);
		}

		if ((ptr = strstr(line, "winc")) && pChessboard->SideToMove() == WHITE) {
			inc = atoi(ptr + 5);
		}

		if ((ptr = strstr(line, "wtime")) && pChessboard->SideToMove() == WHITE) {
			time = atoi(ptr + 6);
		}

		if ((ptr = strstr(line, "btime")) && pChessboard->SideToMove() == BLACK) {
			time = atoi(ptr + 6);
		}

		if ((ptr = strstr(line, "movestogo"))) {
			movestogo = atoi(ptr + 10);
		}

		if ((ptr = strstr(line, "movetime"))) {
			movetime = atoi(ptr + 9);
		}

		if ((ptr = strstr(line, "depth"))) {
			depth = atoi(ptr + 6);
		}

		if (movetime != -1) {
			time = movetime;
			movestogo = 1;
		}

		_pSearchInfo->starttime = GetMilliTime();
		_pSearchInfo->depth = depth;

		if (time != -1) {
			_pSearchInfo->timeset = true;
			if (movestogo == -1) {
				time /= (pChessboard->HistoryPly() / 10) + 10;
			}
			else {
				time /= movestogo;
			}
			time -= 50;
			_pSearchInfo->stoptime = _pSearchInfo->starttime + time + inc;
		}

		if (depth == -1) {
			_pSearchInfo->depth = SEARCH_MAX_MOVES;
		}

		printf("time:%d start:%d stop:%d depth:%d timeset:%d\n",
			time, (int)_pSearchInfo->starttime, (int)_pSearchInfo->stoptime, (int)_pSearchInfo->depth, (int)_pSearchInfo->timeset);
		StartSearch();
	}

	// position fen fenstr
	// position startpos
	// ... moves e2e4 e7e5 b7b8q
	void ParsePosition(char* lineIn) {

		lineIn += 9;
		char* ptrChar = lineIn;

		if (strncmp(lineIn, "startpos", 8) == 0) {
			pChessboard->ParseFEN((char*)START_FEN);
		}
		else {
			ptrChar = strstr(lineIn, "fen");
			if (ptrChar == NULL) {
				pChessboard->ParseFEN((char*)START_FEN);
			}
			else {
				ptrChar += 4;
				pChessboard->ParseFEN(ptrChar);
			}
		}

		ptrChar = strstr(lineIn, "moves");
		U64 move;
		S_MemoryFrame Frame = pMemory->GetMemoryFrame(ThreadHeap);
		if (ptrChar != NULL) {
			ptrChar += 6;
			while (*ptrChar) {
				move = ParseMove(ptrChar);
				if (move == NOMOVE) break;
				pChessboard->doMove(&move);
				while (*ptrChar && *ptrChar != ' ') ptrChar++;
				ptrChar++;
			}
		}
		pMemory->ReleaseMemoryFrame(&Frame);
		pChessboard->setPly(0);
		pChessboard->PrintBoard();
	}
	U64 ParseMove(char* ptrChar) {
		if (ptrChar[1] > '8' || ptrChar[1] < '1') return NOMOVE;
		if (ptrChar[3] > '8' || ptrChar[3] < '1') return NOMOVE;
		if (ptrChar[0] > 'h' || ptrChar[0] < 'a') return NOMOVE;
		if (ptrChar[2] > 'h' || ptrChar[2] < 'a') return NOMOVE;

		int from = FR2SQ(ptrChar[0] - 'a', ptrChar[1] - '1');
		int to = FR2SQ(ptrChar[2] - 'a', ptrChar[3] - '1');

		
		S_MOVELIST list;
		list.MovePtr = (S_MOVE*)pMemory->AllocFrameMemory<ThreadHeap>(sizeof(S_MOVE) * BOARD_MAX_MOVES);
		list.count = pChessboard->genMove(list.MovePtr);
		int MoveNum = 0;
		U64 Move = 0;
		int PromPce = Empty;

		for (MoveNum = 0; MoveNum < list.count; ++MoveNum) {
			Move = list.MovePtr[MoveNum].Move;
			if (MOVE_FROMSQ(Move) == from && MOVE_TOSQ(Move) == to) {
				PromPce = MOVE_PROMOTED(Move);
				if (PromPce != Empty) {
					if (IsRook(PromPce) && ptrChar[4] == 'r') {
						return Move;
					}
					else if (IsBishop(PromPce) && ptrChar[4] == 'b') {
						return Move;
					}
					else if (IsQueen(PromPce) && ptrChar[4] == 'q') {
						return Move;
					}
					else if (IsKnight(PromPce) && ptrChar[4] == 'n') {
						return Move;
					}
					ASSERT(false);
					continue;
				}
				
				return Move;
			}
		}

		return NOMOVE;
	}
};

