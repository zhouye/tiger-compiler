CC = gcc
CPP = g++
LEX = flex
LFLAGS= -8     
YACC= bison 
YFLAGS= -d -t -y

CPPFLAGS = `llvm-config --cppflags` -O2
LDFLAGS = `llvm-config --ldflags`
LIBS = `llvm-config --libs`

RM = /bin/rm -f

all: tigerc

tigerc: tigerc.o ast.o
	${CPP} -o tigerc lex.yy.o y.tab.o tigerc.o ast.o ${CPPFLAGS} ${LDFLAGS} ${LIBS} -lfl

tigerc.o: tigerc.cpp y.tab.o lex.yy.o
	${CPP} ${CPPFLAGS} -c tigerc.cpp
	
ast.o: ast.cpp ast.h 
	${CPP} ${CPPFLAGS} -c ast.cpp
	
y.tab.o: tiger.y ast.o
	${YACC} ${YFLAGS} tiger.y
	${CPP} ${CPPFLAGS} y.tab.c -c 

lex.yy.o: tiger.lex ast.o
	${LEX} $(LFLAGS) tiger.lex
	${CPP} ${CPPFLAGS} lex.yy.c -c 

clean:
	/bin/rm -f lex.yy.* y.tab.* *.o tigerc
