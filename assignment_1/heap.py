# Max_heap을 만든 다음, (front 를 맨 뒤로 보내서 오름차순으로 정렬, 걔 빼고 나머지를 다시 Max Heap 완성) -> 괄호 반복 
# time : O(n * log n) : (맨 처음 max heap을 만들 때 n에 비례해서 비교(?)) + (하나 빼고 나머지 max heap 을 만들때 비교를 log n 만큼 호출하니까 * 각 비교는 n에 비례(?))
# space : O(1) : 원래 쓰던 array 사용해서 정렬 

# list 가 heap 배열, n이 내가 max heap 만들어야 하는 갯수?, i가 여기부터 시작
def heapify(list, n, i):
    largest = i
    left_child = (2 * i) + 1
    right_child = (2 * i) + 2

    # 만약 왼쪽 자식 index가 내가 고려해야할 max heap list 인덱스보다 작고
    # 왼쪽 자식 val 크기가 root val 보다 크다면
    if (left_child < n) and (list[left_child] > list[largest]):
        largest = left_child

    # 여기부터 따라 안적었는데 아이패드에도 있듯이 이렇게 하면 아이패드 내용 다시 적어야함
    # 부모 왼쪽자식 오른쪽자식 비교할때 이 순서대로면 무조건 오른쪽 자식이 왼쪽보다 크게 설정됨
    # 뭐가 더 heapify 호출을 적게 하는지 고려할 필요가 있음 
    if (left_child < n) and (list[left_child] > list[largest]):
        largest = left_child
    
    if (left_child < n) and (list[left_child] > list[largest]):
        largest = left_child
    