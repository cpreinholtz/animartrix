#!/usr/bin/env python


#https://stackoverflow.com/questions/33510979/generator-of-evenly-spaced-points-in-a-circle-in-python


import numpy as np
import matplotlib.pyplot as plt

# .........................................
# 30 leds per meter, convert to inches per led.
# .........................................

spacing = 1.34 #1/30*39.3701




def linearDif(start, dif, n):
  stop = [0,0,0]
  for i in range(3):
    stop[i] = start[i] + dif[i]

  x = np.linspace(start[0],stop[0],n)
  y = np.linspace(start[1],stop[1],n)
  z = np.linspace(start[2],stop[2],n)

  leds = []
  for led in range(n):
    leds.append([x[led], y[led], z[led]])

  print(leds)
  return leds



def linearRun(start, stop, n):

  x = np.linspace(start[0],stop[0],n)
  y = np.linspace(start[1],stop[1],n)
  z = np.linspace(start[2],stop[2],n)

  leds = []
  for led in range(n):
    leds.append([x[led], y[led], z[led]])

  print(leds)
  return leds







#...............................................................
#calc results HERE

rings = [[],[]]

#left side
rings[0] += linearRun( [-27, 5, 0], [-48, 12, 0], 10)
rings[0] += linearRun( [-48, 15, 0], [-47, 34, 0], 9)
rings[0] += linearRun( [-46, 36, 0], [-40, 42, 0], 4)
rings[0] += linearRun( [-41, 43, 0], [-53, 48, 0], 5)
rings[0] += linearDif( [-54, 50, 0], [-12, 48, 0], 22)# top of wing outside
rings[0] += linearDif( [-65, 98, 0], [52, -41, 0], 28) # top of wing inside
rings[0] += linearDif( [-6, 56, 0], [-6, -40, 0], 17) # low wing inside
rings[0] += linearDif( [-13, 14, 0], [-8, -6, 0], 5) # bottom inside
#right side
rings[1] += linearDif( [33 , 7, 0], [10, 8, 0], 7)#bottom outside
rings[1] += linearDif( [44 , 17, 0], [-1, 22, 0], 9)
rings[1] += linearDif( [43 , 36, 0], [-7, 7, 0], 4)
rings[1] += linearDif( [39 , 48, 0], [11, 2, 0], 5)
rings[1] += linearDif( [54, 50, 0], [12, 48, 0], 19)# top of wing outside
rings[1] += linearDif( [65, 98, 0], [-52, -41, 0], 29) # top of wing inside
rings[1] += linearDif( [6, 56, 0], [6, -40, 0], 17) # low wing inside

rings[1] += linearDif( [13, 14, 0], [14, -8, 0], 8) # bottom inside
rings[1] += linearDif( [27, 6, 0], [2, 0, 0], 2) # bottom inside

#rings[0].append(linearRun( [ , , 0], [ , , 0], ))


print(rings)
print(rings)
print(rings)

#...............................................................
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
  print()
  print()
  print(ring)
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

with open("../examples/ANIMapping_3d/bf.h", "w") as text_file:
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




