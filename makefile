## Compiler, tools and options
CC      = gcc
LINK    = gcc
OPT     = -g

CCFLAGS = $(OPT) -Wall
LDFLAGS = $(OPT)

## Libraries
LIBS = -lpthread
INC  = -I.

## FILES
OBJECTS1 = mpmc.o semaphore.o
TARGET1  = mpmc

## Build rules
all: $(TARGET1)

$(TARGET1): $(OBJECTS1)
	$(LINK) -o $@ $(OBJECTS1) $(LDFLAGS) $(LIBS)

clean:
	rm -f $(OBJECTS1) $(TARGET1)
	rm -f *~ core
