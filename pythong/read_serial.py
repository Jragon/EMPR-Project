import serial
import os
import array

ser = serial.Serial('/dev/ttyACM0')
ppm = []


def writePPM(ppm):
    pplen = min(map(len, ppm))
    with open("out.ppm", "w") as f:
        f.write(f"P3 {pplen} {len(ppm)} 250\n")
        for arr in ppm:
            a = arr[0:pplen]
            f.write(' '.join(arr) + '\n')


def scanPPM():
    out = []
    print("[Python]: Scan to PPM ")
    line = ser.readline()
    while (True):
        dec = line.decode()
        print(dec)
        if "end" in dec:
            break

        arr = dec.strip()[:-1].split(';')

        print(len(arr))
        out.append(arr)
        writePPM(out)
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
