import sys
import matplotlib.pyplot as plt
import pandas as pd 

logfile = str(sys.argv[1])

times = []
resived = []
sent = []
with open(logfile, 'r') as fw:
    for line in fw.readlines():
        splited = line.split(" ")
        if len(splited) == 3:
            times.append(splited[0])
            resived.append(splited[1])
            sent.append(splited[2])
fw.close()

cpu_plot = pd.DataFrame({"resived" : resived, "sent" : sent, "time" : times})
cpu_plot['time'] = pd.to_datetime(cpu_plot['time'], format='%H:%M:%S')


plt.plot(cpu_plot["time"], cpu_plot["resived"], label="resived")
plt.plot(cpu_plot["time"], cpu_plot["sent"], label="sent")
plt.title("Network")
plt.xlabel("time")
plt.ylabel("bytes")
plt.legend()
plt.xticks(rotation=45)

plt.show(block=True)
