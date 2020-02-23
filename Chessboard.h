#pragma once
typedef unsigned long long U64;
typedef enum Colors { BLACK, WHITE };
typedef enum Pieces { WhitePawn, WhiteKnight, WhiteBishop, WhiteRook, WhiteQueen, WhiteKing, BlackPawn, BlackKnight, BlackBishop, BlackRook, BlackQueen, BlackKing, Empty };


/*
	////// set and clear single bits //////
*/
constexpr U64 SetMask[64] = {
0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80,
0x100, 0x200, 0x400, 0x800, 0x1000, 0x2000, 0x4000, 0x8000,
0x10000, 0x20000, 0x40000, 0x80000, 0x100000, 0x200000, 0x400000, 0x800000,
0x1000000, 0x2000000, 0x4000000, 0x8000000, 0x10000000, 0x20000000, 0x40000000, 0x80000000,
0x100000000, 0x200000000, 0x400000000, 0x800000000, 0x1000000000, 0x2000000000, 0x4000000000, 0x8000000000,
0x10000000000, 0x20000000000, 0x40000000000, 0x80000000000, 0x100000000000, 0x200000000000, 0x400000000000, 0x800000000000,
0x1000000000000, 0x2000000000000, 0x4000000000000, 0x8000000000000, 0x10000000000000, 0x20000000000000, 0x40000000000000, 0x80000000000000,
0x100000000000000, 0x200000000000000, 0x400000000000000, 0x800000000000000, 0x1000000000000000, 0x2000000000000000, 0x4000000000000000, 0x8000000000000000
};
constexpr inline void SetBit(U64* bb, const short sq) { *bb |= SetMask[sq]; }

constexpr U64 ClearMask[64] = {
0xfffffffffffffffe, 0xfffffffffffffffd, 0xfffffffffffffffb, 0xfffffffffffffff7, 0xffffffffffffffef, 0xffffffffffffffdf, 0xffffffffffffffbf, 0xffffffffffffff7f,
0xfffffffffffffeff, 0xfffffffffffffdff, 0xfffffffffffffbff, 0xfffffffffffff7ff, 0xffffffffffffefff, 0xffffffffffffdfff, 0xffffffffffffbfff, 0xffffffffffff7fff,
0xfffffffffffeffff, 0xfffffffffffdffff, 0xfffffffffffbffff, 0xfffffffffff7ffff, 0xffffffffffefffff, 0xffffffffffdfffff, 0xffffffffffbfffff, 0xffffffffff7fffff,
0xfffffffffeffffff, 0xfffffffffdffffff, 0xfffffffffbffffff, 0xfffffffff7ffffff, 0xffffffffefffffff, 0xffffffffdfffffff, 0xffffffffbfffffff, 0xffffffff7fffffff,
0xfffffffeffffffff, 0xfffffffdffffffff, 0xfffffffbffffffff, 0xfffffff7ffffffff, 0xffffffefffffffff, 0xffffffdfffffffff, 0xffffffbfffffffff, 0xffffff7fffffffff,
0xfffffeffffffffff, 0xfffffdffffffffff, 0xfffffbffffffffff, 0xfffff7ffffffffff, 0xffffefffffffffff, 0xffffdfffffffffff, 0xffffbfffffffffff, 0xffff7fffffffffff,
0xfffeffffffffffff, 0xfffdffffffffffff, 0xfffbffffffffffff, 0xfff7ffffffffffff, 0xffefffffffffffff, 0xffdfffffffffffff, 0xffbfffffffffffff, 0xff7fffffffffffff,
0xfeffffffffffffff, 0xfdffffffffffffff, 0xfbffffffffffffff, 0xf7ffffffffffffff, 0xefffffffffffffff, 0xdfffffffffffffff, 0xbfffffffffffffff, 0x7fffffffffffffff
};
constexpr inline void ClearBit(U64* bb, const short sq) { *bb &= ClearMask[sq]; }
/*
	////// set and clear single bits //////
*/



class Chessboard
{
private:
	U64 PiecesBB[12];

public:
	Chessboard() {
		for (int i = WhitePawn; i < Empty; i++)
			PiecesBB[i] = 0ULL;
	}
};

