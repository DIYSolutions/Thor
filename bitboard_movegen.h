#pragma once
#include "bitboard.h"

/*
	////// directions //////
*/
typedef enum Directions { Right, Left, Up, Down, UpRight, UpLeft, DownRight, DownLeft, InvalidDirection } Directions;
constexpr short OppositeDir[9] = { Left, Right, Down, Up, DownLeft, DownRight, UpLeft, UpRight, InvalidDirection };

constexpr U64 DirectionsBB[8][64] = {
	{
	0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0x80, 0x0,
	0xfe00, 0xfc00, 0xf800, 0xf000, 0xe000, 0xc000, 0x8000, 0x0,
	0xfe0000, 0xfc0000, 0xf80000, 0xf00000, 0xe00000, 0xc00000, 0x800000, 0x0,
	0xfe000000, 0xfc000000, 0xf8000000, 0xf0000000, 0xe0000000, 0xc0000000, 0x80000000, 0x0,
	0xfe00000000, 0xfc00000000, 0xf800000000, 0xf000000000, 0xe000000000, 0xc000000000, 0x8000000000, 0x0,
	0xfe0000000000, 0xfc0000000000, 0xf80000000000, 0xf00000000000, 0xe00000000000, 0xc00000000000, 0x800000000000, 0x0,
	0xfe000000000000, 0xfc000000000000, 0xf8000000000000, 0xf0000000000000, 0xe0000000000000, 0xc0000000000000, 0x80000000000000, 0x0,
	0xfe00000000000000, 0xfc00000000000000, 0xf800000000000000, 0xf000000000000000, 0xe000000000000000, 0xc000000000000000, 0x8000000000000000, 0x0
	},
	{
	0x0, 0x1, 0x3, 0x7, 0xf, 0x1f, 0x3f, 0x7f,
	0x0, 0x100, 0x300, 0x700, 0xf00, 0x1f00, 0x3f00, 0x7f00,
	0x0, 0x10000, 0x30000, 0x70000, 0xf0000, 0x1f0000, 0x3f0000, 0x7f0000,
	0x0, 0x1000000, 0x3000000, 0x7000000, 0xf000000, 0x1f000000, 0x3f000000, 0x7f000000,
	0x0, 0x100000000, 0x300000000, 0x700000000, 0xf00000000, 0x1f00000000, 0x3f00000000, 0x7f00000000,
	0x0, 0x10000000000, 0x30000000000, 0x70000000000, 0xf0000000000, 0x1f0000000000, 0x3f0000000000, 0x7f0000000000,
	0x0, 0x1000000000000, 0x3000000000000, 0x7000000000000, 0xf000000000000, 0x1f000000000000, 0x3f000000000000, 0x7f000000000000,
	0x0, 0x100000000000000, 0x300000000000000, 0x700000000000000, 0xf00000000000000, 0x1f00000000000000, 0x3f00000000000000, 0x7f00000000000000
	},
	{
	0x101010101010100, 0x202020202020200, 0x404040404040400, 0x808080808080800, 0x1010101010101000, 0x2020202020202000, 0x4040404040404000, 0x8080808080808000,
	0x101010101010000, 0x202020202020000, 0x404040404040000, 0x808080808080000, 0x1010101010100000, 0x2020202020200000, 0x4040404040400000, 0x8080808080800000,
	0x101010101000000, 0x202020202000000, 0x404040404000000, 0x808080808000000, 0x1010101010000000, 0x2020202020000000, 0x4040404040000000, 0x8080808080000000,
	0x101010100000000, 0x202020200000000, 0x404040400000000, 0x808080800000000, 0x1010101000000000, 0x2020202000000000, 0x4040404000000000, 0x8080808000000000,
	0x101010000000000, 0x202020000000000, 0x404040000000000, 0x808080000000000, 0x1010100000000000, 0x2020200000000000, 0x4040400000000000, 0x8080800000000000,
	0x101000000000000, 0x202000000000000, 0x404000000000000, 0x808000000000000, 0x1010000000000000, 0x2020000000000000, 0x4040000000000000, 0x8080000000000000,
	0x100000000000000, 0x200000000000000, 0x400000000000000, 0x800000000000000, 0x1000000000000000, 0x2000000000000000, 0x4000000000000000, 0x8000000000000000,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0
	},
	{
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80,
	0x101, 0x202, 0x404, 0x808, 0x1010, 0x2020, 0x4040, 0x8080,
	0x10101, 0x20202, 0x40404, 0x80808, 0x101010, 0x202020, 0x404040, 0x808080,
	0x1010101, 0x2020202, 0x4040404, 0x8080808, 0x10101010, 0x20202020, 0x40404040, 0x80808080,
	0x101010101, 0x202020202, 0x404040404, 0x808080808, 0x1010101010, 0x2020202020, 0x4040404040, 0x8080808080,
	0x10101010101, 0x20202020202, 0x40404040404, 0x80808080808, 0x101010101010, 0x202020202020, 0x404040404040, 0x808080808080,
	0x1010101010101, 0x2020202020202, 0x4040404040404, 0x8080808080808, 0x10101010101010, 0x20202020202020, 0x40404040404040, 0x80808080808080
	},
	{
	0x8040201008040200, 0x80402010080400, 0x804020100800, 0x8040201000, 0x80402000, 0x804000, 0x8000, 0x0,
	0x4020100804020000, 0x8040201008040000, 0x80402010080000, 0x804020100000, 0x8040200000, 0x80400000, 0x800000, 0x0,
	0x2010080402000000, 0x4020100804000000, 0x8040201008000000, 0x80402010000000, 0x804020000000, 0x8040000000, 0x80000000, 0x0,
	0x1008040200000000, 0x2010080400000000, 0x4020100800000000, 0x8040201000000000, 0x80402000000000, 0x804000000000, 0x8000000000, 0x0,
	0x804020000000000, 0x1008040000000000, 0x2010080000000000, 0x4020100000000000, 0x8040200000000000, 0x80400000000000, 0x800000000000, 0x0,
	0x402000000000000, 0x804000000000000, 0x1008000000000000, 0x2010000000000000, 0x4020000000000000, 0x8040000000000000, 0x80000000000000, 0x0,
	0x200000000000000, 0x400000000000000, 0x800000000000000, 0x1000000000000000, 0x2000000000000000, 0x4000000000000000, 0x8000000000000000, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0
	},
	{
	0x0, 0x100, 0x10200, 0x1020400, 0x102040800, 0x10204081000, 0x1020408102000, 0x102040810204000,
	0x0, 0x10000, 0x1020000, 0x102040000, 0x10204080000, 0x1020408100000, 0x102040810200000, 0x204081020400000,
	0x0, 0x1000000, 0x102000000, 0x10204000000, 0x1020408000000, 0x102040810000000, 0x204081020000000, 0x408102040000000,
	0x0, 0x100000000, 0x10200000000, 0x1020400000000, 0x102040800000000, 0x204081000000000, 0x408102000000000, 0x810204000000000,
	0x0, 0x10000000000, 0x1020000000000, 0x102040000000000, 0x204080000000000, 0x408100000000000, 0x810200000000000, 0x1020400000000000,
	0x0, 0x1000000000000, 0x102000000000000, 0x204000000000000, 0x408000000000000, 0x810000000000000, 0x1020000000000000, 0x2040000000000000,
	0x0, 0x100000000000000, 0x200000000000000, 0x400000000000000, 0x800000000000000, 0x1000000000000000, 0x2000000000000000, 0x4000000000000000,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0
	},
	{
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80, 0x0,
	0x204, 0x408, 0x810, 0x1020, 0x2040, 0x4080, 0x8000, 0x0,
	0x20408, 0x40810, 0x81020, 0x102040, 0x204080, 0x408000, 0x800000, 0x0,
	0x2040810, 0x4081020, 0x8102040, 0x10204080, 0x20408000, 0x40800000, 0x80000000, 0x0,
	0x204081020, 0x408102040, 0x810204080, 0x1020408000, 0x2040800000, 0x4080000000, 0x8000000000, 0x0,
	0x20408102040, 0x40810204080, 0x81020408000, 0x102040800000, 0x204080000000, 0x408000000000, 0x800000000000, 0x0,
	0x2040810204080, 0x4081020408000, 0x8102040800000, 0x10204080000000, 0x20408000000000, 0x40800000000000, 0x80000000000000, 0x0
	},
	{
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40,
	0x0, 0x100, 0x201, 0x402, 0x804, 0x1008, 0x2010, 0x4020,
	0x0, 0x10000, 0x20100, 0x40201, 0x80402, 0x100804, 0x201008, 0x402010,
	0x0, 0x1000000, 0x2010000, 0x4020100, 0x8040201, 0x10080402, 0x20100804, 0x40201008,
	0x0, 0x100000000, 0x201000000, 0x402010000, 0x804020100, 0x1008040201, 0x2010080402, 0x4020100804,
	0x0, 0x10000000000, 0x20100000000, 0x40201000000, 0x80402010000, 0x100804020100, 0x201008040201, 0x402010080402,
	0x0, 0x1000000000000, 0x2010000000000, 0x4020100000000, 0x8040201000000, 0x10080402010000, 0x20100804020100, 0x40201008040201
	}
};
constexpr inline const U64 getRay(const short Direction, const short SQ) { return DirectionsBB[Direction][SQ]; }
constexpr inline const short getOppositeDir(const short dir) { return OppositeDir[dir]; }

