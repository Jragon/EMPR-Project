import serial

s = serial.Serial("/dev/ttyACM0", 115200)


def main():
    s.write(b"hello")
    menu = []

    while True:
        c = s.readline()
        if c == b"[Menu]: Sending menu items\r\n":
            break
    print("reading menu items")
    for i in range(64):
        c = s.readline()
        if c == b"done\r\n":
            break
        menu.append(c)

    menu = [i[i.index("-")+2:-5] for i in [str(x) for x in menu]]

    for c, item in enumerate(menu):
        if c == 0:
            continue
        print(f"{c}: {item}")
    choice = int(input("Please select the menu item to execute: "))

    s.write(f"execute: {choice:02d}".encode())


# send hello when started
# recieve the menu items
# print them
# ask for input of a number
# send that to the board


if __name__ == "__main__":
    main()
