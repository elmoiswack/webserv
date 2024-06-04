#!/usr/bin/env python3
import os
from urllib.parse import parse_qs

print("Content-type: text/html\n")

# Parse the query string
query_string = os.environ.get('QUERY_STRING', '')
params = parse_qs(query_string)

# Get the values of first_name and last_name parameters
first_name = params.get('first_name', [''])[0]
last_name = params.get('last_name', [''])[0]

# Print the extracted values

print("<html>")
print("<head><title>Extracted Names</title></head>")
print("<body>")
print("<h1>Extracted Names:</h1>")
print(f"<p>First Name: {first_name}</p>")
print(f"<p>Last Name: {last_name}</p>")
print("</body>")
print("</html>")


# import os

# print("Content-type: text/plain\n")

# for key, value in os.environ.items():
#     print(f"{key}: {value}")