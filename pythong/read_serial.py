import serial
import os
import array

ser = serial.Serial('/dev/ttyACM0')
ppm = []


def writePPM(ppm):
    pplen = min(map(len, ppm))
    with open("out.ppm", "wb") as f:
        f.write(bytearray(f"P3 {len(ppm)} {pplen//3} 250\n", 'ascii'))
        for arr in ppm:
            a = arr[0:pplen]
            a.tofile(f)


def scanPPM():
    out = []
    print("[Python]: Scan to PPM ")
    line = ser.readline()
    while (True):
        dec = line.decode()
        print(dec)
        if "end" in dec:
            break

        arr = array.array('B')

        for k in dec.strip()[:-1].split(';'):
            arr += array.array('B', map(int, k.split(' ')))

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
