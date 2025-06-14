#include "../utils/utils.h"
#include "hhi_greedy.h"

#include <vector>
#include <cmath>
#include <limits>

#define M_PI 3.14159265358979323846
#define M_PI_2 1.57079632679489661923


static inline double clampd(double x, double lo, double hi) {
    return (x < lo) ? lo : (x > hi) ? hi : x;
}

static inline double dist2(const point& a, const point& b) {
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    return dx * dx + dy * dy;
}

point unit_vector(const point& a, const point& b) {
    double dx = b.x - a.x;
    double dy = b.y - a.y;
    double n  = std::sqrt(dx * dx + dy * dy);
    if (n == 0.0) return {0.0, 0.0};
    return {dx / n, dy / n};
}

double cosine_similarity(const point& a, const point& b) {
    double dot = a.x * b.x + a.y * b.y;
    double norm_a = std::sqrt(a.x * a.x + a.y * a.y);
    double norm_b = std::sqrt(b.x * b.x + b.y * b.y);
    if (norm_a == 0.0 || norm_b == 0.0)
        return 0.0;
    return dot / (norm_a * norm_b);
}


struct Grid {
    int dim;
    double minX, minY, cell;
    std::vector<std::vector<int>> bucket;

    Grid(const std::vector<point>& pts, int d) : dim(d) {
        minX = pts[0].x; minY = pts[0].y;
        double maxX = pts[0].x, maxY = pts[0].y;
        for (const auto& p : pts) {
            if (p.x < minX) minX = p.x;
            if (p.y < minY) minY = p.y;
            if (p.x > maxX) maxX = p.x;
            if (p.y > maxY) maxY = p.y;
        }
        cell = ((maxX - minX) > (maxY - minY) ? (maxX - minX) : (maxY - minY)) / dim + 1e-9;
        bucket.resize(dim * dim);
        for (int i = 0; i < (int)pts.size(); ++i) {
            int c = cellIndex(pts[i]);
            bucket[c].push_back(i);
        }
    }

    inline int cellIndex(const point& p) const {
        int cx = int((p.x - minX) / cell);
        int cy = int((p.y - minY) / cell);
        if (cx < 0) cx = 0; if (cx >= dim) cx = dim - 1;
        if (cy < 0) cy = 0; if (cy >= dim) cy = dim - 1;
        return cy * dim + cx;
    }

    
    void kNeighbours(const point& center, const std::vector<char>& seen, int K, std::vector<int>& result) const {
        result.clear();
        int cx = int((center.x - minX) / cell);
        int cy = int((center.y - minY) / cell);

        int radius = 0;
        while ((int)result.size() < K && radius <= dim) {
            for (int dx = -radius; dx <= radius; ++dx) {
                for (int dy = -radius; dy <= radius; ++dy) {
                    if (std::abs(dx) != radius && std::abs(dy) != radius) continue; // 가장자리만

                    int nx = cx + dx, ny = cy + dy;
                    if (nx < 0 || nx >= dim || ny < 0 || ny >= dim) continue;

                    int idx = ny * dim + nx;
                    for (int id : bucket[idx]) {
                        if (!seen[id]) result.push_back(id);
                        if ((int)result.size() >= K) break;
                    }
                    if ((int)result.size() >= K) break;
                }
                if ((int)result.size() >= K) break;
            }
            ++radius;
        }
    }


};


static inline void reverse_range(std::vector<int>& v, int l, int r) {
    while (l < r) {
        int t = v[l];
        v[l] = v[r];
        v[r] = t;
        ++l; --r;
    }
}

std::vector<int> compute_convex_hull(const std::vector<point>& pts) {
    int n = pts.size();
    if (n < 3) return {};

    std::vector<int> hull;

    int leftmost = 0;
    for (int i = 1; i < n; ++i) {
        if (pts[i].x < pts[leftmost].x ||
            (pts[i].x == pts[leftmost].x && pts[i].y < pts[leftmost].y)) {
            leftmost = i;
        }
    }

    int p = leftmost;
    do {
        hull.push_back(p);
        int q = (p + 1) % n;
        for (int r = 0; r < n; ++r) {
            double cross = (pts[q].x - pts[p].x) * (pts[r].y - pts[p].y) -
                           (pts[q].y - pts[p].y) * (pts[r].x - pts[p].x);
            if (cross < 0.0) q = r;
        }
        p = q;
    } while (p != leftmost);

    return hull;
}

