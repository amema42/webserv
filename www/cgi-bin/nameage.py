#!/usr/bin/env python3
import cgi
import cgitb  # Per mostrare gli errori nel browser (utile per il debug)

cgitb.enable()

# Ottieni i dati del form (in questo caso, dalla query string)
form = cgi.FieldStorage()

# Recupera i valori dei parametri 'nome' e 'eta'
nome = form.getvalue('nome')
eta = form.getvalue('eta')

# Stampa l'header Content-Type per indicare che stiamo inviando HTML
print("Content-Type: text/html\n")

# Inizia a generare la pagina HTML
print("<!DOCTYPE html>")
print("<html lang=\"it\">")
print("<head>")
print("<meta charset=\"UTF-8\">")
print("<title>Risultato Query</title>")
print("</head>")
print("<body>")
print("<h1>Informazioni Ricevute dalla Query</h1>")

if nome and eta:
    print(f"<p>Nome: {nome}</p>")
    print(f"<p>Età: {eta}</p>")
elif nome:
    print(f"<p>Nome: {nome}</p>")
    print("<p>Età non specificata.</p>")
elif eta:
    print("<p>Nome non specificato.</p>")
    print(f"<p>Età: {eta}</p>")
else:
    print("<p>Nessun nome o età forniti nella query.</p>")

print("</body>")
print("</html>")