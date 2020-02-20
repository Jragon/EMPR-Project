import serial

s = serial.Serial("/dev/ttyACM0", 115200)


def main():
    s.write(b"hello")
    x = f'{int(input("Please enter the x coordinate: ")):04d}'
    y = f'{int(input("Please enter the y coordinate: ")):04d}'
    z = f'{int(input("Please enter the z coordinate: ")):04d}'
    print(x)
    print(y)
    print(z)
    s.write(x.encode())
    s.write(y.encode())
    s.write(z.encode())


if __name__ == "__main__":
    main()