// greedy 함수 구현 
// 2opt 없고 90까지는 패널티 없고 90-180까지 패널티 부과 
std::vector<int> solve_hhi_greedy(const std::vector<point>& pts,
                                  double lambda0_factor = 1.0) {
    int K = 16;
    const int n = (int)pts.size();
    if (n == 0) return {};

    int dim = (int)std::sqrt((double)n / 4.0) + 1;
    Grid grid(pts, dim);

    double diag = std::sqrt(dist2(pts.front(), pts.back()));
    double avgLen = diag / std::sqrt((double)n);
    double lambda0 = lambda0_factor * avgLen;
    double tau = 0.3 * n;

    auto lambda = [lambda0, tau](int step) {
        return lambda0 * std::exp(-step / tau);
    };

    std::vector<int> tour; tour.reserve(n + 1);
    std::vector<char> seen(n, 0);

    int cur = 0; seen[cur] = 1; tour.push_back(cur);
    point prevDir{1.0, 0.0};

    std::vector<int> cand; cand.reserve(K);

    for (int step = 1; step < n; ++step) {
        grid.kNeighbours(pts[cur], seen, K, cand);
        if (cand.empty()) {
            for (int id = 0; id < n; ++id) if (!seen[id]) cand.push_back(id);
        }

        double bestScore = std::numeric_limits<double>::infinity();
        int best = -1; point bestDir{0, 0};

        for (int nb : cand) {
            double d2 = dist2(pts[cur], pts[nb]);
            point dir = unit_vector(pts[cur], pts[nb]);

            double cosSim = cosine_similarity(prevDir, dir);
            double pen = (cosSim >= 0.0) ? 0.0 : std::exp(-cosSim);

            double score = d2 + lambda(step) * pen;
            if (score < bestScore) {
                bestScore = score;
                best = nb;
                bestDir = dir;
            }
        }

        cur = best;
        prevDir = bestDir;
        seen[cur] = 1;
        tour.push_back(cur);
    }

    tour.push_back(tour.front());
    return tour;
}

// 2opt 넣은 버전으로 
std::vector<int> solve_hhi_greedy_2opt(const std::vector<point>& pts,
                                  double lambda0_factor = 1.0) {
    int K = 16;
    const int n = (int)pts.size();
    if (n == 0) return {};

    int dim = (int)std::sqrt((double)n / 4.0) + 1;
    Grid grid(pts, dim);

    double diag = std::sqrt(dist2(pts.front(), pts.back()));
    double avgLen = diag / std::sqrt((double)n);
    double lambda0 = lambda0_factor * avgLen;
    double tau = 0.3 * n;

    auto lambda = [lambda0, tau](int step) {
        return lambda0 * std::exp(-step / tau);
    };

    std::vector<int> tour; tour.reserve(n + 1);
    std::vector<char> seen(n, 0);

    int cur = 0; seen[cur] = 1; tour.push_back(cur);
    point prevDir{1.0, 0.0};

    std::vector<int> cand; cand.reserve(K);

    for (int step = 1; step < n; ++step) {
        grid.kNeighbours(pts[cur], seen, K, cand);
        if (cand.empty()) {
            for (int id = 0; id < n; ++id) if (!seen[id]) cand.push_back(id);
        }

        double bestScore = std::numeric_limits<double>::infinity();
        int best = -1; point bestDir{0, 0};

        for (int nb : cand) {
            double d2 = dist2(pts[cur], pts[nb]);
            point dir = unit_vector(pts[cur], pts[nb]);

            double cosSim = cosine_similarity(prevDir, dir);
            double pen = (cosSim >= 0.0) ? 0.0 : std::exp(-cosSim);

            double score = d2 + lambda(step) * pen;
            if (score < bestScore) {
                bestScore = score;
                best = nb;
                bestDir = dir;
            }
        }

        cur = best;
        prevDir = bestDir;
        seen[cur] = 1;
        tour.push_back(cur);
    }

    tour.push_back(tour.front());

    const int N = tour.size() - 1;
    bool improved = true;
    int sweeps = 0, MAX_SWEEP = 1;

    while (improved && sweeps < MAX_SWEEP) {
        improved = false; ++sweeps;
        for (int i = 1; i < N - 2 && !improved; ++i) {
            for (int j = i + 2; j < N - 1; ++j) {
                int a = tour[i - 1], b = tour[i];
                int c = tour[j], d = tour[j + 1];
                double before = dist2(pts[a], pts[b]) + dist2(pts[c], pts[d]);
                double after  = dist2(pts[a], pts[c]) + dist2(pts[b], pts[d]);
                if (after < before) {
                    reverse_range(tour, i, j);
                    improved = true;
                    break;
                }
            }
        }
    }

    return tour;
}

