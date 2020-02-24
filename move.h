#pragma once
#include "bitboard.h"

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
		(((U64)fromSQ) & 0x3f) |
		((((U64)ToSQ) << 6) & 0xfc0) |
		((((U64)Captured) << 12) & 0xf000) |
		((((U64)Promoted) << 16) & 0xf0000) |
		((((U64)Moved) << 20) & 0xf00000);
}