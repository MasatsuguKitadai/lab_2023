#include <iostream>
#include <vector>
#include <unordered_set>

// Point構造体（ポイントを表すデータ構造）
struct Point
{
    double x;
    double y;
    // 他の次元を追加する場合はここに追加
};

// クラスタを表す構造体
struct Cluster
{
    std::vector<Point> points;
};

// ε-neighborhood内のポイントを見つける関数
std::vector<Point> regionQuery(const std::vector<Point> &dataset, const Point &p, double eps)
{
    std::vector<Point> neighbors;
    for (const Point &q : dataset)
    {
        // ポイントpからの距離がε以下の場合は、neighborsに追加
        double distance = std::sqrt((p.x - q.x) * (p.x - q.x) + (p.y - q.y) * (p.y - q.y));
        if (distance <= eps)
        {
            neighbors.push_back(q);
        }
    }
    return neighbors;
}

// クラスタを拡張する関数
void expandCluster(const std::vector<Point> &dataset, Point &p, std::vector<Point> &neighborPts, Cluster &cluster, double eps, int minPts)
{
    cluster.points.push_back(p); // ポイントpをクラスタに追加
    for (Point &q : neighborPts)
    {
        if (!q.visited)
        {
            q.visited = true;
            std::vector<Point> neighborPts_q = regionQuery(dataset, q, eps);
            if (neighborPts_q.size() >= minPts)
            {
                // NeighborPts_qとneighborPtsを結合
                neighborPts.insert(neighborPts.end(), neighborPts_q.begin(), neighborPts_q.end());
            }
        }
        if (!q.clustered)
        {
            q.clustered = true;
            cluster.points.push_back(q);
        }
    }
}

// DBSCANアルゴリズムを実行する関数
std::vector<Cluster> DBSCAN(const std::vector<Point> &dataset, double eps, int minPts)
{
    std::vector<Cluster> clusters;
    for (Point &p : dataset)
    {
        if (p.visited)
        {
            continue;
        }
        p.visited = true;
        std::vector<Point> neighborPts = regionQuery(dataset, p, eps);
        if (neighborPts.size() < minPts)
        {
            // ノイズとしてマーク
            continue;
        }
        // 新しいクラスタを作成
        clusters.push_back(Cluster());
        Cluster &cluster = clusters.back();
        expandCluster(dataset, p, neighborPts, cluster, eps, minPts);
    }
    return clusters;
}

int main()
{
    // テスト用データセットを作成（適切なデータを用意してください）
    std::vector<Point> dataset = {
        {1.0, 2.0},
        {2.0, 3.0},
        {3.0, 4.0},
        // データを追加
    };

    double eps = 1.0; // εパラメータの値
    int minPts = 3;   // MinPtsパラメータの値

    // DBSCANアルゴリズムを実行
    std::vector<Cluster> clusters = DBSCAN(dataset, eps, minPts);

    // 結果を出力（クラスタごとに表示）
    for (size_t i = 0; i < clusters.size(); ++i)
    {
        std::cout << "Cluster " << i + 1 << ":\n";
        for (const Point &p : clusters[i].points)
        {
            std::cout << "(" << p.x << ", " << p.y << ")\n";
        }
        std::cout << "====================\n";
    }

    return 0;
}
