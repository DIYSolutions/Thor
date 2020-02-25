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

	inline void init(MemoryBlock* pMemory) {
		PiecesBB = (U64*)pMemory->AllocFrameMemory<ThreadHeap>(sizeof(U64) * 12);
		History = (U64*)pMemory->AllocFrameMemory<ThreadHeap>(sizeof(U64) * GAME_MAX_MOVES);
		PinBySquare = (S_PinnedPiece*)pMemory->AllocFrameMemory<ThreadHeap>(sizeof(S_PinnedPiece) * 64);
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
private:

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

	template <Pieces Type>
	inline void getEnemyAttack(U64 pieceBB) {}

	template <>
	inline void getEnemyAttack<WhitePawn>(U64 pieceBB) {
		ClearBit(&allPiecesBB, myKingSQ);// if in check by bishop, rook or queen then we must check attack without king!
		while (pieceBB) {
			U64 fromSQ = PopBit(&pieceBB);
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
			U64 fromSQ = PopBit(&pieceBB);
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
			U64 fromSQ = PopBit(&pieceBB);
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
			U64 fromSQ = PopBit(&pieceBB);
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
			U64 fromSQ = PopBit(&pieceBB);
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
			U64 fromSQ = PopBit(&pieceBB);
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
			U64 fromSQ = PopBit(&pieceBB);
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
			U64 fromSQ = PopBit(&pieceBB);
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
			U64 fromSQ = PopBit(&pieceBB);
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
			U64 fromSQ = PopBit(&pieceBB);
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
	inline U64* GenMove(U64* MovePtr) {
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


	}

	inline void HASH_PCE(short pce, short sq) { PosKey ^= PieceKeys[pce][sq]; }
	inline void HASH_CA(void) { PosKey ^= CastleKeys[CastlePerm]; }
	inline void HASH_SIDE(void) { PosKey ^= SideKey; }
	inline void HASH_EP(void) { PosKey ^= PieceKeys[Empty][EnPas]; }

};

inline Chessboard* newChessboard(MemoryBlock* pMemory) {
	Chessboard* pNewChessboard = (Chessboard*)pMemory->AllocFrameMemory<ThreadHeap>(sizeof(Chessboard));
	if(!pNewChessboard)
		error_exit("Chessboard: memory alloc failed!");

	pNewChessboard->init(pMemory);
	return pNewChessboard;
}