#pragma once
#include "hash.h"
#include "move_data.h"
#include "board.h"
#include "bitboard_attackboards.h"
#include "ThreadMemory.h"
#include <chrono>

typedef struct S_PinnedPiece {
	short pinner_sq = NO_SQ;
	short attack_sq = NO_SQ;
	U64 blockingBB = 0ULL;
	bool absolutePin = false;
	S_PinnedPiece* next = nullptr;
} S_PinnedPiece;
constexpr short PinnedPiece_Size = sizeof(S_PinnedPiece);
constexpr short PinnedPieceArray_Size = 64 * PinnedPiece_Size;



constexpr inline short getEnPasSQWhite(short sq) {
	return sq - 8;
}

constexpr inline short getEnPasSQBlack(short sq) {
	return sq + 8;
}

#define HASH_PCE(pce,sq) (gameboard->posKey ^= (PieceKeys[(pce)][(sq)]))
#define HASH_CA (gameboard->posKey ^= (CastleKeys[(gameboard->castlePerm)]))
#define HASH_SIDE (gameboard->posKey ^= (SideKey))
#define HASH_EP (gameboard->posKey ^= (PieceKeys[Empty][(gameboard->enPas)]))

inline U64 GetTimeMs() {
	using namespace std::chrono;
	return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

#define RETURN(Frame, value) ReleaseMemoryFrame(Frame); return value

#define NEW_MOVE(move_ptr, move_bb, score) *move_ptr->Move = move_bb;*(move_ptr++)->Score = score

class AlphaBetaClass : ThreadMemory
{
public:
	AlphaBetaClass(S_SEARCHINFO* _info): ThreadMemory(ThreadMemory_MAX, 8) {
		gameboard = new S_Chessboard();
		info = _info;
	}

	~AlphaBetaClass() {
		ThreadMemory::~ThreadMemory();
	}
	inline BoardValue NegaMax(const short maxdepth) {}

	inline BoardValue Quiescence(const BoardValue alpha, const BoardValue beta) {}

	inline BoardValue AlphaBeta(const short maxdepth) {
		return AlphaBeta(MIN_INFINTE, MAX_INFINTE, maxdepth);
	}

	inline BoardValue AlphaBeta(const BoardValue alpha, const BoardValue beta, const short maxdepth) {
		S_MemoryFrame Frame = newMemoryFrame;
		BoardValue* AlphaPtr = (BoardValue*)MemoryFrameAlloc(maxdepth * BoardValue_Size);
		BoardValue* BetaPtr = (BoardValue*)MemoryFrameAlloc(maxdepth * BoardValue_Size);
		BoardValue* CurrentMaxScorePtr = (BoardValue*)MemoryFrameAlloc(maxdepth * BoardValue_Size);
		U64* CurrentBestMovePtr = (U64*)MemoryFrameAlloc(maxdepth * U64_Size);

		int* CurrentMoveIndex = (int*)MemoryFrameAlloc(maxdepth * int_Size);
		int* BeginMoveIndex = (int*)MemoryFrameAlloc(maxdepth * int_Size);
		int* EndMoveIndex = (int*)MemoryFrameAlloc(maxdepth * int_Size);
		S_Movelist* MovePtr = (S_Movelist*)MemoryFrameAlloc(CONTAINER_MAX_MOVES * Movelist_Size);

		int current_depth = 0;
		S_Movelist* TempMovePtr;
		U64 current_move;

		BeginMoveIndex[0] = CurrentMoveIndex[0] = 0;
		if (gameboard->side == WHITE)
			TempMovePtr = GenerateLegalMoves<WHITE>(MovePtr);
		else
			TempMovePtr = GenerateLegalMoves<BLACK>(MovePtr);
		EndMoveIndex[0] = TempMovePtr - MovePtr;
		CurrentMaxScorePtr[0] = MIN_INFINTE;
		AlphaPtr[0] = alpha;
		BetaPtr[0] = beta;
		while (CheckUp()) {
			// Is true when at current depth all moves where played
			if (CurrentMoveIndex[current_depth] == EndMoveIndex[current_depth]) {
				// Is true when all moves at maxdepth where played
				gameboard->score = CurrentMaxScorePtr[current_depth];

				if (current_depth == 0)
					goto return_score;

				current_depth--;

				if (gameboard->side == WHITE)
					undoMove<BLACK>();
				else
					undoMove<WHITE>();
				
				goto PositionScore;
			}

			current_move = MovePtr[(CurrentMoveIndex[current_depth++])++].Move;
			if (gameboard->side == WHITE)
				doMove<WHITE>(current_move);
			else
				doMove<BLACK>(current_move);

			// set alpha beta at current depth
			AlphaPtr[current_depth] = -BetaPtr[current_depth - 1];
			BetaPtr[current_depth] = -AlphaPtr[current_depth - 1];
			// Is true when maxdepth is reached
			if (current_depth == maxdepth) {

				gameboard->score = Quiescence(AlphaPtr[current_depth], BetaPtr[current_depth]);
				current_depth--;
				
				if (gameboard->side == WHITE)
					undoMove<BLACK>();
				else
					undoMove<WHITE>();

				goto PositionScore;
			}

			info->nodes++;
			BeginMoveIndex[current_depth] = CurrentMoveIndex[current_depth] = EndMoveIndex[current_depth - 1];
			
			CurrentMaxScorePtr[current_depth] = MIN_INFINTE;

			if (gameboard->side == WHITE)
				TempMovePtr = GenerateLegalMoves<WHITE>(MovePtr);
			else
				TempMovePtr = GenerateLegalMoves<BLACK>(MovePtr);
			
			EndMoveIndex[current_depth] = TempMovePtr - MovePtr;

		PositionScore:
			if (gameboard->score > CurrentMaxScorePtr[current_depth]) {
				CurrentBestMovePtr[current_depth] = current_move;
				CurrentMaxScorePtr[current_depth] = gameboard->score;
				if (gameboard->score > AlphaPtr[current_depth]) {
					AlphaPtr[current_depth] = gameboard->score;
				}
			}
			if (gameboard->score >= BetaPtr[current_depth]) {
				if (CurrentMoveIndex[current_depth] == 1)
					info->fhf++;
				info->fh++;
				StoreHashEntry(gameboard, CurrentBestMovePtr[current_depth], BetaPtr[current_depth], HFBETA, maxdepth - current_depth);
				// fail-soft beta-cutoff
				if (current_depth == 0)
					goto return_score;

				current_depth--;

				if (gameboard->side == WHITE)
					undoMove<BLACK>();
				else
					undoMove<WHITE>();

			}
		}
		RETURN(Frame, DRAW_VALUE);
	return_score:
		RETURN(Frame, gameboard->score);
	}

	inline void ResetBoard(void) {
		gameboard->posKey = gameboard->piecesBB[WhitePawn] = gameboard->piecesBB[WhiteKnight] = gameboard->piecesBB[WhiteBishop] = gameboard->piecesBB[WhiteRook] = gameboard->piecesBB[WhiteQueen] = gameboard->piecesBB[WhiteKing] = gameboard->piecesBB[BlackPawn] = gameboard->piecesBB[BlackKnight] = gameboard->piecesBB[BlackBishop] = gameboard->piecesBB[BlackRook] = gameboard->piecesBB[BlackQueen] = gameboard->piecesBB[BlackKing] = 0ULL;
		gameboard->score = gameboard->hisPly = gameboard->ply = gameboard->fiftyMove = gameboard->castlePerm = 0;
		gameboard->enPas = NO_SQ;
	}

	inline short getPieceType(U64* piecesListBB, int sq) {
		int pieceType = getWhitePieceType(piecesListBB, sq);
		if (pieceType == Empty)
			return getBlackPieceType(piecesListBB, sq);
		return pieceType;
	}

	inline bool FEN(char * fen) {

		int  rank = RANK_8;
		int  file = FILE_A;
		int piece = Empty;
		int  count = 0;
		int  i = 0;
		int  sq64 = 0;

		ResetBoard();
		while ((rank >= RANK_1) && *fen) {
			count = 1;
			switch (*fen) {
			case 'p': piece = BlackPawn; break;
			case 'r': piece = BlackRook; break;
			case 'n': piece = BlackKnight; break;
			case 'b': piece = BlackBishop; break;
			case 'k': piece = BlackKing; break;
			case 'q': piece = BlackQueen; break;
			case 'P': piece = WhitePawn; break;
			case 'R': piece = WhiteRook; break;
			case 'N': piece = WhiteKnight; break;
			case 'B': piece = WhiteBishop; break;
			case 'K': piece = WhiteKing; break;
			case 'Q': piece = WhiteQueen; break;

			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
				piece = Empty;
				count = *fen - '0';
				break;

			case '/':
			case ' ':
				rank--;
				file = FILE_A;
				fen++;
				continue;

			default:
				//std::cout << "FEN error \n";
				return false;
			}

			for (i = 0; i < count; i++) {
				sq64 = rank * 8 + file;

				if (piece != Empty) {
					newPiece(gameboard, piece, sq64);
					gameboard->posKey ^= PieceKeys[piece][sq64];
				}

				file++;
			}
			fen++;
		}

		//ASSERT(*fen == 'w' || *fen == 'b');

		gameboard->side = BLACK;
		if (*fen == 'w') {
			gameboard->side = WHITE;
			gameboard->posKey ^= SideKey;
		}

		fen += 2;
		for (i = 0; i < 4; i++) {
			if (*fen == ' ') {
				break;
			}
			switch (*fen) {
			case 'K': gameboard->castlePerm |= WKCA; break;
			case 'Q': gameboard->castlePerm |= WQCA; break;
			case 'k': gameboard->castlePerm |= BKCA; break;
			case 'q': gameboard->castlePerm |= BQCA; break;
			default:	     break;
			}
			fen++;
		}
		fen++;
		gameboard->posKey ^= CastleKeys[gameboard->castlePerm];

		//ASSERT(gameboard->CastlePerm >= 0 && gameboard->CastlePerm <= 15);

		if (*fen != '-') {
			file = fen[0] - 'a';
			rank = fen[1] - '1';

			//ASSERT(file >= FILE_A && file <= FILE_H);
			//ASSERT(rank >= RANK_1 && rank <= RANK_8);

			gameboard->enPas = FR2SQ(file, rank);
			gameboard->posKey ^= PieceKeys[Empty][gameboard->enPas];
		}
		else
			gameboard->enPas = NO_SQ;

		return true;
	}

	inline char* FEN(void) {

		char* fen = fen_line;
		char* fen_start = fen_line;
		int  rank = RANK_8;
		int  file = FILE_A;

		int numberEmpty = 0;
		int oldRank = RANK_8;
		for (int r = 7; r >= 0; r--) {
			for (int f = 0; f < 8; f++) {
				int sq = FR2SQ(f, r);
				if (r != oldRank) {
					if (numberEmpty) {
						*(fen++) = ('0' + numberEmpty);
						numberEmpty = 0;
					}
					*(fen++) = 47;
					oldRank = r;
				}
				switch (getPieceType(gameboard->piecesBB, sq)) {
				case BlackPawn:
					if (numberEmpty) {
						*(fen++) = ('0' + numberEmpty);
						numberEmpty = 0;
					}
					*(fen++) = 'p';
					break;
				case BlackKnight:
					if (numberEmpty) {
						*(fen++) = ('0' + numberEmpty);
						numberEmpty = 0;
					}
					*(fen++) = 'n';
					break;
				case BlackBishop:
					if (numberEmpty) {
						*(fen++) = ('0' + numberEmpty);
						numberEmpty = 0;
					}
					*(fen++) = 'b';
					break;
				case BlackRook:
					if (numberEmpty) {
						*(fen++) = ('0' + numberEmpty);
						numberEmpty = 0;
					}
					*(fen++) = 'r';
					break;
				case BlackQueen:
					if (numberEmpty) {
						*(fen++) = ('0' + numberEmpty);
						numberEmpty = 0;
					}
					*(fen++) = 'q';
					break;
				case BlackKing:
					if (numberEmpty) {
						*(fen++) = ('0' + numberEmpty);
						numberEmpty = 0;
					}
					*(fen++) = 'k';
					break;
				case WhitePawn:
					if (numberEmpty) {
						*(fen++) = ('0' + numberEmpty);
						numberEmpty = 0;
					}
					*(fen++) = 'P';
					break;
				case WhiteKnight:
					if (numberEmpty) {
						*(fen++) = ('0' + numberEmpty);
						numberEmpty = 0;
					}
					*(fen++) = 'N';
					break;
				case WhiteBishop:
					if (numberEmpty) {
						*(fen++) = ('0' + numberEmpty);
						numberEmpty = 0;
					}
					*(fen++) = 'B';
					break;
				case WhiteRook:
					if (numberEmpty) {
						*(fen++) = ('0' + numberEmpty);
						numberEmpty = 0;
					}
					*(fen++) = 'R';
					break;
				case WhiteQueen:
					if (numberEmpty) {
						*(fen++) = ('0' + numberEmpty);
						numberEmpty = 0;
					}
					*(fen++) = 'Q';
					break;
				case WhiteKing:
					if (numberEmpty) {
						*(fen++) = ('0' + numberEmpty);
						numberEmpty = 0;
					}
					*(fen++) = 'K';
					break;
				case Empty:
					numberEmpty++;
					break;
				}
			}
		}
		if (numberEmpty) {
			*(fen++) = ('0' + numberEmpty);
			numberEmpty = 0;
		}
		*(fen++) = 32;

		*(fen++) = gameboard->side == BLACK ? 98 : 119;

		*(fen++) = 32;

		char* test = fen;
		if (gameboard->castlePerm & WKCA) *(fen++) = 75;
		if (gameboard->castlePerm & WQCA) *(fen++) = 81;
		if (gameboard->castlePerm & BKCA) *(fen++) = 107;
		if (gameboard->castlePerm & BQCA) *(fen++) = 113;
		if (test == fen) *(fen++) = '-';

		*(fen++) = 32;
		if (gameboard->enPas != NO_SQ) {
			file = gameboard->enPas % 8;
			rank = gameboard->enPas / 8;

			*(fen++) = 'a' + file;
			*(fen++) = '1' + rank;
		}
		else
			*(fen++) = '-';

		*(fen++) = ' ';
		*(fen++) = '0';
		*(fen++) = ' ';
		*(fen++) = '1';
		*(fen++) = 0;
		return fen_line;
	}

private:
	
	S_Chessboard* gameboard;
	S_SEARCHINFO* info;
	char fen_line[256] = "                                                                                                                                                                                                                                                               ";


	inline bool CheckUp() {
		if (info->nodes % 1024) {
			// .. check if time up, or interrupt from GUI
			if (info->timeset && GetTimeMs() > info->stoptime) {
				info->stopped = true;
				return false;
			}
			else if (info->stopped) {
				return false;
			}
		}
		return true;
	}

	template <Colors Us>
	inline void doMove(const U64 move) {
		constexpr Colors Them = Us == WHITE ? BLACK : WHITE;
		constexpr Fields CastleKingSideSQ = Us == WHITE ? G1 : G8;
		constexpr Fields CastleKingSideRStartSQ = Us == WHITE ? H1 : H8;
		constexpr Fields CastleKingSideRStopSQ = Us == WHITE ? F1 : F8;
		constexpr Fields CastleQueenSideSQ = Us == WHITE ? C1 : C8;
		constexpr Fields CastleQueenSideRStartSQ = Us == WHITE ? A1 : A8;
		constexpr Fields CastleQueenSideRStopSQ = Us == WHITE ? D1 : D8;
		constexpr Pieces myKING = Us == WHITE ? WhiteKing : BlackKing;
		constexpr Pieces myROOK = Us == WHITE ? WhiteRook : BlackRook;
		constexpr Pieces myPAWN = Us == WHITE ? WhitePawn : BlackPawn;
		constexpr Pieces enPAWN = Us == WHITE ? BlackPawn : WhitePawn;
		constexpr int (*getEnPasSQ)(int) = (Us == WHITE ? getEnPasSQWhite : getEnPasSQBlack);

		int fromSQ = MOVE_FROMSQ(move), toSQ = MOVE_TOSQ(move);
		gameboard->side = Them;
		gameboard->history[gameboard->hisPly++] = move;
		gameboard->ply++;

		int moved = MOVE_MOVED(move), promoted, captured, enPas;
		HASH_PCE(moved, fromSQ);
		HASH_PCE(moved, toSQ);
		HASH_SIDE;
		if (gameboard->enPas != NO_SQ) {
			HASH_EP;
			gameboard->enPas = NO_SQ;
		}

		switch (MOVE_MODE(move))
		{
		case NormalExec:
			movePiece(gameboard->piecesBB, moved, fromSQ, toSQ);
			gameboard->fiftyMove++;
			HASH_CA;
			gameboard->castlePerm = gameboard->castlePerm & CastlePerm[fromSQ] & CastlePerm[toSQ];
			HASH_CA;
			return;
		case PawnDoubleMove:
			movePiece(gameboard->piecesBB, myPAWN, fromSQ, toSQ);
			gameboard->fiftyMove = 0;
			gameboard->enPas = getEnPasSQ(toSQ);
			HASH_EP;
			return;
		case Castle:
			movePiece(gameboard->piecesBB, myKING, fromSQ, toSQ);
			if (CastleKingSideSQ == toSQ) {
				HASH_PCE(myROOK, CastleKingSideRStartSQ);
				HASH_PCE(myROOK, CastleKingSideRStopSQ);
				movePiece(gameboard->piecesBB, myROOK, CastleKingSideRStartSQ, CastleKingSideRStopSQ);
			}
			else {
				HASH_PCE(myROOK, CastleQueenSideRStartSQ);
				HASH_PCE(myROOK, CastleQueenSideRStopSQ);
				movePiece(gameboard->piecesBB, myROOK, CastleQueenSideRStartSQ, CastleQueenSideRStopSQ);
			}
			gameboard->fiftyMove++;
			HASH_CA;
			gameboard->castlePerm = gameboard->castlePerm & CastlePerm[fromSQ] & CastlePerm[toSQ];
			HASH_CA;
			return;
		case EnPassant:
			enPas = getEnPasSQ(toSQ);
			HASH_PCE(enPAWN, enPas);
			movePiece(gameboard->piecesBB, myPAWN, fromSQ, toSQ);
			delPiece(gameboard->piecesBB, enPAWN, enPas);
			gameboard->fiftyMove = 0;
			return;
		case Capture:
			captured = MOVE_CAPTURED(move);
			delPiece(gameboard->piecesBB, captured, toSQ);
			HASH_PCE(captured, toSQ);
			movePiece(gameboard->piecesBB, moved, fromSQ, toSQ);
			gameboard->fiftyMove = 0;
			HASH_CA;
			gameboard->castlePerm = gameboard->castlePerm & CastlePerm[fromSQ] & CastlePerm[toSQ];
			HASH_CA;
			return;
		case Promote:
			promoted = MOVE_PROMOTED(move);
			movePiece(gameboard->piecesBB, myPAWN, fromSQ, toSQ);
			modPiece(gameboard->piecesBB, myPAWN, promoted, toSQ);
			HASH_PCE(myPAWN, toSQ);
			HASH_PCE(promoted, toSQ);
			gameboard->fiftyMove = 0;
			return;
		case PromoteCapture:
			promoted = MOVE_PROMOTED(move);
			captured = MOVE_CAPTURED(move);
			delPiece(gameboard->piecesBB, captured, toSQ);
			movePiece(gameboard->piecesBB, myPAWN, fromSQ, toSQ);
			modPiece(gameboard->piecesBB, myPAWN, promoted, toSQ);
			HASH_PCE(myPAWN, toSQ);
			HASH_PCE(captured, toSQ);
			HASH_PCE(promoted, toSQ);
			gameboard->fiftyMove = 0;
			HASH_CA;
			gameboard->castlePerm = gameboard->castlePerm & CastlePerm[fromSQ] & CastlePerm[toSQ];
			HASH_CA;
			return;
		default:
			print_console("MODE: %d\n", MOVE_MODE(move));
			print_console("MOVED: %d\n", MOVE_MOVED(move));
			print_console("CAPTURED: %d\n", MOVE_CAPTURED(move));
			print_console("PROMOTED: %d\n", MOVE_PROMOTED(move));
			print_console("FROMSQ: %d\n", MOVE_FROMSQ(move));
			print_console("TOSQ: %d\n", MOVE_TOSQ(move));
			ASSERT(false);
		}

	}

	template <Colors Us>
	inline void undoMove(void) {
		constexpr Fields CastleKingSideSQ = Us == WHITE ? G1 : G8;
		constexpr Fields CastleKingSideRStartSQ = Us == WHITE ? H1 : H8;
		constexpr Fields CastleKingSideRStopSQ = Us == WHITE ? F1 : F8;
		constexpr Fields CastleQueenSideSQ = Us == WHITE ? C1 : C8;
		constexpr Fields CastleQueenSideRStartSQ = Us == WHITE ? A1 : A8;
		constexpr Fields CastleQueenSideRStopSQ = Us == WHITE ? D1 : D8;
		constexpr Pieces myKING = Us == WHITE ? WhiteKing : BlackKing;
		constexpr Pieces myROOK = Us == WHITE ? WhiteRook : BlackRook;
		constexpr Pieces myPAWN = Us == WHITE ? WhitePawn : BlackPawn;
		constexpr Pieces enPAWN = Us == WHITE ? BlackPawn : WhitePawn;
		constexpr int (*getEnPasSQ)(int) = (Us == WHITE ? getEnPasSQWhite : getEnPasSQBlack);

		U64 move = gameboard->history[--gameboard->hisPly];

		int fromSQ = MOVE_FROMSQ(move), toSQ = MOVE_TOSQ(move);
		gameboard->fiftyMove = MOVE_FIFTY_MOVE(move);
		if (gameboard->enPas != NO_SQ) HASH_EP;
		gameboard->enPas = MOVE_ENPAS(move);
		if (gameboard->enPas != NO_SQ) HASH_EP;
		HASH_CA;
		gameboard->castlePerm = MOVE_CASTLE_PERM(move);
		HASH_CA;
		gameboard->ply--;
		gameboard->side = Us;
		HASH_SIDE;

		int moved = MOVE_MOVED(move), promoted, captured, enPas;

		switch (MOVE_MODE(move))
		{
		case NormalExec:
			movePiece(gameboard->piecesBB, moved, toSQ, fromSQ);
			HASH_PCE(moved, fromSQ);
			HASH_PCE(moved, toSQ);
			return;
		case PawnDoubleMove:
			movePiece(gameboard->piecesBB, myPAWN, toSQ, fromSQ);
			HASH_PCE(myPAWN, fromSQ);
			HASH_PCE(myPAWN, toSQ);
			return;
		case Castle:
			movePiece(gameboard->piecesBB, myKING, toSQ, fromSQ);
			HASH_PCE(myKING, fromSQ);
			HASH_PCE(myKING, toSQ);
			if (CastleKingSideSQ == toSQ) {
				HASH_PCE(myROOK, CastleKingSideRStartSQ);
				HASH_PCE(myROOK, CastleKingSideRStopSQ);
				movePiece(gameboard->piecesBB, myROOK, CastleKingSideRStopSQ, CastleKingSideRStartSQ);
			}
			else {
				HASH_PCE(myROOK, CastleQueenSideRStartSQ);
				HASH_PCE(myROOK, CastleQueenSideRStopSQ);
				movePiece(gameboard->piecesBB, myROOK, CastleQueenSideRStopSQ, CastleQueenSideRStartSQ);
			}
			return;
		case EnPassant:
			HASH_PCE(myPAWN, fromSQ);
			HASH_PCE(myPAWN, toSQ);
			enPas = getEnPasSQ(toSQ);
			HASH_PCE(enPAWN, enPas);
			movePiece(gameboard->piecesBB, myPAWN, toSQ, fromSQ);
			newPiece(gameboard->piecesBB, enPAWN, enPas);
			return;
		case Capture:
			captured = MOVE_CAPTURED(move);
			HASH_PCE(moved, fromSQ);
			HASH_PCE(moved, toSQ);
			HASH_PCE(captured, toSQ);
			movePiece(gameboard->piecesBB, moved, toSQ, fromSQ);
			newPiece(gameboard->piecesBB, captured, toSQ);
			return;
		case Promote:
			HASH_PCE(myPAWN, fromSQ);
			promoted = MOVE_PROMOTED(move);
			HASH_PCE(promoted, toSQ);
			modPiece(gameboard->piecesBB, promoted, myPAWN, toSQ);
			movePiece(gameboard->piecesBB, myPAWN, toSQ, fromSQ);
			return;
		case PromoteCapture:
			HASH_PCE(myPAWN, fromSQ);
			promoted = MOVE_PROMOTED(move);
			captured = MOVE_CAPTURED(move);
			HASH_PCE(captured, toSQ);
			HASH_PCE(promoted, toSQ);
			modPiece(gameboard->piecesBB, promoted, myPAWN, toSQ);
			movePiece(gameboard->piecesBB, myPAWN, toSQ, fromSQ);
			newPiece(gameboard->piecesBB, captured, toSQ);
			return;
		default:
			ASSERT(false);
		}
	}

	/*
	get pin for given attack_sq
	*/
	template <Colors Us, bool Absolute>
	inline void getPinByAttackSQ(short attack_sq, S_PinnedPiece* PinsBySquare) {
		constexpr Pieces enBISHOP = Us == BLACK ? WhiteBishop : BlackBishop;
		constexpr Pieces enROOK = Us == BLACK ? WhiteRook : BlackRook;
		constexpr Pieces enQUEEN = Us == BLACK ? WhiteQueen : BlackQueen;
		
		kingBishopAttackBB = magicGetBishopAttackBB(attack_sq, allPiecesBB) & friends;
		kingRookAttackBB = magicGetRookAttackBB(attack_sq, allPiecesBB) & friends;

		while (kingBishopAttackBB) {
			fromSQ = PopBit(&kingBishopAttackBB);
			Directions pinDir = getBishopDirection(attack_sq, fromSQ);
			ClearBit(&allPiecesBB, fromSQ);
			U64 newAttackBB = magicGetBishopAttackBB(attack_sq, allPiecesBB) & enemys & getRay(pinDir, attack_sq);

			if (newAttackBB & (gameboard->piecesBB[enBISHOP] | gameboard->piecesBB[enQUEEN])) {
				S_PinnedPiece* temp = (S_PinnedPiece*)MemoryFrameAlloc(PinnedPiece_Size);
				temp->absolutePin = Absolute;
				temp->attack_sq = attack_sq;
				temp->pinner_sq = PopBit(newAttackBB);
				temp->blockingBB = getRay(pinDir, myKing_sq) & getRay(getOppositeDir(pinDir), attack_sq) & ClearMask[fromSQ];
				temp->next = PinsBySquare[fromSQ];
				PinsBySquare[fromSQ] = temp;
			}

			SetBit(&allPiecesBB, fromSQ);
		}

		while (kingRookAttackBB) {
			fromSQ = PopBit(&kingRookAttackBB);
			Directions pinDir = getRookDirection(attack_sq, fromSQ);
			ClearBit(&allPiecesBB, fromSQ);
			U64 newAttackBB = magicGetRookAttackBB(attack_sq, allPiecesBB) & enemys & getRay(pinDir, attack_sq);

			if (newAttackBB & (gameboard->piecesBB[enROOK] | gameboard->piecesBB[enQUEEN])) {
				S_PinnedPiece* temp = (S_PinnedPiece*)MemoryFrameAlloc(PinnedPiece_Size);
				temp->absolutePin = Absolute;
				temp->attack_sq = attack_sq;
				temp->pinner_sq = PopBit(newAttackBB);
				temp->blockingBB = getRay(pinDir, myKing_sq) & getRay(getOppositeDir(pinDir), attack_sq) & ClearMask[fromSQ];
				temp->next = PinsBySquare[fromSQ];
				PinsBySquare[fromSQ] = temp;
			}

			SetBit(&allPiecesBB, fromSQ);
		}
	}

	/*
	get attacks from both sides
	get attacks to the king
	get possible blocking squares
	get non absolute pins and blockingSQ for eval usage
	*/
	template <Colors Us, Pieces PieceType>
	inline void getPieceTypeAttackBB(U64 piecesBB, S_PinnedPiece* PinsBySquare) {}
	/*
		getPieceTypeAttackBB specialized TEMPLATES
	*/
#if true


	template <>
	inline void getPieceTypeAttackBB<WHITE, WhitePawn>(U64 piecesBB, S_PinnedPiece* PinsBySquare) {
		getPieceTypeAttackBB<WHITE, WhiteKnight>(gameboard->piecesBB[WhiteKnight], PinsBySquare);

		while (piecesBB)
			MyAttackBB |= AttackBrdwPawnBB[PopBit(&piecesBB)];
	}
	template <>
	inline void getPieceTypeAttackBB<BLACK, WhitePawn>(U64 piecesBB, S_PinnedPiece* PinsBySquare) {
		getPieceTypeAttackBB<BLACK, WhiteKnight>(gameboard->piecesBB[WhiteKnight], PinsBySquare);

		ClearBit(&allPiecesBB, myKing_sq);// if in check by bishop, rook or queen then we must check attack without king!
		while (piecesBB) {
			fromSQ = PopBit(&piecesBB);
			PieceAttacksBB = AttackBrdwPawnBB[fromSQ];
			if (PieceAttacksBB & myKingBB) {
				SetBit(&CheckersBB, fromSQ);
				checkersNum++;
				checkersType = WhitePawn;
			}
			EnAttackBB |= PieceAttacksBB;
		}
	}

	template <>
	inline void getPieceTypeAttackBB<WHITE, WhiteKnight>(U64 piecesBB, S_PinnedPiece* PinsBySquare) {
		getPieceTypeAttackBB<WHITE, WhiteBishop>(gameboard->piecesBB[WhiteBishop], PinsBySquare);

		while (piecesBB)
			MyAttackBB |= AttackBrdKnightBB[PopBit(&piecesBB)];
	}
	template <>
	inline void getPieceTypeAttackBB<BLACK, WhiteKnight>(U64 piecesBB, S_PinnedPiece* PinsBySquare) {
		getPieceTypeAttackBB<BLACK, WhiteBishop>(gameboard->piecesBB[WhiteBishop], PinsBySquare);

		while (piecesBB) {
			fromSQ = PopBit(&piecesBB);
			PieceAttacksBB = AttackBrdKnightBB[fromSQ];
			if (PieceAttacksBB & myKingBB) {
				SetBit(&CheckersBB, fromSQ);
				checkersNum++;
				checkersType = WhiteKnight;
			}
			EnAttackBB |= PieceAttacksBB;
		}
	}

	template <>
	inline void getPieceTypeAttackBB<WHITE, WhiteBishop>(U64 piecesBB, S_PinnedPiece* PinsBySquare) {
		getPieceTypeAttackBB<WHITE, WhiteRook>(gameboard->piecesBB[WhiteRook], PinsBySquare);

		while (piecesBB) {
			tempAttackBB = magicGetBishopAttackBB(PopBit(&piecesBB), allPiecesBB);
			MyAttackBB |= tempAttackBB;
			// am i pinning someone?


			
		}
	}
	template <>
	inline void getPieceTypeAttackBB<BLACK, WhiteBishop>(U64 piecesBB, S_PinnedPiece* PinsBySquare) {
		getPieceTypeAttackBB<BLACK, WhiteRook>(gameboard->piecesBB[WhiteRook], PinsBySquare);

		while (piecesBB) {
			fromSQ = PopBit(&piecesBB);
			PieceAttacksBB = magicGetBishopAttackBB(fromSQ, allPiecesBB);
			if (PieceAttacksBB & myKingBB) {
				SetBit(&CheckersBB, fromSQ);
				checkersNum++;
				blockingSQ_BB = getBishopBlockingSQ_BB(fromSQ, myKing_sq);
				checkersType = WhiteBishop;
			}
			EnAttackBB |= PieceAttacksBB;
		}
	}

	template <>
	inline void getPieceTypeAttackBB<WHITE, WhiteRook>(U64 piecesBB, S_PinnedPiece* PinsBySquare) {
		getPieceTypeAttackBB<WHITE, WhiteQueen>(gameboard->piecesBB[WhiteQueen], PinsBySquare);
		
		while (piecesBB) {
			fromSQ = PopBit(&piecesBB);
			MyAttackBB |= magicGetRookAttackBB(fromSQ, allPiecesBB);
			getPinByAttackSQ<WHITE, false>(fromSQ, PinsBySquare);
		}
	}
	template <>
	inline void getPieceTypeAttackBB<BLACK, WhiteRook>(U64 piecesBB, S_PinnedPiece* PinsBySquare) {
		getPieceTypeAttackBB<BLACK, WhiteQueen>(gameboard->piecesBB[WhiteQueen], PinsBySquare);

		while (piecesBB) {
			fromSQ = PopBit(&piecesBB);
			PieceAttacksBB = magicGetRookAttackBB(fromSQ, allPiecesBB);
			if (PieceAttacksBB & myKingBB) {
				SetBit(&CheckersBB, fromSQ);
				checkersNum++;
				blockingSQ_BB = getRookBlockingSQ_BB(fromSQ, myKing_sq);
				checkersType = WhiteRook;
			}
			EnAttackBB |= PieceAttacksBB;
			getPinByAttackSQ<WHITE, false>(fromSQ, PinsBySquare);
		}
	}

	template <>
	inline void getPieceTypeAttackBB<WHITE, WhiteQueen>(U64 piecesBB, S_PinnedPiece* PinsBySquare) {
		getPieceTypeAttackBB<WHITE, WhiteKing>(gameboard->piecesBB[WhiteKing], PinsBySquare);

		while (piecesBB) {
			fromSQ = PopBit(&piecesBB);
			MyAttackBB |= magicGetQueenAttackBB(fromSQ, allPiecesBB);
			getPinByAttackSQ<WHITE, false>(fromSQ, PinsBySquare);
		}
	}
	template <>
	inline void getPieceTypeAttackBB<BLACK, WhiteQueen>(U64 piecesBB, S_PinnedPiece* PinsBySquare) {
		getPieceTypeAttackBB<BLACK, WhiteKing>(gameboard->piecesBB[WhiteKing], PinsBySquare);

		while (piecesBB) {
			fromSQ = PopBit(&piecesBB);
			PieceAttacksBB = magicGetQueenAttackBB(fromSQ, allPiecesBB);
			if (PieceAttacksBB & myKingBB) {
				SetBit(&CheckersBB, fromSQ);
				checkersNum++;
				blockingSQ_BB = getBlockingSQ_BB(fromSQ, myKing_sq);
				checkersType = WhiteQueen;
			}
			EnAttackBB |= PieceAttacksBB;
			getPinByAttackSQ<WHITE, false>(fromSQ, PinsBySquare);
		}
	}

	template <>
	inline void getPieceTypeAttackBB<WHITE, WhiteKing>(U64 piecesBB, S_PinnedPiece* PinsBySquare) {
		getPieceTypeAttackBB<WHITE, BlackPawn>(gameboard->piecesBB[BlackPawn], PinsBySquare);

		MyAttackBB |= AttackBrdKingBB[myKing_sq];
		getPinByAttackSQ<WHITE, true>(myKing_sq, PinsBySquare);
	}
	template <>
	inline void getPieceTypeAttackBB<BLACK, WhiteKing>(U64 piecesBB, S_PinnedPiece* PinsBySquare) {
		getPieceTypeAttackBB<BLACK, BlackPawn>(gameboard->piecesBB[BlackPawn], PinsBySquare);

		fromSQ = PopBit(&piecesBB);
		EnAttackBB |= AttackBrdKingBB[fromSQ];
		getPinByAttackSQ<WHITE, true>(fromSQ, PinsBySquare);
		SetBit(&allPiecesBB, myKing_sq);// put king back
	}


	template <>
	inline void getPieceTypeAttackBB<WHITE, BlackPawn>(U64 piecesBB, S_PinnedPiece* PinsBySquare) {
		getPieceTypeAttackBB<WHITE, BlackKnight>(gameboard->piecesBB[BlackKnight], PinsBySquare);

		ClearBit(&allPiecesBB, myKing_sq);// if in check by bishop, rook or queen then we must check attack without king!
		while (piecesBB) {
			fromSQ = PopBit(&piecesBB);
			PieceAttacksBB = AttackBrdbPawnBB[fromSQ];
			if (PieceAttacksBB & myKingBB) {
				SetBit(&CheckersBB, fromSQ);
				checkersNum++;
				checkersType = BlackPawn;
			}
			EnAttackBB |= PieceAttacksBB;
		}
	}
	template <>
	inline void getPieceTypeAttackBB<BLACK, BlackPawn>(U64 piecesBB, S_PinnedPiece* PinsBySquare) {
		getPieceTypeAttackBB<BLACK, BlackKnight>(gameboard->piecesBB[BlackKnight], PinsBySquare);

		while (piecesBB)
			MyAttackBB |= AttackBrdKnightBB[PopBit(&piecesBB)];
	}

	template <>
	inline void getPieceTypeAttackBB<WHITE, BlackKnight>(U64 piecesBB, S_PinnedPiece* PinsBySquare) {
		getPieceTypeAttackBB<WHITE, BlackBishop>(gameboard->piecesBB[BlackBishop], PinsBySquare);

		while (piecesBB) {
			fromSQ = PopBit(&piecesBB);
			PieceAttacksBB = AttackBrdKnightBB[fromSQ];
			if (PieceAttacksBB & myKingBB) {
				SetBit(&CheckersBB, fromSQ);
				checkersNum++;
				checkersType = BlackKnight;
			}
			EnAttackBB |= PieceAttacksBB;
		}
	}
	template <>
	inline void getPieceTypeAttackBB<BLACK, BlackKnight>(U64 piecesBB, S_PinnedPiece* PinsBySquare) {
		getPieceTypeAttackBB<BLACK, BlackBishop>(gameboard->piecesBB[BlackBishop], PinsBySquare);

		while (piecesBB)
			MyAttackBB |= AttackBrdKnightBB[PopBit(&piecesBB)];
	}

	template <>
	inline void getPieceTypeAttackBB<WHITE, BlackBishop>(U64 piecesBB, S_PinnedPiece* PinsBySquare) {
		getPieceTypeAttackBB<WHITE, BlackRook>(gameboard->piecesBB[BlackRook], PinsBySquare);

		while (piecesBB) {
			fromSQ = PopBit(&piecesBB);
			PieceAttacksBB = magicGetBishopAttackBB(fromSQ, allPiecesBB);
			if (PieceAttacksBB & myKingBB) {
				SetBit(&CheckersBB, fromSQ);
				checkersNum++;
				blockingSQ_BB = getBishopBlockingSQ_BB(fromSQ, myKing_sq);
				checkersType = BlackBishop;
			}
			EnAttackBB |= PieceAttacksBB;
		}
	}
	template <>
	inline void getPieceTypeAttackBB<BLACK, BlackBishop>(U64 piecesBB, S_PinnedPiece* PinsBySquare) {
		getPieceTypeAttackBB<BLACK, BlackRook>(gameboard->piecesBB[BlackRook], PinsBySquare);

		while (piecesBB)
			MyAttackBB |= magicGetBishopAttackBB(PopBit(&piecesBB), allPiecesBB);
	}

	template <>
	inline void getPieceTypeAttackBB<WHITE, BlackRook>(U64 piecesBB, S_PinnedPiece* PinsBySquare) {
		getPieceTypeAttackBB<WHITE, BlackQueen>(gameboard->piecesBB[BlackQueen], PinsBySquare);

		while (piecesBB) {
			fromSQ = PopBit(&piecesBB);
			PieceAttacksBB = magicGetRookAttackBB(fromSQ, allPiecesBB);
			if (PieceAttacksBB & myKingBB) {
				SetBit(&CheckersBB, fromSQ);
				checkersNum++;
				blockingSQ_BB = getRookBlockingSQ_BB(fromSQ, myKing_sq);
				checkersType = BlackRook;
			}
			EnAttackBB |= PieceAttacksBB;
			getPinByAttackSQ<BLACK, false>(fromSQ, PinsBySquare);

		}
	}
	template <>
	inline void getPieceTypeAttackBB<BLACK, BlackRook>(U64 piecesBB, S_PinnedPiece* PinsBySquare) {
		getPieceTypeAttackBB<BLACK, BlackQueen>(gameboard->piecesBB[BlackQueen], PinsBySquare);

		while (piecesBB) {
			fromSQ = PopBit(&piecesBB);
			MyAttackBB |= magicGetRookAttackBB(fromSQ, allPiecesBB);
			getPinByAttackSQ<BLACK, false>(fromSQ, PinsBySquare);
		}
	}

	template <>
	inline void getPieceTypeAttackBB<WHITE, BlackQueen>(U64 piecesBB, S_PinnedPiece* PinsBySquare) {
		getPieceTypeAttackBB<WHITE, BlackKing>(gameboard->piecesBB[BlackKing], PinsBySquare);

		while (piecesBB) {
			fromSQ = PopBit(&piecesBB);
			PieceAttacksBB = magicGetQueenAttackBB(fromSQ, allPiecesBB);
			if (PieceAttacksBB & myKingBB) {
				SetBit(&CheckersBB, fromSQ);
				checkersNum++;
				blockingSQ_BB = getBlockingSQ_BB(fromSQ, myKing_sq);
				checkersType = BlackQueen;
			}
			EnAttackBB |= PieceAttacksBB;
			getPinByAttackSQ<BLACK, false>(fromSQ, PinsBySquare);
		}
	}
	template <>
	inline void getPieceTypeAttackBB<BLACK, BlackQueen>(U64 piecesBB, S_PinnedPiece* PinsBySquare) {
		getPieceTypeAttackBB<BLACK, BlackKing>(gameboard->piecesBB[BlackKing], PinsBySquare);

		while (piecesBB) {
			fromSQ = PopBit(&piecesBB);
			MyAttackBB |= magicGetQueenAttackBB(fromSQ, allPiecesBB);
			getPinByAttackSQ<BLACK, false>(fromSQ, PinsBySquare);
		}
	}

	template <>
	inline void getPieceTypeAttackBB<WHITE, BlackKing>(U64 piecesBB, S_PinnedPiece* PinsBySquare) {
		fromSQ = PopBit(&piecesBB);
		EnAttackBB |= AttackBrdKingBB[fromSQ];
		getPinByAttackSQ<WHITE, true>(fromSQ, PinsBySquare);
		SetBit(&allPiecesBB, myKing_sq);// put king back
	}
	template <>
	inline void getPieceTypeAttackBB<BLACK, BlackKing>(U64 piecesBB, S_PinnedPiece* PinsBySquare) {
		MyAttackBB |= AttackBrdKingBB[myKing_sq];
		getPinByAttackSQ<WHITE, true>(myKing_sq, PinsBySquare);
	}
#endif


	/*
		GenerateLegalMoves vars
	*/
#if true
	U64 PremoveBBNormalExec = 0ULL;
	U64 PremoveBBPawnDoubleMove = 0ULL;
	U64 PremoveBBCapture = 0ULL;
	U64 PremoveBBPromote = 0ULL;
	U64 PremoveBBPromoteCapture = 0ULL;
	U64 PremoveBBCastle = 0ULL;
	U64 MyAttackBB = 0ULL;
	U64 EnAttackBB = 0ULL;

	U64 myKingBB = 0ULL;
	int myKing_sq = 0;
	U64 enKingBB = 0ULL;
	int enKing_sq = 0;

	U64 friends = 0ULL;
	U64 allPiecesBB = 0ULL;
	U64 emptyBB = 0ULL;

	int fromSQ = 0, toSQ = 0, checkersType = 0;
	U64 PieceAttacksBB = 0ULL, captureSQ_BB = 0ULL;
	U64 CheckersBB = 0ULL, blockingSQ_BB = 0ULL;
	int checkersNum = 0;

	U64 pinnedPiecesBB = 0ULL, pinnedEnemyPiecesBB = 0ULL, notPinnedPiecesBB = 0ULL;
	U64 kingBishopAttackBB = 0ULL, kingRookAttackBB = 0ULL;

	U64 tempAttackBB = 0ULL;
	short checkerSQ = 0;
	short checkerType = 0;

	U64 tempMove = 0ULL;
	U64 _allPiecesBB = 0ULL, PremoveBBEnPassant = 0ULL, EnPasPieceBB = 0ULL;
	short enPas;

	bool endgame = false;
	bool opening = false;
	bool inCheck = false;

	S_PinnedPiece* MyPinnedPieces = nullptr;
	S_PinnedPiece* EnPinnedPieces = nullptr;
#endif
	//TODO get pinned pieces
	/*
		genrates and sort moves
		the algorithm will eval position and moves while generating the moves

		gameboard->score will be updated
		gameboard->InCheck will be updated

		each move has a given score

		the whole array gets quick sorted at the end		
	*/
	template <Colors Us>
	inline S_Movelist* GenerateLegalMoves(S_Movelist* MovePtr) {
		S_MemoryFrame Frame = newMemoryFrame;

		constexpr Pieces myPAWN = Us == WHITE ? WhitePawn : BlackPawn;
		constexpr Pieces myKNIGHT = Us == WHITE ? WhiteKnight : BlackKnight;
		constexpr Pieces myBISHOP = Us == WHITE ? WhiteBishop : BlackBishop;
		constexpr Pieces myROOK = Us == WHITE ? WhiteRook : BlackRook;
		constexpr Pieces myQUEEN = Us == WHITE ? WhiteQueen : BlackQueen;
		constexpr Pieces myKING = Us == WHITE ? WhiteKing : BlackKing;

		constexpr Pieces enPAWN = Us == BLACK ? WhitePawn : BlackPawn;
		constexpr Pieces enKNIGHT = Us == BLACK ? WhiteKnight : BlackKnight;
		constexpr Pieces enBISHOP = Us == BLACK ? WhiteBishop : BlackBishop;
		constexpr Pieces enROOK = Us == BLACK ? WhiteRook : BlackRook;
		constexpr Pieces enQUEEN = Us == BLACK ? WhiteQueen : BlackQueen;
		constexpr Pieces enKING = Us == BLACK ? WhiteKing : BlackKing;

		constexpr U64 KingCastleFreeBB = Us == WHITE ? WhiteKingCastleFree : BlackKingCastleFree;
		constexpr U64 QueenCastleFreeBB = Us == WHITE ? WhiteQueenCastleFree : BlackQueenCastleFree;
		constexpr U64 KingCastleCheckBB = Us == WHITE ? WhiteKingCastleCheck : BlackKingCastleCheck;
		constexpr U64 QueenCastleCheckBB = Us == WHITE ? WhiteQueenCastleCheck : BlackQueenCastleCheck;
		constexpr U64 CastleRightsBB = Us == WHITE ? WKCA | WQCA : BKCA | BQCA;
		constexpr U64 CastleKingSideBB = Us == WHITE ? WKCA : BKCA;
		constexpr U64 CastleQueenSideBB = Us == WHITE ? WQCA : BQCA;
		constexpr Fields CastleFrom = Us == WHITE ? E1 : E8;
		constexpr Fields CastleKingSideTo = Us == WHITE ? G1 : G8;
		constexpr Fields CastleQueenSideTo = Us == WHITE ? C1 : C8;

		constexpr U64 Rank8BB = Us == WHITE ? the8Rank : the1Rank;
		constexpr U64 Rank2BB = Us == WHITE ? the2Rank : the7Rank;

		constexpr int (*getMyPieceType)(U64*, int) = (Us == WHITE ? getWhitePieceType : getBlackPieceType);
		constexpr int (*getEnPieceType)(U64*, int) = (Us == WHITE ? getBlackPieceType : getWhitePieceType);

		constexpr U64(*getMyPiecesBB)(U64*) = (Us == WHITE ? getWhitePiecesBB : getBlackPiecesBB);
		constexpr U64(*getEnPiecesBB)(U64*) = (Us == WHITE ? getBlackPiecesBB : getWhitePiecesBB);

		constexpr int(*getPawnDistance)(int, int) = (Us == WHITE ? getWhitePawnDistance : getBlackPawnDistance);

		MyPinnedPieces = nullptr;
		EnPinnedPieces = nullptr;

		S_PinnedPiece* PinsBySquare[64] = {
			nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
			nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
			nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
			nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
			nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
			nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
			nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
			nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr
		};

		PremoveBBNormalExec = MOVE_NEW_PREMOVE(NormalExec, gameboard->castlePerm, gameboard->fiftyMove, gameboard->enPas);
		PremoveBBPawnDoubleMove = MOVE_NEW_PREMOVE(PawnDoubleMove, gameboard->castlePerm, gameboard->fiftyMove, gameboard->enPas);
		PremoveBBCapture = MOVE_NEW_PREMOVE(Capture, gameboard->castlePerm, gameboard->fiftyMove, gameboard->enPas);
		PremoveBBPromote = MOVE_NEW_PREMOVE(Promote, gameboard->castlePerm, gameboard->fiftyMove, gameboard->enPas);
		PremoveBBPromoteCapture = MOVE_NEW_PREMOVE(PromoteCapture, gameboard->castlePerm, gameboard->fiftyMove, gameboard->enPas);
		PremoveBBCastle = MOVE_NEW_PREMOVE(Castle, gameboard->castlePerm, gameboard->fiftyMove, gameboard->enPas);

		endgame = Endgame(gameboard->piecesBB);
		opening = Opening<Us>(gameboard->piecesBB);

		myKingBB = gameboard->piecesBB[myKING];
		myKing_sq = PopBit(&myKingBB);
		SetBit(&myKingBB, myKing_sq);

		enKingBB = gameboard->piecesBB[enKING];
		enKing_sq = PopBit(&enKingBB);
		SetBit(&enKingBB, enKing_sq);

		friends = getMyPiecesBB(gameboard->piecesBB);
		enemys = getEnPiecesBB(gameboard->piecesBB);
		allPiecesBB = friends | enemys;
		emptyBB = ~allPiecesBB;
		
		MyAttackBB = EnAttackBB = CheckersBB = blockingSQ_BB = pinnedPiecesBB = notPinnedPiecesBB = kingBishopAttackBB = kingRookAttackBB = 0ULL;
		checkersNum = 0;

		getPieceTypeAttackBB<Us, WhitePawn>(gameboard->piecesBB[WhitePawn], PinsBySquare);
		
		
		
		if (gameboard->enPas != NO_SQ) {
			enPas = gameboard->enPas;
			PremoveBBEnPassant = MOVE_NEW_PREMOVE(EnPassant, gameboard->castlePerm, gameboard->fiftyMove, gameboard->enPas);
			// get rid of EnPas-calc by check while doing move if any possible
			EnPasPieceBB = (Us == WHITE ? (AttackBrdbPawnBB[enPas]) : (AttackBrdwPawnBB[enPas])) & gameboard->piecesBB[myPAWN];
			while (EnPasPieceBB) {
				//MOVE_NEW_MOVE(int mod, int f, int t, int mov, int ca, int pro, int cp, int fm, int ep)		

				tempMove = MOVE_NEW(PopBit(&EnPasPieceBB), enPas, myPAWN, Empty, Empty, PremoveBBEnPassant);
				//make EnPas move and check valid or not
				doMove<Us>(tempMove);
				_allPiecesBB = getWhitePiecesBB(gameboard->piecesBB) | getBlackPiecesBB(gameboard->piecesBB);
				// now check attacks from king_sq - only sliding moves are possible checks
				// save move if not in check			
				if (((magicGetBishopAttackBB(myKing_sq, _allPiecesBB) & (gameboard->piecesBB[enQUEEN] | gameboard->piecesBB[enBISHOP])) | (magicGetRookAttackBB(myKing_sq, _allPiecesBB) & (gameboard->piecesBB[enQUEEN] | gameboard->piecesBB[enROOK]))) == 0ULL) {
					NEW_MOVE(MovePtr, tempMove, PIECES_VALUES_ABS[enPAWN] + 1000);
				}
				undoMove<Us>();
			}
		}
		inCheck = CheckersBB != 0ULL;

		kingBishopAttackBB = magicGetBishopAttackBB(myKing_sq, allPiecesBB) & friends;
		kingRookAttackBB = magicGetRookAttackBB(myKing_sq, allPiecesBB) & friends;

		tempAttackBB = AttackBrdKingBB[myKing_sq] & ~EnAttackBB;
		//king moves
		PieceAttacksBB = tempAttackBB & emptyBB;
		while (PieceAttacksBB) {
			NEW_MOVE(MovePtr,
				MOVE_NEW(myKing_sq, toSQ, myKING, Empty, Empty, PremoveBBNormalExec)
				, 100);
		}

		PieceAttacksBB = tempAttackBB & enemys;
		while (PieceAttacksBB) {
			toSQ = PopBit(&PieceAttacksBB);
			NEW_MOVE(MovePtr,
				MOVE_NEW(myKing_sq, toSQ, myKING, getEnPieceType(gameboard->piecesBB, toSQ), Empty, PremoveBBCapture)
				, 100);
		}

		if (inCheck) {// king in check
			if (checkersNum == 1) {// CountBits(CheckersBB) > 1 == multiple check == king move forced
				// check for capture moves!
				captureSQ_BB = CheckersBB;
				checkerSQ = PopBit(&CheckersBB);
				checkerType = getEnPieceType(gameboard->piecesBB, checkerSQ);

				// check for blocking moves by => AttackBoardBB & blockingSQ_BB

			}
		}
		else {
			// castle
			if (CastleRightsBB & castlePerm) {
				if (CastleKingSideBB & castlePerm)
					if ((allPiecesBB & KingCastleFreeBB) == 0ULL)
						if ((EnemyAttacksBB & KingCastleCheckBB) == 0ULL)
							NEW_MOVE(MovePtr,
								MOVE_NEW(CastleFrom, CastleKingSideTo, myKING, Empty, Empty, PremoveBBCastle)
								, 100);

				if (CastleQueenSideBB & castlePerm)
					if ((allPiecesBB & QueenCastleFreeBB) == 0ULL)
						if ((EnemyAttacksBB & QueenCastleCheckBB) == 0ULL)
							NEW_MOVE(MovePtr,
								MOVE_NEW(CastleFrom, CastleQueenSideTo, myKING, Empty, Empty, PremoveBBCastle)
								, 100);
			}


		}


		if (MaterialDraw(gameboard->fiftyMove, gameboard->piecesBB))
			gameboard->score = DRAW_VALUE;

		RETURN(Frame, MovePtr);
	}



	inline short GetPvLine(const short depth) {

		U64 move = ProbePvMove(gameboard->posKey);
		short count = 0;

		while (move != 0ULL && count < depth) {
			if (gameboard->side == WHITE)
				doMove<WHITE>(move);
			else
				doMove<BLACK>(move);
			gameboard->PvArray[count++] = move;
			move = ProbePvMove(gameboard->posKey);
		}
		int temp = count;
		while (temp > 0) {
			temp--;
			if (gameboard->side == WHITE)
				undoMove<BLACK>();
			else
				undoMove<WHITE>();
		}

		return count;

	}

};

