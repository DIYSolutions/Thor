#pragma once
#include "bitboard.h"
constexpr U64 NOMOVE = 0ULL;

typedef struct S_MOVE {
	U64 Move = 0ULL;
	BoardValue Score = 0;
} S_MOVE;

typedef struct S_MOVELIST {
	S_MOVE* MovePtr = nullptr;
	short count = 0;
} S_MOVELIST;

/*
MOVE - U64 == 8 Byte == 64 Bits
0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0011 1111 -> From - 6 bits - 64 values possible / 64 needed
0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 1111 1100 0000 -> To - 6 bits - 64 values possible / 64 needed
0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 1111 0000 0000 0000 -> Moved Piece - 4 bits - 16 values possible / 13 needed
0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 1111 0000 0000 0000 0000 -> Captured Piece - 4 bits - 16 values possible / 13 needed
0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 1111 0000 0000 0000 0000 0000 -> Promoted Piece - 4 bits - 16 values possible / 13 needed
0000 0000 0000 0000 0000 0000 0000 0000 0000 1111 0000 0000 0000 0000 0000 0000 -> CastlePerm - 4 bits - 16 values possible / 15 needed
0000 0000 0000 0000 0000 0000 0000 0000 0111 0000 0000 0000 0000 0000 0000 0000 -> Mode - 3 bits - 8 values possible / 7 needed
0000 0000 0000 0000 0000 0000 0011 1111 1000 0000 0000 0000 0000 0000 0000 0000 -> FiftyMove - 7 bits - 128 values possible / 101 needed
0000 0000 0000 0000 0001 1111 1100 0000 0000 0000 0000 0000 0000 0000 0000 0000 -> EnPas - 7 bits - 128 values possible / 65 needed
1111 1111 1111 1111 1110 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 -> unused bits - 19 bits
*/

constexpr inline short MOVE_FROMSQ(const U64 m) { return  (short)(m & 0x3f); }
constexpr inline short MOVE_TOSQ(const U64 m) { return  (short)((m & 0xfc0) >> 6); }
constexpr inline short MOVE_CAPTURED(const U64 m) { return  (short)((m & 0xf000) >> 12); }
constexpr inline short MOVE_PROMOTED(const U64 m) { return  (short)((m & 0xf0000) >> 16); }
constexpr inline short MOVE_MOVED(const U64 m) { return  (short)((m & 0xf00000) >> 20); }
constexpr inline short MOVE_CASTLE_PERM(const U64 u) { return  (short)((u & 0xf000000) >> 24); }
constexpr inline short MOVE_MODE(const U64 m) { return  (short)((m & 0x70000000) >> 28); }
constexpr inline short MOVE_FIFTY_MOVE(const U64 u) { return  (short)((u & 0x3f80000000) >> 31); }
constexpr inline short MOVE_ENPAS(const U64 u) { return (short)((u & 0x1fc000000000) >> 38); }

constexpr inline U64 MOVE_NEW_PREMOVE_U64(const short Mode, const short CastlePerm, const short FiftyMove, const short enPas) {
	return 0ULL |
		((((U64)CastlePerm) << 24) & 0xf000000) |
		((((U64)Mode) << 28) & 0x70000000) |
		((((U64)FiftyMove) << 31) & 0x3f80000000) |
		((((U64)enPas) << 38) & 0x1fc000000000);
}

constexpr inline U64 MOVE_NEW_U64(const short FromSQ, const short ToSQ, const short Moved, const short Captured, const short Promoted, const U64 PreMove) {
	return PreMove |
		(((U64)FromSQ) & 0x3f) |
		((((U64)ToSQ) << 6) & 0xfc0) |
		((((U64)Captured) << 12) & 0xf000) |
		((((U64)Promoted) << 16) & 0xf0000) |
		((((U64)Moved) << 20) & 0xf00000);
}

inline S_MOVE* NEW_MOVE(S_MOVE* MovePtr, const U64 Move, const BoardValue Score) {
	MovePtr->Move = Move;
	MovePtr->Score = Score;
	return ++MovePtr;
}

