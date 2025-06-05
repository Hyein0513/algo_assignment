#include "held_karp.h"
#include "../utils/utils.h"

// 되나...? 일단 쓰고 여쭤보기
#include <vector>

using namespace std;

// 다이나믹 프로그래밍임 
// 지금까지 방문한 node의 집합인 s랑 현재 node i를 기준으로 최소 비용을 저장해나가는 함수 

/*  비트 마스크라는걸 씀 
    node가 4개(0, 1, 2, 3)일 때,
    0   방문    비트 1
    1   방문    비트 1
    2   미방문  비트 0
    3   방문    비트 1
    따라서 비트마스크는 1101(10진수로 13)

    현재까지 방문한 node들의 집합을 비트 마스크로 표현
*/
const int INF = 1000000000;

vector<int> solve_held_karp(const vector<point>& coord){
    // 일단 size 받아오고 
    int n = coord.size();
    // 전체 node 집합의 비트 마스크 표현 크기 
    // n개만큼의 2진수 자리를 만들어줌
    int full = 1 << n;

    // dp[mask][i] = node 집합 mask를 방문했고 마지막 node가 i일때 최소 비용을 저장함
    vector<vector<int>> dp(full, vector<int>(n, INF));
    // parent[mask][i] = dp[mask][i]인 상황에서 이전 node 기록 (경로 복원용)
    vector<vector<int>> parent(full, vector<int>(n, -1));
    
    // 0번 node만 방문
    // mask = 1임, 비용은 0
    dp[1][0] = 0;

    // 모든 node 집함 mask를 싹 돌아보면서 
    for (int mask = 1 ; mask < full; ++mask){
        // mask내의 각 node i를 마지막으로 방문했다고 가정하고 가지치기 
        for (int i = 0 ; i < n ; ++i){
            // 만약 mask 안에 i가 포함되어 있지 않다면 건너뛰기 
            // 1(방문했음)을 i번 shift 해서 mask에 사용할 수 있게 바꾼 다음
            // mask랑 and 연산으로 처리해서 만약 u 가 mask안에 있다면 1 & 1 = 1 이 되도록해서 전체 mask가 1로만 이루어지게 
            if (!(mask & (1 << i)))
                continue;

            // mask에서 i에 도달하기 전에 있었을 수 있는 node j를 탐색하는데 
            for ( int j = 0 ; j < n ; ++j){
                // 만약 j == i 이거나 j가 mask에 없으면 건너뜀 
                if ((i == j) || !(mask & (1 << j)))
                    continue;
                
                // 그럼 mask안에 i랑 j가 둘 다 존재한다면(mask의 size=2인 모든 부분집합에 대해서)
                // 이전 상태 mask에서 i를 뺸 상태를 prev_mask에 저장(j까지만 방문한 상태) : xor 연산임임
                int prev_mask = mask ^ (1 << i);
                // j까지 오는데 든 비용(j까지의 최소비용)+ j에서 i로 오는 거리 -> 이 케이스(mask, i)에서의 i까지 오는데 최소 비용용 
                int new_cost = dp[prev_mask][j] + calculate_distance(coord[j], coord[i]);

                // 지금의 경우에서(mask, i) 찾은 new_cost가 지금까지 찾은 최소 비용보다 작으면 업데이터 
                if (new_cost < dp[mask][i]){
                    // 최소 비용 저장하고
                    dp[mask][i] = new_cost;
                    // 이전 node 기록에도 저장하고 
                    parent[mask][i] = j;
                }
            }
        }
    }

    // 이렇게 mask의 모든 경우의 수에 대해서 최소 비용을 구했다면
    // dp[mask][i] 안에는 mask를 방문하고 마지막으로 i를 방문하는 경우에 얻을 수 있는 최소 비용이 들어있을것임
    // 전체 node 방문(mask = full - 1) 후 출발 node로 돌아가는 최소 비용 찾기 
    int min_cost = INF;
    int last = -1;
    // 출발 node로 돌아가는 최소 비용이 뭔지 for 문으로 탐색
    for (int k = 0 ; k < n ; ++k){
        // 돌아가는 비용 포함해서 cost 업데이트
        int cost = dp[full- 1][k] + calculate_distance(coord[k], coord[0]);
        // dp[full] 인 경우를 계산하기 위해서 min으로 비교 
        if (cost < min_cost) {
            // min cost 업데이트
            min_cost = cost;
            // 마지막으로 돌아가야하는 출발점(최소) 업데이트
            last = k;
        }
    }

    // 이제 tsp의 최적 경로를 복원
    // path 잡고 
    vector<int> path;
    // 모든 node 방문한 상태일떄, 
    int mask = full - 1;
    // 마지막 node부터 거꾸로 추적 시작 
    int cur = last;
    // while문으로 싹 돌면서 역순으로 path에 저장
    while (cur != -1){
        // 현재 node 추가(맨 처음에는 맨 마지막 node(출발점으로 돌아왔을 때 그거))
        path.push_back(cur);
        // 최소 비용을 가지는 경로에서 cur을 방문하기 직전 node 찾고 
        int prev = parent[mask][cur];
        // 현재 node를 mask에서 제거해서 mask갱신(xor 연산)
        mask = mask ^ (1 << cur);
        // 다음 루프에서 추적할 node 갱신
        cur = prev;
    }
    // 시작 node도 포함시킴(0번에서 출발했으니까)
    // path.push_back(0);
    
    // 굳이? 이거 빼 말 빼 말 gt랑 비교해야함
    // path에는 경로가 역순으로 저장되어있으니까 순서대로 정리 해야하나 굳이?
    vector<int> result;
    for (int i = path.size() - 1 ; i >= 0 ; --i){
        result.push_back(path[i]);
    }

    // result 반환
    return result;
}   