// Adaptive K, 2-opt 없음
// K는 어떻게 변형되냐면 현지 점이 위치한 grid 셀의 점 밀도에 따라 결정됨 
// 걍 단순하게 밀도가 30보다 크면 8, 15-30이면 16, 0-15면 32
// 2 거듭제곱이니까 8 16 32
std::vector<int> solve_hhi_greedy_basic_adapK(const std::vector<point>& pts,
                                               double lambda0_factor = 1.0) {
    const int n = (int)pts.size();
    if (n == 0) return {};

    int dim = (int)std::sqrt((double)n / 4.0) + 1;
    Grid grid(pts, dim);

    double diag = std::sqrt(dist2(pts.front(), pts.back()));
    double avgLen = diag / std::sqrt((double)n);
    double lambda0 = lambda0_factor * avgLen;
    double tau = 0.3 * n;

    auto lambda = [lambda0, tau](int step) {
        return lambda0 * std::exp(-step / tau);
    };

    auto compute_adaptive_K = [&](const point& p) {
        int density = (int)grid.bucket[grid.cellIndex(p)].size();
        if (density >= 30) return 8;
        if (density >= 15) return 16;
        return 32;
    };

    std::vector<int> tour; tour.reserve(n + 1);
    std::vector<char> seen(n, 0);

    int cur = 0; seen[cur] = 1; tour.push_back(cur);
    point prevDir{1.0, 0.0};

    std::vector<int> cand;

    for (int step = 1; step < n; ++step) {
        int K = compute_adaptive_K(pts[cur]);
        cand.reserve(K);
        grid.kNeighbours(pts[cur], seen, K, cand);
        if (cand.empty()) {
            for (int id = 0; id < n; ++id) if (!seen[id]) cand.push_back(id);
        }

        double bestScore = std::numeric_limits<double>::infinity();
        int best = -1; point bestDir{0, 0};

        for (int nb : cand) {
            double d2 = dist2(pts[cur], pts[nb]);
            point dir = unit_vector(pts[cur], pts[nb]);
            double cosSim = cosine_similarity(prevDir, dir);
            double pen = (cosSim >= 0.0) ? 0.0 : std::exp(-cosSim);
            double score = d2 + lambda(step) * pen;
            if (score < bestScore) {
                bestScore = score;
                best = nb;
                bestDir = dir;
            }
        }

        cur = best;
        prevDir = bestDir;
        seen[cur] = 1;
        tour.push_back(cur);
    }

    tour.push_back(tour.front());
    return tour;
}

// Adaptive K + 1-sweep 2-opt
std::vector<int> solve_hhi_greedy_2opt_adapK(const std::vector<point>& pts,
                                              double lambda0_factor = 1.0) {
    std::vector<int> tour = solve_hhi_greedy_basic_adapK(pts, lambda0_factor);
    const int N = tour.size() - 1;
    bool improved = true;
    int sweeps = 0, MAX_SWEEP = 1;

    while (improved && sweeps < MAX_SWEEP) {
        improved = false; ++sweeps;
        for (int i = 1; i < N - 2 && !improved; ++i) {
            for (int j = i + 2; j < N - 1; ++j) {
                int a = tour[i - 1], b = tour[i];
                int c = tour[j], d = tour[j + 1];
                double before = dist2(pts[a], pts[b]) + dist2(pts[c], pts[d]);
                double after  = dist2(pts[a], pts[c]) + dist2(pts[b], pts[d]);
                if (after < before) {
                    reverse_range(tour, i, j);
                    improved = true;
                    break;
                }
            }
        }
    }
    return tour;
}


