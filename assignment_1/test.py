# 지금까지 만든 함수 입력 data넣어서 테스트 하는 함수

def test_sort(func, input):
    arr = input.copy()  # 원본 유지
    start_time = time.time()
    result = sort(arr)
    end_time = time.time()

    
    # 결과 확인
    print(f"\n▶️ {sort_func.__name__}")
    print(f"정렬 결과: {result}")
    print(f"걸린 시간: {end_time - start_time:.6f}초")