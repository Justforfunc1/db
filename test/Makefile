#LANG=C
SRC_PATH=../test
BIN_PATH=../bin
INC_PATH=-I. -I../include
LIB_PATH=-L../lib
SHRLIB_PATH = ../lib

LIB= -lpthread -lmysqlclient -llpbase -llpdb -lhiredis -ljsoncpp
SHRLIB = $(SHRLIB_PATH)/liblpbase.so		\
		 $(SHRLIB_PATH)/libmysqlclient.so	\
		 $(SHRLIB_PATH)/libhiredis.so		\
		 $(SHRLIB_PATH)/liblpdb.so	

STLIB = 

CC= gcc
CPPFLAGS= -Wall -g 

OBJS = 

.PHONY:all clean 

all: cache_test sql_test

compile:
#	$(CC) $(CPPFLAGS) $(INC_PATH) $(LIB_PATH) -c epoll_connection.cpp -o epoll_connection.o
	

cache_test:
	$(CC) $(CPPFLAGS) $(INC_PATH) $(LIB_PATH) $(SHRLIB) $(LIB) -Wl,-rpath=$(SHRLIB_PATH) cache_test.cpp $(STLIB) -o cache_test_main
	@printf "\033[32m CACHE_TEST SUCCESS \033[0m\n"

sql_test:
	$(CC) $(CPPFLAGS) $(INC_PATH) $(LIB_PATH) $(SHRLIB) $(LIB) -Wl,-rpath=$(SHRLIB_PATH)  sql_test.cpp $(STLIB) -o sql_test_main
	@printf "\033[32m SQL_TEST SUCCESS \033[0m\n"


run:

clean:
	rm -f *.o cache_test_main sql_test_main

tags:
	rm -rf tags
	touch tags
	find . -iname '*.cpp' -or -iname '*.c' -or -iname '*.h'-or -iname '*.hpp' | xargs ctags --c++-kinds=+p --fields=+iaS --extra=+q --langmap=c++:+.inl -a tags

