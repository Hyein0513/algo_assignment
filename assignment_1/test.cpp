/*


#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <random>
#include <algorithm>
#include <fstream>
#include <chrono>
#include <functional>
#include <filesystem>

#include "bubble.h"
#include "heap.h"
#include "insertion.h"
#include "library.h"
#include "merge.h"
#include "quick.h"
#include "selection.h"
#include "tim.h"
#include "cocktail_shaker.h"
#include "comb.h"
#include "tim.h"
#include "introsort.h"
#include "tournament.h"

////////////////////////////////////////////
// 1) 리눅스에서 RSS(Resident Set Size) 조회
////////////////////////////////////////////
#ifdef __linux__
#include <sys/resource.h>
#include <unistd.h>
#include <fstream>
#include <sstream>

// getPeakRSS: 현재 프로세스가 지금까지 사용한 최대(피크) 메모리(KB 단위)
// getCurrentRSS: 현재 시점의 메모리 사용량(KB 단위)

// (1) 최대(피크) 메모리 사용량
size_t getPeakRSS() {
    // getrusage로 maxrss(단위: KB) 구하기 (Linux, macOS 등 일부 Unix 계열)
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return static_cast<size_t>(usage.ru_maxrss);
}

// (2) 현재 메모리 사용량
size_t getCurrentRSS() {
    // /proc/self/statm 파싱 (Linux 전용)
    // statm: [전체 페이지 수] [메모리 사용 페이지 수] ...
    // 보통 RSS 항목이 두 번째
    std::ifstream stat_stream("/proc/self/statm");
    if(!stat_stream) {
        return 0; // 실패 시 0 리턴
    }

    long rss_pages = 0;
    long dummy;
    // statm 순서: total, resident, share, text, lib, data, dt
    stat_stream >> dummy >> rss_pages;
    stat_stream.close();

    long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // 페이지 크기(KB)
    size_t rss_kb = rss_pages * page_size_kb;
    return rss_kb;
}
#else
// 다른 OS(Windows 등)에서는 따로 API를 써야 함.
// 여기선 더미로 0 리턴
size_t getPeakRSS() { return 0; }
size_t getCurrentRSS() { return 0; }
#endif

using namespace std;
namespace fs = std::filesystem;

// 입력하는 배열 만드는 함수
vector<int> make_data(int n, const string& set, int seed = -1) {
    mt19937 rng(seed >= 0 ? seed : random_device{}());
    uniform_int_distribution<int> dist(1, n * 10);
    vector<int> random_set(n);
    for (int& x : random_set) x = dist(rng);

    if (set == "random") {
        return random_set;
    } else if (set == "sorted") {
        sort(random_set.begin(), random_set.end());
        return random_set;
    } else if (set == "reversed") {
        sort(random_set.begin(), random_set.end(), greater<>());
        return random_set;
    } else if (set == "part_sorted") {
        int k = 1;
        while ((double)k / n > 0.3 || k == 0) {
            k = dist(rng) % n;
        }
        int start = rng() % (n - k + 1);
        sort(random_set.begin() + start, random_set.begin() + start + k);
        return random_set;
    }

    return random_set;
}

// 테스트 및 로그 기록 함수
void run_experiment(const string& algo_name,
                    function<vector<int>(vector<int>)> algo_func,
                    int size, const string& input_type,
                    int repetitions, const string& log_dir = "logs") {

    fs::create_directories(log_dir);
    string filename = log_dir + "/" + algo_name + "__" + input_type + "__n" + to_string(size) + ".txt";
    ofstream f(filename);

    f << "알고리즘: " << algo_name
      << "\n데이터 유형: " << input_type
      << "\n입력 크기: " << size
      << "\n반복 횟수: " << repetitions << "\n\n";

    for (int i = 0; i < repetitions; ++i) {
        vector<int> input_data = make_data(size, input_type, i);
        vector<int> correct = input_data;
        sort(correct.begin(), correct.end());
        vector<int> data_copy = input_data;

        // 정렬 시작 전 메모리 측정
        size_t mem_before = getCurrentRSS(); // KB
        size_t peak_before = getPeakRSS();   // KB

        auto start = chrono::high_resolution_clock::now();
        // 정렬 수행
        vector<int> result = algo_func(data_copy);
        auto end = chrono::high_resolution_clock::now();

        // 정렬 끝난 후 메모리 측정
        size_t mem_after = getCurrentRSS(); // KB
        size_t peak_after = getPeakRSS();   // KB

        chrono::duration<double> elapsed = end - start;

        if (!result.empty()) {
            data_copy = result;
        }

        bool is_correct = (data_copy == correct);

        f << "실험 " << (i + 1) << ":\n";
        f << " - 정렬 정확성: " << (is_correct ? " O 일치" : " X 불일치") << "\n";
        f << " - 실행 시간: " << elapsed.count() << " 초\n";

        // 메모리 사용(현재, 피크) 기록 (MB 단위 변환: / 1024.0)
        f << " - 메모리 사용량 (CurrentRSS): " << (mem_after / 1024.0) << " MB\n";
        f << " - 피크 메모리 (PeakRSS):       " << (peak_after / 1024.0) << " MB\n\n";
    }

    cout << "✔ 저장 완료: " << filename << endl;
}

// 메인 함수
int main() {
    map<string, function<vector<int>(vector<int>)>> algorithms = {
        {"quick_sort", quick_sort}
    };
    

    vector<int> sizes = {1000, 10000, 100000, 1000000};
    vector<string> input_types = {"random", "sorted", "reversed", "part_sorted"};

    for (const auto& [algo_name, algo_func] : algorithms) {
        for (int size : sizes) {
            int repetitions = 10;
            for (const string& input_type : input_types) {
                run_experiment(algo_name, algo_func, size, input_type, repetitions);
            }
        }
    }

    cout << "------한바퀴 끝------" << endl;
    return 0;
}

*/