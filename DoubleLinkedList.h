#pragma once
#include "ThreadMemory.h"

template <typename T>
class DoubleLinkedList
{
private:
	ThreadMemory* MemoryManager;
	T* item = nullptr;
	DoubleLinkedList(T* _item, ThreadMemory* _MemoryManager, DoubleLinkedList<T>* _next) {
		item = _item;
		MemoryManager = _MemoryManager;
		last = _next->last;
		next = _next;
		if (_next != nullptr)
			next->last = this;
	}
protected:
	DoubleLinkedList<T>* last = nullptr;
	DoubleLinkedList<T>* next = nullptr;
public:
	DoubleLinkedList(ThreadMemory* _MemoryManager) {
		MemoryManager = _MemoryManager;
	}

	inline void newEntry(T* _item, bool(*compare)(T*, T*)) {
		DoubleLinkedList<T>* current = next;
		while (current->next != nullptr && compare(item, current)) {
			current = current->next;
		}
		new(MemoryManager->MemoryFrameAlloc(sizeof(DoubleLinkedList<T>))) DoubleLinkedList<T>(_item, _MemoryManager, current);
	}

	inline T* getItem(void) { return item; }
	inline DoubleLinkedList<T>* getNext(void) { return next; }
	inline DoubleLinkedList<T>* getLast(void) { return last; }
};

