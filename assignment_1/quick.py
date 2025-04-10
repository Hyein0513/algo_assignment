# pivot 을 고르고 작은거 왼쪽 큰거 오른쪽 해서 나누는 분할 재귀
# time : O(n * log n) : log n번의 재귀 호출이 있으니까 * 호출마다 n개 훑으니까
# space : O(log n) : 재귀 깊이 대로 새 list 만들어야 하니까


def quick_sort(list):
    # 만약 len이 1 이하면 반환
    if (len(list) <= 1):
        return list
    
    # pivot을 맨 처음으로 고르고 왼쪽 오른쪽 나누기
    pivot = list[0]
    left = [x for x in list[1:] if x <= pivot]
    right = [x for x in list[1:] if x > pivot]

    # 재귀 호출해서 전체 반환 list 만들기
    result = quick_sort(left) + [pivot] + quick_sort(right)    

    return result