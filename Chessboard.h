#pragma once
#include "bitboard_magic.h"

class Chessboard
{
private:
	short Side;
	short EnPas;
	short FiftyMove;
	short CastlePerm;

	U64 PosKey;

	short Ply;
	short HisPly;
	U64 History[GAME_MAX_MOVES];
	U64 PiecesBB[12];

	inline void NewPiece(const short Type, const short SQ) { 
		SetBit(&PiecesBB[Type], SQ); 
	}
	inline void DelPiece(const short Type, const short SQ) { 
		ClearBit(&PiecesBB[Type], SQ); 
	}
	inline void MovePiece(const short Type, const short fromSQ, const short toSQ) { 
		MoveBit(&PiecesBB[Type], fromSQ, toSQ); 
	}

	template <Colors Us>
	inline void DoMove(U64* Move) {}

	template <Colors Us>
	inline void UndoMove() {}

	template <Colors Us>
	inline U64* GenMove(U64* MovePtr) {}

public:
	Chessboard() {
		for (int i = WhitePawn; i < Empty; i++)
			PiecesBB[i] = 0ULL;
		PosKey = 0ULL;
	}

	inline void DoMove(U64* Move) {
		if (Side == WHITE)
			return DoMove<WHITE>(Move);
		return DoMove<BLACK>(Move);
	}
	inline void UndoMove() {
		if (Side == WHITE)
			return UndoMove<WHITE>();
		return UndoMove<BLACK>();
	}
	inline U64* GenMove(U64* MovePtr) {
		if (Side == WHITE)
			return GenMove<WHITE>(MovePtr);
		return GenMove<BLACK>(MovePtr);
	}

	inline const U64 getPosKey(void) { return PosKey; }
};

