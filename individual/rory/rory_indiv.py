from inputimeout import inputimeout, TimeoutOccurred
import serial
import re
import os

ser = serial.Serial('/dev/ttyACM0', 115200)


def collateScans():
    scans = []
    for scanname in os.listdir("./scans"):
        with open(f"scans/{scanname}") as f:
            scans.append(f.readline().strip())

    with open("data.c", 'w') as f:
        f.write('#include "data.h"\n\n')

        fs = ',\n'.join(scans)
        f.write(f"data_t data[] = {{ \n {fs} \n  }};")

    with open("data_len.h", "w") as f:
        f.write(f"#define DATA_LEN {len(scans)}\n")


def scan(check=False):
    print("[Python]: scanner!")
    if check:
        try:
            x = inputimeout(
                prompt="[Python]: Would you like to store this scan? ", timeout=3)
        except TimeoutOccurred:
            return

    name = input("[Python]: Enter scan name: ")
    print("[Python]: Scanning ...")

    line = ser.readline()
    while (True):
        try:
            dec = line.decode()
        except:
            dec = False

        if dec:
            print(dec, end='')
            if "Data" in dec:
                vals = dec.strip().replace("[Data]: ", "")
                break

        line = ser.readline()

    print("[Python]: Writing ...")
    with open(f"scans/{name}", "w") as f:
        f.write(f'{{ .name = "{name}", .errors = {{-1}}, {vals} }}')

    collateScans()


def main():
    line = ser.readline()
    while (True):
        try:
            dec = line.decode()
        except:
            dec = False

        if dec:
            print(dec, end='')
            if "Recognise" in dec:
                scan(check=True)
            if "Scan Image" in dec:
                scan()

        line = ser.readline()


if __name__ == "__main__":
    main()
