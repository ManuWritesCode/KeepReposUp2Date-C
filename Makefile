# Which is the Operating System in use ?
UNAME_S := $(shell uname -s)

# Whici is the user HOME path ?
USER_HOME := $(HOME)

# In function of the OS, paths and options are different
ifeq ($(UNAME_S), Darwin) # MacOS
	CFLAGS = -I/opt/homebrew/opt/json-c/include -I/opt/homebrew/opt/libgit2/include
	LDFLAGS = -L/opt/homebrew/opt/json-c/lib -L/opt/homebrew/opt/libgit2/lib -ljson-c -lcurl -lgit2
else ifeq ($(UNAME_S), Linux) # Linux
	CFLAGS = -I/usr/include/json-c
	LDFLAGS = -L/usr/lib -ljson-c -lcurl -lgit2
endif
 

kru2d: build_dir build/main.o build/kru2d.o build/github.o
	$(CC) -o build/kru2d build/main.o build/kru2d.o build/github.o $(LDFLAGS)

build/github.o: src/github.c
	$(CC) -c src/github.c -o build/github.o $(CFLAGS)
	
build/kru2d.o: src/kru2d.c
	$(CC) -c src/kru2d.c -o build/kru2d.o $(CFLAGS)

build/main.o: src/main.c
	$(CC) -c src/main.c -o build/main.o $(CFLAGS)

build_dir:
	mkdir -p build