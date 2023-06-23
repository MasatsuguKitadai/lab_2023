import cv2
import os

name = input("Case Name:")
# dir_path = '/mnt/d/kitadai/03_numerical_simulation/'
# dir_path = "/mnt/e/workspace_SSD/03_numerical_simulation/"  # 研究室
dir_path = '/mnt/d/workspace_HDD/03_numerical_simulation/'  # 自宅
num = 100

# 前方LLSを通過する粒子像の変換
os.makedirs(dir_path + name + "/LLS_1/particle_image_png", exist_ok=True)
for i in range(1, num + 1):
    img = cv2.imread(dir_path + name +
                     "/LLS_1/particle_image_bmp/" + str(i) + ".bmp")
    cv2.imwrite(dir_path + name + "/LLS_1/particle_image_png/" +
                str(i) + ".png", img)

# 後方LLSを通過する粒子像の変換
os.makedirs(dir_path + name + "/LLS_2/particle_image_png", exist_ok=True)
for i in range(1, num + 1):
    img = cv2.imread(dir_path + name +
                     "/LLS_2/particle_image_bmp/" + str(i) + ".bmp")
    cv2.imwrite(dir_path + name + "/LLS_2/particle_image_png/" +
                str(i) + ".png", img)
