CFLAGS=-std=gnu99 -O2 -Wall -lpthread -D_LINUX_
OBJS = process_manager.o linux_manager.o
SOURCES=$(wildcard src/**/*.c src/*.c)


all:
	@echo "You have to specify a target, process_manager or control_mager"


manager: $(OBJS)
	cc $(CFLAGS) $(OBJS) -o process_manager
	mv process_manager bin/
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