// 주변 9cell 평균내서 k 정함 
// 왜냐면 최외각 점(cell 하나에 하나밖에 없는 경우) 체크하기 위해서 
std::vector<int> solve_hhi_greedy_basic_adapK_9CellMean(const std::vector<point>& pts,
                                                      double lambda0_factor = 1.0) {
    const int n = (int)pts.size();
    if (n == 0) return {};

    int dim = (int)std::sqrt((double)n / 4.0) + 1;
    Grid grid(pts, dim);

    double diag = std::sqrt(dist2(pts.front(), pts.back()));
    double avgLen = diag / std::sqrt((double)n);
    double lambda0 = lambda0_factor * avgLen;
    double tau = 0.3 * n;

    auto lambda = [lambda0, tau](int step) {
        return lambda0 * std::exp(-step / tau);
    };

    auto compute_local_density = [&](const point& p) {
        int cx = int((p.x - grid.minX) / grid.cell);
        int cy = int((p.y - grid.minY) / grid.cell);
        int count = 0;
        for (int dx = -1; dx <= 1; ++dx) {
            for (int dy = -1; dy <= 1; ++dy) {
                int nx = cx + dx, ny = cy + dy;
                if (nx < 0 || nx >= grid.dim || ny < 0 || ny >= grid.dim) continue;
                int idx = ny * grid.dim + nx;
                count += grid.bucket[idx].size();
            }
        }
        return count;
    };

    auto compute_adaptive_K = [&](const point& p) {
        int density = compute_local_density(p);
        if (density >= 90) return 8;
        if (density >= 45) return 16;
        return 32;
    };

    std::vector<int> tour; tour.reserve(n + 1);
    std::vector<char> seen(n, 0);

    int cur = 0; seen[cur] = 1; tour.push_back(cur);
    point prevDir{1.0, 0.0};

    std::vector<int> cand;

    for (int step = 1; step < n; ++step) {
        int K = compute_adaptive_K(pts[cur]);
        cand.reserve(K);
        grid.kNeighbours(pts[cur], seen, K, cand);
        if (cand.empty()) {
            for (int id = 0; id < n; ++id) if (!seen[id]) cand.push_back(id);
        }

        double bestScore = std::numeric_limits<double>::infinity();
        int best = -1; point bestDir{0, 0};

        for (int nb : cand) {
            double d2 = dist2(pts[cur], pts[nb]);
            point dir = unit_vector(pts[cur], pts[nb]);
            double cosSim = cosine_similarity(prevDir, dir);
            double pen = (cosSim >= 0.0) ? 0.0 : std::exp(-cosSim);
            double score = d2 + lambda(step) * pen;
            if (score < bestScore) {
                bestScore = score;
                best = nb;
                bestDir = dir;
            }
        }

        cur = best;
        prevDir = bestDir;
        seen[cur] = 1;
        tour.push_back(cur);
    }

    tour.push_back(tour.front());
    return tour;
}

// 2opt 적용 
std::vector<int> solve_hhi_greedy_2opt_adapK_9CellMean(const std::vector<point>& pts,
                                                      double lambda0_factor = 1.0) {
    std::vector<int> tour = solve_hhi_greedy_basic_adapK_9CellMean(pts, lambda0_factor);
    const int N = tour.size() - 1;
    bool improved = true;
    int sweeps = 0, MAX_SWEEP = 1;

    while (improved && sweeps < MAX_SWEEP) {
        improved = false; ++sweeps;
        for (int i = 1; i < N - 2 && !improved; ++i) {
            for (int j = i + 2; j < N - 1; ++j) {
                int a = tour[i - 1], b = tour[i];
                int c = tour[j], d = tour[j + 1];
                double before = dist2(pts[a], pts[b]) + dist2(pts[c], pts[d]);
                double after  = dist2(pts[a], pts[c]) + dist2(pts[b], pts[d]);
                if (after < before) {
                    reverse_range(tour, i, j);
                    improved = true;
                    break;
                }
            }
        }
    }
    return tour;
}

