#pragma once
#include "Singleton.h"
#include "basic.h"
#include <atomic>

class SearchInfo : public Singleton <SearchInfo>
{
	friend class Singleton <SearchInfo>;
public:
	SearchInfo(void) {
	}
	inline U64 getStarttime(void) { return Starttime; }
	inline U64 getStoptime(void) { return Stoptime; }
	inline short getDepth(void) { return Depth; }
	inline bool getTimeset(void) { return Timeset; }
	inline short getMovestogo(void) { return Movestogo; }
	inline U64 getNodes(void) { return Nodes; }
	inline bool getUseBook(void) { return UseBook; }
	inline bool getQuit(void) { return Quit; }
	inline bool getStopped(void) { return Stopped; }
	inline U64 getNullCut(void) { return NullCut; }

	inline void incNullCut(void) { NullCut++; }
	inline void incNodes(void) { Nodes++; }

	inline void setStarttime(U64 value) { Starttime = value; }
	inline void setStoptime(U64 value) { Stoptime = value; }
	inline void setDepth(short value) { Depth = value; }
	inline void setTimeset(bool value) { Timeset = value; }
	inline void setMovestogo(short value) { Movestogo = value; }
	inline void setNodes(U64 value) { Nodes = value; }
	inline void setUseBook(bool value) { UseBook = value; }
	inline void setQuit(bool value) { Quit = value; }
	inline void setStopped(bool value) { Stopped = value; }
	inline void setNullCut(U64 value) { NullCut = value; }
private:
	std::atomic<U64> Starttime = 0ULL;
	std::atomic<U64> Stoptime = 0ULL;
	std::atomic<short> Depth = 0;
	std::atomic<bool> Timeset = false;
	std::atomic<short> Movestogo = 0;

	std::atomic<U64> Nodes = 0ULL;

	std::atomic<bool> UseBook = false;
	std::atomic<bool> Quit = false;
	std::atomic<bool> Stopped = true;

	std::atomic<U64> NullCut = 0;
};

#define SEARCHINFO SearchInfo::instance()
