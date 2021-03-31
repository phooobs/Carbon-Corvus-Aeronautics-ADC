import matplotlib
import matplotlib.pyplot as plt
import numpy
fileName = input("Please enter file name ")
file = open(str(fileName + ".csv"), 'r')

file.readline()

data = {
    "time": [], "pressure": [], "temperature": [],
    "axBody" : [], "ayBody" : [], "azBody" : [], "gxBody" : [], "gyBody" : [], "gzBody" : [], # body
    "axLeft" : [], "ayLeft" : [], "azLeft" : [], "gxLeft" : [], "gyLeft" : [], "gzLeft" : [], # wing tip
    "axRight" : [], "ayRight" : [], "azRight" : [], "gxRight" : [], "gyRight" : [], "gzRight" : []  # wing tip
}

dataLine = file.readline().split(",")
while True:
    try:
        data["time"].append(float(dataLine[0]))
        data["pressure"].append(float(dataLine[2]))
        data["temperature"].append(float(dataLine[3]))
        data["axBody"].append(float(dataLine[4]))
        data["ayBody"].append(float(dataLine[5]))
        data["azBody"].append(float(dataLine[6]))
        data["gxBody"].append(float(dataLine[7]))
        data["gyBody"].append(float(dataLine[8]))
        data["gzBody"].append(float(dataLine[9]))
        data["axLeft"].append(float(dataLine[10]))
        data["ayLeft"].append(float(dataLine[11]))
        data["azLeft"].append(float(dataLine[12]))
        data["gxLeft"].append(float(dataLine[13]))
        data["gyLeft"].append(float(dataLine[14]))
        data["gzLeft"].append(float(dataLine[15]))
        data["axRight"].append(float(dataLine[16]))
        data["ayRight"].append(float(dataLine[17]))
        data["azRight"].append(float(dataLine[18]))
        data["gxRight"].append(float(dataLine[19]))
        data["gyRight"].append(float(dataLine[20]))
        data["gzRight"].append(float(dataLine[21]))
        dataLine = file.readline().split(",")
    except:
        data["time"].pop()
        data["pressure"].pop()
        data["temperature"].pop()
        data["axBody"].pop()
        data["ayBody"].pop()
        data["azBody"].pop()
        data["gxBody"].pop()
        data["gyBody"].pop()
        data["gzBody"].pop()
        data["axLeft"].pop()
        data["ayLeft"].pop()
        data["azLeft"].pop()
        data["gxLeft"].pop()
        data["gyLeft"].pop()
        data["gzLeft"].pop()
        data["axRight"].pop()
        data["ayRight"].pop()
        data["azRight"].pop()
        data["gxRight"].pop()
        data["gyRight"].pop()
        data["gzRight"].pop()
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

ax[2].plot(data["time"], data["axBody"])
ax[2].plot(data["time"], data["axLeft"])
ax[2].plot(data["time"], data["axRight"])
ax[2].set_ylabel("AX (g)")
ax[2].legend(("Body", "Left", "Right"), loc="lower left")
ax[2].grid(linestyle="-")
ax[2].axis([None, None, -4, 4])

ax[3].plot(data["time"], data["gxBody"])
ax[3].plot(data["time"], data["gxLeft"])
ax[3].plot(data["time"], data["gxRight"])
ax[3].set_ylabel("GX (\N{DEGREE SIGN}/s)")
ax[3].legend(("Body", "Left", "Right"), loc="lower left")
ax[3].grid(linestyle="-")
ax[3].axis([None, None, -500, 500])

ax[4].plot(data["time"], data["ayBody"])
ax[4].plot(data["time"], data["ayLeft"])
ax[4].plot(data["time"], data["ayRight"])
ax[4].set_ylabel("AY (g)")
ax[4].legend(("Body", "Left", "Right"), loc="lower left")
ax[4].grid(linestyle="-")
ax[4].axis([None, None, -4, 4])

ax[5].plot(data["time"], data["gyBody"])
ax[5].plot(data["time"], data["gyLeft"])
ax[5].plot(data["time"], data["gyRight"])
ax[5].set_ylabel("GY (\N{DEGREE SIGN}/s)")
ax[5].legend(("Body", "Left", "Right"), loc="lower left")
ax[5].grid(linestyle="-")
ax[5].axis([None, None, -500, 500])

ax[6].plot(data["time"], data["azBody"])
ax[6].plot(data["time"], data["azLeft"])
ax[6].plot(data["time"], data["azRight"])
ax[6].set_ylabel("AZ (g)")
ax[6].set_xlabel("Time (s)")
ax[6].legend(("Body", "Left", "Right"), loc="lower left")
ax[6].grid(linestyle="-")
ax[6].axis([None, None, -4, 4])

ax[7].plot(data["time"], data["gzBody"])
ax[7].plot(data["time"], data["gzLeft"])
ax[7].plot(data["time"], data["gzRight"])
ax[7].set_ylabel("GZ (\N{DEGREE SIGN}/s)")
ax[7].set_xlabel("Time (s)")
ax[7].legend(("Body", "Left", "Right"), loc="lower left")
ax[7].grid(linestyle="-")
ax[7].axis([None, None, -500, 500])

#fig.tight_layout()
fig.savefig(fileName + ".png", dpi = 600)
#plt.show()
