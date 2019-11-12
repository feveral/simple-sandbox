CC = gcc
CFLAGS = -O2 -g -std=c99 -Wall -I include

OBJS = src/main.o \
		src/utility.o
SHARE_SOURCE = src/sandboxLib.c

EXEC = ./sandbox
SHARE = sandbox.so

all: $(EXEC) $(SHARE)
run: $(EXEC) $(SHARE)
	$(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@  -ldl

$(SHARE): $(SHARE_SOURCE)
	$(CC) -Wall -fPIC -shared -o $@ $(SHARE_SOURCE) -I include -ldl

src/%.o: src/%.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm $(OBJS) $(SHARE) $(EXEC)