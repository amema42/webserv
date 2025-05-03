Critical Issues in Your HTTP Server Code
After reviewing your code, I've found several critical issues that would result in a failing grade based on the evaluation criteria:

1. Not Checking Read and Write Events Simultaneously
In your eventLoop() function, you're only monitoring sockets for read events:

p.fd = _listenSockets[s];
p.events = POLLIN;  // Only checking for read events, not POLLOUT
p.revents = 0;

The requirement specifically states: "The select() (or equivalent) should check file descriptors for read and write AT THE SAME TIME."

2. Direct Socket Writing Without Poll
In handleClientRequest(), you write directly to client sockets without checking if they're ready for writing:

std::string respStr = response.toString();
ssize_t sent = write(clientConn->getFd(), respStr.c_str(), respStr.size());

This violates: "Writing or reading ANY file descriptor without going through the select() (or equivalent) is strictly FORBIDDEN."

3. Checking errno After write()
After writing to a socket, you check errno:

if (sent < 0) {
    std::cerr << "Errore in write(): " << strerror(errno) << std::endl;
}

This violates: "If errno is checked after read/recv/write/send, the grade is 0."

4. Not Removing Clients on Errors
When a read error occurs, you don't remove the client:

ssize_t n = read(fd, buffer, sizeof(buffer));
if (n <= 0) {
    continue;  // Just skips, doesn't remove the client
}

5. Improper Return Value Checking
For write operations, you only check for errors (-1) but not for partial writes:

if (sent < 0) {
    // Error handling
}
// No check if sent < respStr.size() (partial write)

Recommended Fix
To pass evaluation, you need to:

Set up poll to monitor both read and write events: p.events = POLLIN | POLLOUT;
Buffer outgoing data and only write when poll indicates the socket is ready for writing
Remove all direct errno checks after read/write operations
Properly handle and remove clients when errors occur
Check for both complete and partial writes, and queue remaining data for later
These issues must be addressed before your code would pass the evaluation requirements.