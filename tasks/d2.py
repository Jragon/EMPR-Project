import serial

s = serial.Serial("/dev/ttyACM0", 115200)


def main():
    s.write(b"hello")
    x = f'{int(input("Please enter the x coordinate: ")):04d}'
    y = f'{int(input("Please enter the y coordinate: ")):04d}'
    print(x)
    print(y)
    s.write(x.encode())
    s.write(y.encode())

    while True:
        c = s.readline()
        print(c)
        if c == b"[Task D2]: Sending rgb values\r\n":
            break
    rgbstr = str(s.readline()).replace("'", "")[1:-4]
    rgb = rgbstr.split("|")
    print(f"R: {rgb[0]}, G: {rgb[1]}, B: {rgb[2]}")


if __name__ == "__main__":
    main()
