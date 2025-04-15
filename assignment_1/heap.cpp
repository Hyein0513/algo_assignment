#include <vector>
using namespace std;

// list 가 heap 배열, n이 내가 max heap 만들어야 하는 갯수, i가 list에서 시작 index
void heapify(vector<int>& list, int n, int i) {
    int largest = i;
    int left_child = (2 * i) + 1;
    int right_child = (2 * i) + 2;

    // 만약 왼쪽 자식 index가 내가 고려해야할 max heap list 인덱스보다 작고
    // 왼쪽 자식 val 크기가 root val 보다 크다면
    if ((left_child < n) && (list[left_child] > list[largest])) {
        largest = left_child;
    }

    // 오른쪽 자식이 더 크면 업데이트 
    if ((right_child < n) && (list[right_child] > list[largest])) {
        largest = right_child;
    }

    // root, left_child, right_child 중에서 제일 큰 것이 root가 아니라면 swap 
    // 만약에 swap 사항이 있으면 재귀 호출
    if (largest != i) {
        // swap 처리 하고 
        int a = list[i], b = list[largest];
        list[i] = b;
        list[largest] = a;
        // 재귀 호출 :  swap이 일어난 자식 인덱스인 largest 에서 heapify 재귀 호출 
        heapify(list, n, largest);
    }

    return;
}

// heap sort 함수 
vector<int> heap_sort(vector<int> list) {
    // list 요소의 수가 n이고
    int n = list.size();

    // internal node 리스트를 순회하면서 밑에서 부터 list 전체를(n개를) 차곡차곡 max_heap 만들기 
    int inter_index = (n / 2) - 1;
    for (int i = inter_index; i >= 0; --i) {
        heapify(list, n, i);
    }

    // list 의 front를 하나씩 꺼내서 정렬
    for (int i = n - 1; i > 0; --i) {
        // root(=largest) 랑 맨 뒤랑 교체
        int c = list[0], d = list[i];
        list[0] = d;
        list[i] = c;
        // heapify 로 root 부터 정리
        heapify(list, i, 0);
    }

    return list;
}
