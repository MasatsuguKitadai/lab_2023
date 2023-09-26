from PIL import Image
import glob
import os

from matplotlib.pyplot import close

files = sorted(glob.glob('images/*.bmp'))
images = list(map(lambda file: Image.open(file), files))
images[0].save('result/result.gif', save_all=True,
               append_images=images[1:], duration=100.00, loop=0)
