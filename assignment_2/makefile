# 컴파일러 및 플래그
CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall

# 디렉토리 구조
SRCDIR = .
OUTDIR = output
UTILDIR = utils
MSTDIR = MST
HKDIR = Held-Karp
HHIDIR = hhi
GREEDYDIR = greedy
INSERTDIR = insertion
ANNDIR = annealing


# 소스 파일
SRC = $(SRCDIR)/test.cpp \
      $(UTILDIR)/utils.cpp \
      $(MSTDIR)/MST.cpp \
      $(HKDIR)/held_karp.cpp\
      $(HHIDIR)/hhi.cpp\
      $(GREEDYDIR)/greedy.cpp\
      $(INSERTDIR)/insertion.cpp\
      $(ANNDIR)/annealing.cpp

# 출력 파일
TARGET = $(OUTDIR)/test

# 기본 빌드 명령
all: $(TARGET)

$(TARGET): $(SRC)
	@mkdir -p $(OUTDIR)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

# 정리 명령
clean:
	rm -f $(TARGET)

.PHONY: all clean