/*
	////// directions //////
*/

/*
	////// some constants //////
*/
constexpr U64 notAFile = 0xfefefefefefefefe;
constexpr U64 notHFile = 0x7f7f7f7f7f7f7f7f;
constexpr U64 not2Rank = 0xffffffffffff00ff;
constexpr U64 not7Rank = 0xff00ffffffffffff;
constexpr U64 the1Rank = 0xff;
constexpr U64 the8Rank = 0xff00000000000000;
constexpr U64 the2Rank = 0xff00;
constexpr U64 the7Rank = 0xff000000000000;
constexpr U64 the4Rank = 0xff000000;
constexpr U64 the5Rank = 0xff00000000;
constexpr U64 notABFile = 0xfcfcfcfcfcfcfcfc;
constexpr U64 notGHFile = 0x3f3f3f3f3f3f3f3f;

constexpr U64 WhiteKingCastleFree = 0x60;
constexpr U64 WhiteKingCastleCheck = 0x60;
constexpr U64 WhiteQueenCastleFree = 0xe;
constexpr U64 WhiteQueenCastleCheck = 0xc;
constexpr U64 BlackKingCastleFree = 0x6000000000000000;
constexpr U64 BlackKingCastleCheck = 0x6000000000000000;
constexpr U64 BlackQueenCastleFree = 0xe00000000000000;
constexpr U64 BlackQueenCastleCheck = 0xc00000000000000;

