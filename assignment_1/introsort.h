#pragma once
#include <vector>
#include <cmath>

using namespace std;
// insertion : 구간 정해서 진행 
void insertion_sort(vector<int>& list, int left, int right);
// heapify
void heapify_2(vector<int>& list, int n, int i);
// heap_sort : 구간 정해서 진행
void heap_sort(vector<int>& list, int left, int right) ;
// partition
int partition(vector<int>& list, int low, int high);
// introsortion
void introsortion(vector<int>& list, int left, int right, int depth_limit);
// introsort_sort
vector<int> introsort_sort(vector<int> list);
