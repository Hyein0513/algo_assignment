#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <sys/resource.h>

#include "utils/utils.h"
#include "hhi_greedy/hhi_greedy.h"
#include "hhi_MST/hhi_MST.h"
#include "hhi_euler/hhi_euler.h"

using namespace std;
using namespace std::chrono;

// 메모리 사용량 반환 (KB)
long get_memory_usage_kb() {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss;
}

point get_vector(const point& from, const point& to) {
    return {to.x - from.x, to.y - from.y};
}

double angle_between(const point& a, const point& b) {
    double dot = a.x * b.x + a.y * b.y;
    double norm_a = sqrt(a.x * a.x + a.y * a.y);
    double norm_b = sqrt(b.x * b.x + b.y * b.y);
    if (norm_a == 0 || norm_b == 0) return 0.0;
    double cos_theta = dot / (norm_a * norm_b);
    cos_theta = (cos_theta < -1.0) ? -1.0 : ((cos_theta > 1.0) ? 1.0 : cos_theta);
    return acos(cos_theta) * 180.0 / M_PI;
}

pair<int, double> compute_turning_stats(const vector<int>& path, const vector<point>& coords) {
    int count = 0;
    double angle_sum = 0;

    for (size_t i = 1; i + 1 < path.size(); ++i) {
        point prev = coords[path[i - 1]];
        point curr = coords[path[i]];
        point next = coords[path[i + 1]];

        point v1 = get_vector(curr, prev);
        point v2 = get_vector(curr, next);

        double angle = angle_between(v1, v2);
        if (angle > 1.0) {
            count++;
            angle_sum += angle;
        }
    }

    double avg = count > 0 ? angle_sum / count : 0.0;
    return {count, avg};
}

// tour 파일 읽기 (output 포함 경로)
vector<int> read_tour_file(const string& tour_path) {
    ifstream in(tour_path);
    if (!in.is_open()) {
        cerr << "[경고] tour 파일 열기 실패: " << tour_path << endl;
        cerr << "[DEBUG] 열려고 하는 파일 경로: " << tour_path << endl;
        return {};
    }

    vector<int> path;
    string line;
    bool start = false;
    while (getline(in, line)) {
        if (line == "TOUR_SECTION") {
            start = true;
            continue;
        }
        if (!start || line == "-1" || line == "EOF") break;
        path.push_back(stoi(line) - 1);  // 1-based → 0-based
    }
    return path;
}

