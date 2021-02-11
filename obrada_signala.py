# -*- coding: utf-8 -*-
# import numpy as np
# import scipy
# from scipy import signal #for detrend purposes
from matplotlib import pyplot as plt
import math
from datetime import datetime

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
bmp388_temperature = []
bmp388_pressure = []
dps368_altitude = []
dps368_temperature = []
dps368_pressure = []
smpb_altitude = []
smpb_temperature = []
smpb_pressure = []
for line in data.readlines():
    if line.find(';') == -1:
        continue
    bmp388_altitude.append(float(line.split(";")[0]))
    bmp388_temperature.append(float(line.split(";")[1]))
    bmp388_pressure.append(float(line.split(";")[2])/100)
    dps368_altitude.append(float(line.split(";")[3]))
    dps368_temperature.append(float(line.split(";")[4]))
    dps368_pressure.append(float(line.split(";")[5]))
    smpb_altitude.append(float(line.split(";")[6]))
    smpb_temperature.append(float(line.split(";")[7]))
    smpb_pressure.append(float(line.split(";")[8]))

timestamps = []
timestamps[:] = [x * 0.595163934 for x in range(len(bmp388_altitude))]
time_unit = "s"

tlakovi_24h = [[1612983721000,1002.0],[1612984021000,1002.0],[1612984321000,1002.0],[1612984622000,1001.8],[1612984921000,1001.8],[1612985221000,1001.7],[1612985521000,1001.7],[1612985821000,1001.7],[1612986121000,1001.8],[1612986421000,1001.9],[1612986721000,1002.0],[1612987021000,1001.9],[1612987322000,1001.9],[1612987621000,1002.0],[1612987921000,1001.8],[1612988221000,1001.7],[1612988522000,1001.9],[1612988821000,1001.6],[1612989121000,1001.6],[1612989421000,1001.6],[1612989721000,1001.2],[1612990021000,1001.5],[1612990321000,1001.5],[1612990621000,1001.5],[1612990922000,1001.4],[1612991221000,1001.7],[1612991521000,1001.7],[1612991821000,1001.6],[1612992121000,1001.8],[1612992421000,1002.1],[1612992721000,1002.2],[1612993021000,1002.3],[1612993321000,1002.3],[1612993622000,1002.5],[1612993922000,1002.6],[1612994222000,1002.5],[1612994521000,1002.6],[1612994822000,1002.5],[1612995121000,1002.6],[1612995422000,1002.9],[1612995721000,1002.9],[1612996021000,1003.0],[1612996321000,1003.2],[1612996621000,1003.2],[1612996922000,1003.4],[1612997221000,1003.6],[1612997521000,1003.7],[1612997821000,1003.9],[1612998122000,1004.2],[1612998421000,1004.3],[1612998721000,1004.4],[1612999021000,1004.4],[1612999321000,1004.5],[1612999621000,1004.6],[1612999922000,1004.6],[1613000221000,1004.8],[1613000521000,1004.8],[1613000821000,1005.2],[1613001122000,1005.3],[1613001421000,1005.4],[1613002021000,1005.5],[1613002321000,1005.7],[1613002621000,1005.9],[1613002921000,1005.9],[1613003221000,1006.0],[1613003521000,1006.1],[1613003822000,1006.2],[1613004121000,1006.5],[1613004421000,1006.5],[1613004721000,1006.6],[1613005021000,1006.8],[1613005321000,1006.9],[1613005622000,1006.8],[1613005921000,1006.9],[1613006221000,1007.0],[1613006521000,1007.0],[1613006821000,1007.1],[1613007121000,1007.0],[1613007422000,1007.1],[1613007721000,1007.3],[1613008021000,1007.4],[1613008321000,1007.2],[1613008621000,1007.4],[1613008921000,1007.5],[1613009223000,1007.4],[1613009521000,1007.5],[1613009822000,1007.5],[1613010122000,1007.6],[1613010421000,1007.7],[1613010722000,1007.8],[1613011022000,1007.9],[1613011321000,1008.0],[1613011621000,1008.1],[1613011921000,1008.1],[1613012221000,1008.1],[1613012527000,1008.2],[1613012821000,1008.2],[1613013122000,1008.4],[1613013421000,1008.4],[1613013721000,1008.5],[1613014021000,1008.5],[1613014321000,1008.6],[1613014621000,1008.8],[1613014922000,1008.9],[1613015221000,1009.1],[1613015521000,1009.2],[1613015821000,1009.2],[1613016121000,1009.4],[1613016421000,1009.6],[1613016721000,1009.7],[1613017021000,1009.9],[1613017321000,1010.0],[1613017621000,1010.0],[1613017922000,1010.2],[1613018221000,1010.3],[1613018521000,1010.4],[1613018821000,1010.5],[1613019121000,1010.7],[1613019422000,1010.8],[1613019722000,1011.0],[1613020021000,1011.3],[1613020321000,1011.5],[1613020621000,1011.7],[1613020921000,1011.7],[1613021222000,1011.8],[1613021522000,1011.8],[1613021821000,1011.9],[1613022121000,1011.9],[1613022422000,1012.2],[1613022722000,1012.4],[1613023021000,1012.7],[1613023321000,1012.7],[1613023621000,1012.7],[1613023921000,1012.7],[1613024221000,1012.9],[1613024521000,1012.9],[1613024821000,1012.9],[1613025121000,1013.0],[1613025421000,1013.1],[1613025721000,1013.2],[1613026021000,1013.2],[1613026321000,1013.5],[1613026622000,1013.5],[1613026922000,1013.7],[1613027221000,1013.9],[1613027521000,1013.9],[1613027822000,1014.1],[1613028121000,1014.2],[1613028421000,1014.4],[1613028722000,1014.7],[1613029021000,1014.6],[1613029321000,1014.8],[1613029621000,1014.8],[1613029921000,1014.9],[1613030221000,1015.2],[1613030522000,1015.3],[1613030822000,1015.5],[1613031122000,1015.5],[1613031421000,1015.7],[1613031722000,1015.8],[1613032021000,1016.0],[1613032321000,1016.1],[1613032621000,1016.2],[1613032921000,1016.1],[1613033222000,1016.3],[1613033521000,1016.4],[1613033821000,1016.4],[1613034122000,1016.5],[1613034421000,1016.7],[1613034722000,1016.9],[1613035021000,1017.0],[1613035322000,1017.2],[1613035621000,1017.2],[1613035922000,1017.3],[1613036222000,1017.5],[1613036522000,1017.7],[1613036821000,1017.6],[1613037121000,1017.8],[1613037421000,1018.0],[1613037722000,1017.9],[1613038021000,1018.1],[1613038321000,1018.2],[1613038621000,1018.2],[1613038921000,1018.3],[1613039222000,1018.4],[1613039521000,1018.6],[1613039822000,1018.5],[1613040122000,1018.7],[1613040421000,1018.7],[1613040721000,1018.8],[1613041021000,1019.1],[1613041322000,1019.2],[1613041622000,1019.4],[1613041921000,1019.4],[1613042221000,1019.4],[1613042522000,1019.4],[1613042821000,1019.4],[1613043121000,1019.5],[1613043421000,1019.5],[1613043722000,1019.5],[1613044021000,1019.6],[1613044322000,1019.8],[1613044621000,1019.7],[1613044922000,1019.8],[1613045221000,1019.8],[1613045521000,1020.0],[1613045821000,1020.0],[1613046121000,1020.0],[1613046421000,1020.1],[1613046721000,1020.2],[1613047021000,1020.1],[1613047321000,1020.3],[1613047621000,1020.4],[1613047922000,1020.4],[1613048221000,1020.5],[1613048521000,1020.4],[1613048821000,1020.6],[1613049122000,1020.6],[1613049422000,1020.7],[1613049721000,1020.7],[1613050021000,1020.7],[1613050322000,1020.7],[1613050621000,1020.8],[1613050921000,1020.8],[1613051222000,1020.9],[1613051521000,1021.0],[1613051821000,1021.0],[1613052121000,1021.0],[1613052421000,1021.1],[1613052721000,1021.4],[1613053022000,1021.3],[1613053322000,1021.5],[1613053621000,1021.5],[1613053922000,1021.5],[1613054221000,1021.6],[1613054521000,1021.6],[1613054821000,1021.6],[1613055121000,1021.7],[1613055421000,1021.9],[1613055722000,1021.9],[1613056021000,1022.0],[1613056322000,1022.0],[1613056622000,1022.0],[1613056922000,1022.0],[1613057221000,1022.1],[1613057521000,1022.2],[1613057821000,1022.2],[1613058121000,1022.5],[1613058421000,1022.6],[1613058721000,1022.5],[1613059021000,1022.6],[1613059322000,1022.6],[1613059622000,1022.6],[1613059921000,1022.8],[1613060221000,1023.0],[1613060521000,1023.0],[1613060821000,1023.1],[1613061121000,1023.1],[1613061422000,1023.3],[1613061721000,1023.4],[1613062021000,1023.5],[1613062322000,1023.6],[1613062621000,1023.8],[1613062922000,1023.9],[1613063221000,1023.9],[1613063521000,1024.0],[1613063821000,1024.1],[1613064121000,1024.1],[1613064421000,1024.3],[1613064721000,1024.4],[1613065021000,1024.5],[1613065321000,1024.7],[1613065621000,1024.7],[1613065921000,1024.9],[1613066222000,1025.0],[1613066521000,1025.2],[1613066821000,1025.2],[1613067121000,1025.2],[1613067421000,1025.3],[1613067722000,1025.4],[1613068022000,1025.4],[1613068321000,1025.6],[1613068621000,1025.6],[1613068921000,1025.7],[1613069222000,1025.8],[1613069522000,1025.9],[1613069822000,1026.0]]
stanica_mjerenje = []
for tlak in tlakovi_24h:
    if tlak[0] >= 1612994521000:
        stanica_mjerenje.append(tlak[1])

