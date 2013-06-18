CC = gcc
CPP = g++
LEX = flex
LFLAGS= -8     
YACC= bison 
YFLAGS= -d -t -y

CPPFLAGS = `llvm-config --cppflags` -g
LDFLAGS = `llvm-config --ldflags`
LIBS = `llvm-config --libs`

RM = /bin/rm -f

tigerc: y.tab.o lex.yy.o tigerc.o
	${CPP} ${CPPFLAGS} lex.yy.o y.tab.o tigerc.o -o tigerc -lfl

tigerc.o: tigerc.cpp tigerc.h
	${CPP} ${CPPFLAGS} -c tigerc.cpp
	
y.tab.o: tiger.y
	${YACC} ${YFLAGS} tiger.y
	${CPP} ${CPPFLAGS} y.tab.c -c 

lex.yy.o: tiger.lex
	${LEX} $(LFLAGS) tiger.lex
	${CPP} ${CPPFLAGS} lex.yy.c -c 

clean:
	/bin/rm -f lex.yy.* y.tab.* *.o tigerc
