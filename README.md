<h1 align="center">KEEP REPOS UP TO DATE (C version)</h1>

<p align="center">
    <a href="http://www.gnu.org/licenses/gpl-3.0" alt="License GNU GPL v3"><img alt="License GNU GPL v3" src="https://img.shields.io/badge/License-GPL%20v3-blue.svg"></a>
</p>

<br />

**I use 3 computers to code :**

- a desktop at home running Debian GNU/Linux (SID version)
- a desktop at work, also running Debian GNU/Linux (SD version)
- and a Macbook Pro, with MacOS.

How can I keep my local repositories on my 3 computers automatically up to date ?

To answer that question, I wrote a Bash script ([here](https://github.com/ManuWritesCode/KeepReposUp2Date)), but it is too slow for my usage. 
So, I decided to write a C version, which will launch as many threads as I have Github repositories. Each thread will run in parallel. I hope this will make updating Github repositories faster

<br />

## Installation

### Prerequisites

This program needs some libraries to run correctly : libcurl and json-c.
You can install them by running :

- on MacOS :
```bash
brew install curl json-c libgit2
```

- on Debian GNU/Linux
```bash
sudo apt update
sudo apt install curl libcurl4-openssl-dev libjson-c-dev libgit2-dev
```
You need too a Github token too to let this programm accessing to your Github account. If you do not know what a Github token is, please refer to this link : [Managing your personnal access tokens](https://docs.github.com/fr/authentication/keeping-your-account-and-data-secure/managing-your-personal-access-tokens).

### Building

Clone this repository :

```bash
git clone https://github.com/ManuWritesCode/KeepReposUp2Date-C.git
```

Go to _KeepReposUp2Date-C_ folder :

```bash
cd KeepReposUp2Date-C
```

and compile the sources :

```bash
make
```

### Installing

To install kru2d, 

```bash
make install
```

This will copy the configuration file to your user folder, and the binary to /opt/KeepReposUp2Date.

### Configuring

Edit the configfuration file :

```bash
nano ~/.config/kru2d.conf
```

and modify it like this :

```bash
DEV_PATH=path/of/your/main/development/directory
GITHUB_TOKEN=your_Github_token_to_access_your_Github_account
GITHUB_USERNAME=your_github_username
SSH_PRIVATE_KEY=path/of/your/SSH/private/key
SSH_PUBLIC_KEY=path/of/your/SSH/public/key
SSH_PASSPHRASE=your_SSH_key_passphrase
```

by replacing _path/of/your/main/development/directory_ with the path containing all your development projects, _Your_Github_token_to_access_your_Github_account_ by your Github token, _your_github_username_ by your Github username, _path/of/your/SSH/private/key_ by the path where your private SSH keys are stored (in general : _~/.ssh/<key_file>_), _path/of/your/SSH/public/key_ by the path where your public SSH keys are stored (in general : _~/.ssh/<key_file>.pub_), and _your_SSH_key_passphrase_ by the passphrase you defined when you have created your SSH keys.

## Run the binary automatically

### On Debian GNU/Linux

To run the binary automatically at each session start, the `make install` command creates a systemd service containing :

```ini
[Unit]
Description=Keep Repos Up To Date
After=default.target

[Service]
ExecStart=/opt/KeepReposUp2Date/kru2d
Environment=HOME=/home/manu
Restart=on-failure
RestartSec=5s
StartLimitInterval=30s
StartLimitBurst=3

[Install]
WantedBy=default.target
```

Then enable and start the service:

```bash
sudo systemctl enable kru2d.service
sudo systemctl start kru2d.service
```

### On MacOS

For MacOS, a launchd agent is created :

```xml
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>Label</key>
    <string>com.keepreposuptodate.kru2d</string>
    <key>ProgramArguments</key>
    <array>
        <string>/opt/KeepReposUp2Date/kru2d</string>
    </array>
    <key>RunAtLoad</key>
    <true/>
    <key>KeepAlive</key>
    <true/>
</dict>
</plist>
```

Then load the agent:

```bash
launchctl load ~/Library/LaunchAgents/com.keepreposuptodate.kru2d.plist
```