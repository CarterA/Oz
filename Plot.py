# -*- coding: utf8 -*-

import csv
import matplotlib
import matplotlib.pyplot as plt

# CSV file columns:
# 0 , 1  , 2  , 3  , 4  , 5  , 6  , 7  , 8  , 9  , 10   , 11   , 12    , 13 , 14 , 15
# ms,accX,accY,accZ,magX,magY,magZ,gyrX,gyrY,gyrZ,gyrTem,barTem,barPres,tem1,tem2,hum

f = csv.reader(open('Data/Altitude.csv','r'))
data = data = [line for line in f if len(line) == 16]
data = zip(*data)

# Convert ms to seconds for display purposes
data[0] = [float(t) / 60000 for t in data[0]]
time_label = 'Time (minutes)'

fig = plt.figure(facecolor='white')

# Temperature Graph
temPlot = fig.add_subplot(221)
temPlot.plot(data[0], data[10], 'b-', label='Gyroscope Temperature')
temPlot.plot(data[0], data[11], 'g-', label='Barometer Temperature')
temPlot.plot(data[0], data[13], 'r-', label='External Temperature')
temPlot.plot(data[0], data[14], 'c-', label='Internal Temperature')
temPlot.legend()
temPlot.set_title('Temperature')
temPlot.set_xlabel(time_label)
temPlot.set_ylabel(u'Temperature (‌‌\u00B0C)')

# Humidity Graph
humidities = [161.0 * (float(v)/1023.0 * 3.3)/3.3 - 25.8 for v in data[15]]
humPlot = fig.add_subplot(222)
humPlot.plot(data[0], humidities)
humPlot.set_title('Relative Humidity')
humPlot.set_xlabel(time_label)
humPlot.set_ylabel('Relative Humidity (%)')

# Pressure/Altitude Graph
pressurePlot = fig.add_subplot(223)
pressurePlotFormatter = matplotlib.ticker.ScalarFormatter(useOffset = False)
seaLevelPressure = 101320;
altitude = [(44330 * (1.0 - pow(float(a)/seaLevelPressure,0.1903)))/1000 for a in data[12]]
data[12] = [float(p) / 101325 for p in data[12]] # pascals -> atm
pressurePlot.plot(data[0], data[12], 'b-')
pressurePlot.set_xlabel(time_label)
pressurePlot.set_ylabel('Pressure (atmospheres)')
pressurePlot.yaxis.set_major_formatter(pressurePlotFormatter)
pressurePlot.set_title('Pressure & Altitude')
altitudePlot = pressurePlot.twinx()
#data[13] = [float(a) / 1000 for a in data[13]]
altitudePlot.plot(data[0], altitude, 'r-')
altitudePlot.set_ylabel('Altitude (kilometers)')
altitudePlot.yaxis.set_major_formatter(pressurePlotFormatter)
for label in pressurePlot.get_yticklabels(): label.set_color('b')
for label in altitudePlot.get_yticklabels(): label.set_color('r')

# Accelerometer Graph
accPlot = fig.add_subplot(224)
accPlot.plot(data[0], data[1], 'b-', label='X Axis')
accPlot.plot(data[0], data[2], 'g-', label='Y Axis')
accPlot.plot(data[0], data[3], 'r-', label='Z Axis')
accPlot.legend()
accPlot.set_title('Acceleration')
accPlot.set_xlabel(time_label)
accPlot.set_ylabel('Accleration (G forces)')

plt.show()