#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>
#include <cmath>
#include <sys/resource.h>
#include <cstdlib>
#include <map>
#include <functional>

// === project‑local headers ===
#include "utils/utils.h"
#include "Held-Karp/held_karp.h"
#include "insertion/insertion.h"
#include "greedy/greedy.h"
#include "MST/MST.h"
#include "hhi_MST/hhi_MST.h"
#include "hhi_greedy/hhi_greedy.h"
#include "hhi_greedy1/hhi_greedy1.h"

using namespace std;
using namespace chrono;

//──────────────────────────────────────────────────────────────
//   HELPER UTILITIES
//──────────────────────────────────────────────────────────────

long get_memory_usage_kb() {
    struct rusage usage{};
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss;
}

struct Vec { double x, y; };
Vec diff(const point &a, const point &b) { return {a.x - b.x, a.y - b.y}; }

double angle_between(const Vec &u, const Vec &v) {
    double dot = u.x * v.x + u.y * v.y;
    double nu = sqrt(u.x * u.x + u.y * u.y);
    double nv = sqrt(v.x * v.x + v.y * v.y);
    if (nu == 0 || nv == 0) return 0.0;
    double c = dot / (nu * nv);
    c = max(-1.0, min(1.0, c));
    return acos(c) * 180.0 / M_PI;
}

pair<int,double> turning_stats(const vector<int> &path, const vector<point> &coords) {
    int bends = 0; double sum = 0.0;
    for (size_t i = 1; i + 1 < path.size(); ++i) {
        Vec v1 = diff(coords[path[i]], coords[path[i-1]]);
        Vec v2 = diff(coords[path[i]], coords[path[i+1]]);
        double ang = angle_between(v1, v2);
        if (ang > 1.0) { ++bends; sum += ang; }
    }
    return {bends, bends ? sum / bends : 0.0};
}

double compare_accuracy(const string &dataset, const string &output_prefix) {
    string gt_path = "data/GT/" + dataset + ".tour";
    string my_path = output_prefix + ".tour";
    ifstream gt(gt_path), mine(my_path);
    if (!gt.is_open() || !mine.is_open()) return 0.0;
    string g, m; bool in = false; int tot = 0, eq = 0;
    while (getline(gt, g)) {
        if (g == "TOUR_SECTION") { in = true; continue; }
        if (!in || g == "-1" || g == "EOF") break;
        if (!getline(mine, m)) break;
        ++tot; if (g == m) ++eq;
    }
    return tot ? 100.0 * eq / tot : 0.0;
}

//──────────────────────────────────────────────────────────────
//   MAIN
//──────────────────────────────────────────────────────────────

int main() {
    const vector<string> datasets = { "a20", "a280", "xql662"};
    // "a20", "a280", "xql662", "kz9976", "ei8246", "vm22775", "ja9847", "mona-lisa100K"

    const vector<double> lambdas = {0, 0.5, 1, 1.5, 2, 2.5, 3, 3.5, 4, 5, 7, 10};  // 수정 가능

    // "greedy", "MST", "insertion", "held_karp", 
    const vector<string> algorithm_names = {
        "insertion"
    };


    map<string, function<vector<int>(const vector<point>&, double)>> algorithms;

    algorithms["greedy"] = [](const vector<point>& pts, double) {
        return solve_greedy(pts);
    };

    algorithms["MST"] = [](const vector<point>& pts, double) {
        return solve_MST(pts);
    };

    algorithms["insertion"] = [](const vector<point>& pts, double) {
        return solve_insertion(pts);
    };

    system("mkdir -p output");

    for (const string &algo_name : algorithm_names) {
        
        string csv_path = "output/output_fin/experiment_summary_" + algo_name + ".csv";

        ofstream csv(csv_path);
        if (!csv.is_open()) {
            cerr << "[FATAL] CSV 열기 실패: " << csv_path << endl;
            continue;
        }
        csv << "Dataset,Algorithm,Lambda,MemoryKB,TimeMS,TourLength,Bends,AvgBendAngle,Accuracy%\n";

        for (const string &ds : datasets) {
            string tsp = "data/" + ds + ".tsp";
            cout << "\n=== " << ds << " | " << algo_name << " ============================\n";
            vector<point> coords = read_TSP_file(tsp);
            if (coords.empty()) { cerr << "[WARN] 로드 실패 → 스킵\n"; continue; }

            for (double lambda : lambdas) {
                cout << "λ=" << lambda << " ..." << flush;
                auto t0 = high_resolution_clock::now();
                vector<int> path = algorithms[algo_name](coords, lambda);
                auto t1 = high_resolution_clock::now();
                if (path.empty()) { cerr << " 실패\n"; continue; }

                long mem = get_memory_usage_kb();
                long ms  = duration_cast<milliseconds>(t1 - t0).count();
                int  len = calculate_total_distance(path, coords);
                auto [b, avg] = turning_stats(path, coords);

                // 디렉터리‧파일
                ostringstream dir; 
                dir << "output/" << algo_name << "/lambda" << lambda;
                system(("mkdir -p " + dir.str()).c_str());
                ostringstream prefix; 
                prefix << dir.str() << "/" << ds << "_" << algo_name << "_lambda" << lambda;
                save_tour_file(prefix.str().substr(7), path, coords, ds, true); // drop "output/"
                double acc = compare_accuracy(ds, prefix.str());

                // CSV
                csv << ds << ',' << algo_name << ',' << lambda << ','
                    << mem << ',' << ms << ',' << len << ',' << b << ','
                    << fixed << setprecision(2) << avg << ',' << acc << "\n";

                cout << " 완료 (" << ms << "ms)\n";
            }
        }

        csv.close();
        cout << "\n📄 완료된 결과: " << csv_path << "\n";
    }

    cout << "\n✅ 모든 알고리즘 실험 완료\n";
    return 0;
}
