CC=g++
LDFLAGS=-std=c++11 -O2 -lm
CFLAGS = `pkg-config --cflags opencv`
LIBS = `pkg-config --libs opencv`
SOURCES=src/router.cpp src/util.cpp src/main.cpp src/mergeTree.cpp
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=Router
INCLUDES=src/module.h src/util.h src/router.h src/mergeTree.h

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(CFLAGS) $(LIBS) $(OBJECTS) -o $@

%.o:  %.c  ${INCLUDES}
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o $(EXECUTABLE)
