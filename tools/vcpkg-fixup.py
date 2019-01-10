import os
import re
import shutil
import glob

def fixup_file(filename, newpath, overwrite):
    pattern = r"C:[^;]*vcpkg"

    regex = re.compile(pattern)

    with open(filename, encoding='utf8') as file:
        out = [regex.sub(newpath, line) for line in file]

    with open('temp', 'w') as outfile:
        outfile.writelines(out)

    if overwrite:
        shutil.move('temp', filename)


if __name__ == '__main__':
    path = os.getcwd()
    for file in glob.iglob("./installed/x64-windows/share/**/*.cmake", recursive=True):
        print("Processing: %s" % file)
        fixup_file(file, path, True)

