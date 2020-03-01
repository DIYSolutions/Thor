#pragma once

#include "Chessboard.h"
#include "ChessThreadMessenger.h"
#include "ChessThread.h"


#include "perfttest.h"

#include <stdlib.h>
#include <iostream>
#include <string>
#include <fstream>
using namespace std;
#include <cstdint>

ChessThreadMessenger* Messenger = MESSENGER;
std::chrono::milliseconds PerftSleepTime(1);

U64 perfttest_thread(Chessboard* testboard, int depth) {
	S_ThreadMessage* msg;
	msg = getThreadMessage();
	testboard->GenThreadMessage(msg);
	msg->Depth = depth;
	msg->Mode = ThreadPerftTest;
	_pSearchInfo->nodes = 0;
	Messenger->putNewMessage(msg);// start thread search

	ThreadSleep(PerftSleepTime);
NotReadyYet:
	while ((Messenger->ThreadsWorkingNum() > 0))
		ThreadSleep(PerftSleepTime);

	if (Messenger->MessageWaiting())
		goto NotReadyYet;

	return _pSearchInfo->nodes;
}

void perfttest(Chessboard* testboard, int depth) {
	string line;
	char fen_line[255];
	int fen_len = 0;
	char depth_line[255];
	char temp;
	ifstream myfile("perftsuite.epd");
	int state = 0;
	int depth_state = 0;
	U64 depth_num[6];
	int ii = 0;

	int lines_sum = 0;
	int lines_err = 0;
	int lines_err_num[200];
	int lines_err_depth[200];
	uint64_t end = 0;
	uint64_t start = 0;
	uint64_t total_time = 0;

	U64 temp_long;
	long double mid_value = 0.0;
	long double min_value = 0.0;
	bool first = true;
	long double max_value = 0.0;
	if (depth > 6) {
		std::cout << "depth " << depth << " is to high, changed to 6" << std::endl;
		depth = 6;
	}
	else if (depth < 1) {
		std::cout << "depth " << depth << " is to low, changed to 1" << std::endl;
		depth = 1;
	}
	bool failed = false;
	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			lines_sum++;
			for (int i = 0; i < line.length(); i++) {
				temp = line[i];
				switch (temp) {
				case 59://==";"
					state++;
					break;
				default:
					if (state == 0) {
						fen_line[i] = line[i];
						if (line[i + 1] == 59) {
							fen_len = i + 1;
							fen_line[fen_len] = 0;
							ii = 0;
						}
					}
					else {
						depth_line[ii++] = line[i];
						if (i + 1 == line.length()) {
							fen_len = ii;
							depth_line[fen_len] = 0;
							depth_num[depth_state] = strtoull(depth_line, NULL, 0);
						}
						else if (line[i + 1] == 59) {
							fen_len = ii;
							depth_line[fen_len] = 0;
							depth_num[depth_state] = strtoull(depth_line, NULL, 0);
							depth_state++;
							ii = 0;
						}

					}
				}
			}
			std::cout << '\n' << lines_sum << ": ";
			std::cout << fen_line << '\n';
			state = 0;
			depth_state = 0;


			std::cout << "test this FEN to depth " << depth << std::endl;
			testboard->ParseFEN(fen_line);
			failed = false;
			for (short i = 1; i <= depth; i++) {
				start = GetMilliTime();
				temp_long = perfttest_thread(testboard, i);
				end = GetMilliTime();

				std::cout << "depth " << i << ", found " << temp_long << " nodes ";
				std::cout << "(" << (end - start) / 1000000.0 << "ms) - ";

				long double nmps = 0.000000001 + (temp_long / ((end - start) / 1000000.0));

				std::cout << nmps << " npms - ";
				if (nmps > max_value)
					max_value = nmps;
				if (min_value == 0.0 || min_value > nmps)
					min_value = nmps;

				if (mid_value == 0.0)
					mid_value = nmps;
				else
					mid_value = (mid_value + nmps) / 2;


				if (temp_long == depth_num[i - 1]) {
					std::cout << "ok";
				}
				else {
					std::cout << "FAILED got:" << temp_long << ", expected:" << depth_num[i - 1];
					std::cout << std::endl;
					if (!failed) {
						lines_err_depth[lines_err] = i;
						lines_err_num[lines_err++] = lines_sum;
						failed = true;
					}
					//PrintBoard(testboard);
					//return;
					break;
				}
				std::cout << std::endl;
			}
			std::cout << "done" << std::endl << std::endl;

		}
		myfile.close();
		std::cout << mid_value << " average npms - " << min_value << " min npms - " << max_value << " max npms - ";
		if (lines_err != 0) {
			std::cout << lines_err << " from " << lines_sum << " failed\nthe following failed:" << std::endl;
			for (int i = 0; i < lines_err; i++) {
				std::cout << "LINE:" << lines_err_num[i] << " - depth: " << lines_err_depth[i] << '\n';
			}
		}
		else {
			std::cout << " => without errors <=" << std::endl;
		}
	}

	else std::cout << "Unable to open file";

}
