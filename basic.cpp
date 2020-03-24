#include "basic.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <atomic>

std::atomic_flag lock;
void InitOutputLock() {
	lock.clear();
}

void error_exit(const char* err) {
	std::cout << err << std::endl;
	exit(1);
}

void error_exit(const char* condition, const char* date, const char* time, const char* file, int line_nr) {
	printf("%s - Failed", condition);
	printf("On %s ", date);
	printf("At %s ", time);
	printf("In File %s ", file);
	printf("At Line %d\n", line_nr);
	exit(1);
}


void print_console(const char* line) {
	printing_console_start(); // acquire lock
	std::cout << line << std::endl;
	printing_console_end(); // release lock
}

void print_console(const char* line, char * value) {
	printing_console_start(); // acquire lock
	printf(line, *value);
	printing_console_end(); // release lock
}

void print_console(const char* line, int value) {
	printing_console_start(); // acquire lock
	printf(line, value);
	printing_console_end(); // release lock
}

void print_console(const char* line, U64 value) {
	printing_console_start(); // acquire lock
	printf(line, value);
	printing_console_end(); // release lock
}

void print_console(const char* line, signed short value1, int value2) {
	printing_console_start(); // acquire lock
	printf(line, value1, value2);
	printing_console_end(); // release lock
}



void printing_console_start() {
	while (lock.test_and_set(std::memory_order_acquire)) // acquire lock
		; // spin
}

void print_console_str(const char* line, signed int number) {
	printf(line, number);
}

void print_console_str(const char* line, char* str) {
	printf(line, str);
}

void print_console_str(long double number) {
	std::cout << number;
}
void print_console_str(char* str) {
	std::cout << str;
}

void print_console_endl() {
	std::cout << std::endl;
}

void printing_console_end() {
	fflush(stdout);
	lock.clear(std::memory_order_release); // release lock
}

void print_search_info(const BoardValue Score, const short Depth, U64 nodes, U64 timems){
	std::cout << "info score cp " << Score << " depth " << Depth << " nodes " << nodes << " time " << timems << std::endl;
}

void PrintBitboard(U64 bb) {

	U64 shiftMe = 1ULL;

	int rank = 0;
	int file = 0;
	int sq = 0;
	int sq64 = 0;

	printf("\n");
	for (rank = RANK_8; rank >= RANK_1; --rank) {
		for (file = FILE_A; file <= FILE_H; ++file) {
			sq = FR2SQ(file, rank);	// 64 based	

			if ((shiftMe << sq) & bb)
				printf("X");
			else
				printf("-");

		}
		printf("\n");
	}
	printf("\n\n");
}


FILE* popen(const char* command, const char* flags) { return _popen(command, flags); }
int pclose(FILE* fd) { return _pclose(fd); }

U64 test_perft_line(const short depth, char* line) {
	U64 ret = 0ULL;
	char number_str[30];
	short x = 0, y = 0;
	if (!strncmp(line, "perft( ", 6)) {
		number_str[0] = line[7];
		number_str[1] = 0;
		if (atoi(number_str) == depth) {
			x = 9;
			y = 0;
			while (line[++x] == ' ');
			while (line[x] != ' ') {
				number_str[y++] = line[x++];
			}
			number_str[y] = 0;
			return atoll(number_str);
		}
	}
	return 0ULL;
}

U64 perft_check(const short depth, char* fen)
{
	char psBuffer[128];
	char read_line[128];
	short rx=0;
	FILE* iopipe;
	char cmd_line[256] = "perft ";
	short x = 0, y = 0;
	while (cmd_line[x] != 0)
		x++;
	cmd_line[x++] = '0' + depth;
	cmd_line[x++] = ' ';
	cmd_line[x++] = '"';
	while (fen[y] != 0)
		cmd_line[x++] = fen[y++];
	cmd_line[x++] = '"';
	cmd_line[x++] = 0;
	if ((iopipe = popen(cmd_line, "r")) == NULL)
		exit(1);
	U64 ret = 0ULL;
	while (!feof(iopipe))
	{
		if (fgets(psBuffer, 128, iopipe) != NULL) {
			x = 0;
			while (psBuffer[x] != 0) {
				if (psBuffer[x] == '\n') {
					read_line[rx] = 0;
					ret = test_perft_line(depth, read_line);
					if (ret)
						return ret;
					rx = 0;
					x++;
				}
				if (psBuffer[x] != 0)
					read_line[rx++] = psBuffer[x++];
			}
		}
	}
	std::cout << "error!" << std::endl;
	return 0;
}

void perft_fill_array(short depth, U64 * depth_num, char* fen)
{
	char psBuffer[128];
	char read_line[128];
	short rx = 0;
	FILE* iopipe;
	char cmd_line[256] = "C:\\Users\\Toto\\source\\repos\\DIYSolutions\\Thor\\x64\\Release\\perft ";
	short x = 0, y = 0;
	while (cmd_line[x] != 0)
		x++;
	if (depth == 10) {
		cmd_line[x++] = '1';
		cmd_line[x++] = '0';
	}
	else {
		cmd_line[x++] = '0' + depth;
	}
	cmd_line[x++] = ' ';
	cmd_line[x++] = '"';
	while (fen[y] != 0)
		cmd_line[x++] = fen[y++];
	cmd_line[x++] = '"';
	cmd_line[x++] = 0;
	if ((iopipe = popen(cmd_line, "r")) == NULL)
		exit(1);
	U64 ret = 0ULL;
	short _depth = 7;
	while (!feof(iopipe))
	{
		if (fgets(psBuffer, 128, iopipe) != NULL) {
			x = 0;
			while (psBuffer[x] != 0) {
				if (psBuffer[x] == '\n') {
					read_line[rx] = 0;
					ret = test_perft_line(_depth, read_line);
					if (ret) {
						depth_num[_depth-1] = ret;
						_depth++;
					}
					rx = 0;
					x++;
				}
				if (psBuffer[x] != 0)
					read_line[rx++] = psBuffer[x++];
			}
		}
	}
}