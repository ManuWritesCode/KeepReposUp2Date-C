# Which is the Operating System in use ?
UNAME_S := $(shell uname -s)

# In function of the OS, paths and options are different
ifeq ($(UNAME_S), Darwin) # MacOS
	CFLAGS = -I/opt/homebrew/opt/json-c/include
	LDFLAGS = -L/opt/homebrew/opt/json-c/lib -ljson-c -lcurl
else ifeq ($(UNAME_S), Linux) # Linux
	CFLAGS = -I/usr/include/json-c
	LDFLAGS = -L/usr/lib -ljson-c -lcurl
endif
 

kru2d: build_dir build/main.o
	$(CC) -o build/kru2d build/main.o $(LDFLAGS)

build/main.o: src/main.c
	$(CC) -c src/main.c -o build/main.o $(CFLAGS)

build_dir:
	mkdir -p build

install: kru2d
	mkdir -p ~/.config/KeepReposUp2Date
	cp kru2d-example.conf ~/.config/KeepReposUp2Date/kru2d.conf
	sudo mkdir -p /opt/KeepReposUp2Date
	sudo cp build/kru2d /opt/KeepReposUp2Date/kru2d

clean:
	rm -rf ~/.config/KeepReposUp2Date
	sudo rm -rf /opt/KeepReposUp2Date

.PHONY: install