constexpr U64 BorderBB = 0xff818181818181ff;
constexpr U64 NotBorderBB = 0x7e7e7e7e7e7e00;
constexpr U64 BottomBB = 0xff;
constexpr U64 NotBottomBB = 0xffffffffffffff00;
constexpr U64 TopBB = 0xff00000000000000;
constexpr U64 NotTopBB = 0xffffffffffffff;
constexpr U64 RightBB = 0x8080808080808080;
constexpr U64 NotRightBB = 0x7f7f7f7f7f7f7f7f;
constexpr U64 LeftBB = 0x101010101010101;
constexpr U64 NotLeftBB = 0xfefefefefefefefe;

constexpr U64 BlackFieldsBB = 0x5555555555555555;
constexpr U64 WhiteFieldsBB = 0xaaaaaaaaaaaaaaaa;

constexpr U64 FullBB = ~(0ULL);

constexpr U64 FileBB[8] = {
	getRay(Up,0) | getRay(Down,56),
	getRay(Up,1) | getRay(Down,57),
	getRay(Up,2) | getRay(Down,58),
	getRay(Up,3) | getRay(Down,59),
	getRay(Up,4) | getRay(Down,60),
	getRay(Up,5) | getRay(Down,61),
	getRay(Up,6) | getRay(Down,62),
	getRay(Up,7) | getRay(Down,63)
};

/*
	////// some constants //////
*/