// 멀티 스타트 시작 일단 처음엔 8개로 잡고 그중에서제일 나은거 이미 시간이 짧게 걸리니까 시도해보는거임
constexpr int MULTI_STARTS = 8;
std::vector<int> solve_hhi_greedy_basic_adapK_multi(const std::vector<point>& pts, double lambda0_factor) {
    const int n = (int)pts.size();
    if (n == 0) return {};

    std::vector<int> best_tour;
    double best_length = std::numeric_limits<double>::infinity();

    for (int trial = 0; trial < MULTI_STARTS; ++trial) {
        std::vector<point> rotated_pts = pts;

        double angle = (2.0 * M_PI / MULTI_STARTS) * trial;
        double cos_a = std::cos(angle), sin_a = std::sin(angle);
        for (auto& p : rotated_pts) {
            double x = p.x, y = p.y;
            p.x = cos_a * x - sin_a * y;
            p.y = sin_a * x + cos_a * y;
        }

        std::vector<int> tour = solve_hhi_greedy_basic_adapK(rotated_pts, lambda0_factor);

        double len = 0.0;
        for (size_t i = 0; i + 1 < tour.size(); ++i) {
            len += std::sqrt(dist2(pts[tour[i]], pts[tour[i + 1]]));
        }

        if (len < best_length) {
            best_length = len;
            best_tour = tour;
        }
    }

    return best_tour;
}

std::vector<int> solve_hhi_greedy_2opt_adapK_multi(const std::vector<point>& pts, double lambda0_factor) {
    const int n = (int)pts.size();
    if (n == 0) return {};

    std::vector<int> best_tour;
    double best_length = std::numeric_limits<double>::infinity();

    for (int trial = 0; trial < MULTI_STARTS; ++trial) {
        std::vector<point> rotated_pts = pts;

        double angle = (2.0 * M_PI / MULTI_STARTS) * trial;
        double cos_a = std::cos(angle), sin_a = std::sin(angle);
        for (auto& p : rotated_pts) {
            double x = p.x, y = p.y;
            p.x = cos_a * x - sin_a * y;
            p.y = sin_a * x + cos_a * y;
        }

        std::vector<int> tour = solve_hhi_greedy_2opt_adapK(rotated_pts, lambda0_factor);

        double len = 0.0;
        for (size_t i = 0; i + 1 < tour.size(); ++i) {
            len += std::sqrt(dist2(pts[tour[i]], pts[tour[i + 1]]));
        }

        if (len < best_length) {
            best_length = len;
            best_tour = tour;
        }
    }

    return best_tour;
}

std::vector<int> solve_hhi_greedy_anglePenalty(const std::vector<point>& pts,
                                  double lambda0_factor) {
    int K = 16;
    const int n = (int)pts.size();
    if (n == 0) return {};

    int dim = (int)std::sqrt((double)n / 4.0) + 1;
    Grid grid(pts, dim);

    double diag = std::sqrt(dist2(pts.front(), pts.back()));
    double avgLen = diag / std::sqrt((double)n);
    double lambda0 = lambda0_factor * avgLen;
    double tau = 0.3 * n;

    auto lambda = [lambda0, tau](int step) {
        return lambda0 * std::exp(-step / tau);
    };

    std::vector<int> tour; tour.reserve(n + 1);
    std::vector<char> seen(n, 0);

    int cur = 0; seen[cur] = 1; tour.push_back(cur);
    point prevDir{1.0, 0.0};

    std::vector<int> cand; cand.reserve(K);

    for (int step = 1; step < n; ++step) {
        grid.kNeighbours(pts[cur], seen, K, cand);
        if (cand.empty()) {
            for (int id = 0; id < n; ++id) if (!seen[id]) cand.push_back(id);
        }

        double bestScore = std::numeric_limits<double>::infinity();
        int best = -1; point bestDir{0, 0};

        for (int nb : cand) {
            double d2 = dist2(pts[cur], pts[nb]);
            point dir = unit_vector(pts[cur], pts[nb]);

            double cosSim = cosine_similarity(prevDir, dir);
            // 걍 cos 안쓰고 angle 쓴ㄴ거
            double angle = std::acos(clampd(cosSim, -1.0, 1.0));
            double pen = (angle <= M_PI_2) ? 0.0 : (angle - M_PI_2) * (angle - M_PI_2);


            double score = d2 + lambda(step) * pen;
            if (score < bestScore) {
                bestScore = score;
                best = nb;
                bestDir = dir;
            }
        }

        cur = best;
        prevDir = bestDir;
        seen[cur] = 1;
        tour.push_back(cur);
    }

    tour.push_back(tour.front());
    return tour;
}

