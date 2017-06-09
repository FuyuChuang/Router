CC=g++
LDFLAGS=-std=c++11 -O2 -lm
SOURCES=src/router.cpp src/util.cpp src/main.cpp
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=Router
INCLUDES=src/module.h src/util.h src/router.h

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

%.o:  %.c  ${INCLUDES}
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o $(EXECUTABLE)
