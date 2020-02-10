import serial
import os
import array

ser = serial.Serial('/dev/ttyACM0', 115200)
ppm = []


def writePPM(ppm, max_val):
    pplen = min(map(len, ppm))
    with open("out.ppm", "w") as f:
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
        writePPM(out, max_val + 10)
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
                scanPPM()

        line = ser.readline()


if __name__ == "__main__":
    main()
