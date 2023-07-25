#include <iostream>
#include <vector>

// Function to label the 2D array
void labelComponents(std::vector<std::vector<int>> &arr)
{
    int label = 2; // Start labels from 2 (1 is for unlabeled objects, 0 is for background)
    int rows = arr.size();
    int cols = arr[0].size();

    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            if (arr[i][j] == 1)
            { // If an unlabeled object is found
                std::vector<std::pair<int, int>> stack = {{i, j}};
                arr[i][j] = label;
                while (!stack.empty())
                {
                    auto [x, y] = stack.back();
                    stack.pop_back();

                    // Check the 8-neighbourhood
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