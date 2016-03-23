# names of all the source and header files, respectively.
cpp:=$(shell find ./src -name '*.cpp')
h:=$(shell find ./src -name '*.h')
o:=$(cpp:.cpp=.o)

# compiler.
# add "-DDEBUG" to comFlag to print debug stuff.
compile := g++
comFlag := -std=c++11 -Wall -c
comLink := -lstdc++ -lpthread -lcrypto

#implicit rule to make .cpp files into object files.
.cpp.o:
	$(compile) $(comFlag) $(comLink) $< -o $@

#compile.
all: $(h) $(o)
	$(compile) -o runnable $(o) $(comLink)

clean:
	rm $(o)
