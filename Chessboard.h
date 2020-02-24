#pragma once
#include "bitboard_magic.h"
#include "move.h"

class Chessboard
{
private:
	short Side;
	short EnPas;
	short FiftyMove;
	short CastlePerm;

	U64 PosKey;

	short Ply;
	short HisPly;
	U64 History[GAME_MAX_MOVES];
	U64 PiecesBB[12];

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
	bool InCheck = false;

	template <Pieces Type>
	inline void getEnemyAttack(U64 pieceBB) {}

	template <>
	inline void getEnemyAttack<WhitePawn>(U64 pieceBB) {

		getEnemyAttack<WhiteKnight>(PiecesBB[WhiteKnight]);
	}
	template <>
	inline void getEnemyAttack<WhiteKnight>(U64 pieceBB) {

		getEnemyAttack<WhiteBishop>(PiecesBB[WhiteBishop]);
	}
	template <>
	inline void getEnemyAttack<WhiteBishop>(U64 pieceBB) {

		getEnemyAttack<WhiteRook>(PiecesBB[WhiteRook]);
	}
	template <>
	inline void getEnemyAttack<WhiteRook>(U64 pieceBB) {

		getEnemyAttack<WhiteQueen>(PiecesBB[WhiteQueen]);
	}
	template <>
	inline void getEnemyAttack<WhiteQueen>(U64 pieceBB) {

		getEnemyAttack<WhiteKing>(PiecesBB[WhiteKing]);
	}
	template <>
	inline void getEnemyAttack<WhiteKing>(U64 pieceBB) {

	}

	template <>
	inline void getEnemyAttack<BlackPawn>(U64 pieceBB) {

		getEnemyAttack<BlackKnight>(PiecesBB[BlackKnight]);
	}
	template <>
	inline void getEnemyAttack<BlackKnight>(U64 pieceBB) {

		getEnemyAttack<BlackBishop>(PiecesBB[BlackBishop]);
	}
	template <>
	inline void getEnemyAttack<BlackBishop>(U64 pieceBB) {

		getEnemyAttack<BlackRook>(PiecesBB[BlackRook]);
	}
	template <>
	inline void getEnemyAttack<BlackRook>(U64 pieceBB) {

		getEnemyAttack<BlackQueen>(PiecesBB[BlackQueen]);
	}
	template <>
	inline void getEnemyAttack<BlackQueen>(U64 pieceBB) {

		getEnemyAttack<BlackKing>(PiecesBB[BlackKing]);
	}
	template <>
	inline void getEnemyAttack<BlackKing>(U64 pieceBB) {

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
	}

public:
	Chessboard() {
		for (int i = WhitePawn; i < Empty; i++)
			PiecesBB[i] = 0ULL;
		PosKey = 0ULL;
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
};

