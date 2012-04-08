CFLAGS=-g -O2 -Wall -Isrc src/linux/ -Wextra -lpthread -D_LINUX_

SOURCES=$(wildcard src/**/*.c src/*.c)


all:
	@echo "You have to specify a targer, process_manager or control_mager"


manager: src/common/process_manager/process_manager

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