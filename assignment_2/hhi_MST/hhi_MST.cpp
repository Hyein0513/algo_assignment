#include "hhi_MST.h"
#include "../utils/utils.h"
#include "../hhi_greedy/hhi_greedy.h"

#include <vector>
#include <cmath>

using namespace std;

const int INF = 1000000000;

vector<int> tour_angle;
vector<bool> visited_angle;
vector<vector<int>> mst_adj_angle;

// 🔁 DFS (정렬 없이 순서대로 순회)
void dfs_angle_penalty(int u) {
    visited_angle[u] = true;
    tour_angle.push_back(u);

    for (int v : mst_adj_angle[u]) {
        if (!visited_angle[v]) {
            dfs_angle_penalty(v);
        }
    }
}

// 🎯 MST 구성 시 score = 거리 + λ × max(0, (angle - 90도))
vector<int> solve_hhi_MST(const vector<point>& coord, double lambda) {
    int n = coord.size();

    tour_angle.clear();
    visited_angle.assign(n, false);
    mst_adj_angle.assign(n, vector<int>());

    vector<bool> in_tree(n, false);
    vector<double> key(n, 1e9);
    vector<int> parent(n, -1);
    vector<point> prev_dir(n, {1.0, 0.0});  // 초기 방향

    key[0] = 0.0;

    for (int i = 0; i < n; ++i) {
        int u = -1;
        double min_key = 1e9;
        for (int j = 0; j < n; ++j) {
            if (!in_tree[j] && key[j] < min_key) {
                min_key = key[j];
                u = j;
            }
        }

        if (u == -1) break;
        in_tree[u] = true;

        if (parent[u] != -1) {
            int p = parent[u];
            mst_adj_angle[u].push_back(p);
            mst_adj_angle[p].push_back(u);
        }

        for (int v = 0; v < n; ++v) {
            if (!in_tree[v]) {
                double dist = calculate_distance(coord[u], coord[v]);
                point dir = unit_vector(coord[u], coord[v]);

                double cosine = cosine_similarity(prev_dir[u], dir);
                if (cosine < -1.0) cosine = -1.0;
                else if (cosine > 1.0) cosine = 1.0;

                double angle = acos(cosine); // 라디안
                double angle_penalty = (angle <= M_PI / 2) ? 0.0 : (angle - M_PI / 2);
                double score = dist + lambda * angle_penalty;

                if (score < key[v]) {
                    key[v] = score;
                    parent[v] = u;
                    prev_dir[v] = dir;
                }
            }
        }
    }

    dfs_angle_penalty(0);
    tour_angle.push_back(0);  // 순환 경로 마무리

    return tour_angle;
}
