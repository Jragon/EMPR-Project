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


def scanFlag():
    print("[Python]: scan flag")
    name = input("Enter flag name: ")
    print("[Python]: Scanning ...")
    vals = ser.readline().decode()

    if name != "check":
        print("[Python]: Writing ...")
        with open(f"flags/{name}", "w") as f:
            f.write(vals)

        return


def compFlag():
    print("[Python]: comp flags")
    vals = ser.readline().decode()
    img = loadImage(vals.strip()[:-1])
    img_hash = imagehash.phash(img)

    with open('hashes.pickle', 'rb') as f:
        hashes = pickle.load(f)

    differences = []
    for flag, val in hashes.items():
        differences.append((flag, val - img_hash))

    for x in sorted(differences, key=lambda k: k[1]):
        print(f"{x[1]}: {x[0]}")

    if 'y' in input("Is this correct? "):
        print("safe")
        return

    name = input("What is the correct flag? ")
    filename = name + str(len([x for x in hashes.keys() if name in x]) + 1)
    with open(f"flags/{filename}", 'w') as f:
        f.write(vals)

    hashes[filename] = img_hash
    with open("hashes.pickle", 'wb') as f:
        pickle.dump(hashes, f)


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
                # scanFlag()
                compFlag()

        line = ser.readline()


if __name__ == "__main__":
    main()
