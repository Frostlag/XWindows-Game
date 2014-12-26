CXX = g++
CXXFLAGS = -Wall -MMD -g -L/usr/X11R6/lib -lX11 -lstdc++
EXEC = tdgame
OBJECTS = simpwindow.o main.o timer.o game.o screenobj.o
DEPENDS = ${OBJECTS:.o=.d}

${EXEC}: ${OBJECTS}
	${CXX} ${OBJECTS} ${CXXFLAGS} -o ${EXEC}
	valgrind ./${EXEC}


-include ${DEPENDS}

.PHONY: clean

clean:
	rm ${OBJECTS} ${EXEC} ${DEPENDS}
