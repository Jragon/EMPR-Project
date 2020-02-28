import serial
from plot_flags import loadImage
import os
import array
import imagehash
import pickle
import cv2
import numpy as np
from PIL import Image

ser = serial.Serial('/dev/ttyACM0', 115200)
ppm = []


def writePPM(filename, ppm, max_val):
    pplen = min(map(len, ppm))
    with open(filename, "w") as f:
        f.write(f"P3 {pplen} {len(ppm)} {max_val}\n")
        for arr in ppm:
            a = arr[0:pplen]
            f.write(' '.join(arr) + '\n')


def scanPPM():
    out = []
    print("[Python]: Scan to PPM ")
    line = ser.readline()
    max_val = 0
    while (True):
        dec = line.decode()
        print(dec)
        if "end" in dec:
            break

        arr = dec.strip()[:-1].split(';')
        max_val = max(max([max(map(int, x.split(' '))) for x in arr]), max_val)

        print(len(arr), max_val)
        out.append(arr)
        writePPM("out.ppm", out, max_val + 10)
        line = ser.readline()


def collateFlags():
    flags = []
    for flagname in os.listdir("./flags"):
        with open(f"./flags/{flagname}") as f:
            flags.append(f.readline().strip())

    with open("flags.c", 'w') as f:
        fs = ',\n'.join(flags)
        f.write(f"flag_t flags[] = {{ \n {fs} \n  }};")


def scanFlag():
    print("[Python]: scan flag")
    name = input("Enter flag name: ")
    print("[Python]: Scanning ...")
    vals = ser.readline().decode().strip()

    print("[Python]: Writing ...")
    with open(f"flags/{name}", "w") as f:
        # vals come in like {{r, g, b}, ...}
        f.write(f'{{ "{name}", {vals}, 0 }}')

    collateFlags()


def raster():
    out = []
    print("[Python]: Raster Scan")
    line = ser.readline()
    max_val = 0
    cv2.namedWindow('image', cv2.WINDOW_NORMAL)
    cv2.resizeWindow('image', 500, 500)

    while (True):
        dec = line.decode()
        print(dec)
        if "end" in dec:
            break

        arr = dec.strip()[:-1].split(';')
        max_val = max(max([max(map(int, x.split(' '))) for x in arr]), max_val)
        # arr = [[int(val) / max_val * 255 for val in vals.split(' ')]
        #        for vals in arr]

        for vals in arr:
            rgb = [int(int(val) / max_val * 255) for val in vals.split(' ')]
            out.extend(rgb)

        print(out)
        print(len(arr), max_val)
        # out.extend(arr)

        outb = bytes(out)
        im = Image.frombytes(
            'RGB', (len(arr), int(len(out) / len(arr) / 3)), outb)
        im.save("out.png")

        # cvim = cv2.imread(np.array(im))
        # cv2.resizeWindow('image', (len(arr), int(len(out) / len(arr) / 3)))
        cv2.imshow('image', cv2.cvtColor(np.array(im), cv2.COLOR_RGB2BGR))
        cv2.waitKey(1)

        line = ser.readline()


def main():
    line = ser.readline()
    scan = False
    while (True):
        try:
            dec = line.decode()
        except:
            dec = False

        if dec:
            print(dec, end='')
            if "Raster" in dec:
                # scanPPM()
                raster()
            elif "Flag Scan" in dec:
                scanFlag()

        line = ser.readline()


if __name__ == "__main__":
    main()
