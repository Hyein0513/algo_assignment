#include <vector>
#include <algorithm>
using namespace std;

// list 안에서 이미 정렬되어있는 조각을 찾아서 insertion sort 로 정렬
// time : O(n * log n) : 리스트를 각 구간으로 나누고 insertion sort -> O(n) 하구 log n 만큼 병합 
// space : O(n) : 병합 과정에서 추가 정렬이 필요  

// list 부분 구간 정렬 용으로 insertion sort 작성
void insertion_sort_2(vector<int>& list, int left, int right) {
    for (int i = left + 1; i <= right; ++i) {
        int key = list[i];
        int j = i - 1;
        while ((j >= left) && (list[j] > key)) {
            list[j + 1] = list[j];
            j -= 1;
        }
        list[j + 1] = key;
    }
}

// merge 
void merge(vector<int>& list, int l, int m, int r) {
    // 왼쪽 구간의 길이 
    int len1 = m - l + 1;
    // 오른쪽 구간의 길이
    int len2 = r - m;
    // 왼쪽 오른쪽은 정렬되어있으니까 새 리스트로 복사 하고
    vector<int> left(list.begin() + l, list.begin() + m + 1);
    vector<int> right(list.begin() + m + 1, list.begin() + r + 1);

    int i = 0, j = 0, k = l;

    // 항상 하던대로 merge 진행
    while ((i < left.size()) && (j < right.size())) {
        if (left[i] <= right[j]) {
            list[k] = left[i];
            i += 1;
        } else {
            list[k] = right[j];
            j += 1;
        }
        k += 1;
    }

    // 왼쪽 리스트가 남았을 경우 
    while (i < left.size()) {
        list[k] = left[i];
        i += 1;
        k += 1;
    }

    // 오른쪽 리스트가 남았을 경우
    while (j < right.size()) {
        list[k] = right[j];
        j += 1;
        k += 1;
    }
}

// tim sort 
vector<int> tim_sort(vector<int> list) {
    int n = list.size();
    int run = 32;

    // run 크기마다 insertion sort로 정렬 
    for (int i = 0; i < n; i += run) {
        insertion_sort_2(list, i, min(i + run - 1, n - 1));
    }

    // 정렬했으니 merge
    int size = run;
    while (size < n) {
        for (int left = 0; left < n; left += 2 * size) {
            int mid = left + size - 1;
            int right = min((left + 2 * size - 1), n - 1);
            if (mid < right) {
                merge(list, left, mid, right);
            }
        }
        // 사이즈를 계속 키워가면서 전체 배열 정렬 될 때까지 반복
        size = size * 2;
    }

    return list;
}
