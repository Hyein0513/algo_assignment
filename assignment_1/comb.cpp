// 멀리 떨어진 놈들끼리 먼저 비교해서 맨 끝으로 큰놈을 보내고, 점점 gap을 작게 만들어서 bubble 처럼 작동하게 만듬 
// time : O(n^2) : bubble sort처럼 작동하는데 좀 gap이 큰것 뿐이니까
// space : O(1) : 제자리 정렬임

#include <iostream>
#include <vector>

using namespace std;

// comb_sort
vector<int> comb_sort(vector<int> list) {
    // gap 설정
    int n = list.size();
    int gap = n;
    const double shrink = 1.3;
    bool swapped = true;

    // gap이 1보다 작아지거나, swapped되지 않을 때 종료
    while (gap > 1 || swapped) {
        gap = (int)(gap / shrink);
        // gap 이 1보다 작아지면 걍 1로 설정정
        if (gap < 1) gap = 1;

        swapped = false;

        // arr[i]와 arr[i + gap]을 비교
        for (int i = 0; i + gap < n; i++) {
            if (list[i] > list[i + gap]) {
                swap(list[i], list[i + gap]);
                swapped = true;
            }
        }
    }

    return list;
}
