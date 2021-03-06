#pragma once
#include <earcut.hpp>

#include <memory>
#include <vector>

template <typename Coord, typename Polygon>
class EarcutTesselator {
public:
    using Vertex = std::array<Coord, 2>;
    using Vertices = std::vector<Vertex>;

    EarcutTesselator(const Polygon &polygon_)
        : polygon(polygon_)
    {
        for (const auto& ring : polygon_) {
            for (const auto& vertex : ring) {
                vertices_.emplace_back(Vertex {{ Coord(std::get<0>(vertex)),
                                                 Coord(std::get<1>(vertex)) }});
            }
        }
    }

    void run() {
        earcut(polygon);
    }

    auto indices() const {
        return earcut.indices;
    }

    auto vertices() const {
        return vertices_;
    }

private:
    mapbox::Earcut<Coord> earcut;
    const Polygon &polygon;
    Vertices vertices_;
};
