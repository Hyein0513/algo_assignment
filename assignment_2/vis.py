import os
import matplotlib.pyplot as plt

# 경로 설정
tour_dir = 'output_fin/tour/'
tsp_dir = 'data'
output_dir = 'vis_output_fin'

# 출력 디렉토리 없으면 생성
os.makedirs(output_dir, exist_ok=True)

# TSP 좌표 로더
def load_coordinates(tsp_file):
    coords = {}
    with open(tsp_file, 'r') as f:
        lines = f.readlines()

    reading = False
    for line in lines:
        line = line.strip()
        if line == "NODE_COORD_SECTION":
            reading = True
            continue
        if line == "EOF":
            break
        if reading:
            parts = line.split()
            if len(parts) >= 3:
                idx = int(parts[0])
                x = float(parts[1])
                y = float(parts[2])
                coords[idx] = (x, y)
    return coords

# TOUR 로더
def load_tour(tour_file):
    tour = []
    with open(tour_file, 'r') as f:
        lines = f.readlines()

    reading = False
    for line in lines:
        line = line.strip()
        if line == "TOUR_SECTION":
            reading = True
            continue
        if line in ("-1", "EOF"):
            break
        if reading:
            tour.append(int(line))
    return tour

# 모든 .tour 파일 처리
for filename in os.listdir(tour_dir):
    if filename.endswith('.tour'):
        # datasetname 추출
        datasetname = filename.split('_')[0]
        tsp_file = os.path.join(tsp_dir, f'{datasetname}.tsp')
        tour_file = os.path.join(tour_dir, filename)

        # TSP 좌표 불러오기
        if not os.path.exists(tsp_file):
            print(f"[경고] TSP 파일 없음: {tsp_file}")
            continue
        coords = load_coordinates(tsp_file)

        # TOUR 순서 불러오기
        tour = load_tour(tour_file)

        # 시각화용 좌표 추출
        x = [coords[city][0] for city in tour]
        y = [coords[city][1] for city in tour]

 
        # 시각화
        plt.figure(figsize=(10, 10))  # 더 큰 도화지
        plt.plot(x, y, linestyle='-', linewidth=0.7, color='black', alpha=1.0)  # 진한 선
        plt.title(filename)
        plt.axis('equal')
        plt.axis('off')  # 축 제거하면 더 깔끔
        plt.tight_layout()

        # 고해상도 저장
        output_path = os.path.join(output_dir, filename.replace('.tour', '.png'))
        plt.savefig(output_path, dpi=300)  # 고해상도 저장
        plt.close()

        print(f"✔ 시각화 완료: {output_path}")

print("✅ 모든 시각화 완료!")
