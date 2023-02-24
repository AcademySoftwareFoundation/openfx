import os
import re

oldcopyright = rb"""/\*
[^/]*?Copyright.*?
\s*\*/
"""

print(oldcopyright[0:20])

newcopyright = b"""// Copyright OpenFX and contributors to the OpenFX project.
// SPDX-License-Identifier: BSD-3-Clause
"""

copyright_re=re.compile(oldcopyright, re.MULTILINE | re.DOTALL)

exclude= set(['.git', '.cache', 'Build', 'HostSupport/expat-2.0.1'])
for root, dirs, files in os.walk("."):
    dirs[:] = [d for d in dirs if d not in exclude]
    for file in files:
        file_path = os.path.join(root, file)
        with open(file_path, 'rb') as f:
            try:
                data = f.read()
            except UnicodeDecodeError as e:
                print("Skipping ", file_path)
                data = ''
        if data and re.search(copyright_re, data):
            print("Found copyright in ", file_path)
            data = re.sub(copyright_re, newcopyright, data)
            with open(file_path, 'wb') as outf:
                outf.write(data)
