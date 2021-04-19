import matplotlib
import matplotlib.pyplot as plt
import numpy
import scipy
from scipy.fftpack import fftshift
from scipy.fft import fft, fftfreq
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
#for i in range(10000):
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
    if float(dataLine[0]) > 500:
        break
file.close()

fig = plt.figure(figsize=(6, 4))
ax = [
    fig.add_subplot(211),
    fig.add_subplot(212),
    #fig.add_subplot(423),
    #fig.add_subplot(424),
    #fig.add_subplot(425),
    #fig.add_subplot(426),
    #fig.add_subplot(427),
    #fig.add_subplot(428),
]

N = len(data["time"])
T = (data["time"][-1] - data["time"][0]) / len(data["time"])
xf = fftfreq(N, T)[:N//2]

y1 = numpy.asarray(data["axRight"])
yf1 = fft(y1)
ax[0].plot(xf, 2.0/N * numpy.abs(yf1[0:N//2]), alpha=1, color = 'r')

y2 = numpy.asarray(data["ayRight"])
yf2 = fft(y2)
ax[0].plot(xf, 2.0/N * numpy.abs(yf2[0:N//2]), alpha=1, color = 'g')

y3 = numpy.asarray(data["azRight"])
yf3 = fft(y3)
ax[0].plot(xf, 2.0/N * numpy.abs(yf3[0:N//2]), alpha=1, color = 'b')

ax[0].plot(xf, 2.0/N * numpy.abs(yf2[0:N//2]), alpha=0.5, color = 'g')

ax[0].plot(xf, 2.0/N * numpy.abs(yf1[0:N//2]), alpha=0.33, color = 'r')

ax[0].set_xlabel("Frequency (Hz)")
ax[0].set_ylabel("Magnitude")
ax[0].grid(linestyle="-")
ax[0].set_xscale('log')
ax[0].legend(("ax", "ay", "az"), loc="upper left")
ax[0].axis([None, None, -0.01, 0.08])

y1 = numpy.asarray(data["gxRight"])
yf1 = fft(y1)
ax[1].plot(xf, 2.0/N * numpy.abs(yf1[0:N//2]), alpha=1, color = 'r')

y2 = numpy.asarray(data["gyRight"])
yf2 = fft(y2)
ax[1].plot(xf, 2.0/N * numpy.abs(yf2[0:N//2]), alpha=1, color = 'g')

y3 = numpy.asarray(data["gzRight"])
yf3 = fft(y3)
ax[1].plot(xf, 2.0/N * numpy.abs(yf3[0:N//2]), alpha=1, color = 'b')

ax[1].plot(xf, 2.0/N * numpy.abs(yf2[0:N//2]), alpha=0.5, color = 'g')

ax[1].plot(xf, 2.0/N * numpy.abs(yf1[0:N//2]), alpha=0.33, color = 'r')

ax[1].set_xlabel("Frequency (Hz)")
ax[1].set_ylabel("Magnitude")
ax[1].grid(linestyle="-")
ax[1].set_xscale('log')
ax[1].legend(("gx", "gy", "gz"), loc="upper left")
ax[1].axis([None, None, -0.75, 6])

fig.tight_layout()
fig.savefig(fileName + ".png", dpi = 600)
plt.show()