/*
	////// constant attack boards //////
*/
constexpr U64 AttackBrdKingBB[64] = {
	0x302            , 0x705            , 0xe0a            , 0x1c14            , 0x3828            , 0x7050            , 0xe0a0            , 0xc040,
	0x30203          , 0x70507          , 0xe0a0e          , 0x1c141c          , 0x382838          , 0x705070          , 0xe0a0e0          , 0xc040c0,
	0x3020300        , 0x7050700        , 0xe0a0e00        , 0x1c141c00        , 0x38283800        , 0x70507000        , 0xe0a0e000        , 0xc040c000,
	0x302030000      , 0x705070000      , 0xe0a0e0000      , 0x1c141c0000      , 0x3828380000      , 0x7050700000      , 0xe0a0e00000      , 0xc040c00000,
	0x30203000000    , 0x70507000000    , 0xe0a0e000000    , 0x1c141c000000    , 0x382838000000    , 0x705070000000    , 0xe0a0e0000000    , 0xc040c0000000,
	0x3020300000000  , 0x7050700000000  , 0xe0a0e00000000  , 0x1c141c00000000  , 0x38283800000000  , 0x70507000000000  , 0xe0a0e000000000  , 0xc040c000000000,
	0x302030000000000, 0x705070000000000, 0xe0a0e0000000000, 0x1c141c0000000000, 0x3828380000000000, 0x7050700000000000, 0xe0a0e00000000000, 0xc040c00000000000,
	0x203000000000000, 0x507000000000000, 0xa0e000000000000, 0x141c000000000000, 0x2838000000000000, 0x5070000000000000, 0xa0e0000000000000, 0x40c0000000000000
};

constexpr U64 AttackBrdKnightBB[64] = {
	0x20400          , 0x50800          , 0xa1100           , 0x142200          , 0x284400          , 0x508800          , 0xa01000          , 0x402000,
	0x2040004        , 0x5080008        , 0xa110011         , 0x14220022        , 0x28440044        , 0x50880088        , 0xa0100010        , 0x40200020,
	0x204000402      , 0x508000805      , 0xa1100110a       , 0x1422002214      , 0x2844004428      , 0x5088008850      , 0xa0100010a0      , 0x4020002040,
	0x20400040200    , 0x50800080500    , 0xa1100110a00     , 0x142200221400    , 0x284400442800    , 0x508800885000    , 0xa0100010a000    , 0x402000204000,
	0x2040004020000  , 0x5080008050000  , 0xa1100110a0000   , 0x14220022140000  , 0x28440044280000  , 0x50880088500000  , 0xa0100010a00000  , 0x40200020400000,
	0x204000402000000, 0x508000805000000, 0xa1100110a000000 , 0x1422002214000000, 0x2844004428000000, 0x5088008850000000, 0xa0100010a0000000, 0x4020002040000000,
	0x400040200000000, 0x800080500000000, 0x1100110a00000000, 0x2200221400000000, 0x4400442800000000, 0x8800885000000000, 0x100010a000000000, 0x2000204000000000,
	0x4020000000000  , 0x8050000000000  , 0x110a0000000000  , 0x22140000000000  , 0x44280000000000  , 0x88500000000000  , 0x10a00000000000  , 0x20400000000000
};

constexpr U64 AttackBrdwPawnBB[64] = {
0x200, 0x500, 0xa00, 0x1400, 0x2800, 0x5000, 0xa000, 0x4000,
0x20000, 0x50000, 0xa0000, 0x140000, 0x280000, 0x500000, 0xa00000, 0x400000,
0x2000000, 0x5000000, 0xa000000, 0x14000000, 0x28000000, 0x50000000, 0xa0000000, 0x40000000,
0x200000000, 0x500000000, 0xa00000000, 0x1400000000, 0x2800000000, 0x5000000000, 0xa000000000, 0x4000000000,
0x20000000000, 0x50000000000, 0xa0000000000, 0x140000000000, 0x280000000000, 0x500000000000, 0xa00000000000, 0x400000000000,
0x2000000000000, 0x5000000000000, 0xa000000000000, 0x14000000000000, 0x28000000000000, 0x50000000000000, 0xa0000000000000, 0x40000000000000,
0x200000000000000, 0x500000000000000, 0xa00000000000000, 0x1400000000000000, 0x2800000000000000, 0x5000000000000000, 0xa000000000000000, 0x4000000000000000,
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0
};

