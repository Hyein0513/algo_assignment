#include "MST.h"
#include "../utils/utils.h"

// 되나...? 일단 쓰고 여쭤보기
#include <vector>

using namespace std;

// 전역 변수 선언
vector<int> tour;             // DFS 경로 저장용
vector<bool> visited;         // DFS 방문 체크
vector<vector<int>> mst_adj;  // MST 인접 리스트

const int INF = 1000000000;

// 일단 정확한 mst를 생성하고
// dfs 로 순회해서 tsp 경로 생성하기

// dfs 함수 : 재귀 
void dfs(int u){
    // u를 방문했다고 표시
    visited[u] = true;
    // tour 에 넣고 
    tour.push_back(u);
    // u랑 연결된 도시의 수 mst_adj[u].size() 중에서 아직 방문 안한 도시가 있다면 그 도시로 dfs 타고 계속 타고 들어감
    for (int i = 0 ; i < mst_adj[u].size() ; ++i){
        // u랑 연결된 애들 중 i번째 node를 골라서 j로 설정하고 
        int j = mst_adj[u][i];
        // 만약 j가 방문한 적 없다면 dfs(j)로 재귀해서 
        if (!visited[j]){
            dfs(j);
        }
    }
}

// mst 기반 solve
vector<int> solve_MST(const vector<point>& coord){
    // 노드의 갯수를 구해서 저장해놓고 
    int n = coord.size();
    // 일단 초기화 
    // mst 를 저장할 인접 리스트 초기화
    mst_adj = vector<vector<int>>(n);
    // dfs 에서 방문 체크용으로 일단 false 초기화하고
    visited = vector<bool>(n, false);
    // tour dfs 경로 저장용
    tour.clear();
    // i번째 노드가 mst에 포함되어 있는지 아닌지
    vector<bool> in_tree(n, false);
    // 각 노드까지의 최소 거리 
    vector<int> key(n, INF);
    // 연결된 부모 노드 
    vector<int> parent(n, -1);
    

    // 0번 노드부터 시작 
    key[0] = 0;
    // 시작점은 부모가 없음 
    parent[0] = -1;


    // prim 알고리즘 사용 : mst 구성
    for (int i = 0 ; i < n ; ++i){
        // 각 node마다, 연결된 node중 제일 거리가 짧은 node를 찾아야하니까 기준점인 min_key를 최댓값으로 잡아 놓고 
        int min_key = INF;
        // tree에 추가하게 될, 기준 노드에서 최소 거리를 가진 node
        int u = -1;

        // 모든 노드를 순회하면서 아직 mst에 포함되지 않은 노드 중 최소 key(거리)를 가진 node 선택 
        for (int j = 0 ; j < n ; ++j){
            if (!in_tree[j] && (key[j] < min_key)){
                // min_key 업데이트
                min_key = key[j];
                // min_key 가지고 있는 인덱스 저장
                u = j;
            }
        }

        // 만약 선택된 노드 없으면 다 된거니까 종료
        if (u == -1)    
            break;

        // 선택된 node인 u를 mst에 포함 
        in_tree[u] = true;

        // 양방향으로 간선을 mst에 추가 
        // -1 이면 mst에 들어온 적이 없는 것 
        // u랑 parent[u]랑 연결하는거임 
        // 만약 맨 처음 node를 추가하는 과정이라면 parent[u] = -1 (부모 없음)
        // 두번째~마지막 node를 추가하는 과정이라면 parent[u] 는 미리 정해져있음 연결이 제일일 작은 값으로 밑에서  
        if (parent[u] != -1){
            // 서로에게 추가
            mst_adj[u].push_back(parent[u]);
            mst_adj[parent[u]].push_back(u);
        }

        // 방금 mst에 넣은 u 기준 key값(거리), 가장 작은 key값을 가진 node찾아서 parent 업데이트
        for (int k = 0 ; k < n ; ++k){
            // 모든 node를 싹 확인하면서 
            // k가 mst에 들어가지 않았고, mst에 방금 들어간 u랑 다른 노드와의 거리가 원래 k의 key값보다 작을 떄 업뎃뎃 -> min값일 가능성이 있을 때 
            int dist = calculate_distance(coord[u], coord[k]);
            if (!in_tree[k] && (dist < key[k])){
                key[k] = dist;
                // u랑 제일 짧은 거리로 연결되어있는 k에 대해서 parent를 u로 설정 
                parent[k] = u;
            }
        }
    }
    // dfs 순회로 tsp 경로 구성
    dfs(0);
    // 처음으로 다시 돌아가는 경로 추가 
    tour.push_back(0);

    // 완성된 순환 경로 반환
    return tour;
}