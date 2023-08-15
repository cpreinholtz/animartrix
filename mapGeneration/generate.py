#!/usr/bin/env python


#https://stackoverflow.com/questions/33510979/generator-of-evenly-spaced-points-in-a-circle-in-python


import numpy as np
import matplotlib.pyplot as plt

# .........................................
# 30 leds per meter, convert to inches per led.
# .........................................

spacing = 1.34 #1/30*39.3701



def calcRing(radias, offset_rad, skip, full, fixed):
  ring = []
  deltaRadians = spacing / radias
  for pixel in range(90):
    angle = offset_rad + pixel*deltaRadians
    if pixel in skip or pixel * deltaRadians > 2.1 * np.pi or pixel * deltaRadians < -2.1 * np.pi :
      continue
    ncos = radias * np.cos(angle)
    nsin = radias * np.sin(angle)
    if full:
      ring.append([ncos,nsin,fixed])
    else:
      ring.append([fixed,ncos,nsin])
  return ring

def calcRings():
  res=[]
  bigr=18.25
  littler=13.7
  #############################################
  ########## ring 0 little, left, full
  radias = -littler # + radias for  full left
  offset_rad = .32  * np.pi *2 + np.pi
  skip = []
  full = True
  fixed = 15
  #############################################
  res.append(calcRing(radias, offset_rad, skip, full, fixed))
  ##############################################

  #############################################
  ########## ring 1 big, left, full
  radias = -bigr # + radias for  full left
  offset_rad = .385  * np.pi*2 + np.pi
  skip = []
  full = True
  fixed = 8
  #############################################
  res.append(calcRing(radias, offset_rad, skip, full, fixed))
  ##############################################


  #############################################
  ########## ring 3 big, left, partial
  radias = -bigr # + radias for  full left
  offset_rad = .134  * np.pi*2 + np.pi

  sz = [5,10,5,6,6,5,10,5,6,6]
  skip = []
  idx=0
  for s in sz:
    skip.append(idx+s)
    skip.append(idx+s+1)
    idx += s+2
  print(skip)

  #skip = [5, 6, 17,18,  24,25,  32,33,   40,41, 47,48, 59,60 , 66, 67, 74, 75]
  full = False
  fixed = -8
  #############################################
  res.append(calcRing(radias, offset_rad, skip, full, fixed))
  ##############################################

  #############################################
  ########## ring 3 litte, left, partial
  radias = -littler # + radias for  full left
  offset_rad = .065  * np.pi*2 + np.pi
  sz = [10,8,8,10,8,8]
  skip = []
  idx=0
  for s in sz:
    skip.append(idx+s)
    skip.append(idx+s+1)
    idx += s+2
  print(skip)
  full = False
  fixed = -15
  #############################################
  res.append(calcRing(radias, offset_rad, skip, full, fixed))
  ##############################################


  #############################################
  ########## ring 4 little, right, full
  radias = -littler # + radias for  full left
  offset_rad = .34  * np.pi *2
  skip = []
  full = True
  fixed = -15
  #############################################
  res.append(calcRing(radias, offset_rad, skip, full, fixed))
  ##############################################

  #############################################
  ########## ring 5 big, right, full
  radias = -bigr # + radias for  full left
  offset_rad = .388  * np.pi*2 
  skip = []
  full = True
  fixed = -8
  #############################################
  res.append(calcRing(radias, offset_rad, skip, full, fixed))
  ##############################################


  #############################################
  ########## ring 3 big, right, partial
  radias = -bigr # + radias for  full left
  offset_rad = 0.13  * np.pi*2
  sz = [5,10,5,6,6,5,10,5,6,6]
  skip = []
  idx=0
  for s in sz:
    skip.append(idx+s)
    skip.append(idx+s+1)
    idx += s+2
  print(skip)
  full = False
  fixed = 8
  #############################################
  res.append(calcRing(radias, offset_rad, skip, full, fixed))
  ##############################################

  #############################################
  ########## ring 3 litte, right, partial
  radias = -littler # + radias for  full left
  offset_rad = 0.07  * np.pi*2 
  sz =  [10,8,8,10,8,8]
  skip = []
  idx=0
  for s in sz:
    skip.append(idx+s)
    skip.append(idx+s+1)
    idx += s+2
  print(skip)
  full = False
  fixed = 15
  #############################################
  res.append(calcRing(radias, offset_rad, skip, full, fixed))
  ##############################################


  return res








rings = calcRings()
#parse results HERE
nRings=0
nPixelsFinal = 0
nPixelsPerRing = []

for cnt,ring in enumerate(rings):
  nRings+=1
  nPixelsThisRing=0
  for i in ring:
    nPixelsThisRing+=1
    nPixelsFinal += 1
  nPixelsPerRing.append(nPixelsThisRing)


# .........................................
# print it
# .........................................
#print(rings)
str = ""

str+= "// generated via python\n"

str+= "const int nRings = {}; // number of rings, use this for assigning to pins\n".format(nRings)
str+= "#define NUM_LEDS {}\n".format(nPixelsFinal)
str+= "const int nMaxPixels = {};\n".format(np.max(np.array(nPixelsPerRing)))


str+= "//number of pixels per ring, used to mapto buffers and pins later\n"
str+= "const int nPixelsPerRing [{}] = ".format(nRings)+"{" 
delim=""
for val in nPixelsPerRing:
  str+= delim + "{}".format(val)
  delim=","
str += "};\n"


str+= "float ledMap [NUM_LEDS][3] =\n"
str+="{"
this =0
for cnt,ring in enumerate(rings):
  str+="//ring: {} nPixels this ring:{}\n".format(cnt,nPixelsPerRing[cnt])
  for i in ring:
    this = this +1
    str+= "  {"
    str+= "{:.4f}, {:.4f}, {:.4f}".format(i[0],i[1],i[2])
    str+= "}"
    if this < np.sum(nPixelsFinal):
      str+= ",\n"
    else:
      str+= "\n"


str+= "};\n"


str+= "// end generated via python\n"

#print(str)

with open("../examples/ANIMapping_3d/wag.h", "w") as text_file:
    text_file.write(str)


# .........................................
#plot it
# .........................................
fig = plt.figure()
ax = fig.add_subplot(projection="3d")
ax.set_xlabel('X Label')
ax.set_ylabel('Y Label')
ax.set_zlabel('Z Label')

for cnt,ring in enumerate(rings):
    ring = np.array(ring)
    ax.scatter(ring[2:-1, 0], ring[2:-1, 1], ring[2:-1, 2], c='blue')

    ax.scatter(ring[0, 0], ring[0, 1], ring[0, 2], c='green', marker='*', s=10)
    ax.scatter(ring[1, 0], ring[1, 1], ring[1, 2], c='yellow', marker='*', s=10)
    ax.scatter(ring[-1, 0], ring[-1, 1], ring[-1, 2], c='red', marker='*', s=10)

    #print(cnt)
ax.set_aspect('equal')


plt.show()




