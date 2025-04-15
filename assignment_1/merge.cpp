#include <vector>
using namespace std;

// merge_sort function 제작
vector<int> merge_sort(vector<int> list) {
    // 만약 요소가 작으면 걍 반환
    if (list.size() <= 1) {
        return list;
    }

    // 중앙을 pivot으로 잡고 left랑 right로 나누기 
    int pivot = list.size() / 2;
    vector<int> left_list(list.begin(), list.begin() + pivot);
    vector<int> right_list(list.begin() + pivot, list.end());

    left_list = merge_sort(left_list);
    right_list = merge_sort(right_list);

    // 만약 정렬을 해야한다면, 새 리스트 하나 만들고
    vector<int> result;
    // i 는 left_list 의 index, j는 right_list 의 index
    int i = 0, j = 0;

    // 양쪽 list의 맨 앞끼리 비교해서 새 리스트에 집어넣기
    while ((i < left_list.size()) && (j < right_list.size())) {
        if (left_list[i] <= right_list[j]) {
            result.push_back(left_list[i]);
            i += 1;
        } else {
            result.push_back(right_list[j]);
            j += 1;
        }
    }

    // 한 list 가 다 추가된 다음에는 남은거 마지막에 싹 추가 
    result.insert(result.end(), left_list.begin() + i, left_list.end());
    result.insert(result.end(), right_list.begin() + j, right_list.end());

    return result;
}
