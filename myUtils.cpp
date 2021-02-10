#include "myUtils.h"

GPixel getPixel(const GColor& color) {
    int a = floor(           color.fA * 255 + 0.5);
    int r = floor(color.fR * color.fA * 255 + 0.5);
    int g = floor(color.fG * color.fA * 255 + 0.5);
    int b = floor(color.fB * color.fA * 255 + 0.5);

    
    if (a < 0) { a = r = g = b = 0; }
    if (r < 0) { r = 0; }
    if (g < 0) { g = 0; }
    if (b < 0) { b = 0; }

    return GPixel_PackARGB(a, r, g, b);
}

GRect getBoundingRect(const GPoint points[], int count) {
    GPoint pMin = GPoint::Make(INFINITY, INFINITY);
    GPoint pMax = GPoint::Make(0, 0);

    for (int i = 0; i < count; i++) {
        GPoint pt = points[i];
        pMin.set(
            pt.x() < pMin.x() ? pt.x() : pMin.x(),
            pt.y() < pMin.y() ? pt.y() : pMin.y()
        );
        pMax.set(
            pt.x() > pMax.x() ? pt.x() : pMax.x(),
            pt.y() > pMax.y() ? pt.y() : pMax.y()
        );
    }

    return GRect::MakeXYWH(pMin.x(), pMin.y(), pMax.x()-pMin.x(), pMax.y()-pMin.y());
}

void ptsToVerts(const GPoint points[], int count, Vertex verts[]) {
    for (int i = 0; i < count; i++) {
        verts[i] = Vertex(points[i]);
    }
}

float floatMax(float a, float b) {
    return a>b ? a : b;
}
float floatMin(float a, float b) {
    return a<b ? a : b;
}

GColor colorClamp(GColor c) {
    return GColor::MakeARGB(
        floatMin(1., floatMax(0., c.fA)),
        floatMin(1., floatMax(0., c.fR)),
        floatMin(1., floatMax(0., c.fG)),
        floatMin(1., floatMax(0., c.fB))
    );
}

GColor operator*(const float& c, const GColor& color) {
    return GColor::MakeARGB(
        c*color.fA,
        c*color.fR,
        c*color.fG,
        c*color.fB
    );
}

GColor operator+(const GColor& color1, const GColor& color2) {
    return GColor::MakeARGB(
        color1.fA + color2.fA,
        color1.fR + color2.fR,
        color1.fG + color2.fG,
        color1.fB + color2.fB
    );
}
