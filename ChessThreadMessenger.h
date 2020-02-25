#pragma once
#include "basic.h"
#include <atomic>

typedef struct S_ThreadMessage {
	short Side = BLACK;
	short EnPas = NO_SQ;
	short FiftyMove = 0;
	short CastlePerm = 0;
	U64* PiecesBB = nullptr;
	U64 PosKey = 0ULL;
	
	short Mode = 0;
	S_ThreadMessage* next = nullptr;
} S_ThreadMessage;

static S_ThreadMessage _ThreadMessages[THREAD_MESSENGER_SIZE / sizeof(S_ThreadMessage)];
static int _ThreadMessagesNumEntrys = 0;
static S_ThreadMessage _ThreadMessagesRoot;
static std::atomic_flag _ThreadMessagesLock;


inline S_ThreadMessage* getThreadMessage(const int id) {
	if (id >= _ThreadMessagesNumEntrys)
		return nullptr;
	return &_ThreadMessages[id];
}

void InitMessageContainer(void) {
	_ThreadMessagesNumEntrys = THREAD_MESSENGER_SIZE / sizeof(S_ThreadMessage);
	_ThreadMessagesRoot.next = &_ThreadMessages[0];
	for (int i = 0; i < _ThreadMessagesNumEntrys; i++)
		_ThreadMessages[i].next = getThreadMessage(i + 1);
	_ThreadMessagesLock.clear();
}

inline S_ThreadMessage* getThreadMessage(void) {
	while (_ThreadMessagesLock.test_and_set(std::memory_order_acquire)) // acquire lock
		; // spin
	if (!_ThreadMessagesRoot.next)
		error_exit("S_ThreadMessage* getThreadMessage(void): failed!");
	S_ThreadMessage* ret = _ThreadMessagesRoot.next;
	_ThreadMessagesRoot.next = _ThreadMessagesRoot.next->next;
	_ThreadMessagesLock.clear(std::memory_order_release);
	ret->next = nullptr;
	return ret;
}

inline S_ThreadMessage* NewThreadMessage(const short Side,
	const short EnPas,
	const short FiftyMove,
	const short CastlePerm,
	const U64* PiecesBB,
	const U64 PosKey,
	const short Mode) {

	S_ThreadMessage* ret = getThreadMessage();
	ret->Side = Side;
	ret->EnPas = EnPas;
	ret->FiftyMove = FiftyMove;
	ret->CastlePerm = CastlePerm;
	ret->PosKey = PosKey;
	ret->Mode = Mode;
	ret->PiecesBB[0] = PiecesBB[0];
	ret->PiecesBB[1] = PiecesBB[1];
	ret->PiecesBB[2] = PiecesBB[2];
	ret->PiecesBB[3] = PiecesBB[3];
	ret->PiecesBB[4] = PiecesBB[4];
	ret->PiecesBB[5] = PiecesBB[5];
	ret->PiecesBB[6] = PiecesBB[6];
	ret->PiecesBB[7] = PiecesBB[7];
	ret->PiecesBB[8] = PiecesBB[8];
	ret->PiecesBB[9] = PiecesBB[9];
	ret->PiecesBB[10] = PiecesBB[10];
	ret->PiecesBB[11] = PiecesBB[11];

	return ret;
}

inline void releaseThreadMessage(S_ThreadMessage* old) {
	while (_ThreadMessagesLock.test_and_set(std::memory_order_acquire)) // acquire lock
		; // spin
	old->next = _ThreadMessagesRoot.next;
	_ThreadMessagesRoot.next = old;
	_ThreadMessagesLock.clear(std::memory_order_release);
}

class ChessThreadMessenger
{
public:
	ChessThreadMessenger(void) {
		lock.clear();
	}
	inline S_ThreadMessage* getNewMessage(void) {
		while (lock.test_and_set(std::memory_order_acquire)) // acquire lock
			; // spin
		if (!MessageWaiting()) {
			lock.clear(std::memory_order_release);
			return nullptr;
		}

		S_ThreadMessage* ret = _MessagesRoot.next;
		_MessagesRoot.next = _MessagesRoot.next->next;
		lock.clear(std::memory_order_release);
		ret->next = nullptr;
		return ret;
	}
	inline void putNewMessage(S_ThreadMessage* newMessage) {
		while (lock.test_and_set(std::memory_order_acquire)) // acquire lock
			; // spin
		newMessage->next = _MessagesRoot.next;
		_MessagesRoot.next = newMessage;
		lock.clear(std::memory_order_release);
	}
	inline void ThreadWaiting(void) { _ThreadsWaiting++; }
	inline void ThreadWorking(void) { _ThreadsWaiting--; }
	inline const bool ThreadAvailable(void) {	return _ThreadsWaiting > 0;	}
private:
	std::atomic_flag lock;
	S_ThreadMessage _MessagesRoot;
	std::atomic<short> _ThreadsWaiting = 0;

	inline bool MessageWaiting(void) {
		return _MessagesRoot.next != nullptr;
	}

};
