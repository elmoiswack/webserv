#!/usr/bin/env python3

import cgi
import os
from os import environ
# import cgitb; cgitb.enable()
import sys

message = ""
def handle_post():
    form = cgi.FieldStorage()
    file_item = form['file']
    if file_item.filename:
        # Securely get the filename
        file_name = os.path.basename(file_item.filename)
        upload_dir = os.path.join(os.getcwd(), 'var/www/uploads')
        # Full path to save the file
        file_path = os.path.join(upload_dir, file_name)
        # Check if the upload directory exists, if not, create it
        # print("\n\nFILE_NAME: " + Ofile_name)
        # print("\n\nFILE_PATH: " + file_path)
        # print("\n\nUPLOAD_DIR: " + upload_dir)
        if os.path.exists(file_path):
            message = "File with the same name already exists, upload cancelled!"
        else:
            if not os.path.exists(upload_dir):
                os.makedirs(upload_dir)
            # Write the file to the specified directory
            with open(file_path, 'wb') as fout:
                while True:
                    chunk = file_item.file.read(100000)
                    if not chunk:
                        break
                    fout.write(chunk)
            message = f"Uploaded {file_name}"
    else:
        message = "No file was uploaded!"

    html_content = (
	    "<!DOCTYPE html>\n"
	    "<html>\n"
	    "<body>\n"
	    f"<h1>{message}!</h1>\n"
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

def main():
	handle_post()

if __name__ == "__main__":
    main()


# def save_uploaded_file(fileitem):
#     if fileitem.filename:
#         # Securely get the filename
#         fn = os.path.basename(fileitem.filename)
#         # Ensure the upload directory exists
#         os.makedirs(UPLOAD_DIR, exist_ok=True)
#         # Full path to save the file
#         file_path = os.path.join(UPLOAD_DIR, fn)
        
#         # Write the file to the specified directory
#         with open(file_path, 'wb') as fout:
#             while True:
#                 chunk = fileitem.file.read(100000)
#                 if not chunk:
#                     break
#                 fout.write(chunk)
#         return file_path
#     return None


# import cgi
# import os

# # upload_dir = "/var/www/uploads"
# # //print(cgi.FieldStorage())

# def save_and_respond(filename):
#     """Saves uploaded file and returns HTML response."""
#     # Get uploaded file data
#     form = cgi.FieldStorage()
#     file_data = form["file"]

#     if file_data:
#         # Define upload directory
#         upload_dir = "/var/www/upload"

#         # Construct file path
#         filename = os.path.join(upload_dir, "uploaded_file.txt")  # Or use a more descriptive filename

#         # (Optional) Create upload directory if it doesn't exist
#         if not os.path.exists(upload_dir):
#             os.makedirs(upload_dir)

#         # Save the file
#         with open(filename, "wb") as f:
#             f.write(file_data)
#         message = "File uploaded successfully!"
#     else:
#         message = "No file uploaded."

#     html= (
#     	"<html>\n"
#     	"<body>\n"
#         	f"<h1>TEST{message}</h1>\n"
#     	"</body>\n"
#     	"</html>\n"
# 	)
#     return (html)  # Encode for proper output


# # Main execution (assuming filename is passed as environment variable)
# filename = os.environ.get("UPLOAD_FILENAME")
# if not filename:
#     print("Error: UPLOAD_FILENAME environment variable not set!")
#     exit(1)

# # Generate basic HTTP headers (replace or extend as needed)
# html_headers = (
# 	"HTTP/1.1 200 OK\r\n"
# 	"Content-Type: text/html\r\n\r"
# )

# print(html_headers)
# response = save_and_respond(filename)
# print(response)