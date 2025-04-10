# 리스트를 반으로 나누고 앞 뒤 따로따로 정렬해서 재귀로 합치는 방식
# time : O(n * log n) : log_2 n 번 재귀 호출(n개의 요소 리스트를 2개로 나누니까), 각 재귀 호출마다 n개를 병합해야하니까 
# space : O(n) : 새 리스트 생성해서 결과물 넣음  

# merge_sort function 제작
def merge_sort(list):
    # 만약 요소가 작으면 걍 반환
    if (len(list) <= 1) :
        return list
    
    # 중앙을 pivot으로 잡고 left랑 right로 나누기 
    pivot = len(list) // 2
    left_list = merge_sort(list[:pivot])
    right_list = merge_sort(list[pivot:])

    # 만약 정렬을 해야한다면, 새 리스트 하나 만들고
    result = []
    # i 는 left_list 의 index, j는 right_list 의 index
    i = j = 0

    # 양쪽 list의 맨 앞끼리 비교해서 새 리스트에 집어넣기
    while ((i < len(left_list)) and (j < len(right_list))):
        if (left_list[i] <= right_list[j]):
            result.append(left_list[i])
            i += 1
        else:
            result.append(right_list[j])
            j += 1
        
    # 한 list 가 다 추가된 다음에는 남은거 마지막에 싹 추가 
    result.extend(left_list[i:])
    result.extend(left_list[j:])

    return result