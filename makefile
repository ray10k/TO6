# location of the folder containing the source files.
source:=./src

# names of all the source and header files, respectively.
cpp:=$(wildcard $(source)/*cpp)
h:=$(wildcard $(source)/*.h)
o:=$(cpp:.cpp=.o)

# compiler.
compile := g++
comFlag := -std=c++11 -Wall -Werror -c
comLink := -lstdc++ -lpthread

#implicit rule to make .cpp files into object files.
.cpp.o:
	$(compile) $(comFlag) $(comLink) $< -o $@

#compile.
all: $(h) $(o)
	$(compile) $(comFlag) $(comLink) $(o) -o myFile
