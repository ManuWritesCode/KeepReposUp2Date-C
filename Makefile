CFLAGS = -I/opt/homebrew/opt/json-c/include
LDFLAGS = -L/opt/homebrew/opt/json-c/lib -ljson-c -lcurl

kru2d: build_dir build/main.o
	$(CC) -o build/kru2d build/main.o $(LDFLAGS)

build/main.o: src/main.c
	$(CC) -c src/main.c -o build/main.o $(CFLAGS)

build_dir:
	mkdir -p build