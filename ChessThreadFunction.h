#pragma once
#include "basic.h"


#define LMR_LIMIT = 5
static short PickReduction(short Depth, short MoveNr) { 
	if (Depth > MoveNr)
		return MoveNr;
	return Depth;
}
static bool IsInteresting(S_MOVE * MovePtr, BoardValue * BestScore) { 
	switch (MOVE_MODE(MovePtr->Move)) {
	case Capture:
	case PromoteCapture:
	case Promote:
	case EnPassant:
		return true;
	}
	if (MovePtr->Score >= * BestScore)
		return true;

	return false;
}

/*
	these functions should not store variables in the stack as they are called recursively from multiple threads
*/


static void StopSearch(S_Splitpoint* pSplitpoint) {
	while (pSplitpoint->lock.test_and_set(std::memory_order_acquire)) // acquire lock
		; // spin
	S_Threadlist* threadlistnode = pSplitpoint->Slaves_RootNode.next;
	while (threadlistnode) {
		threadlistnode->current->StopSearch(pSplitpoint);
		threadlistnode = threadlistnode->next;
	}
	pSplitpoint->lock.clear(std::memory_order_release);
}

template <Colors Us>
static BoardValue Search(ChessThread* pThread, Chessboard* pChessboard, MemoryBlock* pMemory, const short Depth, BoardValue Alpha, BoardValue Beta) {
	constexpr Them = Us == WHITE ? BLACK : WHITE;
	if (Depth <= 0) return Quiescence<Us>(pThread,  pChessboard, pMemory, Alpha, Beta);
	// i am the master of this node
	// probe hash
	BoardValue BestScore = MIN_INFINTE;
	U64 BestMove;
	if (HASHTABLE->ProbeHashEntry(pChessboard->getPosKey(), &BestScore, Depth)) {
		// if bestmove for given Depth => return that value
		return BestScore;
	}	
	// generate Splitpoint
	S_MemoryFrame MemoryFrame = pMemory->GetMemoryFrame(ThreadHeap);
	S_Splitpoint* pSplitpoint = (S_Splitpoint*)pMemory->AllocFrameMemory(sizeof(S_Splitpoint));
	pSplitpoint->Master = pThread;
	pSplitpoint->Alpha = Alpha;
	pSplitpoint->Beta = Beta;
	// generate moves
	pSplitpoint->Moves.count = pChessboard->GenMove<Us>(pSplitpoint->Moves.MovePtr);
	// for loop - loop over moves
	for (short MoveNr = 0; MoveNr < pSplitpoint->Moves.count; MoveNr++) {
		// if MoveNr > 0
		if (Depth > 4 && MoveNr > 0) {
			if (pThread->IsStopped()) {
				StopSearch(pSplitpoint);
				while (pSplitpoint->SlaveCount > 0);
				pMemory->ReleaseMemoryFrame(&MemoryFrame);
				return DRAW_VALUE;
			}
			//   => test to give this move to an idle thread
			ChessThread* pThread = GetIdleThread();
			if (pThread) {
				S_ThreadMessage* pMessage = (S_ThreadMessage*)pMemory->AllocFrameMemory(sizeof(S_ThreadMessage));
				pMessage->MoveNr = MoveNr;
				pMessage->pSplitpoint = pSplitpoint;
				if(pThread->putNewMessage( pMessage ));
					continue;
			}
			//   if there is no one => keep searching myself
		}// else search first move

		
		pChessboard->DoMove<Us>(pSplitpoint->Moves.MovePtr[MoveNr].Move);

		if (MoveNr < LMR_LIMIT || IsInteresting(&pSplitpoint->Moves.MovePtr[MoveNr], &BestScore))
			pSplitpoint->Moves.MovePtr[MoveNr].Score = -Search(pThread, pChessboard, pMemory, Depth - 1, -Beta, -Alpha);
		else {
			pSplitpoint->Moves.MovePtr[MoveNr].Score = -Search(pThread, pChessboard, pMemory, Depth - PickReduction(Depth,MoveNr), -Beta, -Alpha);
			if(pSplitpoint->Moves.MovePtr[MoveNr].Score > Alpha)
				pSplitpoint->Moves.MovePtr[MoveNr].Score = -Search(pThread, pChessboard, pMemory, Depth - 1, -Beta, -Alpha);
		}
		
		pChessboard->UndoMove<Us>();

		// if value < Beta
		if (pSplitpoint->Moves.MovePtr[MoveNr].Score >= Beta || pThread->IsStopped()) {
			Beta = pSplitpoint->Moves.MovePtr[MoveNr].Score;
			// tell threads to stop
			StopSearch(pSplitpoint);

			while (pSplitpoint->SlaveCount > 0);
			// save to hash - Beta
			if (!pThread->IsStopped())
				HASHTABLE->SaveHashEntry(pChessboard->getPosKey(), HFBETA, Beta, Depth, pSplitpoint->Moves.MovePtr[MoveNr].Move);
			
			pMemory->ReleaseMemoryFrame(&MemoryFrame);
			return Beta;
		}

		if (pSplitpoint->Moves.MovePtr[MoveNr].Score > BestScore) {
			BestScore = pSplitpoint->Moves.MovePtr[MoveNr].Score;
			BestMove = pSplitpoint->Moves.MovePtr[MoveNr].Move;
			if (BestScore > Alpha)
				Alpha = BestScore;
		}
	}

	while (pSplitpoint->SlaveCount > 0) {
		if (pThread->IsStopped()) {
			StopSearch(pSplitpoint);
			while (pSplitpoint->SlaveCount > 0);
			pMemory->ReleaseMemoryFrame(&MemoryFrame);
			return DRAW_VALUE;
		}
		IdleThread(pThread, nullptr);
	}
	// sort moves
	// save to hash - Alpha
	HASHTABLE->SaveHashEntry(pChessboard->getPosKey(), HFALPHA, &pSplitpoint->Moves.MovePtr[0], Depth);
	pMemory->ReleaseMemoryFrame(&MemoryFrame);
	return BestScore;
}

