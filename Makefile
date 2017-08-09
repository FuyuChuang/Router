CC=g++
LDFLAGS=-std=c++11 -O2 -lm
CFLAGS = `pkg-config --cflags opencv`
LIBS = `pkg-config --libs opencv`
SOURCES=src/router.cpp src/main.cpp src/mergeTree.cpp
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=router
INCLUDES=src/module.h src/router.h src/mergeTree.h

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(CFLAGS) $(LIBS) $(OBJECTS) -o $@

%.o:  %.c  ${INCLUDES}
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o $(EXECUTABLE)
