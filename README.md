<!-- Back to top link -->
<a id="readme-top"></a>

<!-- PROJECT SHIELDS -->
[![C][C-shield]][C-url]
[![LinkedIn][linkedin-shield]][linkedin-url]

<br />
<div align="center">

<h3 align="center">sish - Shell</h3>

  <p align="center">
    A Unix-like interactive shell built in C, supporting command execution, multi-process piping, and command history.
  </p>
</div>

<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li><a href="#about-the-project">About The Project</a></li>
    <li><a href="#built-with">Built With</a></li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#installation">Installation</a></li>
      </ul>
    </li>
    <li><a href="#usage">Usage</a></li>
    <li><a href="#implementation">Implementation Details</a></li>
    <li><a href="#roadmap">Roadmap</a></li>
  </ol>
</details>

---

<!-- ABOUT THE PROJECT -->
## About The Project

`sish` is a lightweight Unix shell written in C from scratch. It replicates core shell behavior including running external programs, chaining commands with pipes, and managing a persistent command history — all implemented using low-level POSIX system calls.

**Key concepts demonstrated:**
- Process creation and management with `fork()`, `execvp()`, and `wait()`
- Inter-process communication via `pipe()` and I/O redirection with `dup2()`
- Dynamic memory allocation and cleanup for history tracking
- Input parsing and built-in command handling

<p align="right">(<a href="#readme-top">back to top</a>)</p>

---

<!-- BUILT WITH -->
## Built With

[![C][C-shield]][C-url]
[![Linux][Linux-shield]][Linux-url]

<p align="right">(<a href="#readme-top">back to top</a>)</p>

---

<!-- GETTING STARTED -->
## Getting Started

### Prerequisites

- GCC or any C99-compatible compiler
- Unix-based OS (Linux or macOS)

### Installation

1. Clone the repo
   ```sh
   git clone https://github.com/JohnK700/Shell-Project.git
   ```
2. Build the shell
   ```sh
   gcc -o sish shell.c
   ```
3. Run it
   ```sh
   ./sish
   ```

<p align="right">(<a href="#readme-top">back to top</a>)</p>

---

<!-- USAGE -->
## Usage

```bash
# Run a basic command
sish> ls -la

# Chain commands with pipes
sish> ls -la | grep ".c" | wc -l

# Navigate directories
sish> cd /usr/local

# View command history
sish> history
0 ls -la
1 cd /usr/local
2 history

# Re-run a previous command by index
sish> history 0

# Clear all history
sish> history -c

# Exit the shell
sish> exit
```

<p align="right">(<a href="#readme-top">back to top</a>)</p>

---

<!-- IMPLEMENTATION -->
## Implementation Details

| Component | Details |
|---|---|
| **Command execution** | `fork()` spawns a child process; `execvp()` replaces it with the target program; parent calls `wait()` to reap the child |
| **Pipe handling** | Iterates over `\|`-delimited commands, connecting each process's stdout to the next's stdin via `dup2()`; file descriptors are closed after duplication to prevent blocking |
| **History** | Stores up to 100 commands as heap-allocated strings; when full, the oldest entry is freed and the array is shifted; `history <n>` re-parses and executes the entry at offset `n` |
| **Input parsing** | `strtok()` tokenizes input on spaces into a `NULL`-terminated array passed directly to `execvp()` |
| **Whitespace handling** | Leading/trailing whitespace is trimmed with `memmove()` before parsing to prevent execution errors |

<p align="right">(<a href="#readme-top">back to top</a>)</p>

---

<!-- ROADMAP -->
## Roadmap

- [ ] Background process execution with `&`
- [ ] I/O redirection (`>`, `<`, `>>`)
- [ ] History persistence across sessions
- [ ] Support for quoted strings with spaces (e.g., `echo "hello world"`)


<p align="right">(<a href="#readme-top">back to top</a>)</p>

---

<!-- MARKDOWN LINKS & BADGES -->
[issues-shield]: https://img.shields.io/github/issues/github_username/repo_name.svg?style=for-the-badge
[issues-url]: https://github.com/github_username/repo_name/issues
[linkedin-shield]: https://img.shields.io/badge/-LinkedIn-black.svg?style=for-the-badge&logo=linkedin&colorB=555
[linkedin-url]: https://www.linkedin.com/in/john-kalu-b60587379/
[C-shield]: https://img.shields.io/badge/C-00599C?style=for-the-badge&logo=c&logoColor=white
[C-url]: https://en.wikipedia.org/wiki/C_(programming_language)
[Linux-shield]: https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black
[Linux-url]: https://www.linux.org/
