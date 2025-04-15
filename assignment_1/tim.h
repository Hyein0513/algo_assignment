#pragma once
#include <vector>
using namespace std;

// tim sort용 insertion sort (부분 구간 정렬)
void insertion_sort_2(vector<int>& list, int left, int right);

// merge (tim sort 병합 과정)
void merge(vector<int>& list, int l, int m, int r);

// tim sort
vector<int> tim_sort(vector<int> list);
