#pragma once
#include <malloc.h>
#include "basic.h"

template <typename T>
struct ListNode {
	T current = nullptr;
	struct ListNode<T>* next = nullptr;
	struct ListNode<T>* last = nullptr;

	template <typename T>
	ListNode(void) {
		current = nullptr;
		ListNode<T>* next = nullptr;
		ListNode<T>* last = nullptr;
	}
};

template <typename T>
class List
{
public:
	List(int NumMaxNodes) {
		init(NumMaxNodes);
	}
	~List() {
		free(container);
	}

	bool empty(void) { return head == nullptr; }

	T pop(void) {// pop from head
		
		if (head == nullptr)
			return nullptr;
		ListNode<T>* front = head;
		T ret = head->current;
		if (head == tail) {
			head = tail = nullptr;
		}else {
			head = head->next;
			head->last = nullptr;
		}
		NewFreeNode(front);
		return ret;
	}

	void push(T obj) {// push to tail
		ListNode<T>* node = GetFreeNode();
		node->current = obj;
		if (tail == nullptr) {
			head = tail = node;
		}else {
			tail->next = node;
			node->last = tail;
			tail = node;
		}
	}

	void del(T obj) {// delete node with this obj
		ListNode<T>* _head = head;
		ListNode<T>* _tail = tail;
		while (true) {
			if (_head->current == obj)
				return del(_head);
			if (_tail->current == obj)
				return del(_tail);
			if (_head == _tail)
				return;
			_head = _head->next;
			_tail = _tail->last;
			if (_head == nullptr)
				return;
		}
	}

	bool FreeNodeAvailable(void) { return freeRootNode != nullptr; }
private:
	struct ListNode<T>* head = nullptr;
	struct ListNode<T>* tail = nullptr;
	struct ListNode<T>* freeRootNode = nullptr;
	struct ListNode<T>* container = nullptr;

	void NewFreeNode(ListNode<T>* Node) {
		Node->last = nullptr;
		Node->next = freeRootNode;
		if(freeRootNode)
			freeRootNode->last = Node;
		freeRootNode = Node;
	}
	ListNode<T>* GetFreeNode(void) {
		ListNode<T>* ret = freeRootNode;
		if (ret == nullptr)
			error_exit("template <typename T> class List - there is no free node!");
		if (ret->next)
			ret->next->last = nullptr;
		freeRootNode = ret->next;
		ret->next = nullptr;
		return ret;
	}
	void init(int NumMaxNodes) {
		container = (ListNode<T>*) malloc(sizeof(ListNode<T>) * NumMaxNodes);
		for (int i = 0; i < NumMaxNodes; i++) {
			NewFreeNode(&container[i]);
		}
	}
	void del(ListNode<T>* node) {// delete node
		if (node->last)
			node->last->next = node->next;
		else
			head = node->next;
		if (node->next)
			node->next->last = node->last;
		else
			tail = node->last;
		NewFreeNode(node);
	}
};

