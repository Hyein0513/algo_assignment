// 상황에 따라서 quick,heap, insertion 번갈아서 사용
// time : O(n * log n) : 
// space : O(log n) : 재귀를 이만큼 함

#include <vector>
#include <cmath>

using namespace std;

// insertion : 특정 구간 사이의 리스트를 insertion으로 정렬하는
void insertion_sort(vector<int>& list, int left, int right) {
    for (int i = left + 1; i <= right; ++i) {
        int val = list[i];
        int j = i - 1;
        while (j >= left && list[j] > val) {
            list[j + 1] = list[j];
            j--;
        }
        list[j + 1] = val;
    }
}

// heapify
void heapify_2(vector<int>& list, int n, int i) {
    int largest = i;
    int left_child = (2 * i) + 1;
    int right_child = (2 * i) + 2;

    if ((left_child < n) && (list[left_child] > list[largest])) {
        largest = left_child;
    }
    if ((right_child < n) && (list[right_child] > list[largest])) {
        largest = right_child;
    }

    if (largest != i) {
        swap(list[i], list[largest]);
        heapify_2(list, n, largest);
    }
}

// heap_sort : 구간 정해서 진행 
void heap_sort(vector<int>& list, int left, int right) {
    int n = right - left + 1;

    // Max Heap 만들기
    for (int i = (left + n / 2 - 1); i >= left; --i) {
        heapify_2(list, right + 1, i);
    }

    // 정렬
    for (int i = right; i > left; --i) {
        swap(list[left], list[i]);
        heapify_2(list, i, left);
    }
}

// partition
int partition(vector<int>& list, int low, int high) {
    // 맨 마지막 값을 pivot으로 잡고 
    int pivot = list[high];
    int i = low;

    for (int j = low; j < high; ++j) {
        // 쭉 훑으면서 pivot 보다 작으면 앞으로 보내고 
        if (list[j] <= pivot) {
            swap(list[i], list[j]);
            i++;
        }
    }
    // pivot 보다 작은 수 | i | pivot보다 큰 수| pivot 상태니까 
    // pivot을 i자리로 swap 
    swap(list[i], list[high]);
    return i;
}

// introsortion : left~rignt 구간별로 나눠서 정렬 
void introsortion(vector<int>& list, int left, int right, int depth_limit) {
    int size = right - left + 1;

    // 작으면 inseriton 사용
    if (size < 16) {
        insertion_sort(list, left, right);
        return;
    }

    // 너무 depth가 깊어지면 heap_sort
    if (depth_limit == 0) {
        heap_sort(list, left, right);
        return;
    }

    // partition 호출해서 구간 사이에서 pivot 찾고, 
    int pivot = partition(list, left, right);
    // pivot 기반으로 잘라서 양쪽 재귀(heap이든 insertion이든 quick이든)
    // 재귀 호출 마다 depth_limit 줄어들기 
    introsortion(list, left, pivot - 1, depth_limit - 1);
    introsortion(list, pivot + 1, right, depth_limit - 1);
}

// introsort sort : introsort_sort 본체 
vector<int> introsort_sort(vector<int> list) {
    int depth_limit = 2 * log2(list.size());
    introsortion(list, 0, list.size() - 1, depth_limit);

    return list;
}

