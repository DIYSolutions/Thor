#pragma once
#include "bitboard_magic.h"
#include "move.h"
#include "hash.h"
#include "MemoryBlock.h"
#include "ChessThreadMessenger.h"

class Chessboard
{

public:
	Chessboard(void) { }
	~Chessboard(void) {
		pMemory->ReleaseMemoryFrame(&MemoryFrame);
	}

	inline void init(MemoryBlock* _pMemory) {
		pMemory = _pMemory;
		MemoryFrame = pMemory->GetMemoryFrame(ThreadHeap);
		PiecesBB = (U64*)pMemory->AllocFrameMemory<ThreadHeap>(sizeof(U64) * 12);
		for (short i = 0; i < NO_SQ; i++)
			PiecesBB[i] = 0ULL;
		History = (U64*)pMemory->AllocFrameMemory<ThreadHeap>(sizeof(U64) * GAME_MAX_MOVES);
		for (short i = 0; i < GAME_MAX_MOVES; i++)
			History[i] = 0ULL;
		PinBySquare = (S_PinnedPiece*)pMemory->AllocFrameMemory<ThreadHeap>(sizeof(S_PinnedPiece) * 64);
	}

	inline void doMove(U64* Move) {
		if (Side == WHITE)
			return DoMove<WHITE>(Move);
		return DoMove<BLACK>(Move);
	}
	inline void undoMove() {
		if (Side == WHITE)
			return UndoMove<WHITE>();
		return UndoMove<BLACK>();
	}
	inline S_MOVE* genMove(S_MOVE* MovePtr) {
		if (Side == WHITE)
			return GenMove<WHITE>(MovePtr);
		return GenMove<BLACK>(MovePtr);
	}

	inline const U64 getPosKey(void) { return PosKey; }

	inline S_ThreadMessage* GenThreadMessage(const short Mode) {
		return NewThreadMessage(Side, EnPas, FiftyMove, CastlePerm, PiecesBB, PosKey, Mode);
	}
	inline void SetThreadMessage(S_ThreadMessage* msg) {
		Side = msg->Side;
		EnPas = msg->EnPas;
		FiftyMove = msg->FiftyMove;
		CastlePerm = msg->CastlePerm;
		PosKey = msg->PosKey;
		PiecesBB[0] = msg->PiecesBB[0];
		PiecesBB[1] = msg->PiecesBB[1];
		PiecesBB[2] = msg->PiecesBB[2];
		PiecesBB[3] = msg->PiecesBB[3];
		PiecesBB[4] = msg->PiecesBB[4];
		PiecesBB[5] = msg->PiecesBB[5];
		PiecesBB[6] = msg->PiecesBB[6];
		PiecesBB[7] = msg->PiecesBB[7];
		PiecesBB[8] = msg->PiecesBB[8];
		PiecesBB[9] = msg->PiecesBB[9];
		PiecesBB[10] = msg->PiecesBB[10];
		PiecesBB[11] = msg->PiecesBB[11];
		Ply = 0;
		HisPly = 0;
	}

	inline short SideToMove(void) { return Side; }
	inline short HistoryPly(void) { return HisPly; }

	inline bool ParseFEN(char * fen) {
		short  rank = RANK_8;
		short  file = FILE_A;
		short  piece = 0;
		short  count = 0;
		short  i = 0;
		short  sq = 0;

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
				printf("FEN error \n");
				return false;
			}

