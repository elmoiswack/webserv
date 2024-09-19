#!/usr/bin/env python3

import os

def list_files(directory):
    if not os.path.exists(directory):
        return ["No files have been uploaded-."]
    try:
        files = os.listdir(directory)
        if not files:
            return ["No files have been uploaded."]
        return files
    except Exception as e:
        return [f"Error accessing directory: {e}"]

def generate_html(file_list):
    html_content = (
        "<!DOCTYPE html>\n"
        "<html lang='en'>\n"
        "<head>\n"
        "<meta charset='UTF-8'>\n"
        "<meta name='viewport' content='width=device-width, initial-scale=1.0'>\n"
        "<title>Uploaded Files</title>\n"
        "</head>\n"
        "<body>\n"
        "<h1>Uploaded Files</h1>\n"
        "<ul>\n"
    )
    for file in file_list:
        html_content += f'    <li>{file}</li>\n'
    html_content += (
        "</ul>\n"
        "<p><a href='../index.html'>Go to Homepage</a></p>\n"
        "</body>\n"
        "</html>\n"
    )
    return html_content

def main():
    directory = "var/www/uploads"
    file_list = list_files(directory)
    html_content = generate_html(file_list)
    
    html_headers = (
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        f"Content-Length: {len(html_content)}\r\n"
        "\r\n"
    )
    
    response = html_headers + html_content
    print(response)

if __name__ == "__main__":
    main()
