#ifndef MST_H
#define MST_H

#include <vector>
#include "../utils/utils.h"

// mst 방식의 tsp 해결 함수 
// input : dist[i][j] : i랑 j node 사이의 거리를 측정한 거리 행렬
// output vector<int> 형태의 순환 경로 
std::vector<int> solve_MST(const std::vector<point>& coord);

#endif  // MST_H