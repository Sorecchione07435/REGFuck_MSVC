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

## 🔧 How to Build

**REGFuck_MSVC** is written in **native Win32 C++** and must be built manually.

For **security reasons**, **NO precompiled binaries are distributed**.

### 🛠 Requirements

* **Microsoft Visual Studio**
* **Windows SDK**
* One of the following toolsets:

| Target OS              | Toolset                    | Recommended SDK    |
| ---------------------- | -------------------------- | ------------------ |
| Windows XP / Vista     | **MSVC v140_xp / v141_xp** | **Windows 7 SDK**  |
| Windows 7 → Windows 11 | **MSVC v143**              | **Windows 10 SDK** |

> ⚠️ XP support requires installing the **XP toolset** via Visual Studio Installer.

### 🧱 Build Targets

#### ▶ Legacy Windows (XP / Vista)

* Platform Toolset: **v140_xp** or **v141_xp**
* Windows SDK: **Windows 7 SDK**
* Architecture: **Win32 (x86)**
* Subsystem: **Windows**
* Runtime: static CRT (`/MT`) recommended

Allows execution on:

* Windows XP
* Windows Vista
* Windows 7+

#### ▶ Modern Windows (7 → 11)

* Platform Toolset: **v143**
* Windows SDK: **Windows 10 SDK**
* Architecture: **Win32 (x86)** or **x64**
* Runtime: static CRT (`/MT`) recommended

### 📦 Build Steps

1. Open the solution in **Visual Studio**
2. Select the desired **Platform Toolset**
3. Ensure the correct **Windows SDK** is selected
4. Choose **Release | Win32**
5. Build the solution

The resulting executable is:

* Fully native
* Single `.exe`
* No .NET
* No external dependencies

## 🔒 No Precompiled Binaries

This repository **does NOT provide precompiled executables**.

Reasons:

* Prevent accidental execution
* Avoid misuse on real systems
* Encourage analysis and understanding
* Force intentional compilation by the user

If you compile this software, **you are fully responsible for how and where it is executed**.

## 🧰 Visual Studio Version Notes

⚠️ **Important**

The latest versions of **Visual Studio (2026 and newer)** **DO NOT support** the following toolsets anymore:

* **MSVC v140_xp**
* **MSVC v141_xp**

Because of this, **Windows XP / Vista builds cannot be compiled** using the newest Visual Studio releases.

### ✅ Recommended Visual Studio Versions

To build legacy-compatible versions, it is recommended to use:

* **Visual Studio 2017** *(best choice for XP builds)*
* **Visual Studio 2022** *(still supports older toolsets, depending on installation options)*

> Visual Studio 2017 provides the **most stable and complete support** for `v140_xp` and `v141_xp`, especially when paired with the **Windows 7 SDK**.

### 📝 Summary

* **Modern builds (v143)** → Visual Studio 2022+
* **Legacy builds (v140_xp / v141_xp)** → **Visual Studio 2017 recommended**
* Visual Studio 2026+ **cannot** be used for XP/Vista targets

## ⚠️ Final Warning

This software **will damage Windows installations**.

### ✅ Intended use

- Educational analysis
- Reverse engineering
- Curiosity
- UI / Win32 experimentation
- Historical recreation

**Use ONLY in:**

* Virtual machines
* Isolated environments
* Disposable test systems
