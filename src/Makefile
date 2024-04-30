CC = gcc
FLAG_C = -c
ASAN = -g -fsanitize=address
FLAG_ER = -Wall -Werror -Wextra -std=c11
VALGRIND_FLAGS = --leak-check=full --show-leak-kinds=all --track-origins=yes
TET_LIB_C = ./brick_game/tetris/fsm.c ./brick_game/tetris/tetris_backend.c ./brick_game/tetris/tetris_lib.c
TET_LIB_O = fsm.o tetris_backend.o tetris_lib.o
TET_LIB_H = ./brick_game/tetris/fsm.h ./brick_game/tetris/tetris_backend.h ./brick_game/tetris/tetris_lib.h
CLI_C = ./gui/cli/cli.c
CLI_h = ./gui/cli/cli.h
CLI_O = cli.o
SUITE_CASES_C = suite_*.c
SUITE_CASES_O = suite_*.o
FLAG_TESTS = -lcheck
TESTS_C=test.c
TESTS_O=test.c
ifeq ($(shell uname), Linux)
	FLAG_TEST +=-lsubunit -lm -lcheck_pic
endif


all: clean uninstall install 

install:tetris_lib.a
	gcc -Wall -Wextra -Werror -Wpedantic -o tetris_game tetris.c  gui/cli/cli.c tetris_lib.a -lncurses
	make clean

uninstall:
	-rm -f tetris_game 

tetris_lib.a:
	$(CC) $(FLAG_C) $(FLAG_ER) $(TET_LIB_C)  
	ar rc tetris_lib.a $(TET_LIB_O) -lm
	ranlib tetris_lib.a

test: clean tetris_lib.a
	gcc -g --coverage $(FLAG_ER) $(TESTS_C) -o test tetris_lib.a -lcheck_pic -lm -lsubunit
	./test

gcov_report: test
	gcc --coverage $(TET_LIB_C) $(TESTS_C) -o greport -lcheck_pic -lm -lsubunit
	./greport
	lcov -t "test" -o tetris.info -c -d .
	genhtml -o ./report tetris.info --exclude test.c
	open ./report/index.html

valgrind_test: tetris_lib.a
	$(CC) $(FLAG_C) $(FLAG_ER) $(TESTS_C) -g
	$(CC) -g $(FLAG_ER) $(FLAG_COV) -o tests $(TET_LIB_C) $(TESTS_O) -lcheck_pic -lm -lsubunit 
	valgrind $(VALGRIND_FLAGS) ./tests

dvi:
	open ./README.md

dist:
	tar -czf tetris.install.tar.gz ./* 

cpp_check:
	cppcheck --enable=all --force $(TET_LIB_C) $(CLI_C) 

style_check:
	cp ../materials/linters/.clang-format ./
	clang-format -n $(TET_LIB_C) $(CLI_C) $(TET_LIB_H) $(CLI_H) 
	clang-format -i $(TET_LIB_C) $(CLI_C) $(TET_LIB_H) $(CLI_H)
	rm -rf .clang-format

clean:
	-rm -rf *.o *.html *.gcda *.gcno *.css *.a *.gcov *.info *.out *.cfg *.txt
	-rm -f test
	-rm -f tests
	-rm -f greport
	-rm -rf report
	-rm -f tetris.install.tar.gz 
	find . -type d -name 'tests.dSYM' -exec rm -r {} +
