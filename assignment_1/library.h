#pragma once
#include <vector>
#include <optional> 
using namespace std;

// library_sort (빈칸을 포함한 리스트에 값 삽입 방식)
vector<int> library_sort(const vector<int> list);

// 정렬 어느 인덱스에 넣어야 하는지 binary_search
int binary_search(const vector<optional<int>>& blank_list,
                  const vector<int>& val_index, int i);

// blank_list 에서 가장 가까운 빈 칸의 인덱스를 찾고 반환
int find_gap(const vector<optional<int>>& blank_list, int val, int n);
