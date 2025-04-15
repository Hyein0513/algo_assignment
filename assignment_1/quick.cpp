#include <vector>
using namespace std;

// pivot 을 고르고 작은거 왼쪽 큰거 오른쪽 해서 나누는 분할 재귀
// time : O(n * log n) : log n번의 재귀 호출이 있으니까 * 호출마다 n개 훑으니까
// space : O(log n) : 재귀 깊이 대로 새 list 만들어야 하니까

#include <vector>
using namespace std;

// 내부 재귀 함수 (in-place)
void quick_sort_recursive(vector<int>& list, int low, int high) {
    if (low >= high) return;

    int mid = (low + high) / 2;
    int pivot = list[mid];  // pivot은 중간값, 위치 고정 X

    int left = low;
    int right = high;

    while (left <= right) {
        while (list[left] < pivot) ++left;
        while (list[right] > pivot) --right;

        if (left <= right) {
            swap(list[left], list[right]);
            ++left;
            --right;
        }
    }

    // 재귀 호출 (pivot 자체는 고정되지 않아도 돼)
    quick_sort_recursive(list, low, right);
    quick_sort_recursive(list, left, high);
}

// 외부 함수 (list → list_copy 복사)
vector<int> quick_sort(vector<int> list) {
    vector<int> list_copy = list;
    quick_sort_recursive(list_copy, 0, list_copy.size() - 1);
    return list_copy;
}
