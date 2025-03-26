**Directory Structure Updates:**

- **Parser:** Located in `src/parser`.
- **Server:** Located in `src/net`.
- **Headers:**  
	All headers are in the `include` directory.  
	@mcamilli, I have modified your makefile so that headers can be included without specifying the full path.  
	**Action:** Please add all `.hpp` files to the `webServ.hpp` header, so that including just that file provides everything.

- **Configuration Files:** Stored in the `config` directory.
- **Connection Tests:** Located in the `dev_test` directory.
- **Web Content:**  
	The `www` directory will contain `index.html` and the corresponding location files.

- **cose da ricordare**
messa cgi funziona aggiungere controlli per la corretta gestione del path e dei tipi di file

--chiedere a matteo per la corretta configurazione di un file di config con le location messe per bene

gestire le richieste post delete e get

get: started working
post: not started
delete: not started

da fare delle pagine html piacevoli e farsi generare degli script in python