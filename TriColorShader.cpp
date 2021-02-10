#include "include/GShader.h"
#include "include/GMatrix.h"
#include "include/GPoint.h"
#include "myUtils.h"

class TriColorShader : public GShader {
private:
    GColor fColors[3];
    bool fOpaque;
    GMatrix fLocalM;
    GMatrix fInverseM;
    GColor deltaC;
public:
    TriColorShader(Triangle tri) {
        Vertex v0 = tri.v0; Vertex v1 = tri.v1; Vertex v2 = tri.v2;

        fColors[0] = v0.col; fColors[1] = v1.col; fColors[2] = v2.col;

        GPoint p0 = v0.pt; GPoint p1 = v1.pt; GPoint p2 = v2.pt;
        fLocalM = GMatrix(
            p1.fX-p0.fX, p2.fX-p0.fX, p0.fX,
            p1.fY-p0.fY, p2.fY-p0.fY, p0.fY
        );

        fOpaque = (v0.col.fA == 1 && v1.col.fA == 1 && v2.col.fA == 1);
    }

    bool isOpaque() override {
        return fOpaque;
    }

    bool setContext(const GMatrix& ctm) override {
        bool success = (ctm*fLocalM).invert(&fInverseM);
        if (success) {
            float a = fInverseM[GMatrix::SX]; float d = fInverseM[GMatrix::KY];
            deltaC = (-a-d)*fColors[0] + a*fColors[1] + d*fColors[2];
        }
        return success;
    }

    void shadeRow(int x, int y, int count, GPixel row[]) override {
        GPoint* pt = (GPoint*)malloc(sizeof(GPoint));
        *pt = {x + .5, y + .5};
        fInverseM.mapPoints(pt, 1);
        GColor C = colorClamp((1 - pt->fX - pt->fY)*fColors[0] + pt->fX*fColors[1] + pt->fY*fColors[2]);

        for (int i = 0; i < count; i++) {
            row[i] = GPixel_PackARGB(C.fA*255, C.fR*C.fA*255, C.fG*C.fA*255, C.fB*C.fA*255);
            C  = colorClamp(C + deltaC);
        }
    }
};

std::unique_ptr<GShader> GCreateTriColorShader(Triangle tri) {
    return std::unique_ptr<GShader>(new TriColorShader(tri));
}