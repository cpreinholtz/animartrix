#!/usr/bin/env python


#https://stackoverflow.com/questions/33510979/generator-of-evenly-spaced-points-in-a-circle-in-python


import numpy as np
import matplotlib.pyplot as plt

# .........................................
# 30 leds per meter, convert to inches per led.
# .........................................

spacing = 1/30*39.3701

def calcRings(nPixels , radias , offset, fixed, xy, invert):
  rings = []

  for nPixels , radias , offset, fixed, xy, invert in zip(nPixels , radias , offset, fixed, xy, invert):  
    ring = np.zeros([nPixels,3])
    deltaRadians = spacing / radias
    if invert: 
      deltaRadians = - deltaRadians
    #print(deltaRadians)
    for pixel in range(nPixels):
      angle = offset + pixel*deltaRadians
      #print(angle)
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


radias = [27.25/2, 36.25/2, 36.25/2, 27.25/2, 27.25/2, 36.25/2, 36.25/2, 27.25/2] # radias of each ring in inches
#Pixels = [70,80,80,70,70,80,80,70]
nPixels = (np.floor(np.array(radias)*2*np.pi/spacing)).astype(int)
nCirc = np.array(radias)*2*np.pi
offset_angle = np.array([0, 0, 0 ,0, 180, 180, 180, 180])

offset = offset_angle * np.pi / 180
fixed = [-15, -8, 8, 15, -15, -8, 8 ,15]
xy = [1,1,0,0,1,1,0,0]
invert = [1,1,1,1,0,0,0,0]

rings = calcRings(nPixels , radias , offset, fixed, xy, invert);


# .........................................
# print it
# .........................................
#print(rings)
print("// generated via python")
print("// total circumference inches: ", np.sum(nCirc))
print("const int nRings = ",np.size(nPixels),"; // number of rings, use this for assigning to pins")
print("#define NUM_LEDS ",np.sum(nPixels))
print("//const int nMaxPixels =",np.max(nPixels))

print("const int nPixelsPerRing [",np.size(nPixels),"] = { //number of pixels per ring, used to mapto buffers and pins later")
delim = ""
for cnt,ring in enumerate(rings):  
  print(delim, nPixels[cnt])
  delim = ","
print("};")


print("float ledMap [",np.sum(nPixels),"][3] = { // { x,y,z} pixel map")
last = nPixels-1
for cnt,ring in enumerate(rings):
  print("//ring: ", cnt, " nPixels this ring:", nPixels[cnt] )
  for i in ring:
    if cnt < last:
      print("  {", i[0], ", ", i[1],", ", i[2],"},")
    else:
      print("  {", i[0], ", ", i[1],", ", i[2],"}")
print("};")

print("//just printing constants again to prevent scrolling")
print("// total circumference inches: ", np.sum(nCirc))
print("//const int nRings = ",np.size(nPixels),"; // number of rings, use this for assigning to pins")
print("//#define NUM_LEDS ",np.sum(nPixels))
print("//const int nMaxPixels = ",np.max(nPixels))

print("// end generated via python")
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
    #print(cnt)
ax.set_aspect('equal')


#plt.show()




