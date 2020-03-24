#pragma once

#include "Chessboard.h"
#include "MemoryBlock.h"
#include "ThreadManager.h"

#include "perfttest.h"

#include <stdlib.h>
#include <iostream>
#include <string>
#include <fstream>
using namespace std;
#include <cstdint>

void MakeThreadAvailable(S_Splitpoint* Splitpoint, Thread* pThread) {
	while (Splitpoint->lock.test_and_set(std::memory_order_acquire)) // acquire lock
		; // spin
	S_Threadlist* listnode = Splitpoint->Slaves_RootNode.next;
	while (listnode) {
		thread_manager->NewIdleThread(pThread, listnode->current->getID());
		listnode = listnode->next;
	}
	Splitpoint->lock.clear(std::memory_order_release); // release lock
}

void EnterSplitpoint(S_Splitpoint* Splitpoint, S_Threadlist* listnode) {
	while (Splitpoint->lock.test_and_set(std::memory_order_acquire)) // acquire lock
		; // spin
	Splitpoint->SlaveCount = Splitpoint->SlaveCount + 1;
	if (Splitpoint->Slaves_RootNode.next == nullptr) {
		Splitpoint->Slaves_RootNode.next = listnode;
	}else {
		listnode->next = Splitpoint->Slaves_RootNode.next;
		Splitpoint->Slaves_RootNode.next = listnode;
		listnode->next->last = listnode;
	}
	Splitpoint->lock.clear(std::memory_order_release); // release lock
}
void LeaveSplitpoint(S_Splitpoint* Splitpoint, S_Threadlist* listnode, U64 nodes) {
	while (Splitpoint->lock.test_and_set(std::memory_order_acquire)) // acquire lock
		; // spin
	if (listnode->last)
		listnode->last->next = listnode->next;
	else
		Splitpoint->Slaves_RootNode.next = listnode->next;

	if (listnode->next)
		listnode->next->last = listnode->last;

	Splitpoint->SlaveCount = Splitpoint->SlaveCount - 1;
	Splitpoint->Nodes = Splitpoint->Nodes + nodes;
	Splitpoint->searched = Splitpoint->searched + 1;
	Splitpoint->lock.clear(std::memory_order_release); // release lock
}


template <Colors Us>
inline U64 PerftTest(MemoryBlock* pMemory, Chessboard* pChessboard, const short Depth) {
	constexpr Colors Them = Us == WHITE ? BLACK : WHITE;
	if (Depth) {
		U64 nodes = 0;
		S_MemoryFrame PerftMemoryFrame = pMemory->GetMemoryFrame(ThreadHeap);
		S_MOVE* MovePtr = (S_MOVE*)pMemory->AllocFrameMemory(sizeof(S_MOVE) * BOARD_MAX_MOVES);
		short MoveCount = pChessboard->GenMove<Us>(MovePtr) - MovePtr;
		for (short i = 0; i < MoveCount; i++) {
			pChessboard->DoMove<Us>(MovePtr[i].Move);
			nodes += PerftTest<Them>(pMemory, pChessboard, Depth - 1);
			pChessboard->UndoMove<Us>();
		}
		pMemory->ReleaseMemoryFrame(&PerftMemoryFrame);
		return nodes;
	}
	else {
		return 1ULL;
	}
}
template <Colors Us>
inline U64 MultiPerftTest(Thread* pMasterThread, Thread* pThread, MemoryBlock* pMemory, Chessboard* pChessboard, const short Depth);

void NodeCounter(Thread* pThread, void* data) {
	S_SplitpointMessage* SplitpointMessage = (S_SplitpointMessage*)data;
	S_Splitpoint* Splitpoint = SplitpointMessage->pSplitpoint;
	S_Threadlist listnode;
	listnode.current = pThread;
	pThread->getChessboard()->SetBoard(Splitpoint);
	EnterSplitpoint(Splitpoint, &listnode);

	U64 nodes;
	if (pThread->getChessboard()->SideToMove() == WHITE) {
		pThread->getChessboard()->DoMove<WHITE>(Splitpoint->Moves.MovePtr[SplitpointMessage->MoveNr].Move);
		nodes = MultiPerftTest<BLACK>(Splitpoint->Master, pThread, pThread->getMemory(), pThread->getChessboard(), Splitpoint->Depth);
	}
	else {
		pThread->getChessboard()->DoMove<BLACK>(Splitpoint->Moves.MovePtr[SplitpointMessage->MoveNr].Move);
		nodes = MultiPerftTest<WHITE>(Splitpoint->Master, pThread, pThread->getMemory(), pThread->getChessboard(), Splitpoint->Depth);
	}

	LeaveSplitpoint(Splitpoint, &listnode, nodes);
}


