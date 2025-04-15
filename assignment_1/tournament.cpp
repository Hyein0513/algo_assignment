// 토너먼트처럼 모든 요소를 binary tree의 leaf 에 두고, 서로 비교해서 작은 값을 부모로 올려서, root에서 하나씩 빼내는 방식
// time : O(n * log n) : log n 번 호출 당하고 n개를 비교하고 
// space : O(n) : 트리 저장공간  

#include <iostream>
#include <vector>
#include <climits>
using namespace std;

// tournament_sort
vector<int> tournament_sort(vector<int> list) {
    // 트리 배열 크기 잡고 만들기 
    int n = list.size();
    int tree_size = 2 * n;
    // tree 내부 값은 걍 intmax 쳐놓고
    vector<int> tree(tree_size, INT_MAX);
    vector<int> result;

    // 트리 초기화 : leaf 에 모든 요소 집어넣기 
    for (int i = 0; i < n; ++i)
        tree[n + i] = list[i];

    // 트리 초기화 : leaf 쪽부터 타고 올라가면서 작은 val 부모로 올리기 
    for (int i = n - 1; i > 0; --i)
        tree[i] = min(tree[2 * i], tree[2 * i + 1]);

    // root값을 n번 추출해서 제일 작은 값부터 정렬 만들기 
    for (int i = 0; i < n; ++i) {
        int min_val = tree[1];
        result.push_back(min_val);

        // root에 올라가서 정렬 리스트로 빠진 최솟값이 어디 leaf에 있는지 찾기
        int j = 1;
        while (j < n) {
            // 왼쪽 자식이 부모랑 같으면 왼쪽 선택
            if (tree[2 * j] == tree[j])
                j = 2 * j;
            // 오른쪽 자식이 부모랑 같으면 오른쪽 선택
            else
                j = 2 * j + 1;
        }

        // 해당 리프를 INT_MAX로 바꾸고 거기가 바뀐거니까 거기부터 위로 갱신
        tree[j] = INT_MAX;
        while (j > 1) {
            j /= 2;
            tree[j] = min(tree[2 * j], tree[2 * j + 1]);
        }
    }

    return result;
}
