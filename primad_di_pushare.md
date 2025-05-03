# Checklist di Valutazione per Webserv

## 1) Main loop deve controllare lettura **E** scrittura insieme

**Requirement**  
> The select() (or equivalent) should be in the main loop and should check file descriptors for read and write AT THE SAME TIME. If not, the grade is 0 and the evaluation process ends now.

**Issue**  
In `src/net/HTTPServer.cpp`, funzione `HTTPServer::eventLoop`, righe 449–452:  
```cpp
p.fd     = _listenSockets[s];
p.events = POLLIN;    // ← solo POLLIN
```
e analogamente per i client nuovi:  
```cpp
pollfd cp = { client_fd, POLLIN, 0 };
```
Non viene mai specificato `POLLOUT`, quindi `poll()` monitora solo la lettura.

**Possible solution**  
Sempre in `HTTPServer::eventLoop`, intorno alle righe 447–452 e 486, cambiare:  
```cpp
p.events = POLLIN;
// …
pollfd cp = { client_fd, POLLIN, 0 };
```
in  
```cpp
p.events = POLLIN | POLLOUT;
// …
pollfd cp = { client_fd, POLLIN | POLLOUT, 0 };
```
In questo modo `poll()` torna sia per socket pronti a leggere che a scrivere.

---

## 2) Solo **UNA** operazione (read **O** write) per client per select

**Requirement**  
> There should be only one read or one write per client per select() (or equivalent). Ask the group to show you the code from the select() (or equivalent) to the read and write of a client.

**Issue**  
In `HTTPServer::eventLoop`, alla ricezione di `revents & POLLIN` (righe 501–503) si fa:  
```cpp
ssize_t n = read(fd, buffer, sizeof(buffer));
// … parsing …
// poi, nella stessa iterazione, si chiama subito write()
ssize_t sent = write(clientConn->getFd(), respStr.c_str(), respStr.size());  // righe 412–413
```
Così in un solo giro di `poll()` si eseguono sia read sia write sullo stesso socket.

**Possible solution**  
- In `eventLoop`, su `revents & POLLIN` **solo** fare il `read()`, accumulare la risposta in un buffer interno di `ClientConnection`.  
- Aggiungere un ramo separato `if (pollfds[i].revents & POLLOUT)` in cui **solo** si chiama `write()` per svuotare il buffer di risposta.  
In questo modo ogni select gestisce o un read o un write, mai entrambi.

---

## 3) Rimuovere il client se read/recv/write/send ritorna errore

**Requirement**  
> Search for all read/recv/write/send on a socket and check that, if an error is returned, the client is removed.

**Issue**  
- Nel blocco di lettura:  
  ```cpp
  if (n <= 0) {
    close(fd);
    pollfds.erase(...);
    _clientConnections.erase(...);
  }
  ```  
  qui va bene.  
- **Ma** nel blocco di scrittura (`src/net/HTTPServer.cpp`, righe 412–416):  
  ```cpp
  ssize_t sent = write(clientFd, respStr.c_str(), respStr.size());
  if (sent < 0) {
    std::cerr << "Errore in write(): " << strerror(errno) << std::endl;
    // → NO remove/close!
  }
  ```  
  in caso di errore di `write` il client **non** viene rimosso.

**Possible solution**  
Subito dopo `if (sent <= 0)` aggiungere:  
```cpp
close(clientFd);
// rimuovere dal vector pollfds e da _clientConnections
```  
in `src/net/HTTPServer.cpp` intorno alle righe 412–416.

---

## 4) Controllare sempre sia valore `0` sia `-1`

**Requirement**  
> Search for all read/recv/write/send and check if the returned value is correctly checked (checking only -1 or 0 values is not enough, both should be checked).

**Issue**  
- `read(...)` è gestito con `if (n <= 0)`, ok.  
- `write(...)` è gestito soltanto con `if (sent < 0)`, quindi un `sent == 0` non viene rilevato.

**Possible solution**  
Cambiare in:  
```cpp
if (sent <= 0) {
  // errore o socket chiuso: chiudi e rimuovi
}
```  
in `src/net/HTTPServer.cpp`, righe 412–416.

---

## 5) Non usare `errno` dopo read/recv/write/send

**Requirement**  
> If errno is checked after read/recv/write/send, the grade is 0 and the evaluation process ends now.

**Issue**  
Ci sono stampe che usano `strerror(errno)`:  
- dopo `poll()` in `src/net/HTTPServer.cpp` linea 460  
- dopo `accept()` in `src/net/HTTPServer.cpp` linea 477  
- dopo `write()` in `src/net/HTTPServer.cpp` linea 415

**Possible solution**  
Rimuovere ogni riferimento a `errno`/`strerror(errno)`. Sostituire con messaggi generici come:  
```cpp
std::cerr << "Error in poll()" << std::endl;
```  
analogamente per `accept()` e `write()`.

---

## 6) Vietato leggere/scrivere FDs fuori da select

**Requirement**  
> Writing or reading ANY file descriptor without going through the select() (or equivalent) is strictly FORBIDDEN.

**Issue**  
Il `write()` col socket client viene chiamato immediatamente (`src/net/HTTPServer.cpp`, righe 412–413), al di fuori di un controllo `revents & POLLOUT`.

**Possible solution**  
Spostare ogni `write()` dentro il ramo `if (pollfds[i].revents & POLLOUT)` nel `eventLoop`. In fase di parsing richiesta si accoda il buffer, poi `eventLoop` lo scrive solo quando `poll()` lo segnala come pronto.
