#ifndef HHI_greedy_H
#define HHI_greedy_H

#include <vector>
#include "../utils/utils.h"

point unit_vector(const point& a, const point& b);
double cosine_similarity(const point& a, const point& b);
std::vector<int> solve_hhi_greedy(const std::vector<point>& coord, double lambda);
std::vector<int> solve_hhi_greedy_2opt(const std::vector<point>& pts, double lambda0_factor);
std::vector<int> solve_hhi_greedy_basic_adapK(const std::vector<point>& pts, double lambda0_factor);
std::vector<int> solve_hhi_greedy_2opt_adapK(const std::vector<point>& pts, double lambda0_factor);
std::vector<int> solve_hhi_greedy_basic_adapK_9CellMean(const std::vector<point>& pts, double lambda0_factor);
std::vector<int> solve_hhi_greedy_2opt_adapK_9CellMean(const std::vector<point>& pts, double lambda0_factor);
std::vector<int> solve_hhi_greedy_basic_adapK_multi(const std::vector<point>& pts, double lambda0_factor);
std::vector<int> solve_hhi_greedy_2opt_adapK_multi(const std::vector<point>& pts, double lambda0_factor);
std::vector<int> solve_hhi_greedy_anglePenalty(const std::vector<point>& coord, double lambda);
std::vector<int> solve_hhi_greedy_linearPenalty(const std::vector<point>& coord, double lambda);
std::vector<int> solve_hhi_greedy_basic_adapK_hull(const std::vector<point>& pts, double lambda0_factor);
std::vector<int> solve_hhi_greedy_2opt_adapK_hull(const std::vector<point>& pts, double lambda0_factor);
std::vector<int> solve_hhi_greedy_basic_adapK_9CellMean_hull(const std::vector<point>& pts, double lambda0_factor);
std::vector<int> solve_hhi_greedy_2opt_adapK_9CellMean_hull(const std::vector<point>& pts, double lambda0_factor);
std::vector<int> solve_hhi_greedy_2opt_adapK_hull_multi(const std::vector<point>& pts, double lambda0_factor);

#endif  // HHI_greedy_H