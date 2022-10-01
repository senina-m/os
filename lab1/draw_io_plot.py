import sys
import matplotlib.pyplot as plt
import pandas as pd 

logfile = str(sys.argv[1])

times = []
reads = []
writes = []
with open(logfile, 'r') as fw:
    for line in fw.readlines():
        splited = line.split()
        if len(splited) >= 9:
            if splited[9] == "./277087":
                times.append(splited[0])
                reads.append(splited[4])
                writes.append(splited[6])
fw.close()

cpu_plot = pd.DataFrame({"read" : reads, "write" : writes, "time" : times})
print(cpu_plot)
cpu_plot['time'] = pd.to_datetime(cpu_plot['time'], format='%H:%M:%S')


plt.plot(cpu_plot["time"], cpu_plot["read"], label="read")
plt.plot(cpu_plot["time"], cpu_plot["write"], label="write")
plt.title("IO")
plt.xlabel("time")
plt.ylabel("Kb")
plt.legend()
plt.xticks(rotation=45)

plt.show(block=True)
