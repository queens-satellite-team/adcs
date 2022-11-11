import sgp4
from sgp4.api import Satrec
from sgp4.api import jday
from sgp4.api import SGP4_ERRORS
from sgp4.api import days2mdhms
from sgp4 import exporter

from mpl_toolkits import mplot3d
import matplotlib.pyplot as plt

# sample tle for satellite
s = '1 25544U 98067A   19343.69339541  .00001764  00000-0  38792-4 0  9991'
t = '2 25544  51.6439 211.2001 0007417  17.6667  85.6398 15.50103472202482'
satellite = Satrec.twoline2rv(s, t)

fig = plt.figure()
ax = plt.axes(projection='3d')
i=0.0

while i<1: # Number of years (ie. 1 is one year)
    
    jd, fr = 2458827, i     
    e, r, v = satellite.sgp4(jd, fr)

    print(e)  # Error indicator
    print(r)  # True Equator Mean Equinox position (km)
    print(v)  # True Equator Mean Equinox velocity (km/s)

    ax.scatter3D(r[0], r[1], r[2])
    ax.scatter3D(0, 0, 0)  
    i+=0.01

plt.show()
