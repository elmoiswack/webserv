#!/usr/bin/env python3

import os
import json
import cgi

print("Content-Type: application/json")
print()

upload_dir = os.path.join(os.getcwd(), 'var/www/uploads')
files = os.listdir(upload_dir)
print(json.dumps(files))