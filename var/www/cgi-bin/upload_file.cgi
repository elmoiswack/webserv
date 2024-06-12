#!/usr/bin/python3

import cgi
import os

upload_dir = "/var/www/uploads"
print(cgi.FieldStorage())

def save_and_respond(filename):
    """Saves uploaded file and returns HTML response."""
    # Get uploaded file data
    form = cgi.FieldStorage()
    file_data = form["file"]

    if file_data:
        # Define upload directory
        upload_dir = "/var/www/upload"

        # Construct file path
        filename = os.path.join(upload_dir, "uploaded_file.txt")  # Or use a more descriptive filename

        # (Optional) Create upload directory if it doesn't exist
        if not os.path.exists(upload_dir):
            os.makedirs(upload_dir)

        # Save the file
        with open(filename, "wb") as f:
            f.write(file_data)
        message = "File uploaded successfully!"
    else:
        message = "No file uploaded."

    html= (
    	"<html>\n"
    	"<body>\n"
        	f"<h1>TEST{message}</h1>\n"
    	"</body>\n"
    	"</html>\n"
	)
    return (html)  # Encode for proper output


# Main execution (assuming filename is passed as environment variable)
filename = os.environ.get("UPLOAD_FILENAME")
if not filename:
    print("Error: UPLOAD_FILENAME environment variable not set!")
    exit(1)

# Generate basic HTTP headers (replace or extend as needed)
html_headers = (
	"HTTP/1.1 200 OK\r\n"
	"Content-Type: text/html\r\n\r"
)

print(html_headers)
response = save_and_respond(filename)
print(response)
