# Webserv

## Intro

The **Webserv** project involves creating your own HTTP server in **C++ 98**. The server must:

- Be able to process HTTP requests and serve responses.
- Allow testing with a web browser.
- Comply with specific requirements, ensuring stability and resilience.

HTTP is a foundational protocol for data communication on the World Wide Web, making this project invaluable for understanding web technologies.

---

## General Rules

1. **Makefile**:

   - Must compile source files without relinking unnecessarily.
   - Required rules: `$(NAME)`, `all`, `clean`, `fclean`, `re`.

2. **Compilation**:

   - Use `c++` with `-Wall -Wextra -Werror` flags.
   - Code must comply with the **C++ 98** standard.

3. **Preferred Practices**:

   - Use C++ features over their C equivalents (e.g., `<cstring>` over `<string.h>`).

4. **Prohibited Libraries**:

   - No external libraries or Boost libraries are allowed.

---

## Mandatory Part

### Program Requirements

- **Name**: `webserv`
- **Files**: Must include `Makefile`, headers (`*.h, *.hpp`), implementation files (`*.cpp, *.tpp, *.ipp`), and configuration files.
- **Execution**: `./webserv [configuration file]`
- **External Functions**:
  - Allowed: `execve`, `dup`, `dup2`, `pipe`, `strerror`, `gai_strerror`, `errno`, `fork`, `socketpair`, `htons`, `htonl`, `ntohs`, `ntohl`, `select`, `poll`, `epoll` (and variants), `kqueue` (and variants), `socket`, `accept`, `listen`, `send`, `recv`, `chdir`, `bind`, `connect`, `getaddrinfo`, `freeaddrinfo`, `setsockopt`, `getsockname`, `fcntl`, `close`, `read`, `write`, `waitpid`, `kill`, `signal`, `access`, `stat`, `open`, `opendir`, `readdir`, and `closedir`.

### Functional Requirements

1. **Core Features**:

   - Handle HTTP methods: `GET`, `POST`, and `DELETE`.
   - Serve a fully static website.
   - Enable file uploads.
   - Default error pages if none are provided.
   - Compatibility with modern web browsers.

2. **Behavior**:

   - Must be non-blocking and use only one `poll()` (or equivalent) for all I/O operations.
   - Ensure accurate HTTP response status codes.
   - Stress-test the server to guarantee availability under load.
   - Support listening on multiple ports.

3. **Restrictions**:

   - Cannot use `fork` for purposes other than CGI execution.
   - Must unchunk `chunked` requests for CGI processing.

### Configuration File

- Define ports and hosts for servers.
- Specify server names (optional).
- Set up default error pages and client body size limits.
- Define route-specific settings, such as:
  - Accepted HTTP methods.
  - HTTP redirection rules.
  - Directory/file mapping for URL paths.
  - Directory listing toggles.
  - CGI execution based on file extensions.
  - File upload locations.

---

## MacOS-Specific Notes

- Use `fcntl()` for non-blocking file descriptors.
- Permitted flags: `F_SETFL`, `O_NONBLOCK`, `FD_CLOEXEC`.

---

## Bonus Part

Bonus features include:

- Support for cookies and session management.
- Handling multiple CGI programs.

---

## Notes

- **Testing**:

  - Compare server behavior with NGINX.
  - Use tools like Telnet and Python scripts to simulate client requests.
  - Verify against the HTTP/1.1 specification for compliance.
  - ...

- **Documentation**:

  - Provide configuration files and example content to demonstrate server functionality.

