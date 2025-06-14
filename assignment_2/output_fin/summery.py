import pandas as pd
import os

# 현재 스크립트 위치 기준
folder_path = os.path.dirname(os.path.abspath(__file__))

# ground truth 값 정의 - 반드시 실제 dataset 이름과 일치해야 함
ground_truths = {
    'a20': 370,
    'a280': 741,
    'ei8246' : 206171, 
    'ja9847' : 491924, 
    'kz9976' : 1061882, 
    'mona-lisa100K' : 5757191, 
    'vm22775' : 569288,
    'xql662' : 2513
}

# 결과 누적
all_results = []

# 폴더 내 모든 CSV 처리
for filename in os.listdir(folder_path):
    if filename.endswith(".csv") and not filename.startswith("~$"):  # 임시 파일 제외
        file_path = os.path.join(folder_path, filename)
        try:
            df = pd.read_csv(file_path)

            # 컬럼명 매핑
            if not {'Dataset', 'Lambda', 'TourLength'}.issubset(df.columns):
                print(f"Skipping {filename}: Missing required columns")
                continue

            # 컬럼 이름 통일
            df = df.rename(columns={
                'Dataset': 'dataset',
                'Lambda': 'lambda',
                'TourLength': 'tour_length'
            })

            # 최소 투어 길이 람다만 선택
            df_best = df.loc[df.groupby("dataset")["tour_length"].idxmin()].copy()

            # accuracy 계산
            def compute_accuracy(row):
                gt = ground_truths.get(row['dataset'], None)
                if gt is None:
                    return None
                return gt / row['tour_length']

            df_best["accuracy"] = df_best.apply(compute_accuracy, axis=1)
            df_best["source_file"] = filename

            all_results.append(df_best)

        except Exception as e:
            print(f"Error processing {filename}: {e}")

# 통합 저장
if all_results:
    combined_df = pd.concat(all_results, ignore_index=True)
    output_path = os.path.join(folder_path, "combined_best_lambdas.csv")
    combined_df.to_csv(output_path, index=False)
    print(f"Saved combined results to {output_path}")
else:
    print("No valid CSV files found or no data processed.")
