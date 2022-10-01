from fileinput import filename
import re
import sys

regex = ".+(sendto|recvfrom).+"
filename = str(sys.argv[1])
print(filename)

with open(filename, 'r') as fw:
    with open('sent_data', 'w+') as fpw:
        data = fw.readlines()
        for line in data:
            if bool(re.match(regex, line)):
                fpw.write(line)
        fpw.close()
fw.close()