template <Colors Us>
static void Splitpointsearch(ChessThread* pThread, MemoryBlock* pMemory, S_Splitpoint* pSplitpoint, Chessboard* pChessboard, const short MoveNr) {
	constexpr Them = Us == WHITE ? BLACK : WHITE;
	pChessboard->SetBoard(pSplitpoint->PiecesBB, pSplitpoint->CastlePerm, pSplitpoint->EnPas, pSplitpoint->FiftyMove, pSplitpoint->PosKey, pSplitpoint->Side );
	pChessboard->DoMove<Us>(pSplitpoint->Moves.MovePtr[MoveNr].Move);
	pSplitpoint->Moves.MovePtr[MoveNr].Score = -Search<Them>(pChessboard, pSplitpoint->Depth, pSplitpoint->Alpha, pSplitpoint->Beta);
}

static void Splitpointsearch(ChessThread* pThread, MemoryBlock* pMemory, S_Splitpoint* pSplitpoint, Chessboard * pChessboard, const short MoveNr) {
	S_Threadlist threadlistnode;
	threadlistnode.current = pThread;
	while (pSplitpoint->lock.test_and_set(std::memory_order_acquire)) // acquire lock
		; // spin

	pSplitpoint->SlaveCount++;
	if (!pSplitpoint->Slaves_RootNode.next)
		pSplitpoint->Slaves_RootNode.next = &threadlistnode;
	else {
		threadlistnode.next = pSplitpoint->Slaves_RootNode.next;
		threadlistnode.next->last = pSplitpoint->Slaves_RootNode.next = &threadlistnode;
	}
	pSplitpoint->lock.clear(std::memory_order_release);

	if(pChessboard->SideToMove() == WHITE)
		Splitpointsearch<WHITE>(pThread, pSplitpoint, pMemory, pChessboard, MoveNr);
	else
		Splitpointsearch<BLACK>(pThread, pSplitpoint, pMemory, pChessboard, MoveNr);

	while (pSplitpoint->lock.test_and_set(std::memory_order_acquire)) // acquire lock
		; // spin
	if (threadlistnode.next)
		threadlistnode.next->last = threadlistnode.last;
	if (threadlistnode.last)
		threadlistnode.last->next = threadlistnode.next;
	pSplitpoint->SlaveCount--;
	pSplitpoint->lock.clear(std::memory_order_release);
}