template <Colors Us>
inline U64 MultiPerftTest(Thread* pMasterThread, Thread* pThread, MemoryBlock* pMemory, Chessboard* pChessboard, const short Depth) {
	constexpr Colors Them = Us == WHITE ? BLACK : WHITE;
	if (Depth) {
		S_MemoryFrame PerftMemoryFrame = pMemory->GetMemoryFrame(ThreadHeap);
		U64 nodes = 0;
		if (Depth > 3) {
			S_Splitpoint thisSplitpoint;
			thisSplitpoint.Depth = Depth - 1;
			thisSplitpoint.Master = pThread;
			
			thisSplitpoint.Moves.MovePtr = (S_MOVE*)pMemory->AllocFrameMemory(sizeof(S_MOVE) * BOARD_MAX_MOVES);
			thisSplitpoint.Moves.count = pChessboard->GenMove<Us>(thisSplitpoint.Moves.MovePtr) - thisSplitpoint.Moves.MovePtr;
			thisSplitpoint.lock.clear();
			thisSplitpoint.Nodes = thisSplitpoint.searched = thisSplitpoint.SlaveCount = 0;
			pChessboard->setSplitpoint(&thisSplitpoint);
			short searched = 0;
			for (short i = 0; i < thisSplitpoint.Moves.count; i++) {
				S_SplitpointMessage* msg = (S_SplitpointMessage*)pMemory->AllocFrameMemory(sizeof(S_SplitpointMessage));
				msg->MoveNr = i;
				msg->pSplitpoint = &thisSplitpoint;

				Thread* worker = thread_manager->GetIdleThread(pThread);
				if (worker)
					if (worker->newMessage(NodeCounter, msg))
						continue;

				if (pMasterThread && pMasterThread->IsIdle())
					if (pMasterThread->newMessage(NodeCounter, msg))
							continue;

				searched++;
				pChessboard->DoMove<Us>(thisSplitpoint.Moves.MovePtr[i].Move);
				nodes += MultiPerftTest<Them>(pMasterThread, pThread, pMemory, pChessboard, thisSplitpoint.Depth);
				pChessboard->UndoMove<Us>();
			}
			if ((thisSplitpoint.searched + searched) != thisSplitpoint.Moves.count) {
				if (pThread) {
					pThread->loop(&thisSplitpoint, searched);
				}else
					while ((thisSplitpoint.searched + searched) != thisSplitpoint.Moves.count);
			}
			nodes += thisSplitpoint.Nodes;
		}else {
			S_MOVE* MovePtr = (S_MOVE*)pMemory->AllocFrameMemory(sizeof(S_MOVE) * BOARD_MAX_MOVES);
			short MoveCount = pChessboard->GenMove<Us>(MovePtr) - MovePtr;
			for (short i = 0; i < MoveCount; i++) {
				pChessboard->DoMove<Us>(MovePtr[i].Move);
				nodes += PerftTest<Them>(pMemory, pChessboard, Depth - 1);
				pChessboard->UndoMove<Us>();
			}
		}

		pMemory->ReleaseMemoryFrame(&PerftMemoryFrame);
		return nodes;
	}
	else {
		return 1ULL;
	}
}


template <Colors Us>
U64 perfttest_thread(MemoryBlock* pMemory, Chessboard* testboard, const short depth) {
	return MultiPerftTest<Us>(nullptr, nullptr, pMemory, testboard, depth);
}

template <Colors Us>
inline U64 PerftTest_detail(MemoryBlock* pMemory, Chessboard* pChessboard, const short Depth) {
	constexpr Colors Them = Us == WHITE ? BLACK : WHITE;
	if (Depth) {
		S_MemoryFrame PerftMemoryFrame = pMemory->GetMemoryFrame(ThreadHeap);
		U64 nodes = 0;
		U64 move_nodes = 0;
		S_MOVE* MovePtr = (S_MOVE*)pMemory->AllocFrameMemory(sizeof(S_MOVE) * BOARD_MAX_MOVES);
		short MoveCount = pChessboard->GenMove<Us>(MovePtr) - MovePtr;
		/*
			probe with perft.exe(by H.G. Muller)
		*/
		if (perft_check(1, pChessboard->genFEN()) != MoveCount) {
			error_exit(pChessboard->genFEN());
		}
		for (short i = 0; i < MoveCount; i++) {
			pChessboard->DoMove<Us>(MovePtr[i].Move);
			move_nodes = PerftTest<Them>(pMemory, pChessboard, Depth - 1);			
			if (perft_check(Depth - 1, pChessboard->genFEN()) != move_nodes) {
				print_console("! ------------------- !");
				error_exit(pChessboard->genFEN());
			}
			nodes += move_nodes;
			pChessboard->UndoMove<Us>();
		}
		pMemory->ReleaseMemoryFrame(&PerftMemoryFrame);
		error_exit("no error found");
		return 0;
	}
	else {
		return 0;
	}
}



