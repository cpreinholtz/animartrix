#!/usr/bin/env python
import numpy as np
import matplotlib.pyplot as plt
import math


#...............................................................
# IF YOU COPY PASTE THIS FILE TO MAKE A NEW SHAPE
# DONT FORGET TO CHANGE THE TARGET FILENAME TO WRITE TO AT THE BOTTOM
#...............................................................


spacing = 2.5 # inches between LEDS



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

  #print(leds)
  return leds



def linearRun(start, stop, n):

  x = np.linspace(start[0],stop[0],n)
  y = np.linspace(start[1],stop[1],n)
  z = np.linspace(start[2],stop[2],n)

  leds = []
  for led in range(n):
    leds.append([x[led], y[led], z[led]])

  #print(leds)
  return leds







#...............................................................
#calc results HERE

rings = []
rings2 = []

#left side
cubeSideLength = 5*12 # 5 feet to inches
nPipes = 8
nLengthsPerPipe = 8
nLedsPerLength = 25

ySpacing = cubeSideLength/(nPipes-1) #5 feet / 8 pipes to inches
xSpacing = cubeSideLength/(nLengthsPerPipe-1) #5 feet / 8 lengths to inches
zSpacing = ySpacing

print("[xSpacing, ySpacing]")
print([xSpacing, ySpacing])


#arrange the structure so front left top corner is 0,0,0
# bottom back right is 5,5,-5

# this is a right handed coordinate system
# +x is right
# +y is back
# +z is up

#
#  +z  +y
#  ^   7
#  |  /
#  | /
#  |/____> +x 

#              pipes are all on z=0 XY plane 
#              _____   <-   5,5,0
#             /    /.     
#            /    / .  
#  0,0,0 ->  ----- .
#            . . . .  <-   5,5,-5
#            . . .   
#  0,0,-5 -> . . .     <- LEDS hang down below pipes  (-z)
#
#

for pipe in range(nPipes):
  rings.append([])
  rings2.append([])

for pipe in range(nPipes):
  for length in range(nLengthsPerPipe):

    #x and y are the same for the whole length (assumed to hang straight down)

    # x is serpintine, starts on left in pipe 0, right in pipe 1

    # if starting on the left (0 to cubeSideLength), pipe % 2 = 0
    # x increments with length
    if pipe%2==1:
      x = xSpacing * length + ((8+cubeSideLength)*pipe)
      x2 = xSpacing * length + ((8+cubeSideLength))

    # if starting on the right(cubeSideLength downto 0), pipe % 2 = 1
    if pipe%2==0:

      # the x is shifted right each pipe 
      x = ((8+cubeSideLength)*(pipe+1)) - (xSpacing * (1+length))
      x2 = ((8+cubeSideLength)) - (xSpacing * (1+length))

    ys=0
    ym=1
    #if pipe >= 4:
      #x=x-4*((8+cubeSideLength))
      #ym=-1
      #ys=8

    # y is just the position of the whole pipe ( and all the leds hanging under it)

    #start Z coordinate is 0 for downward runs (even) and -5 for upward runs (odd)    
    start = [x,(ys+cubeSideLength)* ym * (length%2)-ys,0]
    start2 = [x2,(ys+cubeSideLength)* ym * (length%2)-ys,math.floor(pipe/2)*zSpacing]

    #stop has same XY as start, but the Z is opposite, end at -5 for downward, 0 for upward
    stop = [x, (ys+cubeSideLength)* ym * ((length+1)%2)-ys,0]
    stop2 = [x2, (ys+cubeSideLength)* ym * ((length+1)%2)-ys,math.floor(pipe/2)*zSpacing]

    #print(start)
    #print(start)

    pin = pipe

    rings[pin] += linearRun(start,stop,nLedsPerLength)
    rings2[pin] += linearRun(start2,stop2,nLedsPerLength)




#print(rings)


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
  #print()
  #print(ring)
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


str+= "float ledMap2 [NUM_LEDS][3] =\n"
str+="{"
this =0
for cnt,ring in enumerate(rings2):
  #print()
  #print(ring)
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

with open("../examples/ANIMapping_3d/wall1by8.h", "w") as text_file:
    text_file.write(str)


# .........................................
#plot it
# .........................................
fig2 = plt.figure()
ax2 = fig2.add_subplot(projection="3d")
ax2.set_xlabel('X Label')
ax2.set_ylabel('Y Label')
ax2.set_zlabel('Z Label')

for cnt,ring in enumerate(rings2):
    ring = np.array(ring)

    reds = cnt+1
    ax2.scatter(ring[2:-reds, 0], ring[2:-reds, 1], ring[2:-reds, 2], c='blue')

    ax2.scatter(ring[0, 0], ring[0, 1], ring[0, 2], c='green', marker='*', s=10)
    ax2.scatter(ring[1, 0], ring[1, 1], ring[1, 2], c='yellow', marker='*', s=10)
    ax2.scatter(ring[-reds:, 0], ring[-reds:, 1], ring[-reds:, 2], c='red', marker='*', s=10)

    #print(cnt)
ax2.set_aspect('equal')




fig = plt.figure()
ax = fig.add_subplot()
ax.set_xlabel('X Label')
ax.set_ylabel('Y Label')

for cnt,ring in enumerate(rings):
    ring = np.array(ring)

    reds = cnt+1
    ax.scatter(ring[2:-reds, 0], ring[2:-reds, 1], c='blue')

    ax.scatter(ring[0, 0], ring[0, 1], c='green', marker='*', s=10)
    ax.scatter(ring[1, 0], ring[1, 1], c='yellow', marker='*', s=10)
    ax.scatter(ring[-reds:, 0], ring[-reds:, 1], c='red', marker='*', s=10)

    #print(cnt)
ax.set_aspect('equal')

plt.show()




