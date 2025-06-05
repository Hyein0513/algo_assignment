# tsp랑 tour 파일로 tour 시각화하는 코드 
# vis는 걍 py가 빠름 
import os
import matplotlib.pyplot as plt

def read_tsp_file(tsp_path):
    coords = []
    with open(tsp_path, 'r') as f:
        start = False
        for line in f:
            line = line.strip()
            if line == "NODE_COORD_SECTION":
                start = True
                continue
            if line == "EOF":
                break
            if start:
                parts = line.split()
                if len(parts) >= 3:
                    _, x, y = parts
                    coords.append((float(x), float(y)))
    return coords

def read_tour_file(tour_path):
    tour = []
    with open(tour_path, 'r') as f:
        start = False
        for line in f:
            line = line.strip()
            if line == "TOUR_SECTION":
                start = True
                continue
            if line == "-1" or line == "EOF":
                break
            if start:
                tour.append(int(line) - 1)  # 1-based to 0-based
    return tour

def plot_and_save(coords, tour, save_path):
    if tour[0] != tour[-1]:
        tour.append(tour[0])
    
    x = [coords[i][0] for i in tour]
    y = [coords[i][1] for i in tour]

    # 더 큰 이미지와 얇은 선, 작은 점
    plt.figure(figsize=(12, 12))  # 또는 문제 크기에 따라 조절
    plt.plot(x, y, marker='o', markersize=0.5, linewidth=0.3, alpha=0.8)

    # 제목은 파일 이름에서 추출
    plt.title(os.path.basename(save_path).replace("_", " ").replace(".png", ""))
    plt.axis('equal')
    plt.axis('off')  # 축 눈금 없애면 더 깔끔함
    os.makedirs(os.path.dirname(save_path), exist_ok=True)
    plt.savefig(save_path, dpi=300, bbox_inches='tight')
    plt.close()


def visualize_all(tsp_name, output_root="output", vis_root="vis_output"):
    tsp_path = os.path.join("data", f"{tsp_name}.tsp")
    coords = read_tsp_file(tsp_path)

    for algo in os.listdir(output_root):
        algo_dir = os.path.join(output_root, algo)
        if not os.path.isdir(algo_dir):
            continue

        for file in os.listdir(algo_dir):
            if file.endswith(".tour") and file.startswith(tsp_name + "_"):
                tour_path = os.path.join(algo_dir, file)
                tour = read_tour_file(tour_path)

                vis_path = os.path.join(vis_root, algo, file.replace(".tour", ".png"))
                print(f"🔍 {file} → 저장: {vis_path}")
                plot_and_save(coords, tour, vis_path)

if __name__ == "__main__":
    visualize_all("a20")
    visualize_all("a280")  
    visualize_all("xql662")  
    visualize_all("kz9976")  
    visualize_all("mona-lisa100K")