void perfttest(MemoryBlock* pMemory, Chessboard* testboard, int depth, bool multi) {
	string line;
	char fen_line[255];
	int fen_len = 0;
	char depth_line[255];
	char temp;

	if (depth < 1) {
		std::cout << "depth " << depth << " is to low, changed to 1" << std::endl;
		depth = 1;
	}
	if (depth > 10) {
		std::cout << "depth " << depth << " is to high, changed to 10" << std::endl;
		depth = 10;
	}


	ifstream myfile("C:\\Users\\Toto\\source\\repos\\DIYSolutions\\Thor\\x64\\Release\\perftsuite.epd");
	int state = 0;
	int depth_state = 0;
	U64 depth_num[10] = { 0,0,0,0,0,0,0,0,0,0 };
	int ii = 0;

	int lines_sum = 0;
	int lines_err = 0;
	int lines_err_num[200];
	int lines_err_depth[200];
	uint64_t dif = 0;
	uint64_t end = 0;
	uint64_t start = 0;
	uint64_t total_time = 0;

	U64 temp_long;
	long double mid_value = 0.0;
	long double min_value = 0.0;
	bool first = true;
	long double max_value = 0.0;
	short movegen_fails = 0;
	bool failed = false;
	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			lines_sum++;
			for (int i = 0; i < line.length(); i++) {
				temp = line[i];
				switch (temp) {
				case 59://==";"
					state++;
					break;
				default:
					if (state == 0) {
						fen_line[i] = line[i];
						if (line[i + 1] == 59) {
							fen_len = i + 1;
							fen_line[fen_len] = 0;
							ii = 0;
						}
					}
					else {
						depth_line[ii++] = line[i];
						if (i + 1 == line.length()) {
							fen_len = ii;
							depth_line[fen_len] = 0;
							depth_num[depth_state] = strtoull(depth_line, NULL, 0);
						}
						else if (line[i + 1] == 59) {
							fen_len = ii;
							depth_line[fen_len] = 0;
							depth_num[depth_state] = strtoull(depth_line, NULL, 0);
							depth_state++;
							ii = 0;
						}

					}
				}
			}
			std::cout << '\n' << lines_sum << ": ";
			std::cout << fen_line << '\n';
			state = 0;
			depth_state = 0;

			if (depth > 6) {
				std::cout << "need more data - retrieving from perft.exe " << std::endl;
				perft_fill_array(depth, depth_num, fen_line);
			}

			std::cout << "test this FEN to depth " << depth << std::endl;
			testboard->ParseFEN(fen_line);
			failed = false;
			for (short i = 1; i <= depth; i++) {
				
				if (multi) {
					start = GetNanoTime();

					if (testboard->SideToMove() == WHITE)
						temp_long = perfttest_thread<WHITE>(pMemory, testboard, i);
					else
						temp_long = perfttest_thread<BLACK>(pMemory, testboard, i);

					//temp_long = perfttest_thread(testboard, i);
					end = GetNanoTime();
				}
				else {
					start = GetNanoTime();

					if (testboard->SideToMove() == WHITE)
						temp_long = PerftTest<WHITE>(pMemory, testboard, i);
					else
						temp_long = PerftTest<BLACK>(pMemory, testboard, i);

					//temp_long = perfttest_thread(testboard, i);
					end = GetNanoTime();
				}

				if (temp_long == 0)
					return;

				std::cout << "depth " << i << ", found " << temp_long << " nodes ";
				dif = end - start;
				std::cout << "(" << (dif / 1000000.0) << "ms) - ";

				long double nmps = 0.000000001 + (temp_long / ((end - start) / 1000000.0));

				std::cout << nmps << " npms - ";
				if (nmps > max_value)
					max_value = nmps;
				if (min_value == 0.0 || min_value > nmps)
					min_value = nmps;

				if (mid_value == 0.0)
					mid_value = nmps;
				else
					mid_value = (mid_value + nmps) / 2;


				if (temp_long == depth_num[i - 1]) {
					std::cout << "ok";
				}
				else {

					std::cout << "FAILED got:" << temp_long << ", expected:" << depth_num[i - 1];
					std::cout << std::endl;
					if (!failed) {
						lines_err_depth[lines_err] = i;
						lines_err_num[lines_err++] = lines_sum;
						failed = true;
					}
					//PrintBoard(testboard);
					//return;
					testboard->ParseFEN(fen_line);
					if (testboard->SideToMove() == WHITE)
						movegen_fails += PerftTest_detail<WHITE>(pMemory, testboard, i);
					else
						movegen_fails += PerftTest_detail<BLACK>(pMemory, testboard, i);

					break;
				}
				std::cout << std::endl;
			}
			std::cout << "done" << std::endl << std::endl;

		}
		myfile.close();
		std::cout << mid_value << " average npms - " << min_value << " min npms - " << max_value << " max npms - ";
		if (lines_err != 0) {
			std::cout << lines_err << " from " << lines_sum << " failed\nthe following failed:" << std::endl;
			for (int i = 0; i < lines_err; i++) {
				std::cout << "LINE:" << lines_err_num[i] << " - depth: " << lines_err_depth[i] << '\n';
			}
			std::cout << "movegen_fails: " << movegen_fails << std::endl;
		}
		else {
			std::cout << " => without errors <=" << std::endl;
		}
	}

	else std::cout << "Unable to open file" << std::endl;

}
