import serial
from plot_flags import loadImage
import os
import array
import imagehash
import pickle

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
                scanPPM()
            elif "Flag Scan" in dec:
                scanFlag()

        line = ser.readline()


if __name__ == "__main__":
    main()
