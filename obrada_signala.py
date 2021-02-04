# -*- coding: utf-8 -*-
# import numpy as np
# import scipy
# from scipy import signal #for detrend purposes
from matplotlib import pyplot as plt
import math

# def moving_avg(x,n):
#     mv =  np.convolve(x,np.ones(n)/n,mode='valid')
#     return np.concatenate(([np.NaN for k in range(n-1)],mv))

# def get_rid_of_divergence(x, number_of_windows):
# 	# get rid of signal divergece
# 	length_of_window = len(x) / number_of_windows
# 	breakpoints = []
# 	for i in range(1, number_of_windows + 1):
# 		breakpoints.append(math.floor(length_of_window * i))
# 	altitudes_detrended = scipy.signal.detrend(x, type='linear', bp = breakpoints)
# 	return altitudes_detrended

data = open("/media/istep/SD/data.txt", "r")
bmp388_altitude = []
dps368_altitude = []
smpb_altitude = []
for line in data.readlines():
    if line.find(';') == -1:
        continue
    bmp388_altitude.append(float(line.split(";")[0]))
    dps368_altitude.append(float(line.split(";")[1]))
    smpb_altitude.append(float(line.split(";")[2]))

timestamps = []
timestamps[:] = [x / 2.2 for x in range(len(bmp388_altitude))]
time_unit = "s"

if max(timestamps) > 14400:
    timestamps[:] = [x / (3600*2.2) for x in range(len(bmp388_altitude))]
    time_unit = "h"
elif max(timestamps) > 300:
    timestamps[:] = [x / (60*2.2) for x in range(len(bmp388_altitude))]
    time_unit = "min"

plt.plot(timestamps, bmp388_altitude, 'r-', label='BMP388')
plt.plot(timestamps, dps368_altitude, 'g-', label='DPS368')
plt.plot(timestamps, smpb_altitude, 'b-', label='2SMPB')
plt.legend(loc='upper center', bbox_to_anchor=(0.5, 1.1),
           ncol=3, fancybox=True, shadow=True)
plt.ylabel('Nadmorska visina, [m]')
plt.xlabel('Vrijeme, [' + time_unit + ']')
plt.show()


# #filter out the file
# list_of_lines = in_file.read().split("\n")
# filtered_list = []
# for item in list_of_lines:
#     if '" received' in item:
#         filtered_list.append(item)
# in_file.close()

# #reformat the file
# raw_out_file = open("raw_output.txt", "w")
# for item in filtered_list:
#     raw_out_file.write(item.split("\t")[1] + ';')
#     raw_out_file.write(item.split("\"")[1] + '\n')
# raw_out_file.close()

# #process sensor data
# C1 = 46372
# C2 = 43981
# C3 = 29059
# C4 = 27842
# C5 = 31553
# C6 = 28165
# CONST_PF = 0.1902630958
# deltaT = []
# OFF = []
# SENS = []
# pressures = []
# altitudes = []
# raw_temperatures = []
# temperatures = []
# raw_in_file = open("raw_output.txt", "r")
# out_file = open("output.txt", "w")
# temperature = 25
# dT = 1
# while line := raw_in_file.readline():
# 	timestamp = line.split(";")[0]
# 	raw_pressure = int(line.split(";")[1].split(".")[0])
# 	raw_temperature = int(line.split(";")[1].split(".")[1].rstrip()) - 674856.30
# 	dT = raw_temperature - (C5 * 2**8)
# 	temperature = (2000 + (dT * C6) / 2**23) / 100
# 	off = C2 * 2**17 + (C4 * dT)/2**6
# 	sens = C1 * 2**16 + (C3 * dT)/2**7
# 	pressure = abs(((raw_pressure * sens)/2**21 - off)/2**15) / 100
# 	altitude = (((1013.25/pressure)**(1/5.257) - 1)*(temperature + 273.15))/0.0065
# 	altitudes.append(altitude)
# 	temperatures.append(pressure)
# 	out_file.write(timestamp + ';')
# 	out_file.write(str(pressure) + ';')
# 	out_file.write(str(altitude) + ';')
# 	out_file.write(str(temperature) + '\n')

# out_file.close()
# raw_in_file.close()

# # turn absolute altitude values into relative-to-first-sample values
# first = altitudes[1]
# relative_altitudes = []
# for item in altitudes[1:]:
# 	relative_altitudes.append(item - first)
# normalized_temperature = []
# for item in temperatures:
# 	normalized_temperature.append(item - 1050)
# # get rid of signal divergece
# # relative_altitudes = get_rid_of_divergence(relative_altitudes, 5)
# # use rolling mean to apply low pass filter to signal
# final_values = moving_avg(relative_altitudes, 30)
# normalized_temperature = moving_avg(normalized_temperature, 3000)
# plt.plot(np.arange(0,len(final_values))/10, final_values[0:59716], 'r-', label='altitude')
# plt.plot(np.arange(0,len(final_values))/10, normalized_temperature[0:59716], 'b-', label='normalized pressure')
# plt.legend()
# plt.ylabel('Nadmorska visina, [m]')
# plt.xlabel('Vrijeme, [s]')
# plt.show()