int main() {
    vector<string> datasets = {"a20", "a280", "xql662", "kz9976", "mona-lisa100K", "ei8246", "vm22775", "ja9847"};
    // vector<string> datasets = {"ei8246", "vm22775", "ja9847"};
    // vector<double> lambda_list = {0, 10, 25, 50, 100, 200};
    // vector<double> lambda_list = {0.5, 1, 3, 5, 7};
    vector<double> lambda_list = {0};
    // vector<double> lambda_list = {10,30,50,70,100,200};

    for (const string& data_name : datasets) {
        string tsp_path = "data/" + data_name + ".tsp";
        cout << "\n============================" << endl;
        cout << "[데이터셋] " << data_name << endl;
        cout << "============================" << endl;
        cout << "[로드] " << tsp_path << " 에서 좌표 불러오는 중..." << endl;

        vector<point> coords = read_TSP_file(tsp_path);

        for (double lambda : lambda_list) {
            stringstream log;
            cout << "\n[실행] 알고리즘: hhi_greedy (lambda = " << lambda << ")" << endl;
            // cout << "\n[실행] 알고리즘: hhi_MST (lambda = " << lambda << ")" << endl;
            // cout << "\n[실행] 알고리즘: hhi_euler (lambda = " << lambda << ")" << endl;

            auto t_start = high_resolution_clock::now();
            vector<int> path = solve_hhi_greedy(coords, lambda);
            // vector<int> path = solve_hhi_MST(coords, lambda);
            // vector<int> path = solve_hhi_euler(coords, lambda);
            auto t_end = high_resolution_clock::now();
            auto time_ms = duration_cast<milliseconds>(t_end - t_start).count();
            long mem_kb = get_memory_usage_kb();

            stringstream lambda_ss;
            lambda_ss << fixed << setprecision(1) << lambda;
            string lambda_str = "lambda" + lambda_ss.str();

            string folder = "output/hhi_greedy_1/" + lambda_str;
            // string folder = "output/hhi_Euler/" + lambda_str;
            string filename = data_name + "_" + lambda_str;

            system(("mkdir -p " + folder).c_str());

            cout << "[저장] " << folder << "/" << filename << ".tour 저장 중..." << endl;
            save_tour_file("hhi_greedy_1/" + lambda_str + "/" + filename, path, coords, data_name, true);  // 내부에서 output/ 자동 추가됨
            // save_tour_file("hhi_Euler/" + lambda_str + "/" + filename, path, coords, data_name, true);  // 내부에서 output/ 자동 추가됨

            cout << "[검사] GT와 결과 비교 중..." << endl;
            bool correct = compare_tour_file(data_name, "hhi_greedy_1/" + lambda_str + "/" + filename, log);
            // bool correct = compare_tour_file(data_name, "hhi_Euler/" + lambda_str + "/" + filename, log);

            int total_dist = calculate_total_distance(path, coords);
            auto [turns, avg_angle] = compute_turning_stats(path, coords);

            log << "\n[HHI_GREEDY 결과]\n"; 
            // log << "\n[HHI_MST_1 결과]\n"; 
            // log << "\n[HHI_euler 결과]\n"; 
            log << "경로 길이          : " << total_dist << "\n";
            log << "꺾임 개수          : " << turns << "\n";
            log << "평균 꺾임 각도      : " << fixed << setprecision(2) << avg_angle << "도\n";

            // greedy 경로 비교
            string greedy_tour_path = "output/greedy_1/" + data_name + "_greedy_1.tour";
            vector<int> greedy_path = read_tour_file(greedy_tour_path);

            if (!greedy_path.empty()) {
                int greedy_dist = calculate_total_distance(greedy_path, coords);
                auto [g_turns, g_angle] = compute_turning_stats(greedy_path, coords);
            
                log << "\n[Greedy 결과]\n";
                log << "경로 길이          : " << greedy_dist << "\n";
                log << "꺾임 개수          : " << g_turns << "\n";
                log << "평균 꺾임 각도      : " << fixed << setprecision(2) << g_angle << "도\n";
            } else {
                log << "\n⚠️ Greedy 경로 파일을 읽을 수 없습니다: " << greedy_tour_path << "\n";
            }

/*
            string MST_tour_path = "output/MST/" + data_name + "_MST.tour";
            vector<int> MST_path = read_tour_file(MST_tour_path);

            if (!MST_path.empty()) {
                int MST_dist = calculate_total_distance(MST_path, coords);
                auto [g_turns, g_angle] = compute_turning_stats(MST_path, coords);

                log << "\n[MST 결과]\n";
                log << "경로 길이          : " << MST_dist << "\n";
                log << "꺾임 개수          : " << g_turns << "\n";
                log << "평균 꺾임 각도      : " << fixed << setprecision(2) << g_angle << "도\n";
            } else {
                log << "\n⚠️ MST 경로 파일을 읽을 수 없습니다: " << MST_tour_path << "\n";
            }
*/
            string txt_filename = folder + "/" + filename + ".txt";
            ofstream fout(txt_filename);
            if (!fout.is_open()) {
                cerr << "[에러] 로그 파일 저장 실패: " << txt_filename << endl;
                continue;
            }

            fout << "=== 테스트 결과 요약 ===\n";
            fout << "데이터셋           : " << data_name << "\n";
            fout << "사용 알고리즘      : hhi_greedy_1 (lambda=" << lambda << ")\n";
            // fout << "사용 알고리즘      : hhi_MST(lambda=" << lambda << ")\n";
            // fout << "사용 알고리즘      : hhi_euler (lambda=" << lambda << ")\n";
            fout << "결과 파일          : " << folder << "/" << filename << ".tour\n";
            fout << "정답 일치 여부     : " << (correct ? "일치함" : "불일치") << "\n";
            fout << "걸린 시간          : " << time_ms << " ms\n";
            fout << "메모리 사용량      : " << mem_kb << " KB\n";
            fout << "성능 상태          : 정상 실행됨\n\n";
            fout << "=== 비교 상세 로그 ===\n" << log.str();
            fout.close();

            cout << "[완료] 로그 저장됨: " << txt_filename << endl;
        }
    }

    return 0;
}
