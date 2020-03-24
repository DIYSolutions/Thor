#include <iostream>
#include "UCI.h"
int main()
{
	/*
	InitOutputLock();
	InitMessageContainer();
	InitSearchinfo();

	if (!_InitFrameMemorySystem<FrameInit>(getMemorySize(STD_HASHTABLE_MB, STD_THREAD), 4))
		error_exit("UCI: _InitFrameMemorySystem<FrameInit> failed!");;

	char fen[256] = "8/Pk6/8/8/8/8/5K2/4q3 w - - 0 1";
	MemoryBlock * pMemory = new MemoryBlock();
	pMemory->init(THREAD_MEMORY_SIZE, ThreadHeap);
	Chessboard * pChessboard = newChessboard(pMemory);

	pChessboard->ParseFEN(fen);

	S_MemoryFrame PerftMemoryFrame = pMemory->GetMemoryFrame(ThreadHeap);
	S_MOVE* MovePtr = (S_MOVE*)pMemory->AllocFrameMemory(sizeof(MovePtr) * BOARD_MAX_MOVES);
	short MoveCount = pChessboard->GenMove<WHITE>(MovePtr) - MovePtr;

	for (int i = 0; i < MoveCount; i++) {
		std::cout << (i + 1) << ": " << field_names[MOVE_FROMSQ(MovePtr[i].Move)] << field_names[MOVE_TOSQ(MovePtr[i].Move)] << " \n-  Mode: " << getModeName(MOVE_MODE(MovePtr[i].Move)) << " \n-  Moved: " << getPieceName(MOVE_MOVED(MovePtr[i].Move)) << " \n-  Captured: " << getPieceName(MOVE_CAPTURED(MovePtr[i].Move)) << " \n-  Promoted: " << getPieceName(MOVE_PROMOTED(MovePtr[i].Move)) << std::endl;
	}
	*/
	(new UCI())->uci_loop();
}