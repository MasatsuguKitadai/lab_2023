/******************************************************************************
PROGRAM : Functions.hpp
AUTHER  : Masatsugu Kitadai
DATE    : 2023/7/25
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/stat.h>
#include <iostream>
#include <vector>
#include <cstdio>
#include <cstdlib>
using namespace std;

FILE *fp, *gp;
mode_t dirmode = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IXOTH | S_IXOTH;

/******************************************************************************
FUNCTION：Setting
IN      :
******************************************************************************/

/******************************************************************************
FUNCTION：Image_Thresholding
IN      : vector<vector<int>> &arr / 二値化前の二次元配列
OUT     : vector<vector<int>> &arr / 二値化後の二次元配列
******************************************************************************/
void Image_Thresholding(vector<vector<int>> &arr)
{
    int rows = arr.size();
    int cols = arr[0].size();

    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
        {
            /* code */
        }
}

/******************************************************************************
FUNCTION：Labeling
IN      : vector<vector<int>> &arr / ラベリング前の二値化した二次元配列
OUT     : vector<vector<int>> &arr / ラベリング後の数値の二次元配列
******************************************************************************/
void Labeling(vector<vector<int>> &arr)
{
    int label = 2; // Start labels from 2 (1 is for unlabeled objects, 0 is for background)
    int rows = arr.size();
    int cols = arr[0].size();

    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            if (arr[i][j] == 1)
            { // If an unlabeled object is found
                vector<pair<int, int>> stack = {{i, j}};
                arr[i][j] = label;
                while (!stack.empty())
                {
                    auto [x, y] = stack.back();
                    stack.pop_back();

                    // Check the 8 neighbourhood
                    for (int dx = -1; dx <= 1; dx++)
                        for (int dy = -1; dy <= 1; dy++)
                        {
                            int nx = x + dx;
                            int ny = y + dy;
                            if (nx >= 0 && nx < rows && ny >= 0 && ny < cols && arr[nx][ny] == 1)
                            {
                                stack.push_back({nx, ny});
                                arr[nx][ny] = label;
                            }
                        }
                }

                label++;
            }
}

/******************************************************************************
FUNCTION：Read_Bmp
IN      : const char *file_name              / 読み込むファイル名
        : unsigned char header[HEADER_SIZE]  / ヘッダーサイズ
        : vector<vector<unsigned char>> &arr / バイナリを格納する二次元配列
OUT     : vector<vector<unsigned char>> &arr / バイナリを格納する二次元配列
******************************************************************************/
void Read_Bmp(const char *file_name, vector<vector<unsigned char>> &arr, int mode)
{
    // Determine the header size
    int HEADER_SIZE;
    if (mode == 8)
    {
        HEADER_SIZE = 1078; // Header size of 8bit BMP file
    }
    else if (mode == 24)
    {
        HEADER_SIZE = 54 // Header size of 24bit BMP file
    }

    unsigned char header[HEADER_SIZE];

    // Open the bmp file in binary mode
    FILE *fp = fopen(file_name, "rb");
    if (fp == NULL)
    {
        printf("Not found : %s \n", file_name);
        exit(-1);
    }

    // Read the header information
    if (fread(header, sizeof(unsigned char), HEADER_SIZE, fp) != HEADER_SIZE)
    {
        printf("Failed to read header from: %s \n", file_name);
        exit(-1);
    }

    // Check if the file is a bitmap
    if (!(header[0] == 'B' && header[1] == 'M'))
    {
        printf("Not BMP file : %s \n", file_name);
        exit(-1);
    }

    unsigned char tmp;
    while (fread(&tmp, sizeof(unsigned char), 1, fp))
    {
        binary.push_back(tmp);
    }

    fclose(fp); // Close the file
}