constexpr U64 AttackBrdbPawnBB[64] = {
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
0x2, 0x5, 0xa, 0x14, 0x28, 0x50, 0xa0, 0x40,
0x200, 0x500, 0xa00, 0x1400, 0x2800, 0x5000, 0xa000, 0x4000,
0x20000, 0x50000, 0xa0000, 0x140000, 0x280000, 0x500000, 0xa00000, 0x400000,
0x2000000, 0x5000000, 0xa000000, 0x14000000, 0x28000000, 0x50000000, 0xa0000000, 0x40000000,
0x200000000, 0x500000000, 0xa00000000, 0x1400000000, 0x2800000000, 0x5000000000, 0xa000000000, 0x4000000000,
0x20000000000, 0x50000000000, 0xa0000000000, 0x140000000000, 0x280000000000, 0x500000000000, 0xa00000000000, 0x400000000000,
0x2000000000000, 0x5000000000000, 0xa000000000000, 0x14000000000000, 0x28000000000000, 0x50000000000000, 0xa0000000000000, 0x40000000000000
};

//AttackBrd_Rook
constexpr U64 AttackBrdRookBB[64] = {
0x1010101010101fe, 0x2020202020202fd, 0x4040404040404fb, 0x8080808080808f7, 0x10101010101010ef, 0x20202020202020df, 0x40404040404040bf, 0x808080808080807f,
0x10101010101fe01, 0x20202020202fd02, 0x40404040404fb04, 0x80808080808f708, 0x101010101010ef10, 0x202020202020df20, 0x404040404040bf40, 0x8080808080807f80,
0x101010101fe0101, 0x202020202fd0202, 0x404040404fb0404, 0x808080808f70808, 0x1010101010ef1010, 0x2020202020df2020, 0x4040404040bf4040, 0x80808080807f8080,
0x1010101fe010101, 0x2020202fd020202, 0x4040404fb040404, 0x8080808f7080808, 0x10101010ef101010, 0x20202020df202020, 0x40404040bf404040, 0x808080807f808080,
0x10101fe01010101, 0x20202fd02020202, 0x40404fb04040404, 0x80808f708080808, 0x101010ef10101010, 0x202020df20202020, 0x404040bf40404040, 0x8080807f80808080,
0x101fe0101010101, 0x202fd0202020202, 0x404fb0404040404, 0x808f70808080808, 0x1010ef1010101010, 0x2020df2020202020, 0x4040bf4040404040, 0x80807f8080808080,
0x1fe010101010101, 0x2fd020202020202, 0x4fb040404040404, 0x8f7080808080808, 0x10ef101010101010, 0x20df202020202020, 0x40bf404040404040, 0x807f808080808080,
0xfe01010101010101, 0xfd02020202020202, 0xfb04040404040404, 0xf708080808080808, 0xef10101010101010, 0xdf20202020202020, 0xbf40404040404040, 0x7f80808080808080
};

constexpr U64 BlockersBrdRookBB[64] = {
0x101010101017e, 0x202020202027c, 0x404040404047a, 0x8080808080876, 0x1010101010106e, 0x2020202020205e, 0x4040404040403e, 0x8080808080807e,
0x1010101017e00, 0x2020202027c00, 0x4040404047a00, 0x8080808087600, 0x10101010106e00, 0x20202020205e00, 0x40404040403e00, 0x80808080807e00,
0x10101017e0100, 0x20202027c0200, 0x40404047a0400, 0x8080808760800, 0x101010106e1000, 0x202020205e2000, 0x404040403e4000, 0x808080807e8000,
0x101017e010100, 0x202027c020200, 0x404047a040400, 0x8080876080800, 0x1010106e101000, 0x2020205e202000, 0x4040403e404000, 0x8080807e808000,
0x1017e01010100, 0x2027c02020200, 0x4047a04040400, 0x8087608080800, 0x10106e10101000, 0x20205e20202000, 0x40403e40404000, 0x80807e80808000,
0x17e0101010100, 0x27c0202020200, 0x47a0404040400, 0x8760808080800, 0x106e1010101000, 0x205e2020202000, 0x403e4040404000, 0x807e8080808000,
0x7e010101010100, 0x7c020202020200, 0x7a040404040400, 0x76080808080800, 0x6e101010101000, 0x5e202020202000, 0x3e404040404000, 0x7e808080808000,
0x7e01010101010100, 0x7c02020202020200, 0x7a04040404040400, 0x7608080808080800, 0x6e10101010101000, 0x5e20202020202000, 0x3e40404040404000, 0x7e80808080808000
};

