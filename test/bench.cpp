#include "comparison/earcut.hpp"
#include "comparison/libtess2.hpp"
#include "fixtures/geometries.hpp"

#include <cstdio>
#include <vector>
#include <chrono>

template <template <typename, typename> class Tesselator, typename Coord, typename Polygon>
int64_t bench(const Polygon &polygon) {
    std::vector<uint64_t> runs;
    uint64_t total = 0;
    uint32_t warmup = 0;

    Tesselator<Coord, Polygon> tesselator { polygon };

    while (total < 2e9 || runs.size() < 100) {
        const auto started = std::chrono::high_resolution_clock::now();
        tesselator.run();
        const auto finished = std::chrono::high_resolution_clock::now();

        // Don't count the first couple of iterations.
        if (warmup >= 10) {
            const auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(finished - started).count();
            runs.push_back(duration);
            total += duration;
        } else {
            warmup++;
        }
    }

    return double(runs.size()) / (double(total) / 1e9);
}

template <typename Coord, typename Polygon>
void report(const char *name, const Polygon &polygon, const int cols[]) {

    fprintf(stderr, "| %-*s | ", cols[0], name);
    auto earcut = bench<EarcutTesselator, Coord>(polygon);
    fprintf(stderr, "%'*lld ops/s | ", cols[1] - 6, earcut);
    auto libtess2 = bench<Libtess2Tesselator, Coord>(polygon);
    fprintf(stderr, "%'*lld ops/s |\n", cols[2] - 6, libtess2);
}

void separator(const int cols[]) {
    const char *padding="-------------------------------------------------------------------------";
    for (int i = 0; cols[i]; i++) {
        fprintf(stderr, "+-%.*s-", cols[i], padding);
    }
    fprintf(stderr, "+\n");
}

#include <clocale>
int main() {
    setlocale(LC_NUMERIC, "");

    const int cols[] = { 14, 18, 18, 0 };

    separator(cols);
    fprintf(stderr, "| %-*s | %-*s | %-*s |\n", cols[0], "Polygon", cols[1], "earcut", cols[2], "libtess2");
    separator(cols);

    report<int>("bad_hole", mapbox::fixtures::bad_hole, cols);
    report<int>("building", mapbox::fixtures::building, cols);
    report<int>("degenerate", mapbox::fixtures::degenerate, cols);
    report<double>("dude", mapbox::fixtures::dude, cols);
    report<int>("empty_square", mapbox::fixtures::empty_square, cols);
    report<int>("water_huge", mapbox::fixtures::water_huge, cols);
    report<int>("water_huge2", mapbox::fixtures::water_huge2, cols);
    report<int>("water", mapbox::fixtures::water, cols);
    report<int>("water2", mapbox::fixtures::water2, cols);
    report<int>("water3", mapbox::fixtures::water3, cols);
    report<int>("water3b", mapbox::fixtures::water3b, cols);
    report<int>("water4", mapbox::fixtures::water4, cols);

    separator(cols);

    return 0;
}
