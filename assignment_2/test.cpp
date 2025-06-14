#include <iostream>
#include <string>
#include <vector>

#include "utils/utils.h"
#include "MST/MST.h"
#include "Held-Karp/held_karp.h"
#include "greedy/greedy.h"
#include "insertion/insertion.h"
#include "annealing/annealing.h"

// 출력하려구요
#include <fstream>
#include <sstream>
#include <chrono>
#include <sys/resource.h>

using namespace std;
using namespace std::chrono;

// 진행바 출력 (간격 기반)
void show_progress_bar(int step, int total, const string& message) {
    int width = 40;
    float ratio = static_cast<float>(step) / total;
    int completed = ratio * width;

    cout << "\r" << message << " [";
    for (int i = 0; i < width; ++i) {
        if (i < completed) cout << "=";
        else if (i == completed) cout << ">";
        else cout << " ";
    }
    cout << "] " << int(ratio * 100) << "%" << flush;
}

// 메모리 사용량 반환 (KB)
long get_memory_usage_kb() {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss;
}



int main() {
    // vector<string> datasets = {"a20", "a280", "xql662", "kz9976", "mona-lisa100K"};
    // vector<string> datasets = {"a20", "a280", "xql662", "kz9976"};
    
    vector<string> datasets = {"ei8246", "vm22775", "ja9847"};
    // vector<string> datasets = {"a20"};
    // vector<string> datasets = {"a280"};
    // vector<string> datasets = {"xql662"}
    // vector<string> datasets = {"kz9976"};
    // vector<string> datasets = {"mona-lisa100K"};
    
    // vector<string> algos = {"MST"};
    // vector<string> algos = {"held-karp"};
    vector<string> algos = {"greedy"};
    // vector<string> algos = {"insertion"};
    // vector<string> algos = {"annealing"};

    for (const string& data_name : datasets) {
        string tsp_path = "data/" + data_name + ".tsp";
        cout << "\n============================" << endl;
        cout << "[데이터셋] " << data_name << endl;
        cout << "============================" << endl;

        // 1. 입력 파일 읽기
        cout << "[로드] " << tsp_path << " 에서 좌표 불러오는 중..." << endl;
        vector<point> coords = read_TSP_file(tsp_path);

        for (const string& algo : algos) {
            // 로그 버퍼
            stringstream log;
            cout << "\n[실행] 알고리즘: " << algo << endl;

            // 시간 측정 시작
            auto t_start = high_resolution_clock::now();

            // 2. 알고리즘 실행
            vector<int> path;
            if (algo == "MST") {
                path = solve_MST(coords);
            } else if (algo == "held-karp") {
                path = solve_held_karp(coords);
            } else if (algo == "greedy") {
                path = solve_greedy(coords);
            } else if (algo == "insertion") {
                path = solve_insertion(coords);
            } else if (algo == "annealing") {
                path = solve_annealing(coords);
            } else {
                cerr << "[에러] 알 수 없는 알고리즘: " << algo << endl;
                continue;
            }

            // 시간 측정 종료
            auto t_end = high_resolution_clock::now();
            auto time_ms = duration_cast<milliseconds>(t_end - t_start).count();

            // 메모리 사용량 측정
            long mem_kb = get_memory_usage_kb();

            // 파일명 지정
            string filename = data_name + "_" + algo;

            // 결과 저장
            cout << "[저장] output/" << filename << ".tour 저장 중..." << endl;
            save_tour_file(filename, path, coords, data_name, true);

            // 정답 비교
            cout << "[검사] GT와 결과 비교 중..." << endl;
            bool correct = compare_tour_file(data_name, filename, log);
            string result = correct ? "O : 정답과 일치합니다!" : "X : 정답과 일치하지 않습니다.";
            cout << result << endl;

            // 로그 저장
            string txt_filename = "output/" + algo + "/" + filename + ".txt";
            ofstream fout(txt_filename);
            if (!fout.is_open()) {
                cerr << "[에러] 로그 파일 저장 실패: " << txt_filename << endl;
                continue;
            }

            fout << "=== 테스트 결과 요약 ===\n";
            fout << "데이터셋           : " << data_name << "\n";
            fout << "사용 알고리즘      : " << algo << "\n";
            fout << "결과 파일          : output/" << filename << ".tour\n";
            fout << "정답 일치 여부     : " << (correct ? "일치함" : "불일치") << "\n";
            fout << "걸린 시간          : " << time_ms << " ms\n";
            fout << "메모리 사용량      : " << mem_kb << " KB\n";
            fout << "성능 상태          : 정상 실행됨\n";
            fout << "\n=== 비교 상세 로그 ===\n";
            fout << log.str();
            fout.close();

            cout << "[완료] 로그 저장됨: " << txt_filename << endl;
        }
    }

    return 0;
}
