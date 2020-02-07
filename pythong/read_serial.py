import serial
import os
import array

ser = serial.Serial('/dev/ttyACM0')
ppm = []


def writePPM(ppm):
    pplen = len(ppm[0])
    with open("out.ppm", "wb") as f:
        f.write(bytearray(f"P3 {len(ppm)} {pplen // 2} 65000\n", 'ascii'))
        for arr in ppm:
            a = arr[0:pplen]
            a.tofile(f)


def checkBytePattern(pattern):
    out = array.array('B')

    for p in pattern:
        out.append(int.from_bytes(ser.read(), "big"))
        if out[-1] != p:

            return out

    return True


def scanPPM():
    out = []
    print("[Python]: Scan to PPM ")

    while (True):
        # try:
        #     if "end" in line.decode():
        #         break
        # except:
        #     pass
        b = checkBytePattern([0, 0, 255, 255])
        if b == True:
            break

        arr = array.array('B', b)
        while (True):
            b = checkBytePattern([255, 255, 0, 0])
            if b == True:
                print("row end")
                break

            arr += b

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
