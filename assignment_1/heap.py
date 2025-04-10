# Max_heap을 만든 다음, (front 를 맨 뒤로 보내서 오름차순으로 정렬, 걔 빼고 나머지를 다시 Max Heap 완성) -> 괄호 반복 
# time : O(n * log n) : (맨 처음 max heap을 만들 때 n에 비례해서 비교(?)) + (하나 빼고 나머지 max heap 을 만들때 비교를 log n 만큼 호출하니까 * 각 비교는 n에 비례(?))
# space : O(1) : 원래 쓰던 array 사용해서 정렬 

# list 가 heap 배열, n이 내가 max heap 만들어야 하는 갯수, i가 list에서 시작 index
def heapify(list, n, i):
    largest = i
    left_child = (2 * i) + 1
    right_child = (2 * i) + 2

    # 만약 왼쪽 자식 index가 내가 고려해야할 max heap list 인덱스보다 작고
    # 왼쪽 자식 val 크기가 root val 보다 크다면
    if (left_child < n) and (list[left_child] > list[largest]):
        largest = left_child

    # 오른쪽 자식이 더 크면 업데이트 
    if (right_child < n) and (list[right_child] > list[largest]):
        largest = right_child
    
    # root, left_child, right_child 중에서 제일 큰 것이 root가 아니라면 swap 
    # 만약에 swap 사항이 있으면 재귀 호출
    if (largest != i):
        # swap 처리 하고 
        a, b = list[i], list[largest]
        list[i], list[largest] = b, a
        # 재귀 호출 :  swap이 일어난 자식 인덱스인 largest 에서 heapify 재귀 호출 
        heapify(list, n, largest)

    return

# heap sort 함수 
def heap_sort(list):
    # list 요소의 수가 n이고
    n = len(list)

    # internal node 리스트를 순회하면서 밑에서 부터 list 전체를(n개를) 차곡차곡 max_heap 만들기 
    inter_index = (n // 2) - 1
    for i in range(inter_index, -1 , -1):
        heapify(list, n, i)
    
    # list 의 front를 하나씩 꺼내서 정렬
    for i in range(n - 1, 0 -1):
        # root(=largest) 랑 맨 뒤랑 교체
        c, d = list[0], list[i]
        # heapify 로 root 부터 정리
        heapify(list, i, 0)

    return list
        
    