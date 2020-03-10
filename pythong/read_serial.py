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

    with open("../tasks/flags.c", 'w') as f:
        fs = ',\n'.join(flags)
        f.write('#include "flags.h"\n\n')
        f.write(f"flag_t flags[] = {{ \n {fs} \n  }};")


def scanFlag():
    print("[Python]: scan flag")
    name = input("Enter flag name: ")
    print("[Python]: Scanning ...")

    line = ser.readline()
    while(True):
        dec = line.decode().strip()
        print(dec)
        if "{" in dec:
            break

        line = ser.readline()

    print("[Python]: Writing ...")
    with open(f"flags/{name}", "w") as f:
        # vals come in like {{r, g, b}, ...}
        f.write(f'{{ .error={{0}}, .name="{name}", .data={dec} }}')

    collateFlags()


def scanFlag_2():
    out = []
    print("[Python]: Flag Raster(ish) Scan")
    line = ser.readline()
    max_val = 0
    name = input("Flag name: ")

    cv2.namedWindow('image', cv2.WINDOW_NORMAL)
    cv2.resizeWindow('image', 500, 500)

    while (True):
        dec = line.decode()
        print(dec)
        if "end" in dec:
            break
        if "[" in dec:
            print(dec)
            line = ser.readline()
            continue

        dec = dec.strip()[:-1]
        arr = dec[1:].split(';')
        if dec[0] == '-':
            arr = arr[::-1]

        max_val = max(max([max(map(int, x.split(' '))) for x in arr]), max_val)

        for vals in arr:
            rgb = [int(int(val) / max_val * 255)
                   for val in vals.split(' ')]
            out.extend(rgb)

        print(out)
        print(len(arr), max_val)
        # out.extend(arr)

        outb = bytes(out)
        im = Image.frombytes(
            'RGB', (len(arr), int(len(out) / len(arr) / 3)), outb)
        im.save(f"flags/{name}.ppm")

        # cvim = cv2.imread(np.array(im))
        # cv2.resizeWindow('image', (len(arr), int(len(out) / len(arr) / 3)))
        cv2.imshow('image', cv2.cvtColor(np.array(im), cv2.COLOR_RGB2BGR))
        cv2.waitKey(1)

        line = ser.readline()


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


def _get_int_input(message):
    while True:
        try: 
            inp = int(input(message))
        except ValueError:
            print("Please enter a number")    
            continue

        return inp

    

def d2_control():
    x = f'{int(_get_int_input("[Python]: Please enter the x coordinate: ")):04d}'
    y = f'{int(_get_int_input("[Python]: Please enter the y coordinate: ")):04d}'

    ser.write(x.encode())
    ser.write(y.encode())

    while True:
        line = ser.readline()
        try:
            dec = line.decode()
        except:
            continue
        print(dec.strip())
        if "Sending" in dec:
            break

    rgbstr = ser.readline().decode().replace("'", "")[1:-4]
    rgb = rgbstr.split("|")
    print(f"[Python]: R: {rgb[0]}, G: {rgb[1]}, B: {rgb[2]}")

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
            if "Flag Raster Scan" in dec:
                scanFlag_2()
            elif "Raster" in dec:
                # scanPPM()
                raster()
            elif "Flag Scan" in dec:
                scanFlag()
            elif "Move" in dec:
                d2_control()

        line = ser.readline()


if __name__ == "__main__":
    main()

