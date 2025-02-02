## **1. Componenti base di webserv: funzionamento attuale e ToDo**

### **a. Parsing della Configurazione**

- **Tokenizer**  
  - **Funzione:** Legge il contenuto del file di configurazione e lo divide in *token*
  - **Funzioni chiave:**  
    - `isSymbol(char c)`: Riconosce se un carattere è uno dei simboli speciali (`{`, `}`, `;`).
    - `isWhitespace(char c)`: Identifica spazi bianchi, tabulazioni e interruzioni di linea.
    - `tokenize(const std::string &content)`: Scorre++ la stringa, saltando gli spazi, identificando "simboli" e "parole" - creando un vettore di `Token`.

- **Parser**  
  - **Funzione:** Converte la sequenza di token in "strutture dati" C++ che rappresentano la configurazione (strutture `Config`, `ServerConfig` e `LocationConfig`).
  - **Funzioni chiave:**  
    - `parse(const std::string &configContent)`: Inizia il parsing, scorre i token e chiama (1) `parseServer` ogni volta che trova la direttiva `server`.
    - `parseServer(const std::vector<Token> &tokens, size_t &pos)`: Legge le direttive di un blocco server (come `listen`, `host`, etc.) e gestisce eventuali *blocchi* `location`.
    - `parseLocation(...)`: Processa le direttive specifiche per una location, come `root`, `index`, `autoindex`, `methods`, `cgi`.
    - `expectToken(...)`: Funzione helper che verifica la presenza di un token atteso (ad esempio, il simbolo `;` o `{`).

### **b. Server HTTP**

- **HTTPServer**
  - **Scopo:** Gestire i socket per l’ascolto e la comunicazione con i client in modalità non bloccante, utilizzando `poll()` per gestire I/O.
  
- **initSockets()**
  - **Funzione:**  
    - Per ogni `ServerConfig` ottenuto dal parser, crea un socket TCP con `socket(AF_INET, SOCK_STREAM, 0)`.
    - Configura il socket in modalità non bloccante usando `fcntl()`:
      - `fcntl(sockfd, F_GETFL, 0)` per ottenere i flag correnti.
      - `fcntl(sockfd, F_SETFL, flags | O_NONBLOCK)` per aggiungere il flag `O_NONBLOCK`.
    - Associa il socket all’indirizzo e porta specificati (usando `bind()`) e inizia ad ascoltare con `listen()`.
    - In caso di successo, aggiunge il file descriptor alla lista `_listenSockets`.

- **eventLoop()**
  - **Funzione:**  
    - Costruisce un vettore di `pollfd` a partire dai socket di ascolto.
    - Avvia un loop in cui chiama `poll()` per *monitorare gli eventi* sui *file descriptor*.
    - Ogni volte che `poll()` restituisce un evento, il ciclo verifica:
      - (a) Se l'evento proviene da un socket di ascolto (cioè, è *pronto per accettare nuove connessioni*): esegue `accept()` per creare una connessione con il client.  
        - (a1) Imposta il socket del client in modalità non bloccante.
        - (a2) Aggiunge il nuovo socket alla lista di `pollfd` per monitorare le future operazioni.
      - (b) Se l'evento proviene da un client già connesso, tenta di leggere i dati con la func `read()`.  
        - (b1) Se `read()` restituisce 0 o un errore, chiude il socket e lo rimuove dal vettore.
        - (b2) Se riceve dati, li stampa a console (**ToDo:** Qui vi è poi da * (1)inserire il parsing della richiesta HTTP e (2) la generazione della risposta*).
    - **Importante:** il `continue` viene/deve venire eseguito solo in caso di errore di `poll()`.

- **run()**
  - **Funzione:** Avvia il loop degli eventi chiamando `eventLoop()`.  
    Questo mantiene il server in esecuzione per gestire le connessioni.

### **c. Main**

- **Funzione:**  
  - (1) Legge il file di configurazione (il *percorso* può essere passato *come argomento* o viene usato un *default*).
  - (2) Utilizza il `Parser` per trasformare il file di configurazione in una struttura `Config`.
  - [Debug] Stampa a console alcuni parametri di debug (numero di server, porte, host, ecc.) per verificare il corretto parsing.
  - (3) Crea un'istanza di `HTTPServer` passando la configurazione e chiama `run()` - avviando così il loop di eventi.

---

## **2. ToDo**

1. **Implementare il Parsing della Richiesta HTTP: [In Progress]**
   - Creare una classe o un modulo per leggere la richiesta HTTP ricevuta dal client.
   - Estrarre metodo (GET, POST, DELETE), URI, versioni e header.
   - Gestire la lettura in modo non bloccante e il completamento della richiesta (compresa la gestione di eventuali chunked body).

2. **Generazione della Risposta HTTP: [In Progress]**
   - Costruire risposte corrette in base al metodo, al percorso richiesto e alla configurazione.
   - Gestire le pagine di errore (404, 500, ecc.) e le risposte per metodi non consentiti.
   - Implementare il supporto per l’invio dei file statici (lettura del file dal disco, impostazione del content-type, ecc.).

3. **Implementare il Supporto per CGI (?): [In Progress]**
   - Se una location richiede l’esecuzione di uno script CGI (ad esempio per file con estensione `.php`), implementare il lancio del processo CGI tramite `fork()` e `execve()`.
   - Gestire la comunicazione (stdin/stdout) tra il server e il CGI, assicurandosi di rispettare le specifiche del protocollo CGI.

4. **Gestione Avanzata delle Connessioni: [In Progress]**
   - Implementare timeout per le richieste incomplete o in attesa.
   - Gestire correttamente il mantenimento delle connessioni (keep-alive) se desiderato.

5. **Logging e Gestione degli Errori: [In Progress]**
   - Logging più sofisticato per tracciare gli errori e il traffico.
   - Ogni errore (sia a livello di socket che a livello di parsing) dev'essere gestito senza terminare il *server*.

6. **[Debug]Testing e Stress Test: [In Progress]**
   - Creare script di test (¿anche in altri linguaggi?) per simulare richieste HTTP multiple e verificare il comportamento in condizioni di carico.
   - Testare il server con browser e strumenti come `curl` o `telnet` (e.s. `telnet 127.0.0.1 8080`) per assicurare un
   comportamento conforme alle specifiche HTTP. 

7. **Ottimizzazioni e Refactoring: [In Progress]**
   - Le chiamate di I/O devono avvenire tramite `poll()` (¿else?) per *rispettare il requisito non bloccante*.
