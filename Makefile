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
 

kru2d: build_dir build/main.o build/github.o
	$(CC) -o build/kru2d build/main.o build/github.o $(LDFLAGS)

build/github.o: src/github.c
	$(CC) -c src/github.c -o build/github.o $(CFLAGS)

build/main.o: src/main.c
	$(CC) -c src/main.c -o build/main.o $(CFLAGS)

build_dir:
	mkdir -p build

install: kru2d
# Create the configuration directory if it does not exist
	mkdir -p ~/.config/KeepReposUp2Date
# Copy the configuration file 
	cp kru2d-example.conf ~/.config/KeepReposUp2Date/kru2d.conf
# Create the installation directory if it does not exist
	sudo mkdir -p /opt/KeepReposUp2Date
# Copy the binary to the installation directory
	sudo cp build/kru2d /opt/KeepReposUp2Date/kru2d

# Create systemd service file for Linux
ifeq ($(UNAME_S), Linux)
	@echo "[Unit]" > kru2d.service
	@echo "Description=Keep Repos Up To Date" >> kru2d.service
	@echo "After=default.target" >> kru2d.service
	@echo "" >> kru2d.service
	@echo "[Service]" >> kru2d.service
	@echo "ExecStart=/opt/KeepReposUp2Date/kru2d" >> kru2d.service
	@echo "Environment=HOME=$(USER_HOME)" >> kru2d.service
	@echo "Restart=on-failure" >> kru2d.service
	@echo "RestartSec=5s" >> kru2d.service
	@echo "StartLimitInterval=30s" >> kru2d.service
	@echo "StartLimitBurst=3" >> kru2d.service
	@echo "" >> kru2d.service
	@echo "[Install]" >> kru2d.service
	@echo "WantedBy=default.target" >> kru2d.service
	sudo mv kru2d.service /etc/systemd/system/kru2d.service
	sudo systemctl enable kru2d.service
endif

# Create launchd plist file for MacOS
ifeq ($(UNAME_S), Darwin)
	@echo "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" > com.keepreposuptodate.kru2d.plist
	@echo "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">" >> com.keepreposuptodate.kru2d.plist
	@echo "<plist version=\"1.0\">" >> com.keepreposuptodate.kru2d.plist
	@echo "<dict>" >> com.keepreposuptodate.kru2d.plist
	@echo "    <key>Label</key>" >> com.keepreposuptodate.kru2d.plist
	@echo "    <string>com.keepreposuptodate.kru2d</string>" >> com.keepreposuptodate.kru2d.plist
	@echo "    <key>ProgramArguments</key>" >> com.keepreposuptodate.kru2d.plist
	@echo "    <array>" >> com.keepreposuptodate.kru2d.plist
	@echo "        <string>/opt/KeepReposUp2Date/kru2d</string>" >> com.keepreposuptodate.kru2d.plist
	@echo "    </array>" >> com.keepreposuptodate.kru2d.plist
	@echo "    <key>RunAtLoad</key>" >> com.keepreposuptodate.kru2d.plist
	@echo "    <true/>" >> com.keepreposuptodate.kru2d.plist
	@echo "    <key>KeepAlive</key>" >> com.keepreposuptodate.kru2d.plist
	@echo "    <true/>" >> com.keepreposuptodate.kru2d.plist
	@echo "</dict>" >> com.keepreposuptodate.kru2d.plist
	@echo "</plist>" >> com.keepreposuptodate.kru2d.plist
	mv com.keepreposuptodate.kru2d.plist ~/Library/LaunchAgents/com.keepreposuptodate.kru2d.plist
	launchctl load ~/Library/LaunchAgents/com.keepreposuptodate.kru2d.plist
endif

clean:
	rm -rf ~/.config/KeepReposUp2Date
	sudo rm -rf /opt/KeepReposUp2Date
ifeq ($(UNAME_S), Linux)
	sudo rm -f /etc/systemd/system/kru2d.service
	sudo systemctl disable kru2d.service
endif
ifeq ($(UNAME_S), Darwin)
	rm -f ~/Library/LaunchAgents/com.keepreposuptodate.kru2d.plist
	launchctl unload ~/Library/LaunchAgents/com.keepreposuptodate.kru2d.plist
endif

.PHONY: install clean