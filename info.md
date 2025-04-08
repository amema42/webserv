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

CGI: finished need testing
get: finished need testing
post: on working 
delete: not started

GESTIONE DEL TIMEOUT
qualche altro script in python?? per la CGI