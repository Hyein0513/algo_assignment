#include "insertion.h"
#include "../utils/utils.h"

// 되나...? 일단 쓰고 여쭤보기
#include <vector>

using namespace std;

const int INF = 1000000000;

vector<int> solve_insertion(const vector<point>& coord) {
    // 일단 사이즈 재고 
    int n = coord.size();
    // node 방문 여부 체크 
    vector<bool> visited(n, false);
    // 경로 저장
    vector<int> tour; 

    // 시작 node: 0번
    tour.push_back(0);
    visited[0] = true;

    // 0번에서 가장 가까운 node를 찾아서 시작 경로를 [0, closest]로 만든다
    int closest = -1;
    int min_dist = INF;
    // node 싹 돌면서 최솟값 업데이트
    for (int i = 1; i < n; ++i) {
        int d = calculate_distance(coord[0], coord[i]);
        if (d < min_dist) {
            min_dist = d;
            closest = i;
        }
    }

    // 초기 경로는 [0 → closest → 0] (순환형 경로)
    tour.push_back(closest);
    tour.push_back(0);
    visited[closest] = true;

    // 모든 노드가 경로에 들어올 때까지 반복
    while (tour.size() < n + 1) {
        int next = -1;
        int best_dist = INF;

        // 아직 방문하지 않은 node 중에서 현재 경로에 있는 node마다 순회 돌면서 가장 가까운 node를 찾는다
        for (int i = 0; i < n; ++i) {
            // 방문했으면 뛰어넘고
            if (visited[i]) 
                continue;

            // 이 node i가 현재 경로에 있는 node(tour[j]) 중 뭐랑 가장 가까운지 확인
            for (int j = 0; j < tour.size(); ++j) {
                int tour_city = tour[j];
                int d = calculate_distance(coord[i], coord[tour_city]);
                // 최솟값 갱신
                if (d < best_dist) {
                    best_dist = d;
                    next = i;
                }
            }
        }

        // next를 tour 내 어디에 삽입할지 결정
        int best_pos = -1;
        int min_increase = INF;

        // tour에 삽입할 떄, 앞 node, 뒷node를 각각 a랑 b로 두고 
        for (int i = 0; i < tour.size() - 1; ++i) {
            int a = tour[i];
            int b = tour[i + 1];

            // a → next → b 로 삽입할 때 거리 증가 계산
            int increase =
                calculate_distance(coord[a], coord[next]) +
                calculate_distance(coord[next], coord[b]) -
                calculate_distance(coord[a], coord[b]);

            // 증가값의 최솟값 갱신해서 next를 넣었을 때 비용이 최소한으로 증가하는 위치 찾기(인덱스로 저장)
            if (increase < min_increase) {
                min_increase = increase;
                best_pos = i + 1;
            }
        }

        // 최적 위치(best_pos)에 next 삽입
        tour.insert(tour.begin() + best_pos, next);
        visited[next] = true;
    }

    return tour;
}