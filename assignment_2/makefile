# 컴파일러 및 플래그
CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall

# 디렉토리 구조
SRCDIR = .
OUTDIR = output
UTILDIR = utils
MSTDIR = MST
HKDIR = Held-Karp
HHIGREEDYDIR = hhi_greedy
HHIGREEDYDIR1 = hhi_greedy1
HHIMSTDIR = hhi_MST
HHIEULERDIR = hhi_euler
GREEDYDIR = greedy
INSERTDIR = insertion

# 공통 소스 파일
COMMON_SRC = \
    $(UTILDIR)/utils.cpp \
    $(MSTDIR)/MST.cpp \
    $(HKDIR)/held_karp.cpp \
    $(HHIGREEDYDIR)/hhi_greedy.cpp \
    $(HHIGREEDYDIR1)/hhi_greedy1.cpp \
    $(HHIMSTDIR)/hhi_MST.cpp \
    $(HHIEULERDIR)/hhi_euler.cpp\
    $(GREEDYDIR)/greedy.cpp \
    $(INSERTDIR)/insertion.cpp 

# 타겟 파일들
TARGET = $(OUTDIR)/test $(OUTDIR)/test_hhi $(OUTDIR)/make_csv

# 기본 빌드 명령
all: $(TARGET)

# 개별 타겟 빌드
$(OUTDIR)/test: $(SRCDIR)/test.cpp $(COMMON_SRC)
	@mkdir -p $(OUTDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OUTDIR)/test_hhi: $(SRCDIR)/test_hhi.cpp $(COMMON_SRC)
	@mkdir -p $(OUTDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^
    
$(OUTDIR)/make_csv: $(SRCDIR)/make_csv.cpp $(COMMON_SRC)
	@mkdir -p $(OUTDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# 정리 명령
clean:
	rm -f $(TARGET)

.PHONY: all clean