std::vector<int> solve_hhi_greedy_linearPenalty(const std::vector<point>& pts,
                                  double lambda0_factor) {
    int K = 16;
    const int n = (int)pts.size();
    if (n == 0) return {};

    int dim = (int)std::sqrt((double)n / 4.0) + 1;
    Grid grid(pts, dim);

    double diag = std::sqrt(dist2(pts.front(), pts.back()));
    double avgLen = diag / std::sqrt((double)n);
    double lambda0 = lambda0_factor * avgLen;
    double tau = 0.3 * n;

    auto lambda = [lambda0, tau](int step) {
        return lambda0 * std::exp(-step / tau);
    };

    std::vector<int> tour; tour.reserve(n + 1);
    std::vector<char> seen(n, 0);

    int cur = 0; seen[cur] = 1; tour.push_back(cur);
    point prevDir{1.0, 0.0};

    std::vector<int> cand; cand.reserve(K);

    for (int step = 1; step < n; ++step) {
        grid.kNeighbours(pts[cur], seen, K, cand);
        if (cand.empty()) {
            for (int id = 0; id < n; ++id) if (!seen[id]) cand.push_back(id);
        }

        double bestScore = std::numeric_limits<double>::infinity();
        int best = -1; point bestDir{0, 0};

        for (int nb : cand) {
            double d2 = dist2(pts[cur], pts[nb]);
            point dir = unit_vector(pts[cur], pts[nb]);

            double cosSim = cosine_similarity(prevDir, dir);
            double pen = std::max(0.0, -cosSim);

            double score = d2 + lambda(step) * pen;
            if (score < bestScore) {
                bestScore = score;
                best = nb;
                bestDir = dir;
            }
        }

        cur = best;
        prevDir = bestDir;
        seen[cur] = 1;
        tour.push_back(cur);
    }

    tour.push_back(tour.front());
    return tour;
}

// 최외각 node체크하는 convex hull만들고 걔한테 패널티 0.5, 0.3 -> 최적, 0.1배 줘서
// 만약 가까운 grid 라면 그거 처리하고 넘어가게끔 유도 
// 그리고 현재 node가 hull이라면 맨날 멀리 떨어져있으니까 k는 32로 
std::vector<int> solve_hhi_greedy_basic_adapK_hull(const std::vector<point>& pts,
                                                   double lambda0_factor = 1.0) {
    const int n = (int)pts.size();
    if (n == 0) return {};

    // Convex Hull 계산
    std::vector<int> hull = compute_convex_hull(pts);
    std::vector<bool> is_hull(n, false);
    for (int idx : hull) is_hull[idx] = true;

    int dim = (int)std::sqrt((double)n / 4.0) + 1;
    Grid grid(pts, dim);

    double diag = std::sqrt(dist2(pts.front(), pts.back()));
    double avgLen = diag / std::sqrt((double)n);
    double lambda0 = lambda0_factor * avgLen;
    double tau = 0.3 * n;

    auto lambda = [lambda0, tau](int step) {
        return lambda0 * std::exp(-step / tau);
    };

    auto compute_adaptive_K = [&](const point& p, bool isHull) {
        int density = (int)grid.bucket[grid.cellIndex(p)].size();
        int baseK = (density >= 30) ? 8 : (density >= 15) ? 16 : 32;
        return isHull ? std::max(baseK, 32) : baseK;
    };

    std::vector<int> tour; tour.reserve(n + 1);
    std::vector<char> seen(n, 0);

    int cur = 0; seen[cur] = 1; tour.push_back(cur);
    point prevDir{1.0, 0.0};

    std::vector<int> cand;

    for (int step = 1; step < n; ++step) {
        int K = compute_adaptive_K(pts[cur], is_hull[cur]);
        cand.reserve(K);
        grid.kNeighbours(pts[cur], seen, K, cand);
        if (cand.empty()) {
            for (int id = 0; id < n; ++id) if (!seen[id]) cand.push_back(id);
        }

        double bestScore = std::numeric_limits<double>::infinity();
        int best = -1; point bestDir{0, 0};

        for (int nb : cand) {
            double d2 = dist2(pts[cur], pts[nb]);
            point dir = unit_vector(pts[cur], pts[nb]);
            double cosSim = cosine_similarity(prevDir, dir);
            double pen = (cosSim >= 0.0) ? 0.0 : std::exp(-cosSim);

            // 후보가 Hull에 있으면 penalty 줄이기
            if (is_hull[nb]) pen *= 0.3;

            double score = d2 + lambda(step) * pen;
            if (score < bestScore) {
                bestScore = score;
                best = nb;
                bestDir = dir;
            }
        }

        cur = best;
        prevDir = bestDir;
        seen[cur] = 1;
        tour.push_back(cur);
    }

    tour.push_back(tour.front());
    return tour;
}

