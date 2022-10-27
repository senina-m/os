import sys
import matplotlib.pyplot as plt
import pandas as pd 

logfile = str(sys.argv[1])

times = []
amount = []
with open(logfile, 'r') as fw:
    for line in fw.readlines():
        splited = line.split(" ")
        if len(splited) == 2:
            times.append(splited[0])
            amount.append(splited[1])
            
fw.close()

threads_plot = pd.DataFrame({"amount" : amount, "time" : times})
threads_plot['time'] = pd.to_datetime(threads_plot['time'], format='%H:%M:%S')


plt.plot(threads_plot["time"], threads_plot["amount"], label="amount")
plt.title("Threads state changes")
plt.xlabel("time")
plt.ylabel("num of threads")
plt.legend()
plt.xticks(rotation=45)

plt.show(block=True)
