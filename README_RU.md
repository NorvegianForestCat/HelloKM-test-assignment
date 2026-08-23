# Hello Kernel Module (test assignment)
---
[Ру](README_RU.md) | [En](README.md)
---

Модуль ядра Linux для периодической записи строки в файл и пользовательская программа для задания параметров модуля.

## Требования

- Linux Kernel 5.10+
- Установленные Linux headers: [установка](#подготовка-к-работе)
- Окружение: `build-essential`, `make`, `gcc`

## Подготовка к работе

Прежде, чем начать собирать, необходимо установить заголовки ядра Linux:

1. Проверка версии ядра (5.10+)
```bash
uname -r
```

2. Установка:
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

## Сборка и использование

1. Сборка модуля и его загрузка
```bash
cd kernel-module
make # сборка через таргет "all"
sudo make load # insmod
```

2. Сборка пользовательской программы
```bash
cd userspace
make # via "all"
./set_params <path> <interval>
```

3. Проверка работоспособности
```bash
cat <path>
dmesg | grep hello_module
```

4. Выгрузка модуля
```bash
cd kernel-module
make unload # через таргет "unload", rmmod
make clean 
```

## GitHub 

(Ссылка)[https://github.com/NorvegianForestCat/HelloKM-test-assignment/]