//AttackBrd_Bishop
constexpr U64 AttackBrdBishopBB[64] = {
0x8040201008040200, 0x80402010080500, 0x804020110a00, 0x8041221400, 0x182442800, 0x10204885000, 0x102040810a000, 0x102040810204000,
0x4020100804020002, 0x8040201008050005, 0x804020110a000a, 0x804122140014, 0x18244280028, 0x1020488500050, 0x102040810a000a0, 0x204081020400040,
0x2010080402000204, 0x4020100805000508, 0x804020110a000a11, 0x80412214001422, 0x1824428002844, 0x102048850005088, 0x2040810a000a010, 0x408102040004020,
0x1008040200020408, 0x2010080500050810, 0x4020110a000a1120, 0x8041221400142241, 0x182442800284482, 0x204885000508804, 0x40810a000a01008, 0x810204000402010,
0x804020002040810, 0x1008050005081020, 0x20110a000a112040, 0x4122140014224180, 0x8244280028448201, 0x488500050880402, 0x810a000a0100804, 0x1020400040201008,
0x402000204081020, 0x805000508102040, 0x110a000a11204080, 0x2214001422418000, 0x4428002844820100, 0x8850005088040201, 0x10a000a010080402, 0x2040004020100804,
0x200020408102040, 0x500050810204080, 0xa000a1120408000, 0x1400142241800000, 0x2800284482010000, 0x5000508804020100, 0xa000a01008040201, 0x4000402010080402,
0x2040810204080, 0x5081020408000, 0xa112040800000, 0x14224180000000, 0x28448201000000, 0x50880402010000, 0xa0100804020100, 0x40201008040201
};

constexpr U64 BlockersBrdBishopBB[64] = {
NotBorderBB & AttackBrdBishopBB[0], NotBorderBB & AttackBrdBishopBB[1], NotBorderBB & AttackBrdBishopBB[2], NotBorderBB & AttackBrdBishopBB[3], NotBorderBB & AttackBrdBishopBB[4], NotBorderBB & AttackBrdBishopBB[5], NotBorderBB & AttackBrdBishopBB[6], NotBorderBB & AttackBrdBishopBB[7],
NotBorderBB & AttackBrdBishopBB[8], NotBorderBB & AttackBrdBishopBB[9], NotBorderBB & AttackBrdBishopBB[10], NotBorderBB & AttackBrdBishopBB[11], NotBorderBB & AttackBrdBishopBB[12], NotBorderBB & AttackBrdBishopBB[13], NotBorderBB & AttackBrdBishopBB[14], NotBorderBB & AttackBrdBishopBB[15],
NotBorderBB & AttackBrdBishopBB[16], NotBorderBB & AttackBrdBishopBB[17], NotBorderBB & AttackBrdBishopBB[18], NotBorderBB & AttackBrdBishopBB[19], NotBorderBB & AttackBrdBishopBB[20], NotBorderBB & AttackBrdBishopBB[21], NotBorderBB & AttackBrdBishopBB[22], NotBorderBB & AttackBrdBishopBB[23],
NotBorderBB & AttackBrdBishopBB[24], NotBorderBB & AttackBrdBishopBB[25], NotBorderBB & AttackBrdBishopBB[26], NotBorderBB & AttackBrdBishopBB[27], NotBorderBB & AttackBrdBishopBB[28], NotBorderBB & AttackBrdBishopBB[29], NotBorderBB & AttackBrdBishopBB[30], NotBorderBB & AttackBrdBishopBB[31],
NotBorderBB & AttackBrdBishopBB[32], NotBorderBB & AttackBrdBishopBB[33], NotBorderBB & AttackBrdBishopBB[34], NotBorderBB & AttackBrdBishopBB[35], NotBorderBB & AttackBrdBishopBB[36], NotBorderBB & AttackBrdBishopBB[37], NotBorderBB & AttackBrdBishopBB[38], NotBorderBB & AttackBrdBishopBB[39],
NotBorderBB & AttackBrdBishopBB[40], NotBorderBB & AttackBrdBishopBB[41], NotBorderBB & AttackBrdBishopBB[42], NotBorderBB & AttackBrdBishopBB[43], NotBorderBB & AttackBrdBishopBB[44], NotBorderBB & AttackBrdBishopBB[45], NotBorderBB & AttackBrdBishopBB[46], NotBorderBB & AttackBrdBishopBB[47],
NotBorderBB & AttackBrdBishopBB[48], NotBorderBB & AttackBrdBishopBB[49], NotBorderBB & AttackBrdBishopBB[50], NotBorderBB & AttackBrdBishopBB[51], NotBorderBB & AttackBrdBishopBB[52], NotBorderBB & AttackBrdBishopBB[53], NotBorderBB & AttackBrdBishopBB[54], NotBorderBB & AttackBrdBishopBB[55],
NotBorderBB & AttackBrdBishopBB[56], NotBorderBB & AttackBrdBishopBB[57], NotBorderBB & AttackBrdBishopBB[58], NotBorderBB & AttackBrdBishopBB[59], NotBorderBB & AttackBrdBishopBB[60], NotBorderBB & AttackBrdBishopBB[61], NotBorderBB & AttackBrdBishopBB[62], NotBorderBB & AttackBrdBishopBB[63],
};

