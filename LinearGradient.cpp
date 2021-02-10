#include "include/GShader.h"
#include "include/GBitmap.h"
#include "include/GMatrix.h"
#include "include/GPoint.h"
#include "myUtils.h"

#include <iostream>
#include <math.h>

class LinearGradient : public GShader {
public:
    LinearGradient(GPoint p0, GPoint p1, const GColor colors[], int count, GShader::TileMode mode) {
        fColors = (GColor*)malloc(sizeof(GColor) * count);
        memcpy(fColors, colors, sizeof(GColor) * count);

        fCount = count;

        fMode = mode;

        if (p0.fX > p1.fX) { std::swap(p0, p1); }
        float dx = p1.x() - p0.x();
        float dy = p1.y() - p0.y();
        fLocalM = GMatrix(
            dx, -dy, p0.x(),
            dy,  dx, p0.y()
        );

        fOpaque = true;
        for (int i = 0; i < count; i++) {
            if (colors[i].fA < 1)
                { fOpaque = false; break; }
        }
    }
    ~LinearGradient() {
        free(fColors);
    }

    bool isOpaque() override {
        return fOpaque;
    }

    bool setContext(const GMatrix& ctm) override {
        return (ctm*fLocalM).invert(&fInverseM);
    }

    void shadeRow(int x, int y, int count, GPixel row[]) override {
        GPoint* pt = (GPoint*)malloc(sizeof(GPoint));
        *pt = {x + .5, y + .5};
        fInverseM.mapPoints(pt, 1);
        float _x = pt->fX;
        const float dx = fInverseM[GMatrix::SX];
        free(pt);

        for (int i = 0; i < count; i++) {
            float t = _x;
            if (fMode == GShader::TileMode::kClamp) {
                t = GPinToUnit(t);
            } else if (fMode == GShader::TileMode::kRepeat) {
                t -= floorf(t);
            } else if (fMode == GShader::TileMode::kMirror) {
                if ((int)floorf(t)%2 == 0) {
                    t -= floorf(t);
                } else {
                    t = 1. - (t-floorf(t));
                }
            }
            int colorI = floor((fCount-1) * t);
            float stride = 1. / (fCount-1);
            float mappedI = colorI*stride;

            if (fCount == 1) {
                row[i] = getPixel(fColors[0]);
                continue;
            }
            // assert(t <= fCount-1);

            GColor L_C = fColors[colorI];
            GColor R_C = fColors[colorI+1 >= fCount ? colorI : colorI+1];
                
            t = GPinToUnit((t-mappedI) / stride);

            row[i] = getPixel({
                L_C.fA*(1.-t) + R_C.fA*t,
                L_C.fR*(1.-t) + R_C.fR*t,
                L_C.fG*(1.-t) + R_C.fG*t,
                L_C.fB*(1.-t) + R_C.fB*t
            });

            _x += dx;
        }
    }

private:
    GColor* fColors;
    int fCount;
    GMatrix fLocalM;
    GMatrix fInverseM;
    bool fOpaque;
    GShader::TileMode fMode;
};

std::unique_ptr<GShader> GCreateLinearGradient(GPoint p0, GPoint p1, const GColor colors[], int count, GShader::TileMode mode) {
    if (count < 1) { return nullptr; }

    return std::unique_ptr<GShader>(new LinearGradient(p0, p1, colors, count, mode));
}