std::vector<int> solve_hhi_greedy_2opt_adapK_hull(const std::vector<point>& pts,
                                                  double lambda0_factor = 1.0) {
    // 기본 hull-aware greedy 알고리즘으로 초기 tour 생성
    std::vector<int> tour = solve_hhi_greedy_basic_adapK_hull(pts, lambda0_factor);
    const int N = (int)tour.size() - 1;
    if (N < 4) return tour;

    // 1-sweep 2-opt
    bool improved = true;
    int sweeps = 0, MAX_SWEEP = 1;

    while (improved && sweeps < MAX_SWEEP) {
        improved = false;
        ++sweeps;
        for (int i = 1; i < N - 2 && !improved; ++i) {
            for (int j = i + 2; j < N - 1; ++j) {
                int a = tour[i - 1], b = tour[i];
                int c = tour[j], d = tour[j + 1];
                double before = dist2(pts[a], pts[b]) + dist2(pts[c], pts[d]);
                double after  = dist2(pts[a], pts[c]) + dist2(pts[b], pts[d]);
                if (after < before) {
                    reverse_range(tour, i, j);
                    improved = true;
                    break;
                }
            }
        }
    }

    return tour;
}

// 9cellmean에 hull추가 
// Convex Hull 적용 + 9CellMean
std::vector<int> solve_hhi_greedy_basic_adapK_9CellMean_hull(const std::vector<point>& pts,
                                                              double lambda0_factor = 1.0) {
    const int n = (int)pts.size();
    if (n == 0) return {};

    // Hull 계산
    std::vector<int> hull = compute_convex_hull(pts);
    std::vector<bool> is_hull(n, false);
    for (int idx : hull) is_hull[idx] = true;

    int dim = (int)std::sqrt((double)n / 4.0) + 1;
    Grid grid(pts, dim);

    double diag = std::sqrt(dist2(pts.front(), pts.back()));
    double avgLen = diag / std::sqrt((double)n);
    double lambda0 = lambda0_factor * avgLen;
    double tau = 0.3 * n;

    auto lambda = [lambda0, tau](int step) {
        return lambda0 * std::exp(-step / tau);
    };

    auto compute_local_density = [&](const point& p) {
        int cx = int((p.x - grid.minX) / grid.cell);
        int cy = int((p.y - grid.minY) / grid.cell);
        int count = 0;
        for (int dx = -1; dx <= 1; ++dx) {
            for (int dy = -1; dy <= 1; ++dy) {
                int nx = cx + dx, ny = cy + dy;
                if (nx < 0 || nx >= grid.dim || ny < 0 || ny >= grid.dim) continue;
                int idx = ny * grid.dim + nx;
                count += grid.bucket[idx].size();
            }
        }
        return count;
    };

    auto compute_adaptive_K = [&](const point& p) {
        int density = compute_local_density(p);
        if (density >= 90) return 8;
        if (density >= 45) return 16;
        return 32;
    };

    std::vector<int> tour; tour.reserve(n + 1);
    std::vector<char> seen(n, 0);

    int cur = 0; seen[cur] = 1; tour.push_back(cur);
    point prevDir{1.0, 0.0};

    std::vector<int> cand;

    for (int step = 1; step < n; ++step) {
        int K = compute_adaptive_K(pts[cur]);
        cand.reserve(K);
        grid.kNeighbours(pts[cur], seen, K, cand);
        if (cand.empty()) {
            for (int id = 0; id < n; ++id) if (!seen[id]) cand.push_back(id);
        }

        double bestScore = std::numeric_limits<double>::infinity();
        int best = -1; point bestDir{0, 0};

        for (int nb : cand) {
            double d2 = dist2(pts[cur], pts[nb]);
            point dir = unit_vector(pts[cur], pts[nb]);
            double cosSim = cosine_similarity(prevDir, dir);
            double pen = (cosSim >= 0.0) ? 0.0 : std::exp(-cosSim);

            // Hull 점에 패널티 감소
            if (is_hull[nb]) pen *= 0.25;

            double score = d2 + lambda(step) * pen;
            if (score < bestScore) {
                bestScore = score;
                best = nb;
                bestDir = dir;
            }
        }

        cur = best;
        prevDir = bestDir;
        seen[cur] = 1;
        tour.push_back(cur);
    }

    tour.push_back(tour.front());
    return tour;
}