//AttackBrd_Queen
constexpr U64 AttackBrdQueenBB[64] = {
0x81412111090503fe, 0x2824222120a07fd, 0x404844424150efb, 0x8080888492a1cf7, 0x10101011925438ef, 0x2020212224a870df, 0x404142444850e0bf, 0x8182848890a0c07f,
0x412111090503fe03, 0x824222120a07fd07, 0x4844424150efb0e, 0x80888492a1cf71c, 0x101011925438ef38, 0x20212224a870df70, 0x4142444850e0bfe0, 0x82848890a0c07fc0,
0x2111090503fe0305, 0x4222120a07fd070a, 0x844424150efb0e15, 0x888492a1cf71c2a, 0x1011925438ef3854, 0x212224a870df70a8, 0x42444850e0bfe050, 0x848890a0c07fc0a0,
0x11090503fe030509, 0x22120a07fd070a12, 0x4424150efb0e1524, 0x88492a1cf71c2a49, 0x11925438ef385492, 0x2224a870df70a824, 0x444850e0bfe05048, 0x8890a0c07fc0a090,
0x90503fe03050911, 0x120a07fd070a1222, 0x24150efb0e152444, 0x492a1cf71c2a4988, 0x925438ef38549211, 0x24a870df70a82422, 0x4850e0bfe0504844, 0x90a0c07fc0a09088,
0x503fe0305091121, 0xa07fd070a122242, 0x150efb0e15244484, 0x2a1cf71c2a498808, 0x5438ef3854921110, 0xa870df70a8242221, 0x50e0bfe050484442, 0xa0c07fc0a0908884,
0x3fe030509112141, 0x7fd070a12224282, 0xefb0e1524448404, 0x1cf71c2a49880808, 0x38ef385492111010, 0x70df70a824222120, 0xe0bfe05048444241, 0xc07fc0a090888482,
0xfe03050911214181, 0xfd070a1222428202, 0xfb0e152444840404, 0xf71c2a4988080808, 0xef38549211101010, 0xdf70a82422212020, 0xbfe0504844424140, 0x7fc0a09088848281
};

/*
	////// constant attack boards //////
*/

/*
	////// blocking attack boards //////
*/


