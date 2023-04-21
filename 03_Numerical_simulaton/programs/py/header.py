# -*- coding:utf-8 -*-

import cv2
import numpy as np
dir_path = 'header/'

x = 800
y = 800
size = (x, y)

black_img_8bit = np.zeros(size, np.uint8)  # 8bit深度のフォーマット
cv2.imwrite(dir_path + str(x) + 'x' + str(y) + '_8bit.bmp', black_img_8bit)