stanica_timestamps = []
for x in range(0, 300 * len(stanica_mjerenje), 300): #5 minuta
    stanica_timestamps.append(x)

stanica_timestamps[:] = [x for x in stanica_timestamps if x < max(timestamps)]
stanica_mjerenje = stanica_mjerenje[:len(stanica_timestamps)]

if max(timestamps) > 14400:
    timestamps[:] = [x / (3600) for x in timestamps]
    stanica_timestamps[:] = [x / (3600) for x in stanica_timestamps]
    time_unit = "h"
elif max(timestamps) > 300:
    timestamps[:] = [x / (60) for x in timestamps]
    stanica_timestamps[:] = [x / (60) for x in stanica_timestamps]
    time_unit = "min"

plt.plot(timestamps, bmp388_pressure, 'r-', label='BMP388 pressure')
plt.plot(stanica_timestamps, stanica_mjerenje, 'g-', label='stanica')
plt.legend(loc='upper center', bbox_to_anchor=(0.5, 1.1),
           ncol=3, fancybox=True, shadow=True)
plt.ylabel('Tlak/Temperatura, [Pa]')
plt.xlabel('Vrijeme, [' + time_unit + ']')
plt.show()


# fig, ax1 = plt.subplots()

# color = 'tab:red'
# ax1.set_xlabel('Vrijeme [' + time_unit +']')
# ax1.set_ylabel('BMP388 temperatura', color=color)
# ax1.plot(timestamps, bmp388_temperature, color=color)
# ax1.tick_params(axis='y', labelcolor=color)

