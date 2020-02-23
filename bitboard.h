#pragma once
#include "basic.h"

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
constexpr inline void MoveBit(U64* bb, const short sq_from, const short sq_to) { *bb = (*bb & ClearMask[sq_from]) | SetMask[sq_to]; }
/*
	////// set and clear single bits //////
*/


constexpr short BitTable[64] = {
  63, 30, 3, 32, 25, 41, 22, 33, 15, 50, 42, 13, 11, 53, 19, 34, 61, 29, 2,
  51, 21, 43, 45, 10, 18, 47, 1, 54, 9, 57, 0, 35, 62, 31, 40, 4, 49, 5, 52,
  26, 60, 6, 23, 44, 46, 27, 56, 16, 7, 39, 48, 24, 59, 14, 12, 55, 38, 28,
  58, 20, 37, 17, 36, 8
};

inline short PopBit(U64* bb) {
	U64 b = *bb ^ (*bb - 1);
	unsigned int fold = (unsigned)((b & 0xffffffff) ^ (b >> 32));
	*bb &= (*bb - 1);
	return BitTable[(fold * 0x783a9b23) >> 26];
}

inline short CountBits(U64 b) {
	int r;
	for (r = 0; b; r++, b &= b - 1);
	return r;
}
