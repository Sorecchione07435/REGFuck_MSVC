# REGFuck_MSVC

![REGFuck](https://github.com/user-attachments/assets/f9555a5d-b8ab-4e02-a794-06e45f18736a)

⚠️ **WARNING – EXTREMELY DESTRUCTIVE SOFTWARE** ⚠️

**RegFuck** is a proof-of-concept program designed to intentionally corrupt the Windows Registry, making the operating system unstable, partially unusable, or completely unbootable.

This project is **NOT** meant to be used on real systems.
It exists purely for **educational, experimental, and historical purposes**.

This software was **not originally invented and designed by me 100%** but by another user who wrote it in .NET using the C# language, I with my deeper knowledge of Microsoft Visual C++ wanted to more or less make a "fork" by rewriting it from scratch

The original creator's repository is this: https://github.com/UnforeseenOcean/REGFuck

## 🧠 Why rewrite it in MSVC?

The original REGFuck was written in .NET for convenience and speed of development.

**I decided to rewrite this version for:**

- Native binaries start instantly
- No dependency on Windows Features (.NET 3.5)
- Lower-level control over memory, UI, and system calls
- Better understanding of classic Win32 software internals
- Curiosity and reverse-engineering interest

This was done **not to make REGFuck better** but just to bring an application with .NET dependencies back to the operating system with CRT already statically linked, all .NET instructions were rebuilt in MSVC language

## ✅ Intended use

- Educational analysis
- Reverse engineering
- Curiosity
- UI / Win32 experimentation
- Historical recreation

**Use ONLY in virtual machines or disposable environments.**

## How to Build

