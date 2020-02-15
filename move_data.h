#pragma once
typedef unsigned long long U64;
typedef signed short BoardValue;
#include "bitboard.h"

typedef enum MoveExecuteMode {
	NormalExec = 0, PawnDoubleMove = 1, Castle = 2, EnPassant = 3, Capture = 4, Promote = 5, PromoteCapture = 6, NullMove = 7
} MoveExecuteMode;

const char mod_names[8][15]{
	"NormalExec", "PawnDoubleMove", "Castle", "EnPassant", "Capture", "Promote", "PromoteCapture", "NullMove"
};
#define getModeName(m) (mod_names[m])

typedef enum Pieces {
	WhitePawn = 0, WhiteKnight = 1, WhiteBishop = 2, WhiteRook = 3, WhiteQueen = 4, WhiteKing = 5,
	BlackPawn = 6, BlackKnight = 7, BlackBishop = 8, BlackRook = 9, BlackQueen = 10, BlackKing = 11,
	Empty = 12,
	UnvalidPiece = 13
} Pieces;

constexpr char piece_names[13][12] = {
	"WhitePawn", "WhiteKnight", "WhiteBishop", "WhiteRook", "WhiteQueen", "WhiteKing",
	"BlackPawn", "BlackKnight", "BlackBishop", "BlackRook", "BlackQueen", "BlackKing",
	"Empty"
};
#define getPieceName(p) (piece_names[p])

constexpr Pieces nextPieceType[12] = {
	WhiteKnight, WhiteBishop, WhiteRook, WhiteQueen, WhiteKing,
	BlackPawn, BlackKnight, BlackBishop, BlackRook, BlackQueen, BlackKing,
	Empty
};

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

constexpr inline U64 MOVE_NEW_PREMOVE(const short Mode, const short CastlePerm, const short FiftyMove, const short enPas) {
	return 0ULL |
		((((U64)CastlePerm) << 24) & 0xf000000) |
		((((U64)Mode) << 28) & 0x70000000) |
		((((U64)FiftyMove) << 31) & 0x3f80000000) |
		((((U64)enPas) << 38) & 0x1fc000000000);
}

constexpr inline U64 MOVE_NEW(const short fromSQ, const short ToSQ, const short Moved, const short Captured, const short Promoted, const U64 PreMove) {
	return PreMove |
		(fromSQ & 0x3f) |
		((ToSQ << 6) & 0xfc0) |
		((Captured << 12) & 0xf000) |
		((Promoted << 16) & 0xf0000) |
		((Moved << 20) & 0xf00000);
}

constexpr int CastlePerm[64] = {
	13, 15, 15, 15, 12, 15, 15, 14,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	 7, 15, 15, 15,  3, 15, 15, 11,
};

typedef enum CastleRights {
	WKCA = 1, WQCA = 2, BKCA = 4, BQCA = 8
} CastleRights;

inline U64 getWhitePiecesBB(const U64* piecesListBB) {
	return piecesListBB[WhitePawn] | piecesListBB[WhiteKnight] | piecesListBB[WhiteBishop] | piecesListBB[WhiteRook] | piecesListBB[WhiteQueen] | piecesListBB[WhiteKing];
}

inline U64 getBlackPiecesBB(const U64* piecesListBB) {
	return piecesListBB[BlackPawn] | piecesListBB[BlackKnight] | piecesListBB[BlackBishop] | piecesListBB[BlackRook] | piecesListBB[BlackQueen] | piecesListBB[BlackKing];
}

inline int getWhitePieceType(const U64* piecesListBB, int sq) {
	U64 maskBB = SetMask[sq];
	if (piecesListBB[WhitePawn] & maskBB)
		return WhitePawn;
	if (piecesListBB[WhiteKnight] & maskBB)
		return WhiteKnight;
	if (piecesListBB[WhiteBishop] & maskBB)
		return WhiteBishop;
	if (piecesListBB[WhiteRook] & maskBB)
		return WhiteRook;
	if (piecesListBB[WhiteQueen] & maskBB)
		return WhiteQueen;
	if (piecesListBB[WhiteKing] & maskBB)
		return WhiteKing;
	return Empty;
}

inline int getBlackPieceType(const U64* piecesListBB, int sq) {
	U64 maskBB = SetMask[sq];
	if (piecesListBB[BlackPawn] & maskBB)
		return BlackPawn;
	if (piecesListBB[BlackKnight] & maskBB)
		return BlackKnight;
	if (piecesListBB[BlackBishop] & maskBB)
		return BlackBishop;
	if (piecesListBB[BlackRook] & maskBB)
		return BlackRook;
	if (piecesListBB[BlackQueen] & maskBB)
		return BlackQueen;
	if (piecesListBB[BlackKing] & maskBB)
		return BlackKing;
	return Empty;
}

inline bool MaterialDraw(const short* fiftyMoves, const U64* piecesListBB) {
	U64 wPiecesBB = getWhitePiecesBB(piecesListBB);
	U64 bPiecesBB = getBlackPiecesBB(piecesListBB);
	U64 piecesBB = wPiecesBB | bPiecesBB;
	short countwPieces = CountBits(wPiecesBB);
	short countbPieces = CountBits(bPiecesBB);
	short countPieces = CountBits(piecesBB);
	if (countPieces == 2)
		return true;

	if (*fiftyMoves > 99)
		return true;

	if (countPieces <= 4 && countPieces >= 3) {
		if (piecesListBB[WhiteRook] | piecesListBB[BlackRook] | piecesListBB[WhiteQueen] | piecesListBB[BlackQueen] | piecesListBB[WhitePawn] | piecesListBB[BlackPawn])
			return false;
		return true;
	}

	return false;
}

template <Colors Us>
inline bool Opening(const U64* piecesListBB) {
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

inline bool Endgame(const U64 * piecesListBB) {
		if (CountBits((getWhitePiecesBB(piecesListBB) | getBlackPiecesBB(piecesListBB)) & ~(piecesListBB[WhitePawn] | piecesListBB[BlackPawn])) >= 8)
		return false;
	
	return false;
}