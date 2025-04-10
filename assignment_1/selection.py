# list의 맨 앞부터 제일 작은값을 끼워넣기
# time : O(n^2) : 매 index를 채울 떄 마다 전체를 봐야하니까 
# space : O(1) : 새 list 를 만들지 않으니까


def selection_sort(list):
    n - len(list)
    for i in range(n):
        sort_index = i 

        # 그 다음 구간에서 가장 작은 값 index 찾기
        for j in range(i + 1, n):
            if (list[j] < list[sort_index]):
                sort_index = j
        
        # 최솟값이 현재 위치랑 다르면 swap 
        if (sort_index != i):
            a, b = list[sort_index], list[i]
            list[sort_index], list[i] = b, a

    return list