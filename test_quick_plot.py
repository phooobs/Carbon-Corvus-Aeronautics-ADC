import matplotlib
import matplotlib.pyplot as plt
import numpy
fileName = input("Please enter file name ")
file = open(str(fileName + ".csv"), 'r')

file.readline()

data = {
    "time": [], "pressure": [], "temperature": [],
    "ax0" : [], "ay0" : [], "az0" : [], "gx0" : [], "gy0" : [], "gz0" : [], # body
    "ax1" : [], "ay1" : [], "az1" : [], "gx1" : [], "gy1" : [], "gz1" : [], # wing tip
    "ax2" : [], "ay2" : [], "az2" : [], "gx2" : [], "gy2" : [], "gz2" : []  # wing tip
}

dataLine = file.readline().split(",")
while True:
    try:
        data["time"].append(float(dataLine[0]))
        data["pressure"].append(float(dataLine[2]))
        data["temperature"].append(float(dataLine[3]))
        data["ax0"].append(float(dataLine[4]))
        data["ay0"].append(float(dataLine[5]))
        data["az0"].append(float(dataLine[6]))
        data["gx0"].append(float(dataLine[7]))
        data["gy0"].append(float(dataLine[8]))
        data["gz0"].append(float(dataLine[9]))
        data["ax1"].append(float(dataLine[10]))
        data["ay1"].append(float(dataLine[11]))
        data["az1"].append(float(dataLine[12]))
        data["gx1"].append(float(dataLine[13]))
        data["gy1"].append(float(dataLine[14]))
        data["gz1"].append(float(dataLine[15]))
        data["ax2"].append(float(dataLine[16]))
        data["ay2"].append(float(dataLine[17]))
        data["az2"].append(float(dataLine[18]))
        data["gx2"].append(float(dataLine[19]))
        data["gy2"].append(float(dataLine[20]))
        data["gz2"].append(float(dataLine[21]))
        dataLine = file.readline().split(",")
    except:
        data["time"].pop()
        data["pressure"].pop()
        data["temperature"].pop()
        data["ax0"].pop()
        data["ay0"].pop()
        data["az0"].pop()
        data["gx0"].pop()
        data["gy0"].pop()
        data["gz0"].pop()
        data["ax1"].pop()
        data["ay1"].pop()
        data["az1"].pop()
        data["gx1"].pop()
        data["gy1"].pop()
        data["gz1"].pop()
        data["ax2"].pop()
        data["ay2"].pop()
        data["az2"].pop()
        data["gx2"].pop()
        data["gy2"].pop()
        data["gz2"].pop()
        break
file.close()

fig = plt.figure(figsize=(20, 10))
ax = [
    fig.add_subplot(421),
    fig.add_subplot(422),
    fig.add_subplot(423),
    fig.add_subplot(424),
    fig.add_subplot(425),
    fig.add_subplot(426),
    fig.add_subplot(427),
    fig.add_subplot(428),
]

ax[0].plot(data["time"], data["pressure"])
ax[0].set_ylabel("Pressure (Psi)")
ax[0].grid(linestyle="-")
ax[0].axis([None, None, -0.5, 3.5])

ax[1].plot(data["time"], data["temperature"])
ax[1].set_ylabel("Temperature (\N{DEGREE SIGN}C)")
ax[1].grid(linestyle="-")
ax[1].axis([None, None, 10, 40])

ax[2].plot(data["time"], data["ax0"])
ax[2].plot(data["time"], data["ax1"])
ax[2].plot(data["time"], data["ax2"])
ax[2].set_ylabel("AX (g)")
ax[2].legend(("Body", "Left", "Right"), loc="lower left")
ax[2].grid(linestyle="-")
ax[2].axis([None, None, -4, 4])

ax[3].plot(data["time"], data["gx0"])
ax[3].plot(data["time"], data["gx1"])
ax[3].plot(data["time"], data["gx2"])
ax[3].set_ylabel("GX (\N{DEGREE SIGN}/s)")
ax[3].legend(("Body", "Left", "Right"), loc="lower left")
ax[3].grid(linestyle="-")
ax[3].axis([None, None, -500, 500])

ax[4].plot(data["time"], data["ay0"])
ax[4].plot(data["time"], data["ay1"])
ax[4].plot(data["time"], data["ay2"])
ax[4].set_ylabel("AY (g)")
ax[4].legend(("Body", "Left", "Right"), loc="lower left")
ax[4].grid(linestyle="-")
ax[4].axis([None, None, -4, 4])

ax[5].plot(data["time"], data["gy0"])
ax[5].plot(data["time"], data["gy1"])
ax[5].plot(data["time"], data["gy2"])
ax[5].set_ylabel("GY (\N{DEGREE SIGN}/s)")
ax[5].legend(("Body", "Left", "Right"), loc="lower left")
ax[5].grid(linestyle="-")
ax[5].axis([None, None, -500, 500])

ax[6].plot(data["time"], data["az0"])
ax[6].plot(data["time"], data["az1"])
ax[6].plot(data["time"], data["az2"])
ax[6].set_ylabel("AZ (g)")
ax[6].set_xlabel("Time (s)")
ax[6].legend(("Body", "Left", "Right"), loc="lower left")
ax[6].grid(linestyle="-")
ax[6].axis([None, None, -4, 4])

ax[7].plot(data["time"], data["gz0"])
ax[7].plot(data["time"], data["gz1"])
ax[7].plot(data["time"], data["gz2"])
ax[7].set_ylabel("GZ (\N{DEGREE SIGN}/s)")
ax[7].set_xlabel("Time (s)")
ax[7].legend(("Body", "Left", "Right"), loc="lower left")
ax[7].grid(linestyle="-")
ax[7].axis([None, None, -500, 500])

#fig.tight_layout()
fig.savefig(fileName + ".png", dpi = 600)
#plt.show()
