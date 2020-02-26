#pragma once
#include "hash.h"
#include "ChessThreadManager.h"
#define INPUTBUFFER 400 * 6

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
		setvbuf(stdin, NULL, _IOFBF , INPUTBUFFER);
		setvbuf(stdout, NULL, _IOFBF , INPUTBUFFER);

		char line[INPUTBUFFER];
		print_console("id name %*s\n", (char*)ENGINE_NAME);
		print_console("id author %*s\n", (char*)ENGINE_AUTHOR);
		print_console("option name Threads type spin default 1 min 1 max %d\n", MAX_THREAD);
		print_console("option name Hash type spin default 64 min 4 max %d\n", MAX_HASHTABLE_MB);
		print_console("option name Book type check default false\n");
		print_console("uciok\n");

		int MB = STD_HASHTABLE_MB;
		int T = STD_THREAD;
		while (true) {
			memset(&line[0], 0, sizeof(line));
			fflush(stdout);
			if (!fgets(line, INPUTBUFFER, stdin))
				continue;

			if (line[0] == '\n')
				continue;

			if (!strncmp(line, "isready", 7)) {
				print_console("readyok\n");
				continue;
			}
			else if (!strncmp(line, "position", 8)) {
				ParsePosition(line);
			}
			else if (!strncmp(line, "ucinewgame", 10)) {
				ParsePosition((char*)"position startpos\n");
			}
			else if (!strncmp(line, "go", 2)) {
				print_console("Seen Go..\n");
				ParseGo(line);
			}
			else if (!strncmp(line, "quit", 4)) {
				_pSearchInfo->quit = true;
				break;
			}
			else if (!strncmp(line, "uci", 3)) {
				print_console("id name %s\n", (char*)ENGINE_NAME);
				print_console("id author %s\n", (char*)ENGINE_AUTHOR);
				print_console("uciok\n");
			}
			else if (!strncmp(line, "debug", 4)) {
				pTaskManager->putNewMessage(
					pChessboard->GenThreadMessage(ThreadPerftTest)
				);
				break;
			}
			else if (!strncmp(line, "setoption name Threads value ", 29)) {
				sscanf_s(line, "%*s %*s %*s %*s %d", &T);
				if (T < 4) T = 4;
				if (T > MAX_THREAD) T = MAX_THREAD;
				print_console("Set Threads to %d\n", T);
				SetupThreadsAndMemory(getMemorySize(MemorySize, T), T);
			}
			else if (!strncmp(line, "setoption name Hash value ", 26)) {
				sscanf_s(line, "%*s %*s %*s %*s %d", &MB);
				if (MB < 4) MB = 4;
				if (MB > MAX_HASHTABLE_MB) MB = MAX_HASHTABLE_MB;
				print_console("Set Hash to %d MB\n", MB);
				SetupThreadsAndMemory(getMemorySize(MB, ThreadNum), ThreadNum);
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
				break;
			}

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
		shutdown();

		ThreadNum = _ThreadNum;
		MemorySize = _MemorySize;

		init();
	}

	void shutdown(void) {
		pTaskManager->Shutdown();
		destroyHashTable();
		_ShutdownFrameMemorySystem();
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

		if (ptrChar != NULL) {
			ptrChar += 6;
			while (*ptrChar) {
				move = ParseMove(ptrChar);
				if (move == 0ULL) break;
				pChessboard->doMove(&move);
				while (*ptrChar && *ptrChar != ' ') ptrChar++;
				ptrChar++;
			}
		}
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
		list.count = pChessboard->genMove(list.MovePtr) - list.MovePtr;
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

