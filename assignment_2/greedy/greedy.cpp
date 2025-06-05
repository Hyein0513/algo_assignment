#include "greedy.h"
#include "../utils/utils.h"

// 되나...? 일단 쓰고 여쭤보기
#include <vector>

using namespace std;

/*
    아직 방문하지 않은 노드 중에서 제일 가까운 노드 선택해서 갱신갱신갱신
    빠르고 정확도 망함
*/

const int INF = 1000000000;

vector<int> solve_greedy(const vector<point>& coord) {
    // 초기화 하고 사이즈 재고
    int n = coord.size();
    // 노드에 대해서 방문 여부 저장
    vector<bool> visited(n, false); 
    vector<int> tour;

    // 0번 노드부터 시작 
    int cur = 0;

    // 일단 시작 노드 넣고 시작 
    tour.push_back(cur);
    // 시작 노드 방문했다고 표시
    visited[cur] = true;

    // 모든 node를 체크하면서
    for (int step = 1; step < n; ++step) {
        // 다음에 방문할 도시의 인덱스 
        int next = -1;
        // 초기화 
        int min_dist = INF;

        // 방문하지 않은 node중 현재 node에서 가장 가까운 node 선택
        for (int i = 0; i < n; ++i) {
            // 모든 node중 아직 방문하지 않은 node 찾아서 
            if (!visited[i]) {
                int dist = calculate_distance(coord[cur], coord[i]);
                // 가장 가까운 node 찾기 
                if (dist < min_dist) {
                    min_dist = dist;
                    next = i;
                }
            }
        }

        // 가장 가까운 node 방문
        cur = next;
        // 방문했다고 표시하고 
        visited[cur] = true;
        // 경로에 넣고
        tour.push_back(cur);
    }

    // 마지막에 시작으로 복귀
    tour.push_back(0);

    return tour;
}
