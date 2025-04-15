# 컴파일러 설정
CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall

# 실행파일 이름
TARGET = sort

# 모든 .cpp 파일
SRCS = $(wildcard *.cpp)

# 대응되는 .o 파일들
OBJS = $(SRCS:.cpp=.o)

# 기본 빌드 명령
all: $(TARGET)

# 링크 단계
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# .cpp -> .o 로 컴파일
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 정리용
clean:
	rm -f $(OBJS) $(TARGET)
