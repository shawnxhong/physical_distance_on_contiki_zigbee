import numpy as np
from math import log

data = []

data.append([250, [-77, -74, -67, -65, -67, -64, -70, -68, -69, -70, -70, -70, -73, -69, -68]])
data.append([240, [-62, -61, -62, -62, -61, -60, -64, -63, -63, -63, -64]])
data.append([230, [-60, -60, -60, -59, -60, -60]])
data.append([220, [-64, -67, -69, -70, -71, -70, -70, -74, -68]])
data.append([210, [-68, -65, -64, -68, -65, -65, -67, -67, -67, -68, -71, -65]])
data.append([200, [-62, -63, -62, -62, -63, -62, -63, -65]])
data.append([190, [-62, -64, -66, -71, -69, -69, -65, -67, -65, -65, -64, -67, -64]])
data.append([180, [-65, -64, -64, -68, -67, -64, -64, -64]])
data.append([170, [-59, -58, -60, -59, -59, -58]])
data.append([160, [-59, -59, -60, -61, -62, -64, -61]])
data.append([150, [-56, -55, -56, -56, -56, -57, -56, -57]])


log_d_data, rssi_data = [], []
for i in data:
	i[1] = sum(i[1])/len(i[1])
	log_d_data.append(log(i[0]))
	rssi_data.append(i[1])


# print(x_data)
# print len(y_data)
# y = mx + c
# rssi = a * log(d) + c

log_d = np.array(log_d_data)
rssi = np.array(rssi_data)

A = np.vstack([log_d, np.ones(len(log_d))]).T

a, c = np.linalg.lstsq(A, rssi)[0]

print a, c
# a = -16.022
# c = 20.958
# distance = exp((rssi - c)/a)

