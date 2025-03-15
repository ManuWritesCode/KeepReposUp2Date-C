<h1 align="center">KEEP REPOS UP TO DATE (C version)</h1>

<p align="center">
    <a href="http://www.gnu.org/licenses/gpl-3.0" alt="License GNU GPL v3"><img alt="License GNU GPL v3" src="https://img.shields.io/badge/License-GPL%20v3-blue.svg"></a>
</p>

<br />

I use 3 computers to code :

a desktop at home running Debian GNU/Linux (SID version)
a desktop at work, also running Debian GNU/Linux (SD version)
and a Macbook Pro, with MacOS.
How can I keep my local repositories on my 3 computers automatically up to date ?

To answer that question, I wrote a Bash script ([here](https://github.com/ManuWritesCode/KeepReposUp2Date)), but it is too slow for my usage. 
So, I decided to write a C version, which will launch as many threads as I have Github repositories. Each thread will run in parallel. I hope this will make updating Github repositories faster.

<br />

## Installation

### Prerequisites

This program needs some libraries to run correctly : libcurl and json-c.
You can install them by running :

- on MacOS :
```bash
brew install curl json-c
```

- on Debian GNU/Linux
```bash
sudo apt update
sudo apt install curl libjson-c-dev
```

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

### Configuring

For each your computers, copy _kru2d-example.conf_ to a new file called _kru2d.conf_ :

```bash
cp kru2d-example.conf kru2d.conf
```

Then, open the file you just create :

```bash
nano kru2d.conf
```

and modify it with the correct information.