# PvZ-Modifier

This project is a memory manipulation tool written in C for 
Plants vs Zombies, 
designed to modify in-game values such as suns and money.

The software is currently in a **pre-alpha** stage and may be unstable.  
Use it at your own risk.

![C](https://img.shields.io/badge/language-C-blue)
![Linux](https://img.shields.io/badge/platform-Linux-yellow)

Currently available **only on Linux**.

## ⚙️ Requirements
- Linux (64-bit)
- GCC or compatible C compiler
- `glibc`
- Root privileges (required to access process memory)
- Plants vs. Zombies (from steam) running as a target process
- Linux kernel >= 3.2 (for `process_vm_readv`)

## ⚠️ Disclaimer

This project is intended for educational and research purposes only.

## Usage
After launching the program, a menu with three options will be displayed
(the money-related options are currently non-functional).

- Choose **`1) Suns`**
- Input the current sun value from the game
- Spend some suns in-game
- Input the updated sun value
- Enter the desired amount of suns to add

The program will modify the in-memory value accordingly.