			for (i = 0; i < count; i++) {
				sq = rank * 8 + file;
				if (piece != Empty) {
					NewPiece(piece, sq);
				}
				file++;
			}
			fen++;
		}

		ASSERT(*fen == 'w' || *fen == 'b');

		Side = (*fen == 'w') ? WHITE : BLACK;
		fen += 2;

		for (i = 0; i < 4; i++) {
			if (*fen == ' ') {
				break;
			}
			switch (*fen) {
			case 'K': CastlePerm |= WKCA; break;
			case 'Q': CastlePerm |= WQCA; break;
			case 'k': CastlePerm |= BKCA; break;
			case 'q': CastlePerm |= BQCA; break;
			default:	     break;
			}
			fen++;
		}
		fen++;

		ASSERT(CastlePerm >= 0 && CastlePerm <= 15);

		if (*fen != '-') {
			file = fen[0] - 'a';
			rank = fen[1] - '1';

			ASSERT(file >= FILE_A && file <= FILE_H);
			ASSERT(rank >= RANK_1 && rank <= RANK_8);

			EnPas = FR2SQ(file, rank);
		}

		GeneratePosKey();
		return true;
	}

	inline void setPly(short _Ply) { Ply = _Ply; }

	inline short getPieceType(short SQ) {
		U64 thisSQ = SetMask[SQ];
		if (thisSQ && PiecesBB[WhitePawn])
			return WhitePawn;
		if (thisSQ && PiecesBB[WhiteKnight])
			return WhiteKnight;
		if (thisSQ && PiecesBB[WhiteBishop])
			return WhiteBishop;
		if (thisSQ && PiecesBB[WhiteRook])
			return WhiteRook;
		if (thisSQ && PiecesBB[WhiteQueen])
			return WhiteQueen;
		if (thisSQ && PiecesBB[WhiteKing])
			return WhiteKing;

		if (thisSQ && PiecesBB[BlackPawn])
			return BlackPawn;
		if (thisSQ && PiecesBB[BlackKnight])
			return BlackKnight;
		if (thisSQ && PiecesBB[BlackBishop])
			return BlackBishop;
		if (thisSQ && PiecesBB[BlackRook])
			return BlackRook;
		if (thisSQ && PiecesBB[BlackQueen])
			return BlackQueen;
		if (thisSQ && PiecesBB[BlackKing])
			return BlackKing;
		return Empty;
	}

	inline void PrintBoard(void) {
		int file, rank;
		printing_console_start();
		printf("\nGame Board:\n\n");

		for (rank = RANK_8; rank >= RANK_1; rank--) {
			printf("%d  ", rank + 1);
			for (file = FILE_A; file <= FILE_H; file++)
				printf("%c  ", PceChar[getPieceType(FR2SQ(file, rank))][0]);
			printf("\n");
		}

		printf("\n   ");
		for (file = FILE_A; file <= FILE_H; file++) {
			printf("%3c", 'a' + file);
		}
		printf("\n");
		printf("side:%c\n", Side == WHITE ? 'w' : 'b');
		printf("enPas:%d\n", EnPas);
		printf("castle:%c%c%c%c\n",
			CastlePerm & WKCA ? 'K' : '-',
			CastlePerm & WQCA ? 'Q' : '-',
			CastlePerm & BKCA ? 'k' : '-',
			CastlePerm & BQCA ? 'q' : '-'
		);
		printf("PosKey:%llX\n", PosKey);
		printing_console_end();
	}
