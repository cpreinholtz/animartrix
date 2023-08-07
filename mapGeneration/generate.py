#!/usr/bin/env python


#https://stackoverflow.com/questions/33510979/generator-of-evenly-spaced-points-in-a-circle-in-python


import numpy as np
import matplotlib.pyplot as plt



# .........................................
# 30 leds per meter, convert to inches per led.
# .........................................

spacing = 1/30*39.3701
spacing = 4

# .........................................
#
# .........................................
def calcRings(nPixels , radias , offset, fixed, xy):
  rings = []

  for nPixels , radias , offset, fixed, xy in zip(nPixels , radias , offset, fixed, xy):  
    ring = np.zeros([nPixels,3])
    deltaRadians = spacing / radias
    print(deltaRadians)
    for pixel in range(nPixels):
      angle = offset + pixel*deltaRadians
      print(angle)
      if xy:
        x = radias * np.cos(angle)
        y = radias * np.sin(angle)
        z = fixed
      else:
        x = radias * np.cos(angle)
        y=fixed
        z = radias * np.sin(angle)
      ring[pixel,:] = [x,y,z]
    rings.append(ring)
  return rings





# .........................................
#actual values
# .........................................


radias = [12, 15, 15, 12, 12, 15, 15, 12] # radias of each ring in inches
#nPixels = [8,8,8,8,8,8,8]
nPixels = (np.floor(np.array(radias)*2*np.pi/spacing)).astype(int)
offset_angle = np.array([30, 50, 0 ,0, 0, 0, 0, 0])

offset = offset_angle * np.pi / 180
fixed = [-10, -5, 5, 10, -10, -5, 5 ,10]
xy = [1,1,1,1,0,0,0,0]

rings = calcRings(nPixels , radias , offset, fixed, xy);


# .........................................
# print it
# .........................................
print(rings)




# .........................................
#plot it
# .........................................
fig = plt.figure()
ax = fig.add_subplot(projection="3d")
ax.set_xlabel('X Label')
ax.set_ylabel('Y Label')
ax.set_zlabel('Z Label')

for cnt,ring in enumerate(rings):
    ax.scatter(ring[:, 0], ring[:, 1], ring[:, 2])
    print(cnt)
ax.set_aspect('equal')


plt.show()




