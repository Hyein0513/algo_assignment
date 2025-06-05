#include "annealing.h"
#include "../utils/utils.h"

// 되나...? 일단 쓰고 여쭤보기
#include <vector>

using namespace std;

const int INF = 1000000000;


/*
    일단 처음에는 mst나 greedy 기반으로 초기 해를 잡고 
    2-opt나 swap insert 사용해서 조금씩 최적화 
    만약 바꿨는데 나쁜 해다? 그래도 ㄱㅊ음 확률적으로 습득 함 
    좋은 해를 확률적으로 탐색하며 최적해에 가까운 경로를 찾는거 

*/
vector<int> solve_annealing(const vector<point>& coord) {



}