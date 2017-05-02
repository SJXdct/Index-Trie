LEX = lex
BISON = bison
BISONFLAGS = -d -o command_parser.cpp
CC = g++
OBJ = test

${OBJ}: command_parser.o command_scanner.o test.cpp
	${CC} command_parser.o command_scanner.o test.cpp -o ${OBJ}

command_parser.o: command_parser.cpp command_parser.hpp
	${CC} -c command_parser.cpp

command_scanner.o: command_scanner.cpp
	${CC} -c command_scanner.cpp

command_parser.hpp command_parser.cpp: command_parser.yy
	${BISON} ${BISONFLAGS} command_parser.yy

command_scanner.cpp: command_scanner.ll
	${LEX} command_scanner.ll & mv command_scanner.c command_scanner.cpp

clean:
	rm -f ${OBJ} *.o
