CFLAGS=-std=gnu99 -O2 -Wall -lpthread -D_WIN32_
OBJS = process_manager.o windows_manager.o
COBJS = control_console.o windows_control_console.o
SOURCES=$(wildcard src/**/*.c src/*.c)
CC=gcc


all: console manager

console: $(COBJS)
	$(CC) $(CFLAGS) $(COBJS) -o conctrl
	mv conctrl bin/
	rm *.o

control_console.o:
	$(CC) -c $(CFLAGS) src/common/control_console/control_console.c -o control_console.o

windows_control_console.o:
	$(CC) -c $(CFLAGS) src/windows/windows_control_console.c -o windows_control_console.o


manager: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o procesoctrl
	mv procesoctrl bin/
	rm *.o

process_manager.o:
	$(CC) -c $(CFLAGS) src/common/process_manager/process_manager.c -o process_manager.o

windows_manager.o:
	$(CC) -c $(CFLAGS) src/windows/windows_process_manager.c -o windows_manager.o

clean_build:
	rm build/*

clean:
	@echo "Removing files"
	@ls bin/
	rm bin/*


BADFUNCS='[^_.>a-zA-Z0-9](str(n?cpy|n?cat|xfrm|n?dup|str|pbrk|tok|_)|stpn?cpy|a?sn?printf|byte_)'
check:
	@echo Files with potentially dangerous functions.
	@egrep $(BADFUNCS) $(SOURCES) || true