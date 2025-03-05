#!/usr/bin/python3

import cgi
import sys

# Ensure headers are sent before any content
print("Content-Type: text/html")
print()  # Required blank line after headers

# Read the POST data
form = cgi.FieldStorage()

# Extract a field named "message"
message = form.getvalue("message", "No message received")

# Output response
print(f"""
<html>
<head><title>CGI POST Response</title></head>
<body>
    <h1>Received Data</h1>
    <p><b>Message:</b> {message}</p>
</body>
</html>
""")