inline S_MOVE* NEW_MOVE(S_MOVE* MovePtr, const short FromSQ, const short ToSQ, const short Moved, const short Captured, const short Promoted, const U64 PreMove, const BoardValue Score) {
	MovePtr->Move = MOVE_NEW_U64(FromSQ, ToSQ, Moved, Captured, Promoted, PreMove);
	MovePtr->Score = Score;
	if (Captured != Empty) {
		BoardValue MaxImprovement = MAX_INFINTE - MovePtr->Score;
		BoardValue Improvement = 1000;
		switch (Captured) {
		case WhitePawn:
		case BlackPawn:
			Improvement += PIECES_CAPTURE_VALUES[Moved] * PIECES_VALUES_ABS[WhitePawn];
			break;
		case WhiteKnight:
		case BlackKnight:
			Improvement += PIECES_CAPTURE_VALUES[Moved] * PIECES_VALUES_ABS[WhiteKnight];
			break;
		case WhiteBishop:
		case BlackBishop:
			Improvement += PIECES_CAPTURE_VALUES[Moved] * PIECES_VALUES_ABS[WhiteBishop];
			break;
		case WhiteRook:
		case BlackRook:
			Improvement += PIECES_CAPTURE_VALUES[Moved] * PIECES_VALUES_ABS[WhiteRook];
			break;
		case WhiteQueen:
		case BlackQueen:
			Improvement += PIECES_CAPTURE_VALUES[Moved] * PIECES_VALUES_ABS[WhiteQueen];
			break;
		}
		if (MaxImprovement > Improvement)
			MovePtr->Score += Improvement;
		else
			MovePtr->Score = MAX_INFINTE;
	}
	if (Promoted != Empty) {
		BoardValue MaxImprovement = MAX_INFINTE - MovePtr->Score;
		BoardValue Improvement = 1000;

		switch (Promoted) {
		case WhiteKnight:
		case BlackKnight:
			Improvement += PIECES_VALUES_ABS[WhiteKnight];
			break;
		case WhiteBishop:
		case BlackBishop:
		case WhiteRook:
		case BlackRook:
			break;
		case WhiteQueen:
		case BlackQueen:
			Improvement += PIECES_VALUES_ABS[WhiteQueen];
			break;
		}

		if (MaxImprovement > Improvement)
			MovePtr->Score += Improvement;
		else
			MovePtr->Score = MAX_INFINTE;
	}
	return ++MovePtr;
}

constexpr bool IsRook(const short Type) {
	if (Type == WhiteRook || Type == BlackRook)
		return true;
	return false;
}
constexpr bool IsBishop(const short Type) {
	if (Type == WhiteBishop || Type == BlackBishop)
		return true;
	return false;
}
constexpr bool IsQueen(const short Type) {
	if (Type == WhiteQueen || Type == BlackQueen)
		return true;
	return false;
}
constexpr bool IsKnight(const short Type) {
	if (Type == WhiteKnight || Type == BlackKnight)
		return true;
	return false;
}

constexpr inline short getEnPasSQWhite(short sq) {
	return sq - 8;
}

constexpr inline short getEnPasSQBlack(short sq) {
	return sq + 8;
}

template <Colors Us>
inline const U64 getPawnAttackBoard_reverse(const short sq) {}

template <>
inline const U64 getPawnAttackBoard_reverse<WHITE>(const short sq) { return AttackBrdbPawnBB[sq]; }

template <>
inline const U64 getPawnAttackBoard_reverse<BLACK>(const short sq) { return AttackBrdwPawnBB[sq]; }

template <Colors Us>
inline const U64 getPawnAttackBoard(const short sq) {}

template <>
inline const U64 getPawnAttackBoard<WHITE>(const short sq) { return AttackBrdwPawnBB[sq]; }

template <>
inline const U64 getPawnAttackBoard<BLACK>(const short sq) { return AttackBrdbPawnBB[sq]; }

template <Colors Us>
inline const U64 getPawnMoveBoard_reverse(const short sq) {}

template <>
inline const U64 getPawnMoveBoard_reverse<WHITE>(const short sq) { return SetMask[sq - 8]; }

template <>
inline const U64 getPawnMoveBoard_reverse<BLACK>(const short sq) { return SetMask[sq + 8]; }

template <Colors Us>
inline const U64 getPawnMoveBoard(const short sq) {}

template <>
inline const U64 getPawnMoveBoard<WHITE>(const short sq) { return SetMask[sq + 8]; }

template <>
inline const U64 getPawnMoveBoard<BLACK>(const short sq) { return SetMask[sq - 8]; }

template <Colors Us>
inline const U64 getPawnDoubleMoveBoard_reverse(const short sq) {}

template <>
inline const U64 getPawnDoubleMoveBoard_reverse<WHITE>(const short sq) { return SetMask[sq - 16]; }

template <>
inline const U64 getPawnDoubleMoveBoard_reverse<BLACK>(const short sq) { return SetMask[sq + 16]; }

template <Colors Us>
inline const U64 getPawnDoubleMoveBoard(const short sq) {}

template <>
inline const U64 getPawnDoubleMoveBoard<WHITE>(const short sq) { return SetMask[sq + 16]; }

template <>
inline const U64 getPawnDoubleMoveBoard<BLACK>(const short sq) { return SetMask[sq - 16]; }

