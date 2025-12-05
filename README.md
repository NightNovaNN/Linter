---

# 📦 **Trappist Linter**

A fast, cross-platform, multi-language syntax checker powered by a native high-performance C engine and a Python interface.
Designed for speed, portability, and editor/tool integration.

---

<p align="center">
  <img src="https://img.shields.io/badge/build-native-blue?style=for-the-badge">
  <img src="https://img.shields.io/badge/platform-windows%20%7C%20linux%20%7C%20macos-green?style=for-the-badge">
  <img src="https://img.shields.io/badge/languages-c%20%7C%20cpp%20%7C%20java%20%7C%20python-orange?style=for-the-badge">
  <img src="https://img.shields.io/badge/license-MIT-lightgrey?style=for-the-badge">
</p>

---

# 🚀 Overview

**Trappist Linter** scans source code in:

- **C**
- **C++**
- **Java**
- **Python**

It detects:

- Unclosed quotes (`'`, `"`, `'''`, `"""`)
- Unmatched brackets (`()`, `{}`, `[]`)
- Unclosed C-style comments (`/* */`)
- Python indentation errors
- Python triple-quote mismatches

The linter reports **line + column** and supports **full error lists** for CLI tools, editors, and automation systems.

Built as a **shared C library** for maximum speed, with a Python wrapper for easy integration.

---

# ✨ Features

### ✔ Multi-language support

Each language is scanned with its own rules while sharing a fast core scanner.

### ✔ High-performance native engine

The heavy work is done in C for speed comparable to compiler frontends.

### ✔ Cross-platform builds

Outputs:

| Platform | Output File |
| -------- | ----------- |
| Windows  | `.dll`      |
| Linux    | `.so`       |
| macOS    | `.dylib`    |

### ✔ Full error list reporting

Unlike simple linters, Trappist returns **all errors**, not just the first one.

### ✔ Line + column indexing

Perfect for editor plugins and automated fixing.

### ✔ Modular architecture

The project is split cleanly:

```
src/error.c     - error list management
src/error.h     - structs + enums
src/linter.c    - full scanning engine
src/linter.py   - python interface
```

---

# 📁 Project Structure

```
Linter/
│
├── CMakeLists.txt
├── Makefile
├── README.md
│
├── src/
│   ├── error.c
│   ├── error.h
│   ├── linter.c
│   └── linter.py
│
└── .github/
    └── workflows/
        └── build.yml
```

---

# 🔧 Building the Native Library

### 🛠 Using CMake (recommended)

```sh
cmake -S . -B build
cmake --build build
```

Outputs go to:

```
build/errorcheck.dll
build/errorcheck.so
build/errorcheck.dylib
```

---

### 🛠 Using Makefile

Linux / macOS:

```sh
make
```

Windows (MinGW):

```sh
mingw32-make
```

Clean:

```sh
make clean
```

---

# 🐍 Python Usage

```python
from linter import SmartLinter

code = """
def test():
    x = """Hello
    print("oops)
"""

l = SmartLinter(code)
errors = l.get_errors("python")

for e in errors:
    print(e["line"], e["column"], e["message"])
```

Example output:

```
3 11 Unclosed Python triple quote
4 7 Indentation not multiple of 4
```

---

# 🤖 GitHub Actions (Auto-build)

A workflow is included to automatically build:

- Windows DLL
- Linux SO
- macOS DYLIB

on every push to `main`.

Artifacts are uploaded automatically.

File:

```
.github/workflows/build.yml
```

---

# 🛣 Roadmap

- CLI tool (`trappistlint`)
- JSON output for editors
- Auto-fix engine (quote/bracket repair)
- Support for raw string literals (Java/C++)
- Mismatched bracket detection
- VSCode extension

---

# 📜 License

MIT License — free to use, modify, and distribute.

---

# 🙌 Contributing

Issues and pull requests are welcome.
Please ensure all C code builds on Windows, Linux, and macOS.

---

