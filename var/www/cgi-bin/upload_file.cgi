# #!/usr/bin/env python3

# import os
# import sys

# # Get the content type header
# content_type = os.getenv("CONTENT_TYPE")
# if not content_type or "multipart/form-data" not in content_type:
#     print("Content-Type: text/plain")
#     print()
#     print("Error: This script expects multipart/form-data!")
#     sys.exit(1)

# # Get the boundary string
# boundary = content_type.split("boundary=")[1]

# # Read the request body from stdin
# content_length = int(os.getenv("CONTENT_LENGTH"))
# request_body = sys.stdin.buffer.read(content_length).decode("utf-8")

# # Find the part containing the file data
# file_start = request_body.find("--" + boundary + "\r\n")
# file_end = request_body.find("--" + boundary + "--", file_start)
# file_data = request_body[file_start:file_end]

# # Find filename
# filename_start = file_data.find("filename=\"") + len("filename=\"")
# filename_end = file_data.find("\"", filename_start)
# filename = file_data[filename_start:filename_end]

# # Find file content
# content_start = file_data.find("\r\n\r\n") + len("\r\n\r\n")
# file_content = file_data[content_start:]

# # Save file content to a file
# with open(filename, "wb") as f:
#     f.write(file_content.encode("utf-8"))

# print("Content-Type: text/plain")
# print()
# print("File uploaded successfully!")
