#include "../utils/utils.h"          // point, calculate_distance()

#include <vector>
#include <cmath>
#include <limits>

inline double fast_hypot(double dx, double dy) { return std::sqrt(dx * dx + dy * dy); }
inline double dmin(double a, double b) { return (a < b) ? a : b; }
inline double dmax(double a, double b) { return (a > b) ? a : b; }
inline double dclamp(double v, double lo, double hi) { return (v < lo) ? lo : (v > hi) ? hi : v; }
inline int    iclamp(int v, int lo, int hi)           { return (v < lo) ? lo : (v > hi) ? hi : v; }

struct Vec2 { double x, y; };
inline Vec2 diff(const point &a, const point &b) { return {a.x - b.x, a.y - b.y}; }

// cosθ between vectors a→b and a→c  (requires |u|, |v| > 0 )
inline double cos_between(const Vec2 &u, const Vec2 &v) {
    double dot = u.x * v.x + u.y * v.y;
    double mag = fast_hypot(u.x, u.y) * fast_hypot(v.x, v.y);
    return (mag == 0.0) ? 1.0 : dclamp(dot / mag, -1.0, 1.0);
}

//  Variant 1 — linear angle‑penalty (0 → 2λ)
//   penalty = λ · (1 − cosθ)      (0 → 2λ when 180°)
std::vector<int> solve_hhi_greedy1(const std::vector<point> &coord, double lambda) {
    const int n = static_cast<int>(coord.size());
    if (n == 0) return {};

    std::vector<char> visited(n, 0);
    std::vector<int>  tour; tour.reserve(n + 1);

    int cur = 0; visited[0] = 1; tour.push_back(0);
    Vec2 prev_vec {0.0, 0.0};    // undefined in first step

    for (int step = 1; step < n; ++step) {
        int    best = -1;
        double best_cost = std::numeric_limits<double>::infinity();

        const double prev_mag = (step == 1) ? 0.0 : fast_hypot(prev_vec.x, prev_vec.y);

        for (int cand = 0; cand < n; ++cand) if (!visited[cand]) {
            double dist = calculate_distance(coord[cur], coord[cand]);

            double ang_pen = 0.0;
            if (step >= 2 && lambda > 0.0 && prev_mag > 0.0) {
                Vec2 cur_vec = diff(coord[cand], coord[cur]);
                double cosT  = (prev_vec.x * cur_vec.x + prev_vec.y * cur_vec.y) /
                                (prev_mag * fast_hypot(cur_vec.x, cur_vec.y));
                cosT  = dclamp(cosT, -1.0, 1.0);
                ang_pen = lambda * (1.0 - cosT);      // 0→2λ
            }
            double cost = dist + ang_pen;
            if (cost < best_cost) { best_cost = cost; best = cand; }
        }
        if (best == -1) break;   // should not happen

        prev_vec = diff(coord[best], coord[cur]);
        cur = best; visited[cur] = 1; tour.push_back(cur);
    }
    tour.push_back(0);
    return tour;
}

//  Variant 2 — 90° threshold quadratic penalty
//   θ ≤ 90°  → 0
//   θ > 90°  → λ · (−cosθ)² · 2     (0 → 2λ)
std::vector<int> solve_hhi_greedy2(const std::vector<point> &coord, double lambda) {
    const int n = static_cast<int>(coord.size());
    if (n == 0) return {};

    std::vector<char> visited(n, 0);
    std::vector<int>  tour; tour.reserve(n + 1);

    int cur = 0; visited[0] = 1; tour.push_back(0);
    Vec2 prev_vec {0.0, 0.0};

    for (int step = 1; step < n; ++step) {
        int    best = -1;
        double best_cost = std::numeric_limits<double>::infinity();
        const double prev_mag = (step == 1) ? 0.0 : fast_hypot(prev_vec.x, prev_vec.y);

        for (int cand = 0; cand < n; ++cand) if (!visited[cand]) {
            double dist = calculate_distance(coord[cur], coord[cand]);

            double ang_pen = 0.0;
            if (step >= 2 && lambda > 0.0 && prev_mag > 0.0) {
                Vec2 cur_vec = diff(coord[cand], coord[cur]);
                double cosT  = (prev_vec.x * cur_vec.x + prev_vec.y * cur_vec.y) /
                                (prev_mag * fast_hypot(cur_vec.x, cur_vec.y));
                cosT = dclamp(cosT, -1.0, 1.0);
                if (cosT < 0.0) {           // θ > 90°
                    double sev = -cosT;     // 0→1
                    ang_pen = lambda * sev * sev * 2.0;
                }
            }
            double cost = dist + ang_pen;
            if (cost < best_cost) { best_cost = cost; best = cand; }
        }
        if (best == -1) break;

        prev_vec = diff(coord[best], coord[cur]);
        cur = best; visited[cur] = 1; tour.push_back(cur);
    }
    tour.push_back(0);
    return tour;
}

