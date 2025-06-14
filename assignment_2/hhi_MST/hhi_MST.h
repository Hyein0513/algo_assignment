#ifndef HHI_MST_H
#define HHI_MST_H

#include <vector>
#include "../utils/utils.h"


// hhi_MST.h
void dfs_angle_penalty(int u, int parent, const std::vector<point>& coord, double lambda, const point& prev_dir);
std::vector<int> solve_hhi_MST(const std::vector<point>& coord, double lambda);

#endif  // HHI_MST_H