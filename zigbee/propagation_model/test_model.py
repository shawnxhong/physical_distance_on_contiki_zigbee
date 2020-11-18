from math import exp
a = -16.022
c = 20.958

# rssi = a * log(d) + c

# d = exp((rssi-c)/a)

def get_distance(rssi):
    return exp((rssi - c) / a)


for r in range(-40, -80, -1):
    print("rssi: {}, distance: {} cm".format(r, get_distance(r)))
