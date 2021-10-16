__version__ = '0.1.0'

import argparse
from SerialDevice import *
import matplotlib.animation as animation
from matplotlib import pyplot as plt
import numpy as np
import time

SIM_PLAYBACK_FPS = 0.5


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

def simPlot(line):
    splitstr = line.split("|")
    all_x = []
    all_y = []
    for st in splitstr:
        linesplit = st.split("-")
        if len(linesplit) == 1:
            continue
        x = int(linesplit[0])
        ys = [int(l) for l in linesplit[1].split(",")]
        for y in ys:
            all_x.append(x)
            all_y.append(y)
    plt.clf()
    plt.axis([-0.5,8.5,0,300])
    plt.scatter(all_x, all_y)
    plt.draw()
    plt.pause(1/SIM_PLAYBACK_FPS)



def main():
    parser = argparse.ArgumentParser(description='Holo Display.')
    parser.add_argument('-m', type=int, default=0, help='Operational Mode - 0: yes')
    parser.add_argument('-f', type=str, help='CSV playback file.')
    parser.add_argument('-s', action='store_true', help='Simualte playback.')
    parser.add_argument('-p', action='store_true', help='Purge nozzles')
    args = parser.parse_args()

    sd = SerialDevice()

    if args.p is True:
        for i in range(10):
            sd.command("pp")
        return

    if args.f is None:
        print("ERROR: Must specify playback file with '-f'")
        return

    if args.s:
        plt.ion()
        plt.show()

        with open(args.f) as f:
            for line in f.readlines():
                simPlot(line)

        return

    with open(args.f) as f:
        while True:
            f.seek(0)
            for line in f.readlines():
                sd.command(line)


if __name__=='__main__':
    main()
