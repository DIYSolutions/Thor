#pragma once
#include "bitboard.h"

inline void newPiece(U64* piecesListBB, const short PieceType, const short sq) {
	SetBit(&(piecesListBB[PieceType]), sq);
}

inline void delPiece(U64* piecesListBB, const short PieceType, const short sq) {
	ClearBit(&(piecesListBB[PieceType]), sq);
}

inline void delPiece(S_Chessboard* gameboard, const short PieceType, const short sq) {
	delPiece(gameboard->piecesBB, PieceType, sq);
	gameboard->score -= PIECES_VALUES[PieceType];
}

inline void movePiece(U64* piecesListBB, const short PieceType, const short from_sq, const short to_sq) {
	ClearBit(&(piecesListBB[PieceType]), from_sq);
	SetBit(&(piecesListBB[PieceType]), to_sq);
}

inline void modPiece(U64* piecesListBB, const short oldPieceType, const short newPieceType, const short sq) {
	ClearBit(&(piecesListBB[oldPieceType]), sq);
	SetBit(&(piecesListBB[newPieceType]), sq);
}

inline void newPiece(S_Chessboard* gameboard, const short PieceType, const short sq) {
	newPiece(gameboard->piecesBB, PieceType, sq);
	gameboard->score += PIECES_VALUES[PieceType];
}

inline void modPiece(S_Chessboard* gameboard, const short oldPieceType, const short newPieceType, const short sq) {
	modPiece(gameboard->piecesBB, oldPieceType, newPieceType, sq);
	gameboard->score -= PIECES_VALUES[oldPieceType];
	gameboard->score += PIECES_VALUES[newPieceType];
}
