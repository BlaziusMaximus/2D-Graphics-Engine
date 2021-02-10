#include <vector>

#include "myUtils.h"

class Clip {
public:
    static std::vector<Edge> clip(const std::vector<Vertex> verts, int width, int height, bool complex = false) {
        std::vector<Edge> edges;

        for (int i = 0, j = 1; i < verts.size() && verts.size() > 1; i++) {
            j = (i+1 == verts.size() ? 0 : i+1);

            GPoint p0 = verts[i].pt; GPoint p1 = verts[j].pt;
            int wind = 1;

            // vertical pass
            if (p0.y() < 0 && p1.y() < 0) { continue; }
            if (p0.y() >= height && p1.y() >= height) { continue; }

            // ensure p0.y < p1.y
            if (p0.y() > p1.y()) {
                GPoint tmp = p0;
                p0 = p1;
                p1 = tmp;
                wind *= -1;
            }

            // slope
            if (p0.y() == p1.y()) { continue; }
            float dxdy = p0.x() == p1.x() ? 0 : (p1.x()-p0.x())/(p1.y()-p0.y());
            float dydx = (p1.y()-p0.y())/(p1.x()-p0.x());

            // vertical pass contd. (at least one of the pts must be in now)
            if (p0.y() < 0) {
                // std::cout << "p0: (" << p0.x() << ", " << p0.y() << ")\n";
                // std::cout << "p1: (" << p1.x() << ", " << p1.y() << ")\n";
                // std::cout << "dxdy: " << dxdy << "\n";
                p0.set(p0.x()-dxdy*p0.y(), 0);
            }
            if (p1.y() >= height) {
                p1.set(p1.x()-dxdy*(p1.y()-height), height);
            }

            // horizontal pass
            if (p0.x() < 0 && p1.x() < 0) {
                p0.set(0, p0.y());
                p1.set(0, p1.y());
                dxdy = 0;
            } else if (p0.x() < 0) {
                int oldY = (int)floorf(p0.y()+0.5);
                p0.set(0, p0.y()-dydx*p0.x());
                int newY = (int)floorf(p0.y()+0.5);
                if (oldY < newY) {
                    edges.push_back(Edge(
                        oldY,
                        newY-1,
                        0,
                        0,
                        (int)floorf(p0.x() + 0.5),
                        wind
                    ));
                }
            } else if (p1.x() < 0) {
                int oldY = (int)floorf(p1.y()+0.5);
                p1.set(0, p1.y()-dydx*p1.x());
                int newY = (int)floorf(p1.y()+0.5);
                if (newY < oldY) {
                    edges.push_back(Edge(
                        newY,
                        oldY-1,
                        0,
                        0,
                        (int)floorf(p1.x() + 0.5),
                        wind
                    ));
                }
            }
            if (p0.x() >= width && p1.x() >= width) {
                p0.set(width, p0.y());
                p1.set(width, p1.y());
                dxdy = 0;
            } else if (p0.x() >= width) {
                int oldY = (int)floorf(p0.y()+0.5);
                p0.set(width, p0.y()-dydx*(p0.x()-width));
                int newY = (int)floorf(p0.y()+0.5);
                if (oldY < newY) {
                    edges.push_back(Edge(
                        oldY,
                        newY-1,
                        0,
                        width,
                        (int)floorf(p0.x() + 0.5),
                        wind
                    ));
                }
            } else if (p1.x() >= width) {
                int oldY = (int)floorf(p1.y()+0.5);
                p1.set(width, p1.y()-dydx*(p1.x()-width));
                int newY = (int)floorf(p1.y()+0.5);
                if (newY < oldY) {
                    edges.push_back(Edge(
                        newY,
                        oldY-1,
                        0,
                        width,
                        (int)floorf(p1.x() + 0.5),
                        wind
                    ));
                }
            }
            
            float h = p0.y();
            int p0y = (int)floorf(p0.y()+0.5);
            h = p0y - h + 0.5;
            int p1y = (int)floorf(p1.y()+0.5);
            if (p0y < p1y) {
                edges.push_back(Edge(
                    p0y,
                    p1y-1,
                    abs(10000.0*dxdy) < 1.0 ? 0 : dxdy,
                    p0.x()-dxdy*p0.y(),
                    (int)floorf(p0.x() + dxdy*h + 0.5),
                    wind
                ));
            }
        }

        return edges;
    }
};