template <Colors Us>
inline const short getPawnMoveSQ(const short sq) {}

template <>
inline const short getPawnMoveSQ<WHITE>(const short sq) { return sq + 8; }

template <>
inline const short getPawnMoveSQ<BLACK>(const short sq) { return sq - 8; }

template <Colors Us>
inline const short getPawnDoubleMoveSQ(const short sq) {}

template <>
inline const short getPawnDoubleMoveSQ<WHITE>(const short sq) { return sq + 16; }

template <>
inline const short getPawnDoubleMoveSQ<BLACK>(const short sq) { return sq - 16; }

inline U64 getWhitePiecesBB(const U64* PiecesBB) {
	return PiecesBB[WhitePawn] | PiecesBB[WhiteKnight] | PiecesBB[WhiteBishop] | PiecesBB[WhiteRook] | PiecesBB[WhiteQueen] | PiecesBB[WhiteKing];
}

inline U64 getBlackPiecesBB(const U64* PiecesBB) {
	return PiecesBB[BlackPawn] | PiecesBB[BlackKnight] | PiecesBB[BlackBishop] | PiecesBB[BlackRook] | PiecesBB[BlackQueen] | PiecesBB[BlackKing];
}

inline const short getWhitePieceType(const U64* PiecesBB, const short sq) {
	U64 maskBB = SetMask[sq];
	if (PiecesBB[WhitePawn] & maskBB)
		return WhitePawn;
	if (PiecesBB[WhiteKnight] & maskBB)
		return WhiteKnight;
	if (PiecesBB[WhiteBishop] & maskBB)
		return WhiteBishop;
	if (PiecesBB[WhiteRook] & maskBB)
		return WhiteRook;
	if (PiecesBB[WhiteQueen] & maskBB)
		return WhiteQueen;
	if (PiecesBB[WhiteKing] & maskBB)
		return WhiteKing;
	return Empty;
}

inline const short getBlackPieceType(const U64* PiecesBB, const short sq) {
	U64 maskBB = SetMask[sq];
	if (PiecesBB[BlackPawn] & maskBB)
		return BlackPawn;
	if (PiecesBB[BlackKnight] & maskBB)
		return BlackKnight;
	if (PiecesBB[BlackBishop] & maskBB)
		return BlackBishop;
	if (PiecesBB[BlackRook] & maskBB)
		return BlackRook;
	if (PiecesBB[BlackQueen] & maskBB)
		return BlackQueen;
	if (PiecesBB[BlackKing] & maskBB)
		return BlackKing;
	return Empty;
}

inline void swap(S_MOVE* arr, const short a, const short b)
{
	S_MOVE t = arr[a];
	arr[a] = arr[b];
	arr[b] = t;
}

/* This function takes last element as pivot, places
   the pivot element at its correct position in sorted
	array, and places all smaller (smaller than pivot)
   to left of pivot and all greater elements to right
   of pivot */
inline short partition(S_MOVE * arr, short low, short high)
{
	S_MOVE pivot = arr[high];    // pivot 
	int i = (low - 1);  // Index of smaller element 

	for (int j = low; j <= high - 1; j++)
	{
		// If current element is smaller than or 
		// equal to pivot 
		if (arr[j].Score >= pivot.Score)
		{
			i++;    // increment index of smaller element 
			swap(arr, i, j);
		}
	}
	swap(arr,i + 1,high);
	return (i + 1);
}

/* The main function that implements QuickSort
 arr --> Array to be sorted,
  low  --> Starting index,
  high  --> Ending index */
inline void QuickSort(S_MOVE * arr, short low, short high)
{
	if (low < high)
	{
		/* pi is partitioning index, arr[p] is now
		   at right place */
		short pi = partition(arr, low, high);

		// Separately sort elements before 
		// partition and after partition 
		QuickSort(arr, low, pi - 1);
		QuickSort(arr, pi + 1, high);
	}
}


constexpr BoardValue PawnTable[64] = {
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,
10	,	10	,	0	,	-10	,	-10	,	0	,	10	,	10	,
5	,	0	,	0	,	5	,	5	,	0	,	0	,	5	,
0	,	0	,	10	,	20	,	20	,	10	,	0	,	0	,
5	,	5	,	5	,	10	,	10	,	5	,	5	,	5	,
10	,	10	,	10	,	20	,	20	,	10	,	10	,	10	,
20	,	20	,	20	,	30	,	30	,	20	,	20	,	20	,
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0
};

