import sys
import matplotlib.pyplot as plt
import pandas as pd 

logfile = str(sys.argv[1])

times = []
values = []
with open(logfile, 'r') as fw:
    for line in fw.readlines():
        splited = line.split(" ")
        value = (splited[8])[:-2]
        cur_time = splited[4]
        # print(f"{value} = {cur_time}")
        try:
            values.append(float(value))
            times.append(cur_time)
        except:
            pass
fw.close()

cpu_plot = pd.DataFrame({"value" : values, "time" : times})
cpu_plot['time'] = pd.to_datetime(cpu_plot['time'], format='%H:%M:%S')


plt.plot(cpu_plot["time"], cpu_plot["value"], label="cpu")
plt.title("CPU")
plt.xlabel("time")
plt.ylabel("% of cpu use")
plt.legend()
plt.xticks(rotation=45)

plt.show(block=True)
