#!/usr/bin/env python3
import os

print("Content-Type: text/html\n")
print("<html>")
print("<head><title>CGI Test</title></head>")
print("<body>")
print("<h1>Hello World!</h1>")
print("<p>")
print(os.environ.get('GATEWAY_INTERFACE', 'GATEWAY_INTERFACE not set'))
print(os.environ.get('UPLOAD_DIR', 'UPLOAD_DIR not set'))
print("</p>")
print("</body>")
print("</html>")
