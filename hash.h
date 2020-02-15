#pragma once
#include "move_data.h"
#include <atomic>
#include "malloc.h"
static S_HASHTABLE* _pHashTable = nullptr;

typedef struct S_HASHENTRY {
	std::atomic<U64> posKey = 0ULL;
	std::atomic<short> depth = 0;
	std::atomic<BoardValue> score = 0;
	std::atomic<U64> move = 0ULL;
	std::atomic<short> flags = 0;
	std::atomic<bool> readlock = false;
	std::atomic<bool> writelock = false;
} S_HASHENTRY;

typedef struct S_HASHTABLE {
	S_HASHENTRY* entrys = nullptr;
	std::atomic<U64> numEntries = 0;
	std::atomic<U64> newWrite = 0;
	std::atomic<U64> overWrite = 0;
	std::atomic<U64> hit = 0;
	std::atomic<U64> cut = 0;
} S_HASHTABLE;

typedef struct {

	std::atomic<U64> starttime;
	std::atomic<U64> stoptime;
	std::atomic<int> depth;
	std::atomic<int> timeset;
	std::atomic<int> movestogo;

	std::atomic<U64> nodes;

	std::atomic<bool> quit;
	std::atomic<bool> stopped;

	std::atomic<int> fh;
	std::atomic<int> fhf;
	std::atomic<int> nullCut;

} S_SEARCHINFO;

typedef enum HashFlags {
	HFNONE, HFALPHA, HFBETA, HFEXACT
} HashFlags;

constexpr inline S_HASHENTRY* GET_HASHENTRY(const U64 posKey) { return &(_pHashTable->entrys[posKey % _pHashTable->numEntries]); }

inline void ProbeHashEntry(S_Chessboard* pos, short* depth, BoardValue* score) {
	S_HASHENTRY* hashEntry = GET_HASHENTRY(pos->posKey);
	hashEntry->writelock = true;
	while (hashEntry->readlock);
	if (hashEntry->posKey == pos->posKey) {
		*depth = hashEntry->depth;
		*score = hashEntry->score;
	}
	else {
		*depth = 0;
	}
	hashEntry->writelock = false;
}

inline bool ProbeHashEntry(const S_Chessboard* pos, U64* move, BoardValue* score, const BoardValue alpha, const BoardValue beta, const short depth) {
	S_HASHENTRY* hashEntry = GET_HASHENTRY(pos->posKey);
	hashEntry->writelock = true;
	while (hashEntry->readlock);

	if (hashEntry->posKey == pos->posKey) {
		*move = hashEntry->move;
		if (hashEntry->depth >= depth) {
			_pHashTable->hit++;

			*score = hashEntry->score;
			if (*score > MAX_MATE) *score -= pos->ply;
			else if (*score < MIN_MATE) *score += pos->ply;

			short flags = hashEntry->flags;
			hashEntry->writelock = false;
			switch (flags) {
			case HFALPHA:
				if (*score <= alpha) {
					*score = alpha;
					return true;
				}
				break;
			case HFBETA:
				if (*score >= beta) {
					*score = beta;
					return true;
				}
				break;
			case HFEXACT:
				return true;
				break;
			default: ASSERT(false); break;
			}
		}
	}

	hashEntry->writelock = false;
	return false;
}

static U64 doNotOverrite = 0ULL;
inline void NewRootBoard(const U64 index) { doNotOverrite = index; }

inline void StoreHashEntry(const S_Chessboard* pos, const U64 move, const BoardValue score, const short flags, const short depth) {
	S_HASHENTRY* hashEntry = GET_HASHENTRY(pos->posKey);
	hashEntry->readlock = true;
	while (hashEntry->writelock);

	if (hashEntry->posKey == 0ULL) {
		_pHashTable->newWrite++;
	}
	else {
		if (hashEntry->posKey == doNotOverrite) {
			hashEntry->readlock = false;
			return;
		}
		_pHashTable->overWrite++;
	}

	if (score > MAX_MATE) hashEntry->score = score + pos->ply;
	else if (score < MIN_MATE) hashEntry->score = score - pos->ply;
	else hashEntry->score = score;

	hashEntry->move = move;
	hashEntry->posKey = pos->posKey;
	hashEntry->flags = flags;
	hashEntry->depth = depth;
	hashEntry->readlock = false;
}

inline U64 ProbePvMove(const U64 posKey) {
	S_HASHENTRY* hashEntry = GET_HASHENTRY(posKey);
	hashEntry->writelock = true;
	while (hashEntry->readlock);

	if (hashEntry->posKey == posKey) {
		hashEntry->writelock = false;
		return hashEntry->move;
	}
	hashEntry->writelock = false;
	return 0ULL;
}



inline void ClearHashTable() {
	for (S_HASHENTRY* tableEntry = _pHashTable->entrys; tableEntry < _pHashTable->entrys + ((U64)_pHashTable->_pHashTable); tableEntry++) {
		tableEntry->posKey = tableEntry->move = 0ULL;
		tableEntry->depth = tableEntry->score = tableEntry->flags = 0;
		tableEntry->readlock = false;
		tableEntry->writelock = false;
	}
	_pHashTable->newWrite = _pHashTable->overWrite = _pHashTable->hit = _pHashTable->cut = 0;
}

inline void InitHashTable(const U64 sizeMB) {
	destroyHashTable();
	_pHashTable = new S_HASHTABLE();


	U64 HashSize = 0x100000 * sizeMB;
	_pHashTable->numEntries = HashSize / sizeof(S_HASHENTRY);
	_pHashTable->numEntries -= 2;

	if (_pHashTable->entrys != nullptr) {
		free(_pHashTable->entrys);
	}

	_pHashTable->entrys = (S_HASHENTRY*)malloc(((int)_pHashTable->numEntries) * sizeof(S_HASHENTRY));
	if (_pHashTable->entrys == nullptr) {
		print_console("Hash Allocation Failed, trying %dMB...\n", sizeMB / 2);
		InitHashTable(sizeMB / 2);
	}
	else {
		ClearHashTable();
		print_console("HashTable init complete with %d entries\n", (int)_pHashTable->numEntries);
	}
}

inline void destroyHashTable() {
	if (_pHashTable != nullptr) {
		if (_pHashTable->entrys != nullptr) {
			free(_pHashTable->entrys);
		}
		delete _pHashTable;
		_pHashTable = nullptr;
	}
}

