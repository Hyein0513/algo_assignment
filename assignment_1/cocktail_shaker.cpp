// bubble sort 처럼 앞으로 끼워넣되, 작은쪽->큰쪽, 큰쪽->작은쪽 해서 양쪽으로 정렬
// time : O(n^2) : 앞 뒤로 왔다갔다 하니까
// space : O(1) : 제자리 정렬

#include <iostream>
#include <vector>

using namespace std;

// Cocktail Shaker Sort 함수
vector<int> cocktail_shaker_sort(vector<int> list) {
    // 인덱스 잡고 
    int start = 0;
    int end = list.size() - 1;
    // 교체 되었는지 아닌지
    bool swapped = true;

    // 더이상 swapped 할 게 없으면 정렬 다 된거니까 -> return
    while (swapped) {
        swapped = false;

        // 왼쪽부터 가면서 큰 값을 오른쪽 끝으로 보내기
        for (int i = start; i < end; i++) {
            // 만약 왼쪽에 더 큰 값이 있으면 swap 하기 -> 제일 큰 값을 맨 오른쪽으로 보내기
            if (list[i] > list[i + 1]) {
                swap(list[i], list[i + 1]);
                swapped = true;
            }
        }

        // 왼->오 갈 때, 이미 정렬된 경우 종료
        if (!swapped)
            break;

        // 맨 오른쪽에 제일 큰 값이 있으니까 end 줄이고
        end--;
        swapped = false;

        // 오른쪽부터 가면서 작은 값을 왼쪽으로 보내기
        for (int i = end - 1; i >= start; i--) {
            if (list[i] > list[i + 1]) {
                swap(list[i], list[i + 1]);
                swapped = true;
            }
        }

        // 맨 왼쪽에 제일 작은 값이 있으니까 start 늘리고
        start++;
    }

    return list;
}