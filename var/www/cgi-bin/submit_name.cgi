#!/usr/bin/env python3
import os
from urllib.parse import parse_qs

# print("Content-type: text/html\n")

# Parse the query string
query_string = os.environ.get('QUERY_STRING', '')
params = parse_qs(query_string)

# Get the values of first_name and last_name parameters
first_name = params.get('first_name', [''])[0]
last_name = params.get('last_name', [''])[0]

# Construct html page

html_content = (
	"<!DOCTYPE html>\n"
	"<html>\n"
	"<body>\n"
	f"<h1>Welcome {first_name} {last_name} !</h1>\n"
	"<p><a href='../index.html'>Go to Homepage</a></p>\n"
	"</body>\n"
	"</html>\n"
)

html_headers = (
	"HTTP/1.1 200 OK\r\n"
	"Content-Type: text/html\r\n"
	f"Content-Length: {len(html_content)}\r\n\r"
)

print(html_headers)
print(html_content)

# Print the extracted values

# print("<html>")
# print("<head><title>Extracted Names</title></head>")
# print("<body>")
# print("<h1>Extracted Names:</h1>")
# print(f"<p>First Name: {first_name}</p>")
# print(f"<p>Last Name: {last_name}</p>")
# print("</body>")
# print("</html>")





# import os

# print("Content-type: text/plain\n")

# for key, value in os.environ.items():
#     print(f"{key}: {value}")