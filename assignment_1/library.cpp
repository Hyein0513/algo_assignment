#include <vector>
#include <optional>
#include <cmath>
#include <stdexcept>
#include <iostream>
using namespace std;


// sort 하면서 중간중간 빈칸을 두고, 값을 정렬해서 삽입할 때 여러개의 요소를 밀어낼 필요 없도록 진행 
// time : O(n log n) : 이진 탐색이니까 log n 이고 매번 n 번 훑으니까
// space : O(n) : 빈칸을 넣기 때문에 2-4배 정도의 여유 공간이 필요 

// 정렬 어느 인덱스에 넣어야하는지 binary_search
int binary_search(const vector<optional<int>>& blank_list, const vector<int>& val_index, int i) {
    // val_index에 들어있는 index를 기준으로 탐색
    // 인덱스 범위 잡고
    int head = 0;
    int tail = val_index.size() - 1;

    while (head <= tail) {
        // 일단 가운데 인덱스 구하고 
        int mid = (head + tail) / 2;
        // 이 인덱스의 실제 값이 몇인지 확인해서 비교  
        int mid_val = blank_list[val_index[mid]].value();
        // 만약 들어가야하는 값 i가 중간값보다 작으면 mid 왼쪽으로 가야함
        if (i < mid_val) {
            tail = mid - 1;
        }
        // 만약 들어가야하는 값 i가 중간값보다 크면 mid 오른쪽올 가야함
        else {
            head = mid + 1;
        }
    }
    // while문의 종료 시점이 head > tail 일때니까 head 는 항상 정확한 위치임
    return head;
}

// blank list 에서 가장 가까운 빈 칸의 인덱스를 찾고 반환하는 함수 
// 예외는 구현하지 않음
int find_gap(const vector<optional<int>>& blank_list, int val, int n) {
    // blank list의 한쪽 끝에서 다른 한쪽 끝까지 훑기
    // 이거 없애 
    int c ;
    for (int i = 1; i < 10 * n; ++i) {
        int right = val + i;
        int left = val - i;
        if ((right < blank_list.size()) && !blank_list[right].has_value()) {
            return right;
        }
        if ((left >= 0) && !blank_list[left].has_value()) {
            return left;
        }
        //이거 없없
        c=i;
    }
    
    cout <<"좆됨 "<<n << " " <<c<<" ";
    // 만약 근처에서 못찾음
    for (int i = 0; i < blank_list.size(); ++i) {
        if (!blank_list[i].has_value()) {
            return i;
        }
        cout <<i;
    }

    cout <<"\n";
    return -1; // 예외 상황 처리 안함
}

// library_sort
vector<int> library_sort(const vector<int> list) {
    // 빈칸이 포함되어있는 n*3(여유) 크기의 blank_list 만들기 
    int n = list.size();
    int size = 4 * n;
    vector<optional<int>> blank_list(size);
    // 실제로 값이 들어간 인덱스만 추적하는 빈 list
    vector<int> val_index;

    for (int i : list) {
        // 만약 처음 값을 넣는거라면
        if (val_index.empty()) {
            // 중앙에다가 삽입 -> 경우의 수를 줄이기 위함
            blank_list[size / 2] = i;
            // 실제로 값이 들어간 인덱스의 값을 삽입
            val_index.push_back(size / 2);
            continue;
        }

        // 처음 값을 넣는게 아니라면 binary search 로 넣어야할 index 찾기
        int index = binary_search(blank_list, val_index, i);

        // index를 기준으로 실제 blank_list에 빈칸 두고 삽입 할 수 있는 인덱스 위치를 고르기
        int val;
        // 만약 맨 오른쪽에 넣어야한다면(제일 값이 크다면)
        if (index == val_index.size()) {
            // 맨 마지막에 삽입되어있는 값의 인텍스에서 한칸 빈칸을 두고 뒤에
            // 이게나를죽였다 array 값 튀어나가지 않게 min으로 범위 잡기 미친 
            val = min(size - 2, val_index.back() + 2);
        }
        // 만약 맨 왼쪽에 넣어야한다면(제일 값이 작다면)
        else if (index == 0) {
            // 맨 처음 삽입되어있는 값의 인덱스에서 한칸 빈칸을 두고 앞에 
            // 이게나를죽였다22 array 값 튀어나가지 않게 max으로 범위 잡기 미친 
            val = max(0, val_index[0] - 2);
        }
        // 만약 중앙쯤에 넣어야한다면
        else {
            // val_index 리스트는 정렬 순서대로 되어있으니까 앞 뒤에 있는 원소 사이의 index에  
            int left = val_index[index - 1];
            int right = val_index[index];
            val = (left + right) / 2;
        }

        // 위에서 val_index 리스트의 index를 보고 blank_list의 어느 인덱스에 삽입해야하는지 찾았지만 빈칸이 무한한게 아니기 때문에 충돌이 날 수 있음 
        int fin_val = find_gap(blank_list, val, n);

        // blank_list의 fin_val 인덱스에 값을 삽입
        blank_list[fin_val] = i;
        // val_index에 기록
        val_index.insert(val_index.begin() + index, fin_val);
    }

    // 결과 정리
    vector<int> result;
    for (int j : val_index) {
        result.push_back(blank_list[j].value());
    }

    return result;
}