# ax2 = ax1.twinx()

# color = 'tab:blue'
# ax2.set_ylabel('BMP388 tlak', color=color)
# ax2.plot(timestamps, bmp388_pressure, color=color)
# ax2.tick_params(axis='y', labelcolor=color)

# fig.tight_layout()
# plt.show()

# ################################################################################

# fig, ax1 = plt.subplots()

# color = 'tab:red'
# ax1.set_xlabel('Vrijeme [' + time_unit + ']')
# ax1.set_ylabel('DPS3688 temperatura', color=color)
# ax1.plot(timestamps, dps368_temperature, color=color)
# ax1.tick_params(axis='y', labelcolor=color)

# ax2 = ax1.twinx()

# color = 'tab:blue'
# ax2.set_ylabel('DPS368 tlak', color=color)
# ax2.plot(timestamps, dps368_pressure, color=color)
# ax2.tick_params(axis='y', labelcolor=color)

# fig.tight_layout()
# plt.show()

# ################################################################################

# fig, ax1 = plt.subplots()

# color = 'tab:red'
# ax1.set_xlabel('Vrijeme [' + time_unit +']')
# ax1.set_ylabel('SMPB temperatura', color=color)
# ax1.plot(timestamps, smpb_temperature, color=color)
# ax1.tick_params(axis='y', labelcolor=color)

# ax2 = ax1.twinx()

# color = 'tab:blue'
# ax2.set_ylabel('SMPB tlak', color=color)
# ax2.plot(timestamps, smpb_pressure, color=color)
# ax2.tick_params(axis='y', labelcolor=color)

# fig.tight_layout()
# plt.show()

# plt.plot(timestamps, bmp388_altitude, 'r-', label='BMP388 altitude')
# plt.plot(timestamps, dps368_altitude, 'g-', label='DPS368 altitude')
# plt.plot(timestamps, smpb_altitude, 'b-', label='2SMPB altitude')
# plt.legend(loc='upper center', bbox_to_anchor=(0.5, 1.1),
#            ncol=3, fancybox=True, shadow=True)
# plt.ylabel('Nadmorska visina, [m]')
# plt.xlabel('Vrijeme, [' + time_unit + ']')
# plt.show()






# plt.plt(timestamps, bmp388_pressure, 'r-', label='BMP388 pressure')
# plt.plot(timestamps, bmp388_temperature, 'g-', label='BMP388 temperature')
# plt.legend(loc='upper center', bbox_to_anchor=(0.5, 1.1),
#            ncol=3, fancybox=True, shadow=True)
# plt.ylabel('Tlak/Temperatura, [Pa]')
# plt.xlabel('Vrijeme, [' + time_unit + ']')
# plt.show()

# plt.plot(timestamps, dps368_pressure, 'r-', label='DPS368 pressure')
# plt.plot(timestamps, dps368_temperature, 'g-', label='DPS368 temperature')
# plt.legend(loc='upper center', bbox_to_anchor=(0.5, 1.1),
#            ncol=3, fancybox=True, shadow=True)
# plt.ylabel('Tlak/Temperatura, [Pa]')
# plt.xlabel('Vrijeme, [' + time_unit + ']')
# plt.show()

# plt.plot(timestamps, smpb_pressure, 'r-', label='2SMPB pressure')
# plt.plot(timestamps, smpb_temperature, 'g-', label='2SMPB temperature')
# plt.legend(loc='upper center', bbox_to_anchor=(0.5, 1.1),
#            ncol=3, fancybox=True, shadow=True)
# plt.ylabel('Tlak, [Pa]')
# plt.xlabel('Vrijeme, [' + time_unit + ']')
# plt.show()

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
