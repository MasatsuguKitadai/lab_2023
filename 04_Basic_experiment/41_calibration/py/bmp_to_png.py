import cv2

# フォルダ名の読み込み
name = input('Data Name:')

dirpath = "/mnt/e/workspace_SSD/04_basic_experiment/"
file_path = str(dirpath) + str(name)

## png に変換 ##
img = cv2.imread(file_path + '/41_calibration/binarization/calibration_8bit.bmp',
                 cv2.IMREAD_GRAYSCALE)
# img_rotate = cv2.rotate(img, cv2.ROTATE_90_CLOCKWISE)
cv2.imwrite(file_path + '/41_calibration/binarization/calibration_8bit.png', img)
