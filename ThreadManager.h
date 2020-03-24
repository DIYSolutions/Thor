#pragma once
#include "basic.h"
#include <thread>
#include <mutex>
#include <atomic>
#include <queue> 
#include "Singleton.h"
#include "List.h"

class Thread;
typedef struct S_ThreadMessage {
    void(*func)(Thread*, void*);
    void* data;
    S_ThreadMessage(void(*_func)(class Thread*,void*), void* _data) {
        func = _func;
        data = _data;
    }
    S_ThreadMessage(void) {
        func = nullptr;
        data = nullptr;
    }
} S_ThreadMessage;

typedef class Thread {
public:
	Thread(short tid, class ThreadManager* _manager) {
        std::unique_lock<std::mutex> _lock(lock);
        manager = _manager;
		id = tid;
        Enable = true;
        pChessboard = new Chessboard();
        pChessboardSub = new Chessboard();
        pMemory = new MemoryBlock(THREAD_MEMORY_SIZE);
        msg.data = nullptr;
    }
    inline void Disable(void) { Enable = false; }
    inline bool IsEnabled(void) { return Enable; }
    bool getPrivateMessage(void);
    bool getMessage(void);

    bool newMessage(void(*func)(Thread*, void*), void* data) {
        std::unique_lock<std::mutex> _lock(lock);
        if (msg.data != nullptr)
            return false;
        msg.data = data;
        msg.func = func;
        cv.notify_one();
        return true;
    }
    bool MessagesWaiting(void);
    Chessboard* getChessboard(void) { return SubBoard ? pChessboardSub: pChessboard; }
    short getID(void) { return id; }
    MemoryBlock* getMemory(void) { return pMemory; };
    void(*func)(Thread*, void*);
    void* data;
    void loop(S_Splitpoint* Splitpoint, short searched);
    bool IsIdle(void) { return Idle; }
    void SetIdle(void) { Idle = true; }
    void ClearIdle(void) { Idle = false; }
private:
	short id;
    std::atomic<bool> Enable;
    std::atomic<bool> Idle;
    S_ThreadMessage msg;
    class ThreadManager* manager;
    Chessboard* pChessboard, * pChessboardSub;
    bool SubBoard = false;
    MemoryBlock* pMemory;
    std::mutex lock;
    std::condition_variable cv;
};

class ThreadManager
{
public:
	ThreadManager(short _NumThreads) {
        std::unique_lock<std::mutex> _lock(lock);
        if(std::thread::hardware_concurrency() > 0)
            if (_NumThreads > std::thread::hardware_concurrency())
                _NumThreads = std::thread::hardware_concurrency();

        Messages = new List<S_ThreadMessage*>(1000000);
        IdleThreads = new List<class Thread*>(_NumThreads);
        
        NumThreads = _NumThreads;
        for (short i = 0; i < NumThreads; i++) {
            ThreadObjects[i] = new class Thread(i, this);
            IdleSubThreads[i] = new List<class Thread*>(NumThreads);
        }
        for (short i = 0; i < NumThreads; i++) {
            Thread[i] = new std::thread(ThreadManager::loop, this, ThreadObjects[i]);
        }
        for (short i = NumThreads; i < MAX_THREAD; i++) {
            ThreadObjects[i] = nullptr;
            Thread[i] = nullptr;
        }
    }
	~ThreadManager(void) {
        for (short i = 0; i < NumThreads; i++) {
            ThreadObjects[i]->Disable();
        }
        delete Messages;
        delete IdleThreads;
	}
	static void loop(ThreadManager* manager, Thread* obj);

    void NewIdleThread(Thread* thread, short SubThreadID) {
        std::unique_lock<std::mutex> _lock(lock);
        IdleSubThreads[SubThreadID]->push(thread);
    }
    void NewIdleThread(Thread* thread) {
        std::unique_lock<std::mutex> _lock(lock);
        IdleThreads->push(thread);
    }
    Thread* GetIdleThread(Thread* pThread) {
        std::unique_lock<std::mutex> _lock(lock);
        if (pThread) {
            while (!IdleSubThreads[pThread->getID()]->empty()) {
                class Thread* thread = IdleSubThreads[pThread->getID()]->pop();
                if(thread->IsIdle())
                    return thread;
            }
        }

        if (IdleThreads->empty())
            return nullptr;

        class Thread* thread = IdleThreads->pop();
        return thread;
    }

    void NewWork(void(*func)(Thread*, void*), void* data) {
        std::unique_lock<std::mutex> _lock(lock);
        if (IdleThreads->empty())
            Messages->push(new S_ThreadMessage(func, data));
        else {
            class Thread* thread = IdleThreads->pop();
            if (!thread->newMessage(func, data))
                Messages->push(new S_ThreadMessage(func, data));
        }

    }
    S_ThreadMessage* GetWork(void) {
        std::unique_lock<std::mutex> _lock(lock);
        if (Messages->empty())
            return nullptr;

        S_ThreadMessage* data = Messages->pop();
        Messages->pop();
        return data;
    }
    Thread* GetThreadByID(short tid) {
        if (tid >= NumThreads)
            return nullptr;
        return ThreadObjects[tid]; 
    }
    short GetNumThreads(void) { return NumThreads; }
private:
	short NumThreads;
	std::thread* Thread[MAX_THREAD];
	class Thread* ThreadObjects[MAX_THREAD];

    List<S_ThreadMessage*>* Messages;
    List<class Thread*>* IdleThreads;
    List<class Thread*>* IdleSubThreads[MAX_THREAD * 2];
    std::mutex lock;
};


void ThreadManager::loop(ThreadManager* manager, class Thread* pThread) {
    pThread->SetIdle();
    manager->NewIdleThread(pThread);
    while (pThread->IsEnabled()) {
        if (pThread->getMessage()) {
            pThread->ClearIdle();
            S_MemoryFrame PerftMemoryFrame = pThread->getMemory()->GetMemoryFrame(ThreadHeap);
            pThread->func(pThread, pThread->data);
            pThread->getMemory()->ReleaseMemoryFrame(&PerftMemoryFrame);
            pThread->SetIdle();
            manager->NewIdleThread(pThread);
        }
    }

}

void Thread::loop(S_Splitpoint* Splitpoint, short searched) {
    SubBoard = true;
    SetIdle();
    while ((Splitpoint->searched + searched) < Splitpoint->Moves.count) {
        if (getPrivateMessage()) {
            ClearIdle();
            S_MemoryFrame PerftMemoryFrame = pMemory->GetMemoryFrame(ThreadHeap);
            func(this, data);
            pMemory->ReleaseMemoryFrame(&PerftMemoryFrame);
            SetIdle();
        }
    }
    SubBoard = false;
}

bool Thread::MessagesWaiting(void) {
    return msg.data != nullptr;
}
bool Thread::getPrivateMessage(void) {
    std::unique_lock<std::mutex> _lock(lock);    
    if (msg.data == nullptr) {// no private message
        cv.wait_for(_lock, std::chrono::milliseconds(1));
        return false;
    }
    func = msg.func;
    data = msg.data;
    msg.data = nullptr;

    return true;
}

bool Thread::getMessage(void) {
    std::unique_lock<std::mutex> _lock(lock);
    if (msg.data == nullptr) {// no private message
        S_ThreadMessage* global_msg = manager->GetWork();
        if (!global_msg) {
            cv.wait_for(_lock, std::chrono::milliseconds(1));
            return false;
        }
        msg.func = global_msg->func;
        msg.data = global_msg->data;
    }
    func = msg.func;
    data = msg.data;
    msg.data = nullptr;

    return true;
}
