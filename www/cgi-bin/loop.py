#!/usr/bin/env python3
import time

print("Content-Type: text/plain\n")  # Intestazione CGI corretta per un output di testo
print("Inizio script CGI. Entrato in loop infinito...\n", flush=True)

while True:
    print("Test output CGI - Timestamp:", time.time(), flush=True)
    time.sleep(1)  # Aspetta un secondo per evitare output eccessivamente veloce