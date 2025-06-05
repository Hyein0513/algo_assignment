// mst 랑 held-karp랑 공통으로 사용되는 함수 모아놓은 utils.cpp 


#include "utils.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <vector>
#include <string>

using namespace std;

// .tsp 파일에서 좌표 읽어오는 함수 
vector<point> read_TSP_file(const string& file){
    // 파일 열고 
    ifstream infile(file);
    // x,y 좌표들을 point 형태로 저장할 벡터
    vector<point> coord;
    string line;
    // file 읽으면서 좌표 시작 여부 표시 
    bool start = false;

    // 파일 열렸는지 체크
    if (!infile.is_open()) {
        cout << "파일을 열 수 없습니다 : " << file << endl;
        exit(1);
    }

    // 파일에서 한줄씩 읽어오면서 
    while (getline(infile, line)) {
        // dataset에서는 NODE_COORD_SECTION 로 구분되어 있으니까
        if (line == "NODE_COORD_SECTION"){
            start = true;
            continue;
        }
        // 다 읽었을 때 
        if (line == "EOF") 
            break;
        // 좌표 section이 시작되면 하나씩 뜯어와야함
        if (start) {
            // 한줄을 공백 기준으로 나눔
            // map 말고 좌표를 걍 vector에 때려넣기 
            istringstream iss(line);
            int index;
            double x, y;
            // 좌표만 vector coord에 저장 
            if (iss >> index >> x >> y) {
                coord.push_back({x,y});
            }
        }
    }
    // 이제 vector 반환
    return coord;
}

// 두 점 사이 거리 계산하는 함수 
int calculate_distance(const point& a, const point& b){
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    
    // EUC_2D는 가장 가까운 정수로 반올림한 Eulidean 거리를 사용하는거니까...
    // 일단 유클리드 거리 구하고 반올림해서 int처리
    return static_cast<int>(round(sqrt(dx * dx + dy * dy)));
}

// node 간의 거리를 모두 계산해서 거리 행렬 생성하는 함수
// 대칭 행렬이니까 반만 계산 
vector<vector<int>> calculate_distance_matrix(const vector<point>& coord) {
    // 거리 행렬의 사이즈를 재고 
    int n = coord.size();
    // dim 2인 행렬을 만들기 + 0으로 초기화 
    vector<vector<int>> dist(n, vector<int>(n, 0));

    // 계산 시작
    for (int i = 0 ; i < n ; ++i){
        for (int j = i + 1 ; j < n ; ++j){
            int d = calculate_distance(coord[i], coord[j]);
            dist[i][j] = d;
            dist[j][i] = d;
        }
    }

    return dist;
}

// 특정 경로의 총 길이 계산 하는 함수 
int calculate_total_distance(const vector<int>& path, const vector<point>& coord){
    int total = 0;
    // path 가 길이 1이면 distance 0임 
    if (path.size() == 0)
        return total;
    // 돌면서 계산 
    for (int i = 0 ; i < path.size() - 1; ++i){
        total += calculate_distance(coord[path[i]], coord[path[i+1]]);
    }
    // 시작점으로 돌아오는 거리 더하기 
    total += calculate_distance(coord[path.back()], coord[path[0]]);

    return total;
}

// gt 처럼 tour 파일 만드는 함수
// filename 넣어주고, data 이름 넣어주고, 내가 만든 path 넣어주면 저장함  
// flag == false로 실행하면 루트 출력 없이 길이만 출력함
void save_tour_file(const string& filename, const vector<int>& path, const vector<point>& coord, const string& data_name, bool flag = true){
    ofstream out("output/" + filename + ".tour");
    string dim;
    out << "NAME : " << data_name << endl;
    out << "COMMENT: Tour length " << calculate_total_distance(path, coord) << endl;
    out << "TYPE : TOUR" << endl;
    out << "DIMENSION : " << path.size() - 1 << endl;
    out << "TOUR_SECTION" << endl;

    // false 라서 length만 출력하는거면
    if (!flag){
        out.close();
        return;
    }

    // node 출력
    for (int i = 0 ; i < path.size() ; ++i){
        // vector 기반이라서 일단 1 더해줘야함
        out << path[i] + 1 << "\n";
    }
    
    out << "-1\nEOF";
    out.close();
}


// gt랑 내가 만든 file 이랑 비교해서 log에 저장 (출력 없음 )
bool compare_tour_file(const string& gt_file, const string& my_file, stringstream& log) {
    string gt_path = "data/GT/" + gt_file + ".tour";
    string my_path = "output/" + my_file + ".tour";

    ifstream in1(gt_path);
    ifstream in2(my_path);

    if (!in1.is_open()) {
        log << "GT 파일 열기 실패: " << gt_path << "\n";
        return false;
    } else if (!in2.is_open()) {
        log << "결과 파일 열기 실패: " << my_path << "\n";
        return false;
    }

    string line1, line2;
    int line_num = 1;

    while (getline(in1, line1)) {
        if (!getline(in2, line2)) {
            log << "결과 파일이 먼저 끝남 (라인 " << line_num << ")\n";
            return false;
        }
        if (line1 != line2) {
            log << "차이 발생 (라인 " << line_num << ")\n";
            log << "GT   : " << line1 << "\n";
            log << "내꺼 : " << line2 << "\n";
            return false;
        }
        ++line_num;
    }

    if (getline(in2, line2)) {
        log << "GT 파일이 먼저 끝남 (라인 " << line_num << ")\n";
        return false;
    }

    log << "compare_tour_file(): 파일이 완전히 일치합니다.\n";
    return true;
}
