# Webserv 
en.subject.pdf transcription

## Summary

This project is about writing your own HTTP server.

You will be able to test it with an actual browser.
HTTP is one of the most used protocols on the internet.

Knowing its arcane will be useful, even if you won’t be working on a website.

**Version: 21.2**

---

## Contents

1. [Introduction](#introduction)
2. [General Rules](#general-rules)
3. [Mandatory Part](#mandatory-part)
   - [Requirements](#requirements)
   - [For MacOS Only](#for-macos-only)
   - [Configuration File](#configuration-file)
4. [Bonus Part](#bonus-part)
5. [Submission and Peer-evaluation](#submission-and-peer-evaluation)

---

## Introduction

The Hypertext Transfer Protocol (HTTP) is an application protocol for distributed, collaborative, hypermedia information systems.

HTTP is the foundation of data communication for the World Wide Web, where hypertext documents include hyperlinks to other resources that the user can easily access, for example, by a mouse click or by tapping the screen in a web browser.

HTTP was developed to facilitate hypertext and the World Wide Web.

The primary function of a web server is to store, process, and deliver web pages to clients. The communication between client and server takes place using the Hypertext Transfer Protocol (HTTP).

Pages delivered are most frequently HTML documents, which may include images, style sheets, and scripts in addition to the text content.

Multiple web servers may be used for a high-traffic website.

A user agent, commonly a web browser or web crawler, initiates communication by requesting a specific resource using HTTP, and the server responds with the content of that resource or an error message if unable to do so.

While the primary function is to serve content, full implementation of HTTP also includes ways of receiving content from clients. This feature is used for submitting web forms, including the uploading of files.

---

## General Rules

- Your program should not crash under any circumstances (even when it runs out of memory) and should not quit unexpectedly. If it happens, your project will be considered non-functional, and your grade will be **0**.
- You have to turn in a Makefile that will compile your source files. It must not relink.
- Your Makefile must at least contain the rules: `$(NAME)`, `all`, `clean`, `fclean`, and `re`.
- Compile your code with `c++` and the flags `-Wall -Wextra -Werror`.
- Your code must comply with the **C++ 98 standard**. It should still compile if you add the flag `-std=c++98`.
- Try to always develop using the most C++ features you can (e.g., prefer `<cstring>` over `<string.h>`).
- You are allowed to use C functions, but always prefer their C++ versions if possible.
- Any external library and Boost libraries are **forbidden**.

---

## Mandatory Part

**Program name:** `webserv`

**Turn in files:** `Makefile`, `*.h`, `*.hpp`, `*.cpp`, `*.tpp`, `*.ipp`, configuration files.

**Makefile rules:** `NAME`, `all`, `clean`, `fclean`, `re`.

**Arguments:** `[A configuration file]`

**External functions:** Everything in C++ 98, including:
`execve`, `dup`, `dup2`, `pipe`, `strerror`, `gai_strerror`, `errno`, `fork`, `socketpair`, `htons`, `htonl`, `ntohs`, `ntohl`, `select`, `poll`, `epoll_create`, `epoll_ctl`, `epoll_wait`, `kqueue`, `kevent`, `socket`, `accept`, `listen`, `send`, `recv`, `chdir`, `bind`, `connect`, `getaddrinfo`, `freeaddrinfo`, `setsockopt`, `getsockname`, `getprotobyname`, `fcntl`, `close`, `read`, `write`, `waitpid`, `kill`, `signal`, `access`, `stat`, `open`, `opendir`, `readdir`, `closedir`.

**Libft authorized:** N/A

**Description:** An HTTP server in C++ 98.

Your executable will be run as follows:

```sh
./webserv [configuration file]
```

Even if `poll()` is mentioned, you can use any equivalent such as `select()`, `kqueue()`, or `epoll()`.

### Requirements

- Your program must take a configuration file as an argument or use a default path.
- You **cannot** `execve` another web server.
- Your server must **never** block, and the client should be bounced properly if necessary.
- It must be **non-blocking** and use only **one** `poll()` (or equivalent) for all I/O operations.
- `poll()` (or equivalent) must check **read and write** at the same time.
- Never read/write without going through `poll()` (or equivalent).
- Checking `errno` **after** read/write operations is strictly **forbidden**.
- Your server **must be compatible** with a web browser.
- HTTP response **status codes must be accurate**.
- Your server must have **default error pages** if none are provided.
- Clients must be able to **upload files**.
- Your server **must** support **GET, POST, and DELETE** methods.
- Your server must handle **multiple ports** (see Configuration file).

### For MacOS Only

Since MacOS does not implement `write()` the same way as other Unix OSes, you are allowed to use `fcntl()` with **only** the following flags:

- `F_SETFL`
- `O_NONBLOCK`
- `FD_CLOEXEC`

### Configuration File

Your configuration file should allow:

- Choosing the **port and host** of each server.
- Setting up **server_names** (optional).
- Defining **default error pages**.
- Limiting **client body size**.
- Setting up **routes** with:
  - Accepted HTTP methods.
  - HTTP redirection.
  - Directory/file search paths.
  - Directory listing options.
  - Default response file for directories.
  - CGI execution.
  - File uploads and storage configurations.

---

## Bonus Part

If the **mandatory part is perfect**, you can add:

- **Support for cookies and session management**.
- **Handling multiple CGI**.

The bonus part will only be evaluated **if the mandatory part is 100% functional**.

---

## Submission and Peer-evaluation

Turn in your assignment in your Git repository as usual. Only the work inside your repository will be evaluated during the defense.

**Double-check the names of your files to ensure they are correct.**

