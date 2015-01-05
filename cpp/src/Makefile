
maintarget = graham

sources = Main.cpp Config.cpp Logger.cpp CurrentTime.cpp Utils.cpp Thread.cpp dmmm_dbface.cpp dmmm_utils.cpp MemUsage.cpp RunInfo.cpp ServerUtils.cpp Financials.cpp HttpClient.cpp Url.cpp Tokenizer.cpp Parser.cpp Test.cpp

VPATH = utils main dmmm financials html parser 

objects := $(patsubst %.cpp, %.o, $(sources))
dependencies := $(patsubst %.cpp, %.d, $(sources))

# Directories to include must be either added to this list, or to an include file in the 'include' directory
include_dirs = -I include -I dmmm -I utils -I include -I /usr/include/mysql -I /usr/include/mysql++/ -I /usr/include/boost -I html 

include_libs = -lm -L/usr/lib -lboost_system -lboost_filesystem -lboost_thread -lboost_regex -lboost_date_time /usr/lib/x86_64-linux-gnu/libarchive.so -lmysqlclient /usr/lib/libmysqlpp.so.3 -lcurl -lboost_regex

include Makefile.flags

CC = g++

all: ../bin/$(maintarget)

install: 
	cp -r ../bin ../src 

../bin/$(maintarget): $(objects)
	@echo linking $(maintarget)
	@$(CC) -o ../bin/$(maintarget) $(objects) $(include_libs)

$(objects): %.o: %.cpp %.d Makefile.flags
	@echo compiling $<
	@$(CC) -c $(cflags) $(include_dirs) $< -o $@ 2>&1 

$(dependencies): %.d: %.cpp
	@echo making dependencies for $<
	@$(SHELL) -ec '$(CC) -MM $(include_dirs) $< | sed '\''s/\($*\)\.o[ :]*/\1.o $@ : /g'\'' > $@'

valgrind:
	cd ..;	valgrind ./bin/$(maintarget) --leak-check=full

Makefile.flags:

clean:
	rm -f *.o *.d ../bin/* 

ifneq ($(MAKECMDGOALS),clean)
-include $(sources:.cpp=.d)
endif
