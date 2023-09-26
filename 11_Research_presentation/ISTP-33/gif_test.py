from PIL import Image
import glob
import os

from matplotlib.pyplot import close

files = sorted(glob.glob("data/data_3d/*.png"))
images = list(map(lambda file: Image.open(file), files))
images[0].save(
    "results/3d.gif",
    save_all=True,
    append_images=images[1:],
    duration=500.00,
    loop=0,
)
