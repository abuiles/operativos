CFLAGS=-std=gnu99 -O2 -Wall -lpthread -D_LINUX_
OBJS = process_manager.o linux_manager.o
COBJS = control_console.o linux_control_console.o
SOURCES=$(wildcard src/**/*.c src/*.c)


all: console manager

console: $(COBJS)
	cc $(CFLAGS) $(COBJS) -o conctrl
	mv conctrl bin/
	rm *.o

control_console.o:
	cc -c $(CFLAGS) src/common/control_console/control_console.c -o control_console.o

linux_control_console.o:
	cc -c $(CFLAGS) src/linux/linux_control_console.c -o linux_control_console.o


manager: $(OBJS)
	cc $(CFLAGS) $(OBJS) -o procesoctrl
	mv procesoctrl bin/
	rm *.o

process_manager.o:
	cc -c $(CFLAGS) src/common/process_manager/process_manager.c -o process_manager.o

linux_manager.o:
	cc -c $(CFLAGS) src/linux/linux_process_manager.c -o linux_manager.o

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