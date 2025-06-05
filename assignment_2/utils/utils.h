#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <string>
#include <sstream>

// tsp 파일에서 좌표 뜯어올때 x좌표 y좌표 저장하는 구조체 point 정의 
struct point {
    double x;
    double y;
};

// .tsp 파일에서 좌표 읽기
std::vector<point> read_TSP_file(const std::string& file);

// 두 점 사이의 거리 계산
int calculate_distance(const point& a, const point& b);

// 거리 행렬 생성
std::vector<std::vector<int>> calculate_distance_matrix(const std::vector<point>& coord);

// 경로 총 길이 계산
int calculate_total_distance(const std::vector<int>& path, const std::vector<point>& coord);

// 투어 결과 파일 저장 (길이만 저장할 수도 있음)
void save_tour_file(const std::string& filename,
                    const std::vector<int>& path,
                    const std::vector<point>& coord,
                    const std::string& data_name,
                    bool flag);

// 두 투어 파일 비교 (정답 체크용, 로그 저장용)
bool compare_tour_file(const std::string& gt_file,
                       const std::string& my_file,
                       std::stringstream& log);

#endif // UTILS_H
