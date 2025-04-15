#include <vector>
using namespace std;

// list의 front부터 비교를 시작해서 한번 for문을 돌 때 마다 제일 큰 값이 맨 뒤로 가는
// time : O(n^2) : n 개의 요소에 대해 각각 싹다 비교하니까 n*n임
// space : O(1) : 있는 list 사용

vector<int> bubble_sort(vector<int> list) {
    int n = list.size();

    // 이 for문이 한바퀴 돌 때마다, i 개의 요소가 list 뒤쪽에서부터 정렬됨 
    for (int i = 0; i < n; ++i) {
        bool flag = false;

        // 앞뒤 비교하면서 진행 
        // 뒤에서부터 i개는 이미 정렬되어있는거니까
        for (int j = 0; j < n - i - 1; ++j) {
            // 만약 앞에 있는 val이 더 크면 swap
            if (list[j] > list[j + 1]) {
                int a = list[j], b = list[j + 1];
                list[j] = b;
                list[j + 1] = a;
                // 바뀌었으니까 flag True 로 변환
                flag = true;
            }
        }

        // 한바퀴를 돌았을 때 아무것도 바뀌지 않았다면 정렬 완료된 것 
        // n번 돌 필요 없이 중간에 중단
        if (flag == false) {
            break;
        }
    }

    return list;
}
