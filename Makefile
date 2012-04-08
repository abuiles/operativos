CFLAGS=-g -O2 -Wall -Isrc -Wextra -lpthread -D_LINUX_

SOURCES=$(wildcard src/**/*.c src/*.c)


all:
	@echo "You have to specify a targer, process_manager or control_mager"


manager: dbg.o manager.o
	@echo "done"

clean_build:
	rm build/*


dbg.o:
	cc -c $(CFLAGS) src/dbg.h  -o build/dbg.o

manager.o: linux_manager.o windows_manager.o
	cc $(CFLAGS) src/common/process_manager/process_manager.c -llinux_manager.o -lwindows_manager.o -lbuild/dbg.o  -o manager

linux_manager.o:
	cc -c $(CFLAGS) src/linux/linux_process_manager.c build/dbg.o -o build/linux_process_manager.o

windows_manager.o:
	cc -c $(CFLAGS) src/windows/windows_process_manager.c build/dbg.o -o build/windows_process_manager.o
clean:
	@echo "Removing files"
	@ls bin/
	rm bin/*

BADFUNCS='[^_.>a-zA-Z0-9](str(n?cpy|n?cat|xfrm|n?dup|str|pbrk|tok|_)|stpn?cpy|a?sn?printf|byte_)'
check:
	@echo Files with potentially dangerous functions.
	@egrep $(BADFUNCS) $(SOURCES) || true