//  Variant 3 — adaptive‑beam search (diversity) on top of Variant 2
//      beam_width = clamp(remain / 3, 1, BEAM_CAP)
struct BeamState {
    double          cost;          // accumulated cost
    int             cur;           // current node
    Vec2            prev_vec;      // prev→cur vector
    std::vector<char> vis;         // visited flags
    std::vector<int>  tour;        // path so far
};

std::vector<int> solve_hhi_greedy3(const std::vector<point> &coord, double lambda) {
    const int n = static_cast<int>(coord.size());
    if (n == 0) return {};
    const int BEAM_CAP = 50;

    // initial state
    BeamState init;
    init.cost = 0.0;
    init.cur  = 0;
    init.prev_vec = {0.0, 0.0};
    init.vis.assign(n, 0); init.vis[0] = 1;
    init.tour = {0};

    std::vector<BeamState> beam { init };

    for (int step = 1; step < n; ++step) {
        const int remain  = n - step;
        const int beam_w  = iclamp(remain / 3, 1, BEAM_CAP);
        std::vector<BeamState> next; next.reserve(beam.size() * beam_w);

        for (size_t bi = 0; bi < beam.size(); ++bi) {
            const BeamState &st = beam[bi];
            double prev_mag = (step == 1) ? 0.0 : fast_hypot(st.prev_vec.x, st.prev_vec.y);

            struct Cand { double cost; int idx; Vec2 vec; };
            // keep simple array of size <= beam_w
            Cand top[BEAM_CAP]; int top_sz = 0;

            for (int c = 0; c < n; ++c) if (!st.vis[c]) {
                double dist = calculate_distance(coord[st.cur], coord[c]);

                double ang_pen = 0.0;
                if (step >= 2 && lambda > 0.0 && prev_mag > 0.0) {
                    Vec2 cur_vec = diff(coord[c], coord[st.cur]);
                    double cosT  = (st.prev_vec.x * cur_vec.x + st.prev_vec.y * cur_vec.y) /
                                    (prev_mag * fast_hypot(cur_vec.x, cur_vec.y));
                    cosT = dclamp(cosT, -1.0, 1.0);
                    if (cosT < 0.0) {
                        double sev = -cosT;
                        ang_pen = lambda * sev * sev * 2.0;
                    }
                }
                double tot = st.cost + dist + ang_pen;

                // insert into top[0..top_sz)
                int pos = top_sz;
                while (pos > 0 && top[pos - 1].cost > tot) {
                    if (pos < BEAM_CAP) top[pos] = top[pos - 1];
                    --pos;
                }
                if (pos < BEAM_CAP) top[pos] = {tot, c, diff(coord[c], coord[st.cur])};
                if (top_sz < BEAM_CAP) top_sz = dmin(top_sz + 1, beam_w);
            }
            // materialise
            for (int k = 0; k < top_sz; ++k) {
                BeamState ns = st;
                ns.cur = top[k].idx;
                ns.prev_vec = top[k].vec;
                ns.vis[top[k].idx] = 1;
                ns.tour.push_back(top[k].idx);
                ns.cost = top[k].cost;
                next.push_back(std::move(ns));
            }
        }
        // pick best beam_w states
        const int keep = iclamp(beam_w, 1, static_cast<int>(next.size()));
        for (int i = 0; i < keep; ++i) {
            int best = i;
            for (int j = i + 1; j < static_cast<int>(next.size()); ++j)
                if (next[j].cost < next[best].cost) best = j;
            if (best != i) { BeamState tmp = next[i]; next[i] = next[best]; next[best] = tmp; }
        }
        next.resize(keep);
        beam.swap(next);
    }

    // best of beam
    int best_idx = 0;
    for (size_t i = 1; i < beam.size(); ++i)
        if (beam[i].cost < beam[best_idx].cost) best_idx = i;

    std::vector<int> result = beam[best_idx].tour;
    result.push_back(0);
    return result;
}
