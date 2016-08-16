CC=g++
OPTS=-c -Wall
SOURCES=$(wildcard *.c )
OBJECTS=$(SOURCES:.c=.o)
LIBS=
EXECUTABLE=shtb
all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(LINK.o) $^ -o $@ $(LIBS); mkdir -p "${HOME}/.shtb"

install:
	cp $(EXECUTABLE) /usr/bin

clean:
	rm $(EXECUTABLE) $(OBJECTS)