import os
import matplotlib
import matplotlib.pyplot as plt
import numpy as np
from PIL import Image, ImageFilter
import imagehash
import pickle


def loadScans():
    flags = {}
    for flagname in os.listdir("./flags"):
        with open(f"./flags/{flagname}") as f:
            flags[flagname] = f.readline().strip()[:-1]

    print(flags)
    return flags


def loadImage(flag):
    s = {}
    for y in [x.split(':') for x in flag.split(';')]:
        coords = y[0].split(' ')
        rgbs = y[1][1:].split(' ')

        # normalize rgbs
        rgbs = [int(x) / 2048 for x in rgbs] + [1]

        s[(int(coords[0]), int(coords[1]))] = rgbs

    max_x = max(s.keys(), key=lambda k: k[0])[0]
    max_y = max(s.keys(), key=lambda k: k[1])[1]

    img = np.zeros([max_x + 1, max_y + 1, 4], dtype=np.float64)
    img.fill(0)
    for coords, rgbs in s.items():
        img[coords[0]][coords[1]] = rgbs

    return Image.fromarray(np.uint8(img * 255))


def getImageHash(image):
    return {
        "phash  ": imagehash.phash(image),
        "phash_s": imagehash.phash_simple(image),
        "whash  ": imagehash.whash(image)
    }


def _main():
    flags = loadScans()
    hashes = {}
    for flag, im in flags.items():
        print(flag)
        hashes[flag] = getImageHash(loadImage(im))

    flag = 'france'
    for key, h in hashes.items():
        print()
        print(f"{flag} vs {key}")
        for hashname, hashval in h.items():
            print(
                f"{hashname}: {hashes[flag][hashname]} {hashval} {hashes[flag][hashname] - hashval}")


def main():
    # pickle hashes
    flags = loadScans()
    hashes = {}
    for flag, im in flags.items():
        print(flag)
        hashes[flag] = imagehash.phash(loadImage(im))

    print(hashes)
    with open("hashes.pickle", 'wb') as f:
        pickle.dump(hashes, f)

# def main():
#     flags = loadScans()
#     im1 = loadImage(flags['finland3'])
#     im2 = loadImage(flags['finland2'])

#     im1_hash = imagehash.phash(im1)
#     im2_hash = imagehash.phash(im2)
#     print(im1_hash, im2_hash, im1_hash - im2_hash)

#     im2_hash = imagehash.phash_simple(im2)
#     print(im1_hash, im2_hash, im1_hash - im2_hash)

#     im1_hash = imagehash.whash(im1)
#     im2_hash = imagehash.whash(im2)
#     print(im1_hash, im2_hash, im1_hash - im2_hash)


if __name__ == "__main__":
    matplotlib.use("TkAgg")

    main()
