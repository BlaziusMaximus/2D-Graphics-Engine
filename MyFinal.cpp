#include "GFinal.h"
#include "GShader.h"
#include "myUtils.h"

class MyFinal : public GFinal {
public:
    MyFinal() {}

    std::unique_ptr<GShader> createRadialGradient(GPoint center, float radius,
                                                    const GColor colors[], int count,
                                                    GShader::TileMode mode) override {
        return GCreateRadialGradientShader(center, radius, colors, count, mode);
    }

    void addLine(GPath* path, GPoint p0, GPoint p1, float width, GFinal::CapType capType) override {
        if (p0.fX > p1.fX) { std::swap(p0, p1); }

        float dx = p1.fX - p0.fX;
        float dy = p1.fY - p0.fY;

        float half_w = width/2.;

        GPoint v = GPoint::Make(-dy,dx);
        v = v*(1./sqrtf(v.fX*v.fX + v.fY*v.fY));
        GPoint w = GPoint::Make(p1.fX-p0.fX, p1.fY-p0.fY);
        w = w*(1./sqrt(w.fX*w.fX + w.fY*w.fY));

        switch (capType) {
        case GFinal::CapType::kButt:
            path->moveTo(p0 + v*half_w);
            path->lineTo(p1 + v*half_w);
            path->lineTo(p1 - v*half_w);
            path->lineTo(p0 - v*half_w);
            break;
        case GFinal::CapType::kSquare:
            path->moveTo(p0 + v*  half_w  - w*half_w);
            path->lineTo(p1 + v*  half_w  + w*half_w);
            path->lineTo(p1 + v*(-half_w) + w*half_w);
            path->lineTo(p0 + v*(-half_w) - w*half_w);
            break;
        case GFinal::CapType::kRound:
            path->moveTo(p0 + v*half_w);
            path->lineTo(p1 + v*half_w);
            path->lineTo(p1 - v*half_w);
            path->lineTo(p0 - v*half_w);
            path->addCircle(p0, half_w);
            path->addCircle(p1, half_w);
            break;
        }
    }
};

std::unique_ptr<GFinal> GCreateFinal() {
    return std::unique_ptr<GFinal>(new MyFinal());
}
