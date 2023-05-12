import cv2
import settings
import parameters

file_path = str(settings.dirpath) + str(parameters.dataname)

## png に変換 ##
img = cv2.imread(file_path + '/binarization/8bit.bmp',
                 cv2.IMREAD_GRAYSCALE)
# img_rotate = cv2.rotate(img, cv2.ROTATE_90_CLOCKWISE)
cv2.imwrite(file_path + '/binarization/8bit.png', img)