template <Directions dir>
constexpr const inline U64 getBlockingSQ_BB(const short sq_a, const short sq_b) {
	constexpr short a_dir = dir;
	constexpr short b_dir = getOppositeDir(dir);

	return DirectionsBB[a_dir][sq_a] & DirectionsBB[b_dir][sq_b];
}

inline const short getDirection(const short sq_a, const short sq_b) {
	short temp;
	if (sq_a < sq_b) {
		temp = sq_b - sq_a;
		if (temp % 8 == 0)
			return Up;
		if (temp % 7 == 0)
			return UpLeft;
		if (temp % 9 == 0)
			return UpRight;
		return Right;
	}
	else {// sq_a > sq_b
		temp = sq_a - sq_b;
		if (temp % 8 == 0)
			return Down;
		if (temp % 9 == 0)
			return DownLeft;
		if (temp % 7 == 0)
			return DownRight;
		return Left;
	}
}

constexpr inline const short getRookDirection(const short sq_a, const short sq_b) {

	if (sq_a < sq_b) {
		if ((sq_b - sq_a) % 8 == 0)
			return Up;
		return Right;
	}
	else {// sq_a > sq_b
		if ((sq_a - sq_b) % 8 == 0)
			return Down;
		return Left;
	}
}

constexpr inline const short getBishopDirection(const short sq_a, const short sq_b) {

	if (sq_a < sq_b) {
		if ((sq_b - sq_a) % 7 == 0)
			return UpLeft;
		return UpRight;
	}
	else {// sq_a > sq_b
		if ((sq_a - sq_b) % 7 == 0)
			return DownRight;
		return DownLeft;
	}
}



constexpr inline const U64 getBlockingSQ_BB(const short sq_a, const short sq_b) {

	if (sq_a < sq_b) {
		U64 blockingSQBB = getBlockingSQ_BB<Right>(sq_a, sq_b);
		if (blockingSQBB)
			return blockingSQBB;

		blockingSQBB = getBlockingSQ_BB<UpLeft>(sq_a, sq_b);
		if (blockingSQBB)
			return blockingSQBB;

		blockingSQBB = getBlockingSQ_BB<Up>(sq_a, sq_b);
		if (blockingSQBB)
			return blockingSQBB;

		return getBlockingSQ_BB<UpRight>(sq_a, sq_b);
	}
	else {// sq_a > sq_b
		U64 blockingSQBB = getBlockingSQ_BB<Left>(sq_a, sq_b);
		if (blockingSQBB)
			return blockingSQBB;

		blockingSQBB = getBlockingSQ_BB<DownLeft>(sq_a, sq_b);
		if (blockingSQBB)
			return blockingSQBB;

		blockingSQBB = getBlockingSQ_BB<Down>(sq_a, sq_b);
		if (blockingSQBB)
			return blockingSQBB;

		return getBlockingSQ_BB<DownRight>(sq_a, sq_b);
	}
}

constexpr inline const U64 getRookBlockingSQ_BB(const short sq_a, const short sq_b) {

	if (sq_a < sq_b) {
		U64 blockingSQBB = getBlockingSQ_BB<Up>(sq_a, sq_b);
		if (blockingSQBB)
			return blockingSQBB;

		return getBlockingSQ_BB<Right>(sq_a, sq_b);
	}
	else {// sq_a > sq_b
		U64 blockingSQBB = getBlockingSQ_BB<Down>(sq_a, sq_b);
		if (blockingSQBB)
			return blockingSQBB;

		return getBlockingSQ_BB<Left>(sq_a, sq_b);
	}
}

constexpr inline const U64 getBishopBlockingSQ_BB(const short sq_a, const short sq_b) {

	if (sq_a < sq_b) {
		U64 blockingSQBB = getBlockingSQ_BB<UpLeft>(sq_a, sq_b);
		if (blockingSQBB)
			return blockingSQBB;

		return getBlockingSQ_BB<UpRight>(sq_a, sq_b);
	}
	else {// sq_a > sq_b
		U64 blockingSQBB = getBlockingSQ_BB<DownLeft>(sq_a, sq_b);
		if (blockingSQBB)
			return blockingSQBB;

		return getBlockingSQ_BB<DownRight>(sq_a, sq_b);
	}
}


/*
	////// blocking attack boards //////
*/