constexpr BoardValue KnightTable[64] = {
0	,	-10	,	0	,	0	,	0	,	0	,	-10	,	0	,
0	,	0	,	0	,	5	,	5	,	0	,	0	,	0	,
0	,	0	,	10	,	10	,	10	,	10	,	0	,	0	,
0	,	0	,	10	,	20	,	20	,	10	,	5	,	0	,
5	,	10	,	15	,	20	,	20	,	15	,	10	,	5	,
5	,	10	,	10	,	20	,	20	,	10	,	10	,	5	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0
};

constexpr BoardValue BishopTable[64] = {
0	,	0	,	-10	,	0	,	0	,	-10	,	0	,	0	,
0	,	0	,	0	,	10	,	10	,	0	,	0	,	0	,
0	,	0	,	10	,	15	,	15	,	10	,	0	,	0	,
0	,	10	,	15	,	20	,	20	,	15	,	10	,	0	,
0	,	10	,	15	,	20	,	20	,	15	,	10	,	0	,
0	,	0	,	10	,	15	,	15	,	10	,	0	,	0	,
0	,	0	,	0	,	10	,	10	,	0	,	0	,	0	,
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0
};

constexpr BoardValue RookTable[64] = {
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0
};

constexpr BoardValue QueenTable[64] = {
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	5	,	10	,	15	,	15	,	10	,	5	,	0	,
0	,	0	,	5	,	20	,	20	,	5	,	0	,	0	,
0	,	0	,	5	,	20	,	20	,	5	,	0	,	0	,
5	,	10	,	15	,	15	,	15	,	15	,	10	,	5	,
25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0
};

constexpr BoardValue KingTableEndgame[64] = {
	-50	,	-10	,	0	,	0	,	0	,	0	,	-10	,	-50	,
	-10,	0	,	10	,	10	,	10	,	10	,	0	,	-10	,
	0	,	10	,	20	,	20	,	20	,	20	,	10	,	0	,
	0	,	10	,	20	,	40	,	40	,	20	,	10	,	0	,
	0	,	10	,	20	,	40	,	40	,	20	,	10	,	0	,
	0	,	10	,	20	,	20	,	20	,	20	,	10	,	0	,
	-10,	0	,	10	,	10	,	10	,	10	,	0	,	-10	,
	-50	,	-10	,	0	,	0	,	0	,	0	,	-10	,	-50
};

constexpr BoardValue KingTableOpening[64] = {
	0	,	5	,	5	,	-10	,	-10	,	0	,	10	,	5	,
	-30	,	-30	,	-30	,	-30	,	-30	,	-30	,	-30	,	-30	,
	-50	,	-50	,	-50	,	-50	,	-50	,	-50	,	-50	,	-50	,
	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,
	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,
	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,
	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,
	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70	,	-70
};


template <Colors Us>
inline bool IsOpening(const U64* piecesListBB) {
	constexpr bool WhiteToMove = Us == WHITE;
	constexpr U64 KingOpening = WhiteToMove ? SetMask[E1] : SetMask[E8];
	constexpr U64 QueenOpening = WhiteToMove ? SetMask[D1] : SetMask[D8];
	constexpr U64 RookOpening = WhiteToMove ? SetMask[A1] | SetMask[H1] : SetMask[A8] | SetMask[H8];
	constexpr U64 BishopOpening = WhiteToMove ? SetMask[C1] | SetMask[F1] : SetMask[C8] | SetMask[F8];
	constexpr U64 KnightOpening = WhiteToMove ? SetMask[B1] | SetMask[G1] : SetMask[B8] | SetMask[G8];
	if (WhiteToMove) {
		if (CountBits(
			(piecesListBB[WhiteKing] & KingOpening) |
			(piecesListBB[WhiteQueen] & QueenOpening) |
			(piecesListBB[WhiteRook] & RookOpening) |
			(piecesListBB[WhiteBishop] & BishopOpening) |
			(piecesListBB[WhiteKnight] & KnightOpening)
		) > 1)
			return true;
	}
	else {
		if (CountBits(
			(piecesListBB[BlackKing] & KingOpening) |
			(piecesListBB[BlackQueen] & QueenOpening) |
			(piecesListBB[BlackRook] & RookOpening) |
			(piecesListBB[BlackBishop] & BishopOpening) |
			(piecesListBB[BlackKnight] & KnightOpening)
		) > 1)
			return true;
	}
	return false;
}

inline bool IsEndgame(const U64* piecesListBB) {
	if (CountBits((getWhitePiecesBB(piecesListBB) | getBlackPiecesBB(piecesListBB)) & ~(piecesListBB[WhitePawn] | piecesListBB[BlackPawn])) >= 8)
		return false;

	return false;
}

constexpr BoardValue AttackFaktor = 50;
constexpr BoardValue MobilityFaktor = 10;
