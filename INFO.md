# **webserv/3**
## 0. Risorse:

Elenco di risorse, **suddivise** per macro-tema, che coprono (quasi) tutti gli aspetti di webserv.

### **Networking**

-   **Beej’s Guide to Network Programming**  
    [https://beej.us/guide/bgnet/html/](https://beej.us/guide/bgnet/html/)  
    _La guida più classica per socket in C/C++._
    
-   **IBM - Nonblocking I/O and select()**  
    https://www.ibm.com/docs/en/i/7.1?topic=designs-nonblocking-io  
    _Approfondimento su I/O non bloccante e `select()` (concetti estesi anche per `poll()` e `epoll()`)._
    
-   **Bogotobogo - Sockets Server & Client**  
    [https://bogotobogo.com/cplusplus/sockets_server_client.php](https://bogotobogo.com/cplusplus/sockets_server_client.php)  
    _Esempi passo-passo di server e client in C++._
    
-   **High-Performance Browser Networking** (Capitoli 1 e 3)  
    [https://hpbn.co/](https://hpbn.co/)  
    _Ottimo riferimento per approfondire concetti di rete e performance._
    

----------

### **HTTP**

-   **MDN - HTTP**  
    [https://developer.mozilla.org/en-US/docs/Web/HTTP](https://developer.mozilla.org/en-US/docs/Web/HTTP)  
    _Punto di riferimento per tutti i concetti HTTP (metodi, status code, header, sicurezza)._
    
-   **RFC 7230 - HTTP/1.1**  
    [https://tools.ietf.org/html/rfc7230](https://tools.ietf.org/html/rfc7230)  
    _Specifica ufficiale di HTTP/1.1 (post RFC 2616)._
    
-   **How the web works: HTTP and CGI explained**  
    [https://www.garshol.priv.no/download/text/http-tut.html](https://www.garshol.priv.no/download/text/http-tut.html)  
    _Tutorial introduttivo per capire HTTP e CGI insieme._
    
-   **HTTP Status Codes**  
    [https://developer.mozilla.org/en-US/docs/Web/HTTP/Status](https://developer.mozilla.org/en-US/docs/Web/HTTP/Status)  
    _Elenco dettagliato dei codici di stato HTTP._
    

----------

### **RFC (di Riferimento Principale)**

-   **RFC 9110 - HTTP Semantics**  
    https://www.rfc-editor.org/rfc/rfc9110  
    _Definisce le semantiche HTTP/1.1 aggiornate._
    
-   **RFC 9112 - HTTP/1.1**  
    https://www.rfc-editor.org/rfc/rfc9112  
    _Specifiche più recenti di HTTP/1.1._
    
-   **RFC 6265 - HTTP State Management Mechanism (Cookies)**  
    [https://datatracker.ietf.org/doc/html/rfc6265](https://datatracker.ietf.org/doc/html/rfc6265)
    
-   **RFC 3986 - URI Generic Syntax**  
    https://www.rfc-editor.org/rfc/rfc3986
    
-   **RFC 3875 - CGI**  
    [https://tools.ietf.org/html/rfc3875](https://tools.ietf.org/html/rfc3875)
    

----------

### **CGI**

-   **CGI Programming 101**  
    [http://www.cgi101.com/book/ch1/text.html](http://www.cgi101.com/book/ch1/text.html)  
    _Spiegazione semplice su come funziona CGI._
    
-   **HOW-TO Write a CGI Program in C/C++**  
    [https://www.purplepixie.org/cgi/howto.php](https://www.purplepixie.org/cgi/howto.php)  
    _Piccola guida pratica con esempi._
    
-   **(Video) Creating a file upload page**  
    _Youtube/risorse varie, es.: “C++ CGI File Upload Tutorial”_  
    _Per capire come gestire i form in POST e memorizzare i file._
    

----------

### **Strumenti & Testing**

-   **curl**  
    https://curl.se/docs/  
    _Per testare GET/POST, upload, header personalizzati._
    
-   **netcat (nc), telnet**  
    _Per inviare richieste HTTP “raw” e controllare la risposta._
    
-   **Wireshark / tcpdump**  
    _Per catturare e analizzare traffico di rete, ottimo per debugging avanzato._
    
-   **ab (ApacheBench), siege, wrk**  
    _Strumenti per stress e load test._
    
-   **Postman**  
    _Interfaccia GUI per inviare richieste personalizzate (comodo per test di upload)._
    

----------

### **Altre Risorse Utili**

-   **nginx Source Code**  
    [https://github.com/nginx/nginx](https://github.com/nginx/nginx)  
    _Per vedere come un server HTTP gestisce config e parsing (casi complessi)._
    
-   **DigitalOcean - Understanding Nginx Config**  
    https://www.digitalocean.com/community/tutorials/understanding-nginx-configuration-file-structure-and-configuration-contexts  
    _Per ispirazione nella creazione del parser di configurazione._
    
-   **URL Encoding**  
    [https://en.wikipedia.org/wiki/Percent-encoding](https://en.wikipedia.org/wiki/Percent-encoding)  
    _Fondamentale se devi decodificare query string o path._


## 1. Divisione dei Compiti

### **a. Networking & Event Loop**

-   **Socket & Binding**
    
    -   Creazione dei socket (`socket()`), binding (`bind()`) su una o più porte, `listen()`.
    -   Configurazione delle opzioni di socket (`setsockopt`, `SO_REUSEADDR`, ecc.).
-   **Event Loop (poll/epoll/kqueue)**
    
    -   Implementazione di un loop non-bloccante (un solo `poll()` o equivalente) per tutto il server.
    -   Gestione e registrazione degli eventi di lettura/scrittura su più connessioni contemporaneamente.
-   **Accettazione Connessioni & Parsing Base dei Dati**
    
    -   Accettazione delle connessioni in arrivo (`accept()`) e memorizzazione delle informazioni necessarie (IP, porta, stato della richiesta).
    -   Pre-processing dei dati “grezzi” (controllo se la lettura è completa) e passaggio al modulo di parsing HTTP (Persona B).

----------

### **b. HTTP Parser & Core Server Logic**

-   **Parsing HTTP Requests**
    
    -   Lettura e interpretazione degli header HTTP (metodo, URI, versione, `Host`, `Content-Length`, `Transfer-Encoding`, ecc.).
    -   Decodifica di _chunked requests_ (unchunk del body).
    -   Gestione dei limiti di dimensione (client body size limit) e generazione di eventuali errori (`413 Payload Too Large`).
-   **Routing & Configurazione**
    
    -   Lettura/parsing del file di configurazione (server blocks, location blocks, ecc.).
    -   Mappatura dei percorsi (root directory, directory listing, cgi).
    -   Logica di routing: se servire un file statico, eseguire un upload o avviare un CGI.
-   **Generazione Risposte**
    
    -   Creazione di risposte HTTP valide (status line, header, body).
    -   Impostazione dei codici di stato appropriati (`200`, `301`, `403`, `404`, `500`, ecc.).
    -   Gestione pagine di errore di default o personalizzate.

----------

### **c. CGI, File Handling & Extra Features**

-   **Gestione CGI**
    
    -   Utilizzo di `fork()`, `execve()` per eseguire script esterni (CGI).
    -   Passaggio di variabili d’ambiente (REQUEST_METHOD, CONTENT_LENGTH, SCRIPT_NAME, ecc.).
    -   Raccolta dell’output CGI tramite pipe e re-invio nella risposta HTTP.
-   **File System, Upload e Directory Listing**
    
    -   Apertura, lettura, scrittura di file (file statici e scrittura per gli upload).
    -   Directory listing (se abilitato) usando `opendir`, `readdir`, generazione elenco dei file.
-   **Ottimizzazioni e Bonus**
    
    -   Gestione di cookie e sessioni.
    -   Supporto a multiple CGI (PHP, Python, ecc.) in base all’estensione dei file.

----------

## 2. Argomenti da Studiare/Approfondire

### 2.1. Networking (C++ / Unix)

1.  **Socket a Basso Livello (BSD Sockets)**
    
    -   `socket()`, `bind()`, `listen()`, `accept()`, `send()`, `recv()`.
    -   `sockaddr_in`, `sockaddr_in6`, opzioni di socket (`setsockopt`, `SO_REUSEADDR`).
    -   Error handling (`errno`, `strerror`).
2.  **Gestione Non-bloccante & Multiplexing**
    
    -   `fcntl()`, `O_NONBLOCK`.
    -   Principi di I/O non-bloccante.
    -   `poll()` / `epoll()` (Linux) / `kqueue()` (BSD/macOS).
    -   Gestione time-out e molte connessioni concorrenti.

----------

### 2.2. HTTP 1.1

1.  **Struttura del Protocollo**
    
    -   Request line (metodo, URI, versione), header (`Host`, `Content-Length`, `Transfer-Encoding`, ecc.), body opzionale.
    -   Metodi minimi: GET, POST, DELETE.
    -   Codici di stato (es. 200, 404, 405, 413, 500).
2.  **Chunked Transfer Encoding**
    
    -   Riconoscimento di `Transfer-Encoding: chunked` e scomposizione corretta.
3.  **Stateless ma non sessionless**
    
    -   Differenza fra HTTP “stateless” e sessioni realizzate via cookie.

----------

### 2.3. Gestione Processi / CGI

1.  **fork() & execve()**
    
    -   Creazione di processi figli, sostituzione immagine del processo con uno script/programma CGI.
    -   `dup2()` per reindirizzare STDIN/STDOUT.
2.  **Variabili d’Ambiente per CGI**
    
    -   REQUEST_METHOD, SCRIPT_FILENAME, CONTENT_LENGTH, CONTENT_TYPE, ecc.
    -   Differenze tra GET (query string) e POST (body).
3.  **IPC (Inter-Process Communication)**
    
    -   Utilizzo di pipe per la comunicazione col processo figlio CGI.

----------

### 2.4. File System & Sicurezza

1.  **Syscall di Base**
    
    -   `open()`, `read()`, `write()`, `close()`, `stat()`, `lseek()`.
    -   `opendir()`, `readdir()`, `closedir()` per directory listing.
    -   Evitare directory traversal (`../`), validando i path.
2.  **Upload**
    
    -   Scrittura su file del body in POST.
    -   Rispetto del limite (client_max_body_size).

----------

### 2.5. Parsing & Configurazione

1.  **Parser File di Configurazione (simil-NGINX)**
    
    -   Server block, location block, parametri come root, cgi, index, max body size.
    -   Struttura interna (classi `ServerConfig`, `LocationConfig`) collegata alla logica di routing.
2.  **Parser HTTP**
    
    -   Split su `\r\n`, `\r\n\r\n` per separare header e body.
    -   Interpretazione metodica degli header (host, user-agent, ecc.).

----------

### 2.6. C++ 98 / Good Practices

1.  **Standard C++ 98**
    
    -   `std::string`, `std::vector`, `std::map`, `std::pair`.
    -   Gestione della memoria, costruttore/distruttore/copy constructor (Rule of Three).
2.  **Organizzazione del Codice**
    
    -   Progettazione a classi (Single Responsibility Principle).
    -   Header `.hpp/.h` e sorgenti `.cpp`.
    -   Evitare variabili globali, mantenere un design modulare.

----------

### 2.7. Bonus: Sessioni e Cookies

1.  **HTTP Cookies**
    
    -   `Set-Cookie` / `Cookie` header, gestione basic di sessione.
    -   Politiche di sicurezza (HttpOnly, Secure).
2.  **Session Storage**
    
    -   Mappa (session_id → stato) sul server per sessioni di base.

----------

### 2.8. Testing

1.  **Debug e Test Funzionali**
    
    -   `telnet`, `netcat` (raw HTTP), `curl` (GET/POST, file upload).
    -   Verificare risposte con `curl -I` (solo header) o browser.
2.  **Load Testing**
    
    -   `ab` (ApacheBench), `siege`, `wrk` per valutare prestazioni.
3.  **Confronti con NGINX**
    -   Creare config simile e confrontare risposte e performance.

----------

## 3. Tasks: Piano di Lavoro (Fasi e Timeline)

### **Fase 1: Setup & Studi Preliminari (1 settimana)**

-   **Obiettivo**: allinearsi sui concetti di base.
-   **Persona A**: Studia funzioni di rete (socket, bind, listen) e meccanismi non-bloccanti.
-   **Persona B**: Studia HTTP 1.1, focus su request/response e chunked.
-   **Persona C**: Studia CGI (fork, execve), pipe, variabili d’ambiente, e I/O file.

**Risultato atteso**:

-   Documentazione interna (o UML) su come organizzare le classi (Networking, Parser, CGI, ecc.).

----------

### **Fase 2: Struttura del Progetto & Skeleton (1-2 settimane)**

1.  **Persona A**
    
    -   Implementa la classe/struttura per aprire il socket e metterlo in ascolto.
    -   Imposta il loop di `poll/epoll/kqueue` con test base (senza parse HTTP).
2.  **Persona B**
    
    -   Progetta le classi `HttpRequest`, `HttpResponse`, funzioni di parsing HTTP.
    -   Crea un prototipo di parser file di config (classi `ConfigParser`, `ServerConfig`, ecc.).
3.  **Persona C**
    
    -   Prepara la struttura per il CGI (classe `CgiHandler`, funzioni `executeCgi()`).
    -   Aggiunge funzioni basilari di file I/O (lettura file statici, directory listing).

**Risultato atteso**:

-   Il progetto compila, loop di I/O parzialmente funzionante, parser “scheletro”.

----------

### **Fase 3: HTTP Core & Routing (2 settimane)**

1.  **Persona A**
    
    -   Integra il parser HTTP (di B) nel loop di poll/epoll.
    -   Gestisce la lettura non-bloccante, accumulando i dati finché la request non è completa.
2.  **Persona B**
    
    -   Rifinisce il parser (gestione completa degli header e body, compreso chunked).
    -   Integra la config (root, error pages, metodi ammessi) e definisce il routing.
3.  **Persona C**
    
    -   Testa un primo CGI (script Python/Perl) che risponde “Hello CGI!”.
    -   Gestisce upload (POST) se presente nel mandatory.

**Risultato atteso**:

-   File statici serviti correttamente via GET, supporto base a POST e CGI.

----------

### **Fase 4: Error Handling & Test (1-2 settimane)**

1.  **Persona A**
    
    -   Gestione time-out, chiusura connessioni corrotte, input malevolo (stabilità).
2.  **Persona B**
    
    -   Mappa i codici di stato (404, 405, 413) e pagine di errore di default o custom.
3.  **Persona C**
  
    -   Completa la parte CGI (variabili d’ambiente, chunked input).
    -   Testa directory listing e upload in più casi limite.


----------

### **Fase 5: Refinement & Bonus (1-2 settimane)**

1.  **Persona A**
    
    -   Supporto multi-port e ottimizzazioni performance (refactoring).
2.  **Persona B**
    
    -   HTTP redirection (301, 302), config avanzata (più virtual host).
3.  **Persona C**
    
    -   Cookie e sessioni di base, eventuale supporto multi-CGI (estensione file: `.php`, `.py`).

----------

### **Fase 6: Collaudo Finale & Documentazione (1 settimana)**

1.  Test intensivi con `curl`, `telnet`, `nc`, e confronto con `nginx`.
2.  Load test (`ab`, `siege`, `wrk`) per valutarne la stabilità.
3.  Documentazione:
    -   Guida su `./webserv [config_file]`.
    -   Esempi di configurazione.
    -   & Funzionalità bonus (se implementate).

----------
Questo **piano di lavoro/studio**:

1.  **Organizza il progetto** in tre aree (Networking, HTTP Parser & Logic, CGI/File Handling).
2.  **Timeline**
3.  **Recap risorse** unico e senza duplicati, suddiviso in categorie chiare (Networking, HTTP, RFC, CGI, Strumenti di Test).

-----------

### **possibile tree directory**

webserv/
├── src/                # Codice sorgente del server
│   ├── core/           # Funzionalità principali (socket, parsing, response)
│   │   ├── Server.cpp    # Classe principale del server
│   │   ├── Server.hpp
│   │   ├── Socket.cpp    # Gestione dei socket
│   │   ├── Socket.hpp
│   │   ├── Request.cpp   # Parsing delle richieste HTTP
│   │   ├── Request.hpp
│   │   ├── Response.cpp  # Generazione delle risposte HTTP
│   │   ├── Response.hpp
│   │   ├── ...           # Altri file per funzionalità core
│   ├── config/         # Gestione della configurazione
│   │   ├── ConfigParser.cpp # Parsing del file di configurazione
│   │   ├── ConfigParser.hpp
│   │   ├── ServerConfig.hpp # Strutture dati per la configurazione
│   ├── cgi/            # Gestione dei CGI
│   │   ├── CgiHandler.cpp
│   │   ├── CgiHandler.hpp
│   ├── utils/          # Funzioni di utilità
│   │   ├── Utils.cpp
│   │   ├── Utils.hpp
│   ├── main.cpp        # Punto di ingresso del programma
├── include/            # File header condivisi (alternativa a includerli in ogni sottodirectory src/)
│   ├── Server.hpp
│   ├── ...
├── conf/                # File di configurazione di esempio
│   ├── default.conf
│   ├── altro.conf
├── www/                 # Contenuto web statico (pagine, immagini, ecc.)
│   ├── index.html
│   ├── css/
│   │   ├── style.css
│   ├── images/
│   │   ├── logo.png
├── Makefile              # File di compilazione
└── README.md             # Documentazione del progetto
