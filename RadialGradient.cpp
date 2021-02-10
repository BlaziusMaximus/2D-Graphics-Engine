#include "include/GShader.h"
#include "include/GBitmap.h"
#include "include/GMatrix.h"
#include "include/GPoint.h"
#include "include/GFinal.h"
#include "myUtils.h"

class RadialGradient : public GShader {
private:
    float fRadius;
    GColor* fColors;
    int fCount;
    GMatrix fLocalM;
    GMatrix fInverseM;
    bool fOpaque;
    GShader::TileMode fMode;
public:
    RadialGradient(GPoint center, float radius, const GColor colors[], int count, GShader::TileMode mode) {
        fRadius = radius;

        fColors = (GColor*)malloc(sizeof(GColor) * count);
        memcpy(fColors, colors, sizeof(GColor) * count);

        fCount = count;

        fMode = mode;

        fLocalM = GMatrix(
            1, 0, center.fX,
            0, 1, center.fY
        );

        fOpaque = true;
        for (int i = 0; i < count; i++) {
            if (colors[i].fA < 1)
                { fOpaque = false; break; }
        }
    }
    ~RadialGradient() {
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
        float _x = pt->fX, _y = pt->fY;
        float dx = fInverseM[GMatrix::SX];
        // const float euc = fInverseM[GMatrix::SX]*fInverseM[GMatrix::SX] + fInverseM[GMatrix::SY]*fInverseM[GMatrix::SY];
        free(pt);

        for (int i = 0; i < count; i++) {
            float euc = sqrtf((_x)*(_x) + (_y)*(_y));
            float t = euc / fRadius;
  
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
};

std::unique_ptr<GShader> GCreateRadialGradientShader(GPoint center, float radius,
                                                        const GColor colors[], int count,
                                                        GShader::TileMode mode) {
    return std::unique_ptr<GShader>(new RadialGradient(center, radius, colors, count, mode));
}
