# Hello Kernel Module (test assignment)
---
[Ру](README_RU.md) | [En](README.md)
---

Linux Kernel module for periodic writing in string file and user program for setting module parameters.

## Requirements

- Linux Kernel 5.10+
- Installed Linux headers: [installation](#preparing-to-work)
- Build enviroment: `build-essential`, `make`, `gcc`

## Preparing to work

Before building needs to install Linux kernel headers.

1. Checking kernel version (5.10+)
```bash
uname -r
```

2. Installation:
- Debian/Ubuntu

```bash
sudo apt update && sudo apt install linux-headers-$(uname -r) build-essential
```

- Fedora/RHEL/CentOS

```bash
sudo dnf install kernel-devel-$(uname -r) kernel-headers-$(uname -r) gcc make
```

- ArchLinux/Manjaro
```bash
sudo pacman -S linux-headers base-devel
```

- openSUSE
```bash
sudo zypper install kernel-devel kernel-source gcc make
```

- AlpineLinux
```bash
sudo apk add linux-headers build-base
```

## Building and using

1. Building and load kernel module
```bash
cd kernel-module
make # building via "all" target
sudo make load # insmod
```

2. Building user program
```bash
cd userspace
make # via "all"
./set_params <path> <interval>
```

3. Checking
```bash
cat <path>
dmesg | grep hello_module
```

4. Module unload
```bash
cd kernel-module
make unload # via "unload" target, rmmod
make clean 
```

## GitHub 

(link)[https://github.com/NorvegianForestCat/HelloKM-test-assignment/]