std::vector<int> solve_hhi_greedy_2opt_adapK_9CellMean_hull(const std::vector<point>& pts,
                                                            double lambda0_factor = 1.0) {
    std::vector<int> tour = solve_hhi_greedy_basic_adapK_9CellMean_hull(pts, lambda0_factor);
    const int N = tour.size() - 1;
    bool improved = true;
    int sweeps = 0, MAX_SWEEP = 1;

    while (improved && sweeps < MAX_SWEEP) {
        improved = false; ++sweeps;
        for (int i = 1; i < N - 2 && !improved; ++i) {
            for (int j = i + 2; j < N - 1; ++j) {
                int a = tour[i - 1], b = tour[i];
                int c = tour[j], d = tour[j + 1];
                double before = dist2(pts[a], pts[b]) + dist2(pts[c], pts[d]);
                double after  = dist2(pts[a], pts[c]) + dist2(pts[b], pts[d]);
                if (after < before) {
                    reverse_range(tour, i, j);
                    improved = true;
                    break;
                }
            }
        }
    }

    return tour;
}

std::vector<int> solve_hhi_greedy_2opt_adapK_hull_multi(const std::vector<point>& pts,
                                                        double lambda0_factor = 1.0) {
    const int n = (int)pts.size();
    if (n == 0) return {};

    std::vector<int> best_tour;
    double best_length = std::numeric_limits<double>::infinity();

    for (int trial = 0; trial < MULTI_STARTS; ++trial) {
        std::vector<point> rotated_pts = pts;

        // 점들 회전
        double angle = (2.0 * M_PI / MULTI_STARTS) * trial;
        double cos_a = std::cos(angle), sin_a = std::sin(angle);
        for (auto& p : rotated_pts) {
            double x = p.x, y = p.y;
            p.x = cos_a * x - sin_a * y;
            p.y = sin_a * x + cos_a * y;
        }

        // 회전된 좌표로 초기 greedy + 2-opt
        std::vector<int> tour = solve_hhi_greedy_2opt_adapK_hull(rotated_pts, lambda0_factor);

        // 원래 좌표 기준 거리 계산 (회전된 건 사용 안함)
        double len = 0.0;
        for (size_t i = 0; i + 1 < tour.size(); ++i) {
            len += std::sqrt(dist2(pts[tour[i]], pts[tour[i + 1]]));
        }

        if (len < best_length) {
            best_length = len;
            best_tour = tour;
        }
    }

    return best_tour;
}
