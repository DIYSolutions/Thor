#pragma once
typedef unsigned long long U64;
typedef signed short BoardValue;

#define ENGINE_NAME "Thor"
#define ENGINE_AUTHOR "Torsten Baublies"
#define START_FEN  "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

constexpr int SEARCH_MAX_MOVES = 512;
constexpr int GAME_MAX_MOVES = 2048;
constexpr int BOARD_MAX_MOVES = 256;
constexpr int CONTAINER_MAX_MOVES = BOARD_MAX_MOVES * SEARCH_MAX_MOVES;
constexpr int MAX_THREAD = 128;
constexpr int STD_THREAD = 1;
constexpr int STD_HASHTABLE_MB = 64;// size in MB
constexpr int MAX_HASHTABLE_MB = 64 * 1024;// size in MB
constexpr int THREAD_MEMORY_MB = 3;// size in MB
constexpr int THREAD_MEMORY_SIZE = THREAD_MEMORY_MB * 1024 * 1024;// size in byte
constexpr int THREAD_MESSENGER_SIZE = 1024 * 1024;// size in byte
constexpr inline U64 getMemorySize(const U64 hash_mb, const short thread_num) { return (hash_mb  * 1024 * 1024) + (((U64)THREAD_MEMORY_SIZE) * ((U64)thread_num + 1)); };

constexpr BoardValue MIN_INFINTE = -32765;
constexpr BoardValue MAX_INFINTE = 32765;// 100 = one pawn
constexpr BoardValue MIN_MATE = -30000;
constexpr BoardValue MAX_MATE = 30000;

constexpr BoardValue DRAW_VALUE = 0;// for better code reading
constexpr BoardValue PAWN_VALUE = 100;
constexpr BoardValue KNIGHT_VALUE = 325;
constexpr BoardValue BISHOP_VALUE = 330;
constexpr BoardValue ROOK_VALUE = 450;
constexpr BoardValue QUEEN_VALUE = 900;

constexpr BoardValue AttackFaktor = 50;
constexpr BoardValue MobilityFaktor = 10;

constexpr BoardValue PIECES_VALUES[12] = {
	PAWN_VALUE,KNIGHT_VALUE,BISHOP_VALUE,ROOK_VALUE,QUEEN_VALUE,MAX_MATE,
	-PAWN_VALUE,-KNIGHT_VALUE,-BISHOP_VALUE,-ROOK_VALUE,-QUEEN_VALUE,MIN_MATE
};

constexpr BoardValue PIECES_VALUES_ABS[12] = {
	PAWN_VALUE,KNIGHT_VALUE,BISHOP_VALUE,ROOK_VALUE,QUEEN_VALUE,MAX_MATE,
	PAWN_VALUE,KNIGHT_VALUE,BISHOP_VALUE,ROOK_VALUE,QUEEN_VALUE,MAX_MATE
};

constexpr BoardValue PIECES_CAPTURE_VALUES[12] = {
	5,4,3,2,1,0,
	5,4,3,2,1,0
};

enum { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NONE };
enum { RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NONE };
constexpr char PceChar[13][1] = {
	'P','N','B','R','Q','K','p','n','b','r','q','k','-'
};

void error_exit(const char* err);
void error_exit(const char* condition, const char* date, const char* time, const char* file, int line_nr);
void print_console(const char* line);
void print_console(const char* line, int value);
void print_console(const char* line, U64 value);
void print_console(const char* line, char* value);
void printing_console_start();
void print_console_str(const char* line, signed int number);
void print_console_str(long double number);
void print_console_str(char* str);
void print_console_endl();
void printing_console_end();
void InitOutputLock();
void print_console(const char* line, BoardValue value1, int value2);
void print_console_str(const char* line, char* str);
void PrintBitboard(U64 bb);
void print_search_info(const BoardValue Score, const short Depth, U64 nodes, U64 timems);

#include <chrono>
inline U64 GetMilliTime() {
	using namespace std::chrono;
	return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}
inline U64 GetNanoTime() {
	using namespace std::chrono;
	return duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count();
}

#define DEBUG

#ifndef DEBUG
#define ASSERT(n)
#else
#define ASSERT(n) if(!(n)) { error_exit(#n,__DATE__,__TIME__,__FILE__,__LINE__); }
#endif


enum Colors { BLACK, WHITE };
enum Pieces { WhitePawn, WhiteKnight, WhiteBishop, WhiteRook, WhiteQueen, WhiteKing, BlackPawn, BlackKnight, BlackBishop, BlackRook, BlackQueen, BlackKing, Empty };

typedef enum MoveExecuteMode {
	NormalExec = 0, PawnDoubleMove = 1, Castle = 2, EnPassant = 3, Capture = 4, Promote = 5, PromoteCapture = 6, NullMove = 7
} MoveExecuteMode;

const char mod_names[8][15]{
	"NormalExec", "PawnDoubleMove", "Castle", "EnPassant", "Capture", "Promote", "PromoteCapture", "NullMove"
};
#define getModeName(m) (mod_names[m])

constexpr char piece_names[13][12] = {
	"WhitePawn", "WhiteKnight", "WhiteBishop", "WhiteRook", "WhiteQueen", "WhiteKing",
	"BlackPawn", "BlackKnight", "BlackBishop", "BlackRook", "BlackQueen", "BlackKing",
	"Empty"
};
#define getPieceName(p) (piece_names[p])

typedef enum Fields {
	A1, B1, C1, D1, E1, F1, G1, H1,
	A2, B2, C2, D2, E2, F2, G2, H2,
	A3, B3, C3, D3, E3, F3, G3, H3,
	A4, B4, C4, D4, E4, F4, G4, H4,
	A5, B5, C5, D5, E5, F5, G5, H5,
	A6, B6, C6, D6, E6, F6, G6, H6,
	A7, B7, C7, D7, E7, F7, G7, H7,
	A8, B8, C8, D8, E8, F8, G8, H8, NO_SQ
} Fields;

constexpr short FR2SQ(const short f, const short r) { return (r * 8) + f; }

constexpr char field_names[(65)][3] = {
	"a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
	"a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
	"a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
	"a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
	"a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
	"a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
	"a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
	"a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8", "na"
};
#define getFieldName(sq) (field_names[sq])

constexpr short mirrorBoard[64] = {
	63, 62, 61, 60, 59, 58, 57, 56,
	55, 54, 53, 52, 51, 50, 49, 48,
	47, 46, 45, 44, 43, 42, 41, 40,
	39, 38, 37, 36, 35, 34, 33, 32,
	31, 30, 29, 28, 27, 26, 25, 24,
	23, 22, 21, 20, 19, 18, 17, 16,
	15, 14, 13, 12, 11, 10, 9, 8,
	7, 6, 5, 4, 3, 2, 1, 0
};

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

typedef struct S_PinnedPiece {
	short pinner_sq = NO_SQ;
	short attack_sq = NO_SQ;
	U64 blockingBB = 0ULL;
	bool absolutePin = false;
	S_PinnedPiece* next = nullptr;
} S_PinnedPiece;

constexpr U64 NOMOVE = 0ULL;
