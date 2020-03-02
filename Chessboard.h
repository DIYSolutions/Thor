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
		PinBySquare = (S_PinnedPiece**)pMemory->AllocFrameMemory<ThreadHeap>(sizeof(S_PinnedPiece*) * 64);
		for (short i = 0; i < 64; i++) {
			PinBySquare[i] = nullptr;
		}
	}

	inline U64* getPvLine(U64* MovePtr, const short movesLeft) {
		if (movesLeft) {
			U64 Move = ProbePvMove(PosKey);
			if (Move != 0ULL) {
				// is Move part of current movelist?
				*(MovePtr++) = Move;
				doMove(&Move);
				return getPvLine(MovePtr, movesLeft - 1);
			}
		}
		while (Ply > 0)
			undoMove();
		return MovePtr;
	}

	inline void doMove(U64* Move) {
		if (Side == WHITE)
			return DoMove<WHITE>(*Move);
		return DoMove<BLACK>(*Move);
	}
	inline void undoMove() {
		if (Side == WHITE)
			return UndoMove<BLACK>();
		return UndoMove<WHITE>();
	}
	inline short genMove(S_MOVE* MovePtr) {
		if (Side == WHITE)
			return GenSortedMove<WHITE>(MovePtr);
		return GenSortedMove<BLACK>(MovePtr);
	}
	template <Colors Us>
	inline short GenSortedMove(S_MOVE* MovePtr) {
		short count = GenMove<Us>(MovePtr) - MovePtr;
		QuickSort(MovePtr, 0, count);
		return count;
	}

	inline const U64 getPosKey(void) { return PosKey; }

	inline void GenThreadMessage(S_ThreadMessage * msg) {
		msg->Side = Side;
		msg->EnPas = EnPas;
		msg->FiftyMove = FiftyMove;
		msg->CastlePerm = CastlePerm;
		msg->PosKey = PosKey;
		msg->PiecesBB[0] = PiecesBB[0];
		msg->PiecesBB[1] = PiecesBB[1];
		msg->PiecesBB[2] = PiecesBB[2];
		msg->PiecesBB[3] = PiecesBB[3];
		msg->PiecesBB[4] = PiecesBB[4];
		msg->PiecesBB[5] = PiecesBB[5];
		msg->PiecesBB[6] = PiecesBB[6];
		msg->PiecesBB[7] = PiecesBB[7];
		msg->PiecesBB[8] = PiecesBB[8];
		msg->PiecesBB[9] = PiecesBB[9];
		msg->PiecesBB[10] = PiecesBB[10];
		msg->PiecesBB[11] = PiecesBB[11];
	}
	inline S_ThreadMessage* GenThreadMessage(const short Mode, const short Depth) {
		return NewThreadMessage(Side, EnPas, FiftyMove, CastlePerm, PiecesBB, PosKey, Mode, Depth);
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
	inline short SearchPly(void) { return Ply; }

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

	inline short getPieceType(const short SQ) {
		U64 thisSQ = SetMask[SQ];
		if (thisSQ & PiecesBB[WhitePawn])
			return WhitePawn;
		if (thisSQ & PiecesBB[WhiteKnight])
			return WhiteKnight;
		if (thisSQ & PiecesBB[WhiteBishop])
			return WhiteBishop;
		if (thisSQ & PiecesBB[WhiteRook])
			return WhiteRook;
		if (thisSQ & PiecesBB[WhiteQueen])
			return WhiteQueen;
		if (thisSQ & PiecesBB[WhiteKing])
			return WhiteKing;

		if (thisSQ & PiecesBB[BlackPawn])
			return BlackPawn;
		if (thisSQ & PiecesBB[BlackKnight])
			return BlackKnight;
		if (thisSQ & PiecesBB[BlackBishop])
			return BlackBishop;
		if (thisSQ & PiecesBB[BlackRook])
			return BlackRook;
		if (thisSQ & PiecesBB[BlackQueen])
			return BlackQueen;
		if (thisSQ & PiecesBB[BlackKing])
			return BlackKing;
		return Empty;
	}

	inline void PrintBoard(void) {
		int file, rank;
		printing_console_start();
		printf("\nGame Board:\n\n");

		for (rank = RANK_8; rank >= RANK_1; rank--) {
			printf("%d   ", rank + 1);
			for (file = FILE_A; file <= FILE_H; file++)
				printf("%c  ", PceChar[getPieceType(FR2SQ(file, rank))][0]);
			printf("\n");
		}
		
		printf("\n  ");
		for (file = FILE_A; file <= FILE_H; file++) {
			printf("%3c", 'a' + file);
		}
		printf("\n\n");
		printf("side:%c\n", Side == WHITE ? 'w' : 'b');
		printf("enPas:%d\n", EnPas);
		printf("castle:%c%c%c%c\n",
			CastlePerm & WKCA ? 'K' : '-',
			CastlePerm & WQCA ? 'Q' : '-',
			CastlePerm & BKCA ? 'k' : '-',
			CastlePerm & BQCA ? 'q' : '-'
		);
		printf("PosKey:%llX\n", PosKey);
#ifdef PRINTBOARDDEBUG
		for (short i = 0; i < Empty; i++) {
			printf("%s:\n", piece_names[i]);
			PrintBitboard(PiecesBB[i]);
		}
#endif
		printing_console_end();
	}


	template <Colors Us>
	inline S_MOVE* GenMove(S_MOVE* MovePtr) {
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

		constexpr const short (*getMyPieceType)(const U64*, const short) = (Us == WHITE ? getWhitePieceType : getBlackPieceType);
		constexpr const short (*getEnPieceType)(const U64*, const short) = (Us == WHITE ? getBlackPieceType : getWhitePieceType);

		constexpr U64(*getMyPiecesBB)(const U64*) = (Us == WHITE ? getWhitePiecesBB : getBlackPiecesBB);
		constexpr U64(*getEnPiecesBB)(const U64*) = (Us == WHITE ? getBlackPiecesBB : getWhitePiecesBB);


		PremoveBBNormalExec = MOVE_NEW_PREMOVE_U64(NormalExec, CastlePerm, FiftyMove, EnPas);
		PremoveBBPawnDoubleMove = MOVE_NEW_PREMOVE_U64(PawnDoubleMove, CastlePerm, FiftyMove, EnPas);
		PremoveBBCapture = MOVE_NEW_PREMOVE_U64(Capture, CastlePerm, FiftyMove, EnPas);
		PremoveBBPromote = MOVE_NEW_PREMOVE_U64(Promote, CastlePerm, FiftyMove, EnPas);
		PremoveBBPromoteCapture = MOVE_NEW_PREMOVE_U64(PromoteCapture, CastlePerm, FiftyMove, EnPas);
		PremoveBBCastle = MOVE_NEW_PREMOVE_U64(Castle, CastlePerm, FiftyMove, EnPas);

		EnAttackBB = InCheckBlockingSQ_BB = InCheckAttackSQ_BB = PinnedPiecesBB = 0ULL;
		InCheckNum = 0;

		Opening = IsOpening<Us>(PiecesBB);
		if (Opening)
			Endgame = false;
		else
			Endgame = IsEndgame(PiecesBB);

		myKingBB = PiecesBB[myKING];
		myKingSQ = PopBit(&myKingBB);
		SetBit(&myKingBB, myKingSQ);

		friends = getMyPiecesBB(PiecesBB);
		enemys = getEnPiecesBB(PiecesBB);
		allPiecesBB = friends | enemys;
		emptyBB = ~allPiecesBB;

		getEnemyAttack<enPAWN>(PiecesBB[enPAWN]);
		getPinByAttackSQ<Us, true>(myKingSQ);
		// get enPas
		if (EnPas != NO_SQ) {
			// get rid of EnPas-calc by check while doing move if any possible
			U64 EnPasPieceBB = (Us == WHITE ? (AttackBrdbPawnBB[EnPas]) : (AttackBrdwPawnBB[EnPas])) & PiecesBB[myPAWN];
			if (EnPasPieceBB) {
				PremoveBBEnPassant = MOVE_NEW_PREMOVE_U64(EnPassant, CastlePerm, FiftyMove, EnPas);
				while (EnPasPieceBB) {
					//MOVE_NEW_MOVE(int mod, int f, int t, int mov, int ca, int pro, int cp, int fm, int ep)		

					U64 tempMove = MOVE_NEW_U64(PopBit(&EnPasPieceBB), EnPas, myPAWN, Empty, Empty, PremoveBBEnPassant);
					//make EnPas move and check valid or not
					DoMove<Us>(tempMove);
					U64 _allPiecesBB = getWhitePiecesBB(PiecesBB) | getBlackPiecesBB(PiecesBB);
					// now check attacks from king_sq - only sliding moves are possible checks
					// save move if not in check			
					if (((magicGetBishopAttackBB(myKingSQ, _allPiecesBB) & (PiecesBB[enQUEEN] | PiecesBB[enBISHOP])) | (magicGetRookAttackBB(myKingSQ, _allPiecesBB) & (PiecesBB[enQUEEN] | PiecesBB[enROOK]))) == 0ULL) {
						MovePtr = NEW_MOVE(MovePtr, tempMove, PIECES_VALUES_ABS[enPAWN] + 1000);
					}
					UndoMove<Us>();
				}
			}
		}
		InCheck = InCheckAttackSQ_BB != 0ULL;

		// get king moves
		U64 tempAttackBB = AttackBrdKingBB[myKingSQ] & ~EnAttackBB;
		//king moves
		U64 PieceAttacksBB = tempAttackBB & emptyBB;
		while (PieceAttacksBB) {
			toSQ = PopBit(&PieceAttacksBB);
			MovePtr = NEW_MOVE(
				MovePtr,
				myKingSQ,
				toSQ,
				myKING,
				Empty,
				Empty,
				PremoveBBNormalExec,
				getMoveValue(myKING, myKingSQ, toSQ)
			);
		}

		PieceAttacksBB = tempAttackBB & enemys;
		while (PieceAttacksBB) {
			toSQ = PopBit(&PieceAttacksBB);
			MovePtr = NEW_MOVE(
				MovePtr,
				myKingSQ,
				toSQ,
				myKING,
				getEnPieceType(PiecesBB, toSQ),
				Empty,
				PremoveBBCapture,
				getMoveValue(myKING, myKingSQ, toSQ)
			);
		}
		U64 MoveBB;

		if (InCheck)
			goto KingInCheck;
		// ---------------------------------------
		//		king NOT in check area
		// ---------------------------------------

				// castle
		if (CastleRightsBB & CastlePerm) {
			if (CastleKingSideBB & CastlePerm)
				if ((allPiecesBB & KingCastleFreeBB) == 0ULL)
					if ((EnAttackBB & KingCastleCheckBB) == 0ULL)
						MovePtr = NEW_MOVE(
							MovePtr,
							CastleFrom,
							CastleKingSideTo,
							myKING,
							Empty,
							Empty,
							PremoveBBCastle,
							getMoveValue(myKING, CastleFrom, CastleKingSideTo) + (Endgame ? -10 : 100)
						);

			if (CastleQueenSideBB & CastlePerm)
				if ((allPiecesBB & QueenCastleFreeBB) == 0ULL)
					if ((EnAttackBB & QueenCastleCheckBB) == 0ULL)
						MovePtr = NEW_MOVE(
							MovePtr,
							CastleFrom,
							CastleQueenSideTo,
							myKING,
							Empty,
							Empty,
							PremoveBBCastle,
							getMoveValue(myKING, CastleFrom, CastleQueenSideTo) + (Endgame ? -10 : 100)
						);
		}

		// pawn moves
		MoveBB = PiecesBB[myPAWN];
		while (MoveBB) {
			fromSQ = PopBit(&MoveBB);

			// -> normal move
			if (PinnedPiecesBB & SetMask[fromSQ]) {
				tempAttackBB = PinBySquare[fromSQ]->blockingBB & getPawnMoveBoard<Us>(fromSQ) & emptyBB;
			}
			else {
				tempAttackBB = getPawnMoveBoard<Us>(fromSQ) & emptyBB;
			}
			if (tempAttackBB) {
				// rank 2? -> double move
				if (SetMask[fromSQ] & Rank2BB) {
					// + normal move
					MovePtr = NEW_MOVE(
						MovePtr,
						fromSQ,
						getPawnMoveSQ<Us>(fromSQ),
						myPAWN,
						Empty,
						Empty,
						PremoveBBNormalExec,
						getMoveValue(myPAWN, fromSQ, getPawnMoveSQ<Us>(fromSQ))
					);
					if (getPawnDoubleMoveBoard<Us>(fromSQ)& emptyBB) {
						MovePtr = NEW_MOVE(
							MovePtr,
							fromSQ,
							getPawnDoubleMoveSQ<Us>(fromSQ),
							myPAWN,
							Empty,
							Empty,
							PremoveBBPawnDoubleMove,
							getMoveValue(myPAWN, fromSQ, getPawnDoubleMoveSQ<Us>(fromSQ))
						);
					}
				}
				else if (tempAttackBB & Rank8BB) {// promote
					toSQ = PopBit(&tempAttackBB);
					short captureType = getEnPieceType(PiecesBB, toSQ);
					MovePtr = NEW_MOVE(
						MovePtr,
						fromSQ,
						toSQ,
						myPAWN,
						Empty,
						myKNIGHT,
						PremoveBBPromote,
						getMoveValue(myPAWN, fromSQ, toSQ)
					);

					MovePtr = NEW_MOVE(
						MovePtr,
						fromSQ,
						toSQ,
						myPAWN,
						Empty,
						myBISHOP,
						PremoveBBPromote,
						getMoveValue(myPAWN, fromSQ, toSQ)
					);

					MovePtr = NEW_MOVE(
						MovePtr,
						fromSQ,
						toSQ,
						myPAWN,
						Empty,
						myROOK,
						PremoveBBPromote,
						getMoveValue(myPAWN, fromSQ, toSQ)
					);

					MovePtr = NEW_MOVE(
						MovePtr,
						fromSQ,
						toSQ,
						myPAWN,
						Empty,
						myQUEEN,
						PremoveBBPromote,
						getMoveValue(myPAWN, fromSQ, toSQ)
					);
				}
				else {// normal move
					MovePtr = NEW_MOVE(
						MovePtr,
						fromSQ,
						getPawnMoveSQ<Us>(fromSQ),
						myPAWN,
						Empty,
						Empty,
						PremoveBBNormalExec,
						getMoveValue(myPAWN, fromSQ, getPawnMoveSQ<Us>(fromSQ))
					);
				}
			}
			if (PinnedPiecesBB & SetMask[fromSQ]) {
				tempAttackBB = PinBySquare[fromSQ]->blockingBB & getPawnAttackBoard<Us>(fromSQ) & enemys;
			}
			else {
				tempAttackBB = getPawnAttackBoard<Us>(fromSQ) & enemys;
			}
			if (tempAttackBB & Rank8BB) {// promote capture
				while (tempAttackBB)
				{
					toSQ = PopBit(&tempAttackBB);
					short captureType = getEnPieceType(PiecesBB, toSQ);
					MovePtr = NEW_MOVE(
						MovePtr,
						fromSQ,
						toSQ,
						myPAWN,
						captureType,
						myKNIGHT,
						PremoveBBPromoteCapture,
						getMoveValue(myPAWN, fromSQ, toSQ)
					);

					MovePtr = NEW_MOVE(
						MovePtr,
						fromSQ,
						toSQ,
						myPAWN,
						captureType,
						myBISHOP,
						PremoveBBPromoteCapture,
						getMoveValue(myPAWN, fromSQ, toSQ)
					);

					MovePtr = NEW_MOVE(
						MovePtr,
						fromSQ,
						toSQ,
						myPAWN,
						captureType,
						myROOK,
						PremoveBBPromoteCapture,
						getMoveValue(myPAWN, fromSQ, toSQ)
					);

					MovePtr = NEW_MOVE(
						MovePtr,
						fromSQ,
						toSQ,
						myPAWN,
						captureType,
						myQUEEN,
						PremoveBBPromoteCapture,
						getMoveValue(myPAWN, fromSQ, toSQ)
					);
				}
			}
			else {// capture
				while (tempAttackBB)
				{
					toSQ = PopBit(&tempAttackBB);
					MovePtr = NEW_MOVE(
						MovePtr,
						fromSQ,
						toSQ,
						myPAWN,
						getEnPieceType(PiecesBB, toSQ),
						Empty,
						PremoveBBPromoteCapture,
						getMoveValue(myPAWN, fromSQ, toSQ)
					);
				}
			}
		}

		// knight moves - only if not pinned
		MoveBB = PiecesBB[myKNIGHT] & ~PinnedPiecesBB;
		while (MoveBB) {
			fromSQ = PopBit(&MoveBB);
			tempAttackBB = AttackBrdKnightBB[fromSQ];

			PieceAttacksBB = tempAttackBB & emptyBB;
			while (PieceAttacksBB) {
				toSQ = PopBit(&PieceAttacksBB);
				MovePtr = NEW_MOVE(
					MovePtr,
					fromSQ,
					toSQ,
					myKNIGHT,
					Empty,
					Empty,
					PremoveBBNormalExec,
					getMoveValue(myKNIGHT, fromSQ, toSQ)
				);
			}
			PieceAttacksBB = tempAttackBB & enemys;
			while (PieceAttacksBB) {
				toSQ = PopBit(&PieceAttacksBB);
				MovePtr = NEW_MOVE(
					MovePtr,
					fromSQ,
					toSQ,
					myKNIGHT,
					getEnPieceType(PiecesBB, toSQ),
					Empty,
					PremoveBBCapture,
					getMoveValue(myKNIGHT, fromSQ, toSQ)
				);
			}
		}
		// bishop moves
		MoveBB = PiecesBB[myBISHOP];
		while (MoveBB) {
			fromSQ = PopBit(&MoveBB);
			if (PinnedPiecesBB & SetMask[fromSQ]) {
				tempAttackBB = PinBySquare[fromSQ]->blockingBB & magicGetBishopAttackBB(fromSQ, allPiecesBB);
			}
			else {
				tempAttackBB = magicGetBishopAttackBB(fromSQ, allPiecesBB);
			}
			PieceAttacksBB = tempAttackBB & emptyBB;
			while (PieceAttacksBB) {
				toSQ = PopBit(&PieceAttacksBB);
				MovePtr = NEW_MOVE(
					MovePtr,
					fromSQ,
					toSQ,
					myBISHOP,
					Empty,
					Empty,
					PremoveBBNormalExec,
					getMoveValue(myBISHOP, fromSQ, toSQ)
				);
			}
			PieceAttacksBB = tempAttackBB & enemys;
			while (PieceAttacksBB) {
				toSQ = PopBit(&PieceAttacksBB);
				MovePtr = NEW_MOVE(
					MovePtr,
					fromSQ,
					toSQ,
					myBISHOP,
					getEnPieceType(PiecesBB, toSQ),
					Empty,
					PremoveBBCapture,
					getMoveValue(myBISHOP, fromSQ, toSQ)
				);
			}
		}
		// rook moves
		MoveBB = PiecesBB[myROOK];
		while (MoveBB) {
			fromSQ = PopBit(&MoveBB);
			if (PinnedPiecesBB & SetMask[fromSQ]) {
				tempAttackBB = PinBySquare[fromSQ]->blockingBB & magicGetRookAttackBB(fromSQ, allPiecesBB);
			}
			else {
				tempAttackBB = magicGetRookAttackBB(fromSQ, allPiecesBB);
			}
			PieceAttacksBB = tempAttackBB & emptyBB;
			while (PieceAttacksBB) {
				toSQ = PopBit(&PieceAttacksBB);
				MovePtr = NEW_MOVE(
					MovePtr,
					fromSQ,
					toSQ,
					myROOK,
					Empty,
					Empty,
					PremoveBBNormalExec,
					getMoveValue(myROOK, fromSQ, toSQ)
				);
			}
			PieceAttacksBB = tempAttackBB & enemys;
			while (PieceAttacksBB) {
				toSQ = PopBit(&PieceAttacksBB);
				MovePtr = NEW_MOVE(
					MovePtr,
					fromSQ,
					toSQ,
					myROOK,
					getEnPieceType(PiecesBB, toSQ),
					Empty,
					PremoveBBCapture,
					getMoveValue(myROOK, fromSQ, toSQ)
				);
			}
		}
		// queen moves
		MoveBB = PiecesBB[myQUEEN];
		while (MoveBB) {
			fromSQ = PopBit(&MoveBB);
			if (PinnedPiecesBB & SetMask[fromSQ]) {
				tempAttackBB = PinBySquare[fromSQ]->blockingBB & magicGetQueenAttackBB(fromSQ, allPiecesBB);
			}
			else {
				tempAttackBB = magicGetQueenAttackBB(fromSQ, allPiecesBB);
			}
			PieceAttacksBB = tempAttackBB & emptyBB;
			while (PieceAttacksBB) {
				toSQ = PopBit(&PieceAttacksBB);
				MovePtr = NEW_MOVE(
					MovePtr,
					fromSQ,
					toSQ,
					myQUEEN,
					Empty,
					Empty,
					PremoveBBNormalExec,
					getMoveValue(myQUEEN, fromSQ, toSQ)
				);
			}
			PieceAttacksBB = tempAttackBB & enemys;
			while (PieceAttacksBB) {
				toSQ = PopBit(&PieceAttacksBB);
				MovePtr = NEW_MOVE(
					MovePtr,
					fromSQ,
					toSQ,
					myQUEEN,
					getEnPieceType(PiecesBB, toSQ),
					Empty,
					PremoveBBCapture,
					getMoveValue(myQUEEN, fromSQ, toSQ)
				);
			}
		}

		return MovePtr;
		// ---------------------------------------
		//		king in check area
		// ---------------------------------------
	KingInCheck:
		if (InCheckNum == 1) {
			// double check foces king move
			// thereforce this must not be checked when king is attacked by more then one piece

			// block moves can be checked by InCheckBlockingSQ_BB			
			while (InCheckBlockingSQ_BB)
				MovePtr = getMoveToGivenSQ<Us, false>(PopBit(&InCheckBlockingSQ_BB), MovePtr, Empty);

			// capture moves can be checked by InCheckAttackSQ_BB
			MovePtr = getMoveToGivenSQ<Us, true>(PopBit(&InCheckAttackSQ_BB), MovePtr, InCheckType);
		}

		return MovePtr;
	}


	template <Colors Us>
	inline void DoMove(const U64 move) {
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
		constexpr short (*getEnPasSQ)(const short) = (Us == WHITE ? getEnPasSQWhite : getEnPasSQBlack);

		int fromSQ = MOVE_FROMSQ(move), toSQ = MOVE_TOSQ(move);
		Side = Them;
		History[HisPly++] = move;
		Ply++;

		int moved = MOVE_MOVED(move), promoted, captured, enPas;
		HASH_PCE(moved, fromSQ);
		HASH_PCE(moved, toSQ);
		HASH_SIDE();
		if (EnPas != NO_SQ) {
			HASH_EP();
			EnPas = NO_SQ;
		}

		switch (MOVE_MODE(move))
		{
		case NormalExec:
			MovePiece(moved, fromSQ, toSQ);
			FiftyMove++;
			HASH_CA();
			CastlePerm = CastlePerm & CastlePermArr[fromSQ] & CastlePermArr[toSQ];
			HASH_CA();
			return;
		case PawnDoubleMove:
			MovePiece(myPAWN, fromSQ, toSQ);
			FiftyMove = 0;
			EnPas = getEnPasSQ(toSQ);
			HASH_EP();
			return;
		case Castle:
			MovePiece(myKING, fromSQ, toSQ);
			if (CastleKingSideSQ == toSQ) {
				HASH_PCE(myROOK, CastleKingSideRStartSQ);
				HASH_PCE(myROOK, CastleKingSideRStopSQ);
				MovePiece(myROOK, CastleKingSideRStartSQ, CastleKingSideRStopSQ);
			}
			else {
				HASH_PCE(myROOK, CastleQueenSideRStartSQ);
				HASH_PCE(myROOK, CastleQueenSideRStopSQ);
				MovePiece(myROOK, CastleQueenSideRStartSQ, CastleQueenSideRStopSQ);
			}
			FiftyMove++;
			HASH_CA();
			CastlePerm = CastlePerm & CastlePermArr[fromSQ] & CastlePermArr[toSQ];
			HASH_CA();
			return;
		case EnPassant:
			enPas = getEnPasSQ(toSQ);
			HASH_PCE(enPAWN, enPas);
			MovePiece(myPAWN, fromSQ, toSQ);
			DelPiece(enPAWN, enPas);
			FiftyMove = 0;
			return;
		case Capture:
			captured = MOVE_CAPTURED(move);
			DelPiece(captured, toSQ);
			HASH_PCE(captured, toSQ);
			MovePiece(moved, fromSQ, toSQ);
			FiftyMove = 0;
			HASH_CA();
			CastlePerm = CastlePerm & CastlePermArr[fromSQ] & CastlePermArr[toSQ];
			HASH_CA();
			return;
		case Promote:
			promoted = MOVE_PROMOTED(move);
			MovePiece(myPAWN, fromSQ, toSQ);
			ModPiece(myPAWN, promoted, toSQ);
			HASH_PCE(myPAWN, toSQ);
			HASH_PCE(promoted, toSQ);
			FiftyMove = 0;
			return;
		case PromoteCapture:
			promoted = MOVE_PROMOTED(move);
			captured = MOVE_CAPTURED(move);
			DelPiece(captured, toSQ);
			MovePiece(myPAWN, fromSQ, toSQ);
			ModPiece(myPAWN, promoted, toSQ);
			HASH_PCE(myPAWN, toSQ);
			HASH_PCE(captured, toSQ);
			HASH_PCE(promoted, toSQ);
			FiftyMove = 0;
			HASH_CA();
			CastlePerm = CastlePerm & CastlePermArr[fromSQ] & CastlePermArr[toSQ];
			HASH_CA();
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
	inline void UndoMove(void) {
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
		constexpr short (*getEnPasSQ)(const short) = (Us == WHITE ? getEnPasSQWhite : getEnPasSQBlack);

		U64 move = History[--HisPly];

		int fromSQ = MOVE_FROMSQ(move), toSQ = MOVE_TOSQ(move);
		FiftyMove = MOVE_FIFTY_MOVE(move);
		if (EnPas != NO_SQ) HASH_EP();
		EnPas = MOVE_ENPAS(move);
		if (EnPas != NO_SQ) HASH_EP();
		HASH_CA();
		CastlePerm = MOVE_CASTLE_PERM(move);
		HASH_CA();
		Ply--;
		Side = Us;
		HASH_SIDE();

		int moved = MOVE_MOVED(move), promoted, captured, enPas;

		switch (MOVE_MODE(move))
		{
		case NormalExec:
			MovePiece(moved, toSQ, fromSQ);
			HASH_PCE(moved, fromSQ);
			HASH_PCE(moved, toSQ);
			return;
		case PawnDoubleMove:
			MovePiece(myPAWN, toSQ, fromSQ);
			HASH_PCE(myPAWN, fromSQ);
			HASH_PCE(myPAWN, toSQ);
			return;
		case Castle:
			MovePiece(myKING, toSQ, fromSQ);
			HASH_PCE(myKING, fromSQ);
			HASH_PCE(myKING, toSQ);
			if (CastleKingSideSQ == toSQ) {
				HASH_PCE(myROOK, CastleKingSideRStartSQ);
				HASH_PCE(myROOK, CastleKingSideRStopSQ);
				MovePiece(myROOK, CastleKingSideRStopSQ, CastleKingSideRStartSQ);
			}
			else {
				HASH_PCE(myROOK, CastleQueenSideRStartSQ);
				HASH_PCE(myROOK, CastleQueenSideRStopSQ);
				MovePiece(myROOK, CastleQueenSideRStopSQ, CastleQueenSideRStartSQ);
			}
			return;
		case EnPassant:
			HASH_PCE(myPAWN, fromSQ);
			HASH_PCE(myPAWN, toSQ);
			enPas = getEnPasSQ(toSQ);
			HASH_PCE(enPAWN, enPas);
			MovePiece(myPAWN, toSQ, fromSQ);
			NewPiece(enPAWN, enPas);
			return;
		case Capture:
			captured = MOVE_CAPTURED(move);
			HASH_PCE(moved, fromSQ);
			HASH_PCE(moved, toSQ);
			HASH_PCE(captured, toSQ);
			MovePiece(moved, toSQ, fromSQ);
			NewPiece(captured, toSQ);
			return;
		case Promote:
			HASH_PCE(myPAWN, fromSQ);
			promoted = MOVE_PROMOTED(move);
			HASH_PCE(promoted, toSQ);
			ModPiece(promoted, myPAWN, toSQ);
			MovePiece(myPAWN, toSQ, fromSQ);
			return;
		case PromoteCapture:
			HASH_PCE(myPAWN, fromSQ);
			promoted = MOVE_PROMOTED(move);
			captured = MOVE_CAPTURED(move);
			HASH_PCE(captured, toSQ);
			HASH_PCE(promoted, toSQ);
			ModPiece(promoted, myPAWN, toSQ);
			MovePiece(myPAWN, toSQ, fromSQ);
			NewPiece(captured, toSQ);
			return;
		default:
			ASSERT(false);
		}
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
	inline void ModPiece(const short OldType, const short NewType, const short SQ) {
		ClearBit(&PiecesBB[OldType], SQ);
		SetBit(&PiecesBB[NewType], SQ);
	}


	U64 PremoveBBNormalExec = 0ULL;
	U64 PremoveBBPawnDoubleMove = 0ULL;
	U64 PremoveBBCapture = 0ULL;
	U64 PremoveBBPromote = 0ULL;
	U64 PremoveBBPromoteCapture = 0ULL;
	U64 PremoveBBCastle = 0ULL;
	U64 PremoveBBEnPassant = 0ULL;

	U64 EnAttackBB = 0ULL;
	U64 InCheckBlockingSQ_BB = 0ULL;
	U64 InCheckAttackSQ_BB = 0ULL;
	short InCheckNum = 0;
	short InCheckType = 0;
	bool InCheck = false;

	bool Opening = false;
	bool Endgame = false;

	short myKingSQ = NO_SQ;
	U64 myKingBB = 0ULL;

	S_PinnedPiece ** PinBySquare = nullptr;
	U64 PinnedPiecesBB = 0ULL;

	U64 allPiecesBB = 0ULL;
	U64 emptyBB = 0ULL;
	U64 friends = 0ULL;
	U64 enemys = 0ULL;

	short fromSQ = 0, toSQ = 0;

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

	/*
	get pin for given attack_sq
	*/
	template <Colors Us, bool Absolute>
	inline void getPinByAttackSQ(short attack_sq) {
		constexpr Pieces enBISHOP = Us == BLACK ? WhiteBishop : BlackBishop;
		constexpr Pieces enROOK = Us == BLACK ? WhiteRook : BlackRook;
		constexpr Pieces enQUEEN = Us == BLACK ? WhiteQueen : BlackQueen;

		U64 kingBishopAttackBB = magicGetBishopAttackBB(attack_sq, allPiecesBB) & friends;
		U64 kingRookAttackBB = magicGetRookAttackBB(attack_sq, allPiecesBB) & friends;

		while (kingBishopAttackBB) {
			fromSQ = PopBit(&kingBishopAttackBB);
			short pinDir = getBishopDirection(attack_sq, fromSQ);
			ClearBit(&allPiecesBB, fromSQ);
			U64 newAttackBB = magicGetBishopAttackBB(attack_sq, allPiecesBB) & enemys & getRay(pinDir, attack_sq);

			if (newAttackBB & (PiecesBB[enBISHOP] | PiecesBB[enQUEEN])) {
				S_PinnedPiece* temp = (S_PinnedPiece*)pMemory->AllocFrameMemory(sizeof(S_PinnedPiece));

				temp->absolutePin = Absolute;
				temp->attack_sq = attack_sq;
				temp->pinner_sq = PopBit(&newAttackBB);
				temp->blockingBB = (getRay(pinDir, attack_sq) & getRay(getOppositeDir(pinDir), temp->pinner_sq) & ClearMask[fromSQ]) | SetMask[temp->pinner_sq];

				temp->next = PinBySquare[fromSQ];
				PinBySquare[fromSQ] = temp;

				PinnedPiecesBB |= SetMask[fromSQ];
			}

			SetBit(&allPiecesBB, fromSQ);
		}

		while (kingRookAttackBB) {
			fromSQ = PopBit(&kingRookAttackBB);
			short pinDir = getRookDirection(attack_sq, fromSQ);
			ClearBit(&allPiecesBB, fromSQ);
			U64 newAttackBB = magicGetRookAttackBB(attack_sq, allPiecesBB) & enemys & getRay(pinDir, attack_sq);

			if (newAttackBB & (PiecesBB[enROOK] | PiecesBB[enQUEEN])) {
				S_PinnedPiece* temp = (S_PinnedPiece*)pMemory->AllocFrameMemory(sizeof(S_PinnedPiece));

				temp->absolutePin = Absolute;
				temp->attack_sq = attack_sq;
				temp->pinner_sq = PopBit(&newAttackBB);
				temp->blockingBB = (getRay(pinDir, attack_sq) & getRay(getOppositeDir(pinDir), temp->pinner_sq) & ClearMask[fromSQ]) | SetMask[temp->pinner_sq];

				temp->next = PinBySquare[fromSQ];
				PinBySquare[fromSQ] = temp;

				PinnedPiecesBB |= SetMask[fromSQ];
			}

			SetBit(&allPiecesBB, fromSQ);
		}
	}

	inline BoardValue getMoveValue(const short PieceType, const short fromSQ, const short toSQ) {
		BoardValue Value = 0;
		U64 tempAttack;
		// compare field values
		// get value for new attacks and mobility
		// .. for this piece
		switch (PieceType) {
		case WhitePawn:
			Value += PawnTable[toSQ] - PawnTable[fromSQ] + (CountBits(AttackBrdwPawnBB[toSQ] & enemys) * AttackFaktor);
			break;
		case BlackPawn:
			Value += mirrorBoard[PawnTable[toSQ]] - mirrorBoard[PawnTable[fromSQ]] + (CountBits(AttackBrdbPawnBB[toSQ] & enemys) * AttackFaktor);
			break;
		case WhiteKnight:
			Value += KnightTable[toSQ] - KnightTable[fromSQ] + (CountBits(AttackBrdKnightBB[toSQ] & enemys) * AttackFaktor);
			break;
		case BlackKnight:
			Value += mirrorBoard[KnightTable[toSQ]] - mirrorBoard[KnightTable[fromSQ]] + (CountBits(AttackBrdKnightBB[toSQ] & enemys) * AttackFaktor);
			break;
		case WhiteBishop:
			tempAttack = magicGetBishopAttackBB(toSQ,allPiecesBB);
			Value += BishopTable[toSQ] - BishopTable[fromSQ] + (CountBits(tempAttack & enemys) * AttackFaktor) + (CountBits(tempAttack) * MobilityFaktor);
			break;
		case BlackBishop:
			tempAttack = magicGetBishopAttackBB(toSQ, allPiecesBB);
			Value += mirrorBoard[BishopTable[toSQ]] - mirrorBoard[BishopTable[fromSQ]] + (CountBits(tempAttack & enemys) * AttackFaktor) + (CountBits(tempAttack) * MobilityFaktor);
			break;
		case WhiteRook:
			tempAttack = magicGetRookAttackBB(toSQ, allPiecesBB);
			Value += RookTable[toSQ] - RookTable[fromSQ] + (CountBits(tempAttack & enemys) * AttackFaktor) + (CountBits(tempAttack) * MobilityFaktor);
			break;
		case BlackRook:
			tempAttack = magicGetRookAttackBB(toSQ, allPiecesBB);
			Value += mirrorBoard[RookTable[toSQ]] - mirrorBoard[RookTable[fromSQ]] + (CountBits(tempAttack & enemys) * AttackFaktor) + (CountBits(tempAttack) * MobilityFaktor);
			break;
		case WhiteQueen:
			tempAttack = magicGetQueenAttackBB(toSQ, allPiecesBB);
			Value += QueenTable[toSQ] - QueenTable[fromSQ] + (CountBits(tempAttack & enemys) * AttackFaktor) + (CountBits(tempAttack) * MobilityFaktor);
			break;
		case BlackQueen:
			tempAttack = magicGetQueenAttackBB(toSQ, allPiecesBB);
			Value += mirrorBoard[QueenTable[toSQ]] - mirrorBoard[QueenTable[fromSQ]] + (CountBits(tempAttack & enemys) * AttackFaktor) + (CountBits(tempAttack) * MobilityFaktor);
			break;
		case WhiteKing:
			if(Endgame)
				Value += KingTableEndgame[toSQ] - KingTableEndgame[fromSQ] + (CountBits(AttackBrdKingBB[toSQ] & enemys) * AttackFaktor);
			else
				Value += KingTableOpening[toSQ] - KingTableOpening[fromSQ];
			break;
		case BlackKing:
			if (Endgame)
				Value += mirrorBoard[KingTableEndgame[toSQ]] - mirrorBoard[KingTableEndgame[fromSQ]] + (CountBits(AttackBrdKingBB[toSQ] & enemys) * AttackFaktor);
			else
				Value += mirrorBoard[KingTableOpening[toSQ]] - mirrorBoard[KingTableOpening[fromSQ]];
			break;
		}

		return Value;
	}


	/*
		obtain moves by given destination square
	*/
	template <Colors Us, bool Capture>
	S_MOVE* getMoveToGivenSQ(const short toSQ, S_MOVE* MovePtr, const short captureType) {

		constexpr Pieces myPAWN = Us == WHITE ? WhitePawn : BlackPawn;
		constexpr Pieces myKNIGHT = Us == WHITE ? WhiteKnight : BlackKnight;
		constexpr Pieces myBISHOP = Us == WHITE ? WhiteBishop : BlackBishop;
		constexpr Pieces myROOK = Us == WHITE ? WhiteRook : BlackRook;
		constexpr Pieces myQUEEN = Us == WHITE ? WhiteQueen : BlackQueen;
		constexpr Pieces myKING = Us == WHITE ? WhiteKing : BlackKing;

		constexpr U64 Rank7BB = Us == WHITE ? the7Rank : the2Rank;
		constexpr U64 Rank4BB = Us == WHITE ? the4Rank : the5Rank;

		constexpr U64 AttackBrdPawnBB[64] = Us == WHITE ? AttackBrdwPawnBB : AttackBrdbPawnBB;

		U64 BishopAttackBB = magicGetBishopAttackBB(toSQ, allPiecesBB);
		U64 RookAttackBB = magicGetRookAttackBB(toSQ, allPiecesBB);
		U64 tempBB;
		
		if (Capture) {

			tempBB = getPawnAttackBoard_reverse<Us>(toSQ) & PiecesBB[myPAWN];
			if (tempBB & Rank7BB) {// promote
				while (tempBB) {
					fromSQ = PopBit(&tempBB);
					if (PinnedPiecesBB & SetMask[fromSQ]) {
						if (PinBySquare[fromSQ]->blockingBB & SetMask[toSQ]) {

							MovePtr = NEW_MOVE(
								MovePtr,
								fromSQ,
								toSQ,
								myPAWN,
								captureType,
								myKNIGHT,
								PremoveBBPromoteCapture,
								getMoveValue(myPAWN, fromSQ, toSQ)
							);

							MovePtr = NEW_MOVE(
								MovePtr,
								fromSQ,
								toSQ,
								myPAWN,
								captureType,
								myBISHOP,
								PremoveBBPromoteCapture,
								getMoveValue(myPAWN, fromSQ, toSQ)
							);

							MovePtr = NEW_MOVE(
								MovePtr,
								fromSQ,
								toSQ,
								myPAWN,
								captureType,
								myROOK,
								PremoveBBPromoteCapture,
								getMoveValue(myPAWN, fromSQ, toSQ)
							);

							MovePtr = NEW_MOVE(
								MovePtr,
								fromSQ,
								toSQ,
								myPAWN,
								captureType,
								myQUEEN,
								PremoveBBPromoteCapture,
								getMoveValue(myPAWN, fromSQ, toSQ)
							);
						}
					}
					else {

						MovePtr = NEW_MOVE(
							MovePtr,
							fromSQ,
							toSQ,
							myPAWN,
							captureType,
							myKNIGHT,
							PremoveBBPromoteCapture,
							getMoveValue(myPAWN, fromSQ, toSQ)
						);

						MovePtr = NEW_MOVE(
							MovePtr,
							fromSQ,
							toSQ,
							myPAWN,
							captureType,
							myBISHOP,
							PremoveBBPromoteCapture,
							getMoveValue(myPAWN, fromSQ, toSQ)
						);

						MovePtr = NEW_MOVE(
							MovePtr,
							fromSQ,
							toSQ,
							myPAWN,
							captureType,
							myROOK,
							PremoveBBPromoteCapture,
							getMoveValue(myPAWN, fromSQ, toSQ)
						);

						MovePtr = NEW_MOVE(
							MovePtr,
							fromSQ,
							toSQ,
							myPAWN,
							captureType,
							myQUEEN,
							PremoveBBPromoteCapture,
							getMoveValue(myPAWN, fromSQ, toSQ)
						);
					}
				}
			}
			else {// normal pawn capture
				while (tempBB) {
					fromSQ = PopBit(&tempBB);
					if (PinnedPiecesBB & SetMask[fromSQ]) {
						if (PinBySquare[fromSQ]->blockingBB & SetMask[toSQ])
							MovePtr = NEW_MOVE(
								MovePtr,
								fromSQ,
								toSQ,
								myPAWN,
								captureType,
								Empty,
								PremoveBBCapture,
								getMoveValue(myPAWN, fromSQ, toSQ)
							);
					}
					else {
						MovePtr = NEW_MOVE(
							MovePtr,
							fromSQ,
							toSQ,
							myPAWN,
							captureType,
							Empty,
							PremoveBBCapture,
							getMoveValue(myPAWN, fromSQ, toSQ)
						);
					}
				}
			}



			tempBB = AttackBrdKnightBB[toSQ] & PiecesBB[myKNIGHT];
			while (tempBB) {
				fromSQ = PopBit(&tempBB);
				if (PinnedPiecesBB & SetMask[fromSQ])
					continue;

				MovePtr = NEW_MOVE(
					MovePtr,
					fromSQ,
					toSQ,
					myKNIGHT,
					captureType,
					Empty,
					PremoveBBCapture,
					getMoveValue(myKNIGHT, fromSQ, toSQ)
				);
			}



			tempBB = BishopAttackBB & PiecesBB[myBISHOP];
			while (tempBB) {
				fromSQ = PopBit(&tempBB);
				if (PinnedPiecesBB & SetMask[fromSQ]) {
					if (PinBySquare[fromSQ]->blockingBB & SetMask[toSQ])
						MovePtr = NEW_MOVE(
							MovePtr,
							fromSQ,
							toSQ,
							myBISHOP,
							captureType,
							Empty,
							PremoveBBCapture,
							getMoveValue(myBISHOP, fromSQ, toSQ)
						);
				}
				else {
					MovePtr = NEW_MOVE(
						MovePtr,
						fromSQ,
						toSQ,
						myBISHOP,
						captureType,
						Empty,
						PremoveBBCapture,
						getMoveValue(myBISHOP, fromSQ, toSQ)
					);
				}
			}

			tempBB = RookAttackBB & PiecesBB[myROOK];
			while (tempBB) {
				fromSQ = PopBit(&tempBB);
				if (PinnedPiecesBB & SetMask[fromSQ]) {
					if (PinBySquare[fromSQ]->blockingBB & SetMask[toSQ])
						MovePtr = NEW_MOVE(
							MovePtr,
							fromSQ,
							toSQ,
							myROOK,
							captureType,
							Empty,
							PremoveBBCapture,
							getMoveValue(myROOK, fromSQ, toSQ)
						);
				}
				else {
					MovePtr = NEW_MOVE(
						MovePtr,
						fromSQ,
						toSQ,
						myROOK,
						captureType,
						Empty,
						PremoveBBCapture,
						getMoveValue(myROOK, fromSQ, toSQ)
					);
				}
			}

			tempBB = (BishopAttackBB | RookAttackBB) & PiecesBB[myQUEEN];
			while (tempBB) {
				fromSQ = PopBit(&tempBB);
				if (PinnedPiecesBB & SetMask[fromSQ]) {
					if (PinBySquare[fromSQ]->blockingBB & SetMask[toSQ])
						MovePtr = NEW_MOVE(
							MovePtr,
							fromSQ,
							toSQ,
							myQUEEN,
							captureType,
							Empty,
							PremoveBBCapture,
							getMoveValue(myQUEEN, fromSQ, toSQ)
						);
				}
				else {
					MovePtr = NEW_MOVE(
						MovePtr,
						fromSQ,
						toSQ,
						myQUEEN,
						captureType,
						Empty,
						PremoveBBCapture,
						getMoveValue(myQUEEN, fromSQ, toSQ)
					);
				}
			}


		}
		else {// blocking moves

			if ((SetMask[toSQ] & Rank4BB)) {
				tempBB = getPawnDoubleMoveBoard_reverse<Us>(toSQ) & PiecesBB[myPAWN];
				if (getPawnMoveBoard_reverse<Us>(toSQ)& emptyBB)
					if (tempBB) {
						fromSQ = PopBit(&tempBB);
						if (getPawnMoveBoard<Us>(fromSQ)& emptyBB) {
							if (PinnedPiecesBB & SetMask[fromSQ]) {
								if (PinBySquare[fromSQ]->blockingBB & SetMask[toSQ])
									MovePtr = NEW_MOVE(
										MovePtr,
										fromSQ,
										toSQ,
										myPAWN,
										Empty,
										Empty,
										PremoveBBPawnDoubleMove,
										getMoveValue(myPAWN, fromSQ, toSQ)
									);
							}
							else {
								MovePtr = NEW_MOVE(
									MovePtr,
									fromSQ,
									toSQ,
									myPAWN,
									Empty,
									Empty,
									PremoveBBPawnDoubleMove,
									getMoveValue(myPAWN, fromSQ, toSQ)
								);
							}
						}
					}

			}
			tempBB = getPawnMoveBoard_reverse<Us>(toSQ) & PiecesBB[myPAWN];
			if (tempBB) {
				fromSQ = PopBit(&tempBB);

				if (PinnedPiecesBB & SetMask[fromSQ]) {
					if (PinBySquare[fromSQ]->blockingBB & SetMask[toSQ]) {
						if (Rank7BB & SetMask[fromSQ]) {
							MovePtr = NEW_MOVE(
								MovePtr,
								fromSQ,
								toSQ,
								myPAWN,
								Empty,
								myKNIGHT,
								PremoveBBPromote,
								getMoveValue(myPAWN, fromSQ, toSQ)
							);

							MovePtr = NEW_MOVE(
								MovePtr,
								fromSQ,
								toSQ,
								myPAWN,
								Empty,
								myBISHOP,
								PremoveBBPromote,
								getMoveValue(myPAWN, fromSQ, toSQ)
							);

							MovePtr = NEW_MOVE(
								MovePtr,
								fromSQ,
								toSQ,
								myPAWN,
								Empty,
								myROOK,
								PremoveBBPromote,
								getMoveValue(myPAWN, fromSQ, toSQ)
							);

							MovePtr = NEW_MOVE(
								MovePtr,
								fromSQ,
								toSQ,
								myPAWN,
								Empty,
								myQUEEN,
								PremoveBBPromote,
								getMoveValue(myPAWN, fromSQ, toSQ)
							);
						}
						else {
							MovePtr = NEW_MOVE(
								MovePtr,
								fromSQ,
								toSQ,
								myPAWN,
								Empty,
								Empty,
								PremoveBBNormalExec,
								getMoveValue(myPAWN, fromSQ, toSQ)
							);
						}
					}
				}
				else {
					if (Rank7BB & SetMask[fromSQ]) {
						MovePtr = NEW_MOVE(
							MovePtr,
							fromSQ,
							toSQ,
							myPAWN,
							Empty,
							myKNIGHT,
							PremoveBBPromote,
							getMoveValue(myPAWN, fromSQ, toSQ)
						);

						MovePtr = NEW_MOVE(
							MovePtr,
							fromSQ,
							toSQ,
							myPAWN,
							Empty,
							myBISHOP,
							PremoveBBPromote,
							getMoveValue(myPAWN, fromSQ, toSQ)
						);

						MovePtr = NEW_MOVE(
							MovePtr,
							fromSQ,
							toSQ,
							myPAWN,
							Empty,
							myROOK,
							PremoveBBPromote,
							getMoveValue(myPAWN, fromSQ, toSQ)
						);

						MovePtr = NEW_MOVE(
							MovePtr,
							fromSQ,
							toSQ,
							myPAWN,
							Empty,
							myQUEEN,
							PremoveBBPromote,
							getMoveValue(myPAWN, fromSQ, toSQ)
						);
					}
					else {
						MovePtr = NEW_MOVE(
							MovePtr,
							fromSQ,
							toSQ,
							myPAWN,
							Empty,
							Empty,
							PremoveBBNormalExec,
							getMoveValue(myPAWN, fromSQ, toSQ)
						);
					}
				}
			}

			tempBB = AttackBrdKnightBB[toSQ] & PiecesBB[myKNIGHT];
			while (tempBB) {
				fromSQ = PopBit(&tempBB);
				if (PinnedPiecesBB & SetMask[fromSQ])
					continue;

				MovePtr = NEW_MOVE(
					MovePtr,
					fromSQ,
					toSQ,
					myKNIGHT,
					Empty,
					Empty,
					PremoveBBNormalExec,
					getMoveValue(myKNIGHT, fromSQ, toSQ)
				);
			}




			tempBB = BishopAttackBB & PiecesBB[myBISHOP];
			while (tempBB) {
				fromSQ = PopBit(&tempBB);
				if (PinnedPiecesBB & SetMask[fromSQ]) {
					if (PinBySquare[fromSQ]->blockingBB & SetMask[toSQ])
						MovePtr = NEW_MOVE(
							MovePtr,
							fromSQ,
							toSQ,
							myBISHOP,
							Empty,
							Empty,
							PremoveBBNormalExec,
							getMoveValue(myBISHOP, fromSQ, toSQ)
						);
				}
				else {
					MovePtr = NEW_MOVE(
						MovePtr,
						fromSQ,
						toSQ,
						myBISHOP,
						Empty,
						Empty,
						PremoveBBNormalExec,
						getMoveValue(myBISHOP, fromSQ, toSQ)
					);
				}
			}

			tempBB = RookAttackBB & PiecesBB[myROOK];
			while (tempBB) {
				fromSQ = PopBit(&tempBB);
				if (PinnedPiecesBB & SetMask[fromSQ]) {
					if (PinBySquare[fromSQ]->blockingBB & SetMask[toSQ])
						MovePtr = NEW_MOVE(
							MovePtr,
							fromSQ,
							toSQ,
							myROOK,
							Empty,
							Empty,
							PremoveBBNormalExec,
							getMoveValue(myROOK, fromSQ, toSQ)
						);
				}
				else {
					MovePtr = NEW_MOVE(
						MovePtr,
						fromSQ,
						toSQ,
						myROOK,
						Empty,
						Empty,
						PremoveBBNormalExec,
						getMoveValue(myROOK, fromSQ, toSQ)
					);
				}
			}

			tempBB = (BishopAttackBB | RookAttackBB) & PiecesBB[myQUEEN];
			while (tempBB) {
				fromSQ = PopBit(&tempBB);
				if (PinnedPiecesBB & SetMask[fromSQ]) {
					if (PinBySquare[fromSQ]->blockingBB & SetMask[toSQ])
						MovePtr = NEW_MOVE(
							MovePtr,
							fromSQ,
							toSQ,
							myQUEEN,
							Empty,
							Empty,
							PremoveBBNormalExec,
							getMoveValue(myQUEEN, fromSQ, toSQ)
						);
				}
				else {
					MovePtr = NEW_MOVE(
						MovePtr,
						fromSQ,
						toSQ,
						myQUEEN,
						Empty,
						Empty,
						PremoveBBNormalExec,
						getMoveValue(myQUEEN, fromSQ, toSQ)
					);
				}
			}


		}

		return MovePtr;
	}



	inline bool KingInCheck(void) { return InCheck;	}
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