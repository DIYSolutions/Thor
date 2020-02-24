#pragma once
#include "basic.h"
#include <atomic>
#include <malloc.h>

typedef struct S_ThreadMessage {
	char fen[255] = "                                                                                                                                                                                                                                                    ";
	short Mode = 0;
	S_ThreadMessage* next = nullptr;
} S_ThreadMessage;

static S_ThreadMessage* _ThreadMessages = nullptr;
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
	_ThreadMessages = (S_ThreadMessage*)malloc(sizeof(S_ThreadMessage) * _ThreadMessagesNumEntrys);
	_ThreadMessagesRoot.next = &_ThreadMessages[0];
	for (int i = 0; i < _ThreadMessagesNumEntrys; i++)
		_ThreadMessages[i].next = getThreadMessage(i + 1);
	_ThreadMessagesLock.clear();
}

inline S_ThreadMessage* getThreadMessage(void) {
	while (_ThreadMessagesLock.test_and_set(std::memory_order_acquire)) // acquire lock
		; // spin
	S_ThreadMessage* ret = _ThreadMessagesRoot.next;
	if (!ret)
		error_exit("S_ThreadMessage* getThreadMessage(void): failed!");
	_ThreadMessagesRoot.next = ret->next;
	_ThreadMessagesLock.clear(std::memory_order_release);
	ret->next = nullptr;
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

private:
	std::atomic_flag lock;
	short newMessages = 0;

};
