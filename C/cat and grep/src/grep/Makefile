all: s21_grep
	
s21_grep: s21_grep.c s21_grep.h
	clang-format -i *.c
	clang-format -i *.h
	gcc -Wall -Werror -Wextra -std=c11 -o s21_grep s21_grep.c s21_grep.h -g
check: 
	clang-format -n *.c
	clang-format -n *.h
	valgrind --leak-check=full ./s21_grep

test: 
	sh test.sh
	
clean:
	rm s21_grep