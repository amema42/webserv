import os
import sys

print("Content-Type: text/plain\n")  # Header HTTP obbligatorio
print("*** CGI Test Script ***")
print(f"Metodo: {os.environ.get('REQUEST_METHOD', '')}")
print(f"Query: {os.environ.get('QUERY_STRING', '')}")

# Leggi il body (per POST)
if os.environ.get('REQUEST_METHOD') == 'POST':
    content_length = int(os.environ.get('CONTENT_LENGTH', 0))
    body = sys.stdin.read(content_length)
    print(f"Body: {body}")