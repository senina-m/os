import os
from os.path import isfile, join
import re

localpath = os.getcwd()
pattern = "^log_trace"
filenames = [f for f in os.listdir(localpath) if isfile(join(localpath, f)) and re.match(pattern, f)]

with open("all_log", 'w+') as fw:
    with open('process_log', 'w+') as fpw:
        for filename in filenames:
            fpw.write(f"{filename.split('.')[1]} ")
            with open(join(localpath, filename), 'r') as fr:
                fw.write(fr.read())
            fr.close()
        fpw.close()
fw.close()

print(f"{len(filenames)} processes was created.")

all_log = open("all_log", 'r')
lines = all_log.read()
regex = r"</(.+?)>"
with open('file_log', 'w+') as fw:
    files = list(set(re.findall(regex, lines)))
    fw.write("\n".join([i for i in files]))
fw.close()

print(f"{len(files)} different files programm use.")