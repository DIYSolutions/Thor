#include "basic_data.h"
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
	lock.clear(std::memory_order_release); // release lock
}