private:

	S_MemoryFrame MemoryFrame;
	MemoryBlock* pMemory;
	short Side = BLACK;
	short EnPas = NO_SQ;
	short FiftyMove = 0;
	short CastlePerm = 0;
	U64 * PiecesBB = nullptr;
	U64 PosKey = 0ULL;

	short Ply = 0;
	short HisPly = 0;
	U64 * History = nullptr;

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

	U64 PremoveBBNormalExec = 0ULL;
	U64 PremoveBBPawnDoubleMove = 0ULL;
	U64 PremoveBBCapture = 0ULL;
	U64 PremoveBBPromote = 0ULL;
	U64 PremoveBBPromoteCapture = 0ULL;
	U64 PremoveBBCastle = 0ULL;

	U64 EnAttackBB = 0ULL;
	U64 InCheckBlockingSQ_BB = 0ULL;
	U64 InCheckAttackSQ_BB = 0ULL;
	short InCheckNum = 0;
	short InCheckType = 0;
	bool InCheck = false;

	short myKingSQ = NO_SQ;
	U64 myKingBB = 0ULL;

	S_PinnedPiece * PinBySquare = nullptr;
	U64 PinnedPiecesBB = 0ULL;

	U64 allPiecesBB = 0ULL;

	inline void GeneratePosKey(void) {
		int sq = 0;
		PosKey = 0ULL;
		int piece = Empty;

		// pieces
		for (piece = 0; piece < Empty; ++piece) {
			U64 pieceBB = PiecesBB[piece];
			while (pieceBB) {
				PosKey ^= PieceKeys[piece][PopBit(&pieceBB)];
			}
		}

		if (Side == WHITE) {
			PosKey ^= SideKey;
		}

		if (EnPas != NO_SQ) {
			ASSERT(EnPas >= 0 && EnPas < NO_SQ);
			PosKey ^= PieceKeys[Empty][EnPas];
		}

		ASSERT(CastlePerm >= 0 && CastlePerm <= 15);

		PosKey ^= CastleKeys[CastlePerm];
	}

	inline void ResetBoard(void) {
		Side = BLACK;
		EnPas = NO_SQ;
		FiftyMove = 0;
		CastlePerm = 0;
		PosKey = 0ULL;
		PiecesBB[0] = 0ULL;
		PiecesBB[1] = 0ULL;
		PiecesBB[2] = 0ULL;
		PiecesBB[3] = 0ULL;
		PiecesBB[4] = 0ULL;
		PiecesBB[5] = 0ULL;
		PiecesBB[6] = 0ULL;
		PiecesBB[7] = 0ULL;
		PiecesBB[8] = 0ULL;
		PiecesBB[9] = 0ULL;
		PiecesBB[10] = 0ULL;
		PiecesBB[11] = 0ULL;
		Ply = 0;
		HisPly = 0;
	}

	template <Pieces Type>
	inline void getEnemyAttack(U64 pieceBB) {}

	template <>
	inline void getEnemyAttack<WhitePawn>(U64 pieceBB) {
		ClearBit(&allPiecesBB, myKingSQ);// if in check by bishop, rook or queen then we must check attack without king!
		while (pieceBB) {
			short fromSQ = PopBit(&pieceBB);
			U64 PieceAttacksBB = AttackBrdwPawnBB[fromSQ];
			if (PieceAttacksBB & myKingBB) {
				SetBit(&InCheckAttackSQ_BB, fromSQ);
				InCheckNum++;
				InCheckType = WhitePawn;
			}
			EnAttackBB |= PieceAttacksBB;
		}
		getEnemyAttack<WhiteKnight>(PiecesBB[WhiteKnight]);
	}
	template <>
	inline void getEnemyAttack<WhiteKnight>(U64 pieceBB) {
		while (pieceBB) {
			short fromSQ = PopBit(&pieceBB);
			U64 PieceAttacksBB = AttackBrdKnightBB[fromSQ];
			if (PieceAttacksBB & myKingBB) {
				SetBit(&InCheckAttackSQ_BB, fromSQ);
				InCheckNum++;
				InCheckType = WhiteKnight;
			}
			EnAttackBB |= PieceAttacksBB;
		}
		getEnemyAttack<WhiteBishop>(PiecesBB[WhiteBishop]);
	}
	template <>
	inline void getEnemyAttack<WhiteBishop>(U64 pieceBB) {
		while (pieceBB) {
			short fromSQ = PopBit(&pieceBB);
			U64 PieceAttacksBB = magicGetBishopAttackBB(fromSQ, allPiecesBB);
			if (PieceAttacksBB & myKingBB) {
				SetBit(&InCheckAttackSQ_BB, fromSQ);
				InCheckNum++;
				InCheckType = WhiteBishop;
				InCheckBlockingSQ_BB = getBishopBlockingSQ_BB(fromSQ, myKingSQ);
			}
			EnAttackBB |= PieceAttacksBB;
		}
		getEnemyAttack<WhiteRook>(PiecesBB[WhiteRook]);
	}
	template <>
	inline void getEnemyAttack<WhiteRook>(U64 pieceBB) {
		while (pieceBB) {
			short fromSQ = PopBit(&pieceBB);
			U64 PieceAttacksBB = magicGetRookAttackBB(fromSQ, allPiecesBB);
			if (PieceAttacksBB & myKingBB) {
				SetBit(&InCheckAttackSQ_BB, fromSQ);
				InCheckNum++;
				InCheckType = WhiteRook;
				InCheckBlockingSQ_BB = getRookBlockingSQ_BB(fromSQ, myKingSQ);
			}
			EnAttackBB |= PieceAttacksBB;
		}
		getEnemyAttack<WhiteQueen>(PiecesBB[WhiteQueen]);
	}
	template <>
	inline void getEnemyAttack<WhiteQueen>(U64 pieceBB) {
		while (pieceBB) {
			short fromSQ = PopBit(&pieceBB);
			U64 PieceAttacksBB = magicGetQueenAttackBB(fromSQ, allPiecesBB);
			if (PieceAttacksBB & myKingBB) {
				SetBit(&InCheckAttackSQ_BB, fromSQ);
				InCheckNum++;
				InCheckType = WhiteQueen;
				InCheckBlockingSQ_BB = getBlockingSQ_BB(fromSQ, myKingSQ);
			}
			EnAttackBB |= PieceAttacksBB;
		}
		getEnemyAttack<WhiteKing>(PiecesBB[WhiteKing]);
	}
	template <>
	inline void getEnemyAttack<WhiteKing>(U64 pieceBB) {
		EnAttackBB |= AttackBrdKingBB[PopBit(&pieceBB)];
		SetBit(&allPiecesBB, myKingSQ);// put king back
	}

	template <>
	inline void getEnemyAttack<BlackPawn>(U64 pieceBB) {
		ClearBit(&allPiecesBB, myKingSQ);// if in check by bishop, rook or queen then we must check attack without king!
		while (pieceBB) {
			short fromSQ = PopBit(&pieceBB);
			U64 PieceAttacksBB = AttackBrdbPawnBB[fromSQ];
			if (PieceAttacksBB & myKingBB) {
				SetBit(&InCheckAttackSQ_BB, fromSQ);
				InCheckNum++;
				InCheckType = BlackPawn;
			}
			EnAttackBB |= PieceAttacksBB;
		}
		getEnemyAttack<BlackKnight>(PiecesBB[BlackKnight]);
	}
	template <>
	inline void getEnemyAttack<BlackKnight>(U64 pieceBB) {
		while (pieceBB) {
			short fromSQ = PopBit(&pieceBB);
			U64 PieceAttacksBB = AttackBrdKnightBB[fromSQ];
			if (PieceAttacksBB & myKingBB) {
				SetBit(&InCheckAttackSQ_BB, fromSQ);
				InCheckNum++;
				InCheckType = BlackKnight;
			}
			EnAttackBB |= PieceAttacksBB;
		}
		getEnemyAttack<BlackBishop>(PiecesBB[BlackBishop]);
	}
	template <>
	inline void getEnemyAttack<BlackBishop>(U64 pieceBB) {
		while (pieceBB) {
			short fromSQ = PopBit(&pieceBB);
			U64 PieceAttacksBB = magicGetBishopAttackBB(fromSQ, allPiecesBB);
			if (PieceAttacksBB & myKingBB) {
				SetBit(&InCheckAttackSQ_BB, fromSQ);
				InCheckNum++;
				InCheckType = BlackBishop;
				InCheckBlockingSQ_BB = getBishopBlockingSQ_BB(fromSQ, myKingSQ);
			}
			EnAttackBB |= PieceAttacksBB;
		}
		getEnemyAttack<BlackRook>(PiecesBB[BlackRook]);
	}
	template <>
	inline void getEnemyAttack<BlackRook>(U64 pieceBB) {
		while (pieceBB) {
			short fromSQ = PopBit(&pieceBB);
			U64 PieceAttacksBB = magicGetRookAttackBB(fromSQ, allPiecesBB);
			if (PieceAttacksBB & myKingBB) {
				SetBit(&InCheckAttackSQ_BB, fromSQ);
				InCheckNum++;
				InCheckType = BlackRook;
				InCheckBlockingSQ_BB = getRookBlockingSQ_BB(fromSQ, myKingSQ);
			}
			EnAttackBB |= PieceAttacksBB;
			//getPinByAttackSQ<BLACK, false>(fromSQ, PinsBySquare);

		}
		getEnemyAttack<BlackQueen>(PiecesBB[BlackQueen]);
	}
	template <>
	inline void getEnemyAttack<BlackQueen>(U64 pieceBB) {
		while (pieceBB) {
			short fromSQ = PopBit(&pieceBB);
			U64 PieceAttacksBB = magicGetQueenAttackBB(fromSQ, allPiecesBB);
			if (PieceAttacksBB & myKingBB) {
				SetBit(&InCheckAttackSQ_BB, fromSQ);
				InCheckNum++;
				InCheckType = BlackQueen;
				InCheckBlockingSQ_BB = getBlockingSQ_BB(fromSQ, myKingSQ);
			}
			EnAttackBB |= PieceAttacksBB;
			//getPinByAttackSQ<BLACK, false>(fromSQ, PinsBySquare);
		}
		getEnemyAttack<BlackKing>(PiecesBB[BlackKing]);
	}
	template <>
	inline void getEnemyAttack<BlackKing>(U64 pieceBB) {
		EnAttackBB |= AttackBrdKingBB[PopBit(&pieceBB)];
		SetBit(&allPiecesBB, myKingSQ);// put king back
	}

	template <Colors Us>
	inline S_MOVE* GenMove(S_MOVE * MovePtr) {
		constexpr short myPAWN = Us == WHITE ? WhitePawn : BlackPawn;
		constexpr short myKNIGHT = Us == WHITE ? WhiteKnight : BlackKnight;
		constexpr short myBISHOP = Us == WHITE ? WhiteBishop : BlackBishop;
		constexpr short myROOK = Us == WHITE ? WhiteRook : BlackRook;
		constexpr short myQUEEN = Us == WHITE ? WhiteQueen : BlackQueen;
		constexpr short myKING = Us == WHITE ? WhiteKing : BlackKing;

		constexpr short enPAWN = Us == BLACK ? WhitePawn : BlackPawn;
		constexpr short enKNIGHT = Us == BLACK ? WhiteKnight : BlackKnight;
		constexpr short enBISHOP = Us == BLACK ? WhiteBishop : BlackBishop;
		constexpr short enROOK = Us == BLACK ? WhiteRook : BlackRook;
		constexpr short enQUEEN = Us == BLACK ? WhiteQueen : BlackQueen;
		constexpr short enKING = Us == BLACK ? WhiteKing : BlackKing;

		constexpr U64 KingCastleFreeBB = Us == WHITE ? WhiteKingCastleFree : BlackKingCastleFree;
		constexpr U64 QueenCastleFreeBB = Us == WHITE ? WhiteQueenCastleFree : BlackQueenCastleFree;
		constexpr U64 KingCastleCheckBB = Us == WHITE ? WhiteKingCastleCheck : BlackKingCastleCheck;
		constexpr U64 QueenCastleCheckBB = Us == WHITE ? WhiteQueenCastleCheck : BlackQueenCastleCheck;
		constexpr U64 CastleRightsBB = Us == WHITE ? WKCA | WQCA : BKCA | BQCA;
		constexpr U64 CastleKingSideBB = Us == WHITE ? WKCA : BKCA;
		constexpr U64 CastleQueenSideBB = Us == WHITE ? WQCA : BQCA;
		constexpr short CastleFrom = Us == WHITE ? E1 : E8;
		constexpr short CastleKingSideTo = Us == WHITE ? G1 : G8;
		constexpr short CastleQueenSideTo = Us == WHITE ? C1 : C8;

		constexpr U64 Rank8BB = Us == WHITE ? the8Rank : the1Rank;
		constexpr U64 Rank2BB = Us == WHITE ? the2Rank : the7Rank;

		PremoveBBNormalExec = MOVE_NEW_PREMOVE(NormalExec, CastlePerm, FiftyMove, EnPas);
		PremoveBBPawnDoubleMove = MOVE_NEW_PREMOVE(PawnDoubleMove, CastlePerm, FiftyMove, EnPas);
		PremoveBBCapture = MOVE_NEW_PREMOVE(Capture, CastlePerm, FiftyMove, EnPas);
		PremoveBBPromote = MOVE_NEW_PREMOVE(Promote, CastlePerm, FiftyMove, EnPas);
		PremoveBBPromoteCapture = MOVE_NEW_PREMOVE(PromoteCapture, CastlePerm, FiftyMove, EnPas);
		PremoveBBCastle = MOVE_NEW_PREMOVE(Castle, CastlePerm, FiftyMove, EnPas);

		EnAttackBB = InCheckAttackSQ_BB = InCheckBlockingSQ_BB = InCheckAttackSQ_BB = 0ULL;
		InCheckNum = 0;
		getEnemyAttack<enPAWN>(PiecesBB[enPAWN]);

		return MovePtr;
	}

	inline void HASH_PCE(short pce, short sq) { PosKey ^= PieceKeys[pce][sq]; }
	inline void HASH_CA(void) { PosKey ^= CastleKeys[CastlePerm]; }
	inline void HASH_SIDE(void) { PosKey ^= SideKey; }
	inline void HASH_EP(void) { PosKey ^= PieceKeys[Empty][EnPas]; }

};

inline Chessboard* newChessboard(MemoryBlock* pMemory) {
	Chessboard* pNewChessboard = new Chessboard();
	if(!pNewChessboard)
		error_exit("Chessboard: memory alloc failed!");

	pNewChessboard->init(pMemory);
	return pNewChessboard;
}