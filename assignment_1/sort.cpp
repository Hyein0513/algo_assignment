#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

int main() {
    std::string directory_path = "./logs"; // txt 파일들이 있는 폴더
    std::string output_filename = "merged_output.txt"; // 결과 파일

    std::ofstream output_file(output_filename);
    if (!output_file.is_open()) {
        std::cerr << "출력 파일을 열 수 없습니다: " << output_filename << "\n";
        return 1;
    }

    for (const auto& entry : fs::directory_iterator(directory_path)) {
        if (entry.path().extension() == ".txt") {
            std::ifstream input_file(entry.path());
            if (!input_file.is_open()) {
                std::cerr << "파일을 열 수 없습니다: " << entry.path() << "\n";
                continue;
            }

            // 파일명 주석 추가
            output_file << "==== " << entry.path().filename().string() << " 시작 ====\n";

            std::string line;
            while (std::getline(input_file, line)) {
                output_file << line << "\n";
            }

            output_file << "==== " << entry.path().filename().string() << " 끝 ====\n\n";

            input_file.close(); // 원본 파일은 그대로 유지됨
        }
    }

    output_file.close();
    std::cout << "모든 .txt 파일이 '" << output_filename << "'로 병합되었습니다.\n";
    return 0;
}
