# list를 앞에서부터 하나씩 앞쪽에 정렬해서 끼워넣는 방식
# time : O(n^2) : 각 요소를 어디에 넣어야하는지 싹 살펴봐야하니까 n개 요소 * n번 살피기
# space : O(1) : 새 리스트를 만들지는 않음


def insertion_sort(list):
    n = len(list)

    # 앞에서 부터 하나씩 골라서 
    for i in range(1, len(list)):
        val = list[i]
        j = i - 1

        # 현재 val보다 큰 값은 오른쪽으로 밀어냄
        while (j >= 0 and list[j] > val):
            list[j+1] = list[j]
            j -= 1

        list[j+1] = val

    return list 