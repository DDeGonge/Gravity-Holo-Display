__version__ = '0.1.0'

from math import sin, pi
import matplotlib.pyplot as plt
from time import sleep
import random


"""
                REAR

56 | 57 | 58 | 59 | 60 | 61 | 62 | 63
-------------------------------------
48 | 49 | 50 | 51 | 52 | 53 | 54 | 55
-------------------------------------
40 | 41 | 42 | 43 | 44 | 45 | 46 | 47
-------------------------------------
32 | 33 | 34 | 35 | 36 | 37 | 38 | 39
-------------------------------------
24 | 25 | 26 | 27 | 28 | 29 | 30 | 31
-------------------------------------
16 | 17 | 18 | 19 | 20 | 21 | 22 | 23
-------------------------------------
8  | 9  | 10 | 11 | 12 | 13 | 14 | 15
-------------------------------------
0  | 1  | 2  | 3  | 4  | 5  | 6  | 7

             FRONT (USER)

"""

FRAMERATE = 3
XSPACE_MM = 25

class frame:
    iLim = 7  # Don't save any true indicies > this
    rows = 8
    cols = 8
    def __init__(self):
        self.data = {r: {w: [] for w in range(self.cols)} for r in range(self.rows)}

    def add_value(self, row, col, val):
        self.data[row][col].append(int(val))

    def plot(self):
        for rk, r in self.data.items():
            x = []
            y = []
            for wk, w in r.items():
                y += w
                x += [wk] * len(w)
            plt.scatter(x, y, label=rk)
        plt.legend()
        plt.show()

    def writeString(self, fileObj):
        line = ""
        for rk, r in self.data.items():
            for wk, w in r.items():
                true_index = (rk * self.cols) + wk
                if true_index > self.iLim: continue
                line += str(true_index) + "-"
                if len(w) == 0:
                    line += str(0)
                else:
                    for value in w:
                        line += str(value) + ","
                    line = line[:-1]
                line += "|"

        line += "\n"
        fileObj.write(line)

def main():
    pass

def sineBuilder():
    filename = 'sine.csv'
    nom_height = 200
    amplitude = 20
    wavelen = 1
    speed = 0.2

    with open(filename, 'w+') as f:
        framecount = int(FRAMERATE / speed)
        for i in range(framecount):
            curFrame = frame()
            for k in range(8):
                rads = wavelen * 2 * pi * k / 7
                shift = 2 * pi * i / framecount
                rads -= shift
                curFrame.add_value(0, k, nom_height + (amplitude * sin(rads)))

            # curFrame.plot()
            curFrame.writeString(f)

def flyingV():
    filename = 'flyingV.csv'
    nom_height = 200
    maxDelta = 70
    speed = 0.2
    le_config = [-60,-40,-20,0,0,-20,-40,-60]

    with open(filename, 'w+') as f:
        framecount = int(FRAMERATE / speed)
        inkrement = 375 / framecount
        for i in range(framecount):
            curFrame = frame()
            cleaned_config = [max(min(c, 300), 0) for c in le_config]
            for k in range(8):
                curFrame.add_value(0, k, cleaned_config[k])

            le_config = [l + inkrement for l in le_config]

            # curFrame.plot()
            curFrame.writeString(f)

def GrowSineBuilder():
    filename = 'growingSine.csv'
    nom_height = 200
    amplitude = 20
    wavelen = 1
    speed = 0.2

    with open(filename, 'w+') as f:
        framecount = int(FRAMERATE / speed)
        for i in range(framecount):
            curFrame = frame()
            for k in range(8):
                rads = wavelen * 2 * pi * k / 7
                shift = 2 * pi * i / framecount
                rads -= shift
                curFrame.add_value(0, k, nom_height + (i / framecount)*(amplitude * sin(rads)))

            # curFrame.plot()
            curFrame.writeString(f)

def riseUp():
    filename = 'riseUp.csv'
    nom_height = 200
    start_height = 300
    framesper = 5
    spacing = 2

    positions = [start_height] * 8

    def calcMod(index, frame):
        istart = index * (framesper - spacing)
        iend = istart + framesper
        if frame >= istart and frame < iend:
            return int((nom_height - start_height) / framesper)
        return 0

    with open(filename, 'w+') as f:
        frameNum = 0
        while True:
            mods = [calcMod(i, frameNum) for i in range(8)]
            positions = [p + calcMod(i, frameNum) for i, p in enumerate(positions)]

            curFrame = frame()
            for k, val in enumerate(positions):
                curFrame.add_value(0, k, val)
            # curFrame.plot()
            curFrame.writeString(f)

            frameNum += 1
            if not any(mods):
                return

def randDrop():
    filename = 'randDrop.csv'
    end_height = 0
    mm_per_frame = -20
    spacing = 2

    dropOrder = list(range(8))
    random.shuffle(dropOrder)

    positions = [208, 219, 216, 200, 184, 180, 190, 208]

    def calcMod(index, frame):
        this_i = dropOrder[index]
        istart = this_i * spacing
        if frame >= istart:
            return mm_per_frame
        return 0

    with open(filename, 'w+') as f:
        frameNum = 0
        while True:
            mods = [calcMod(i, frameNum) for i in range(8)]
            positions = [p + calcMod(i, frameNum) for i, p in enumerate(positions)]
            positions = [max(p, end_height) for p in positions]

            curFrame = frame()
            for k, val in enumerate(positions):
                curFrame.add_value(0, k, val)
            # curFrame.plot()
            curFrame.writeString(f)

            frameNum += 1
            if sum(positions) == (end_height * len(positions)):
                return



if __name__=='__main__':
    # main()
    # sineBuilder()
    # GrowSineBuilder()
    # riseUp()
    # randDrop()
    flyingV()
