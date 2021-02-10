#include "include/GShader.h"
#include "include/GMatrix.h"
#include "include/GPoint.h"
#include "myUtils.h"

class ProxyShader : public GShader {
private:
    GShader* fShader;
    GMatrix fLocalM;
    GMatrix fInvShaderM;
    GMatrix fTextureM;
public:
    ProxyShader(Triangle tri, GShader* sh) {
        fShader = sh;

        Vertex v0 = tri.v0; Vertex v1 = tri.v1; Vertex v2 = tri.v2;

        GPoint p0 = v0.pt; GPoint p1 = v1.pt; GPoint p2 = v2.pt;
        fLocalM = GMatrix(
            p1.fX-p0.fX, p2.fX-p0.fX, p0.fX,
            p1.fY-p0.fY, p2.fY-p0.fY, p0.fY
        );

        GPoint t0 = v0.tex; GPoint t1 = v1.tex; GPoint t2 = v2.tex;
        GMatrix(
            t1.fX-t0.fX, t2.fX-t0.fX, t0.fX,
            t1.fY-t0.fY, t2.fY-t0.fY, t0.fY
        ).invert(&fInvShaderM);
    }

    bool isOpaque() override {
        return fShader->isOpaque();
    }

    bool setContext(const GMatrix& ctm) override {
        return fShader->setContext(ctm*fLocalM*fInvShaderM);
        
    }

    void shadeRow(int x, int y, int count, GPixel row[]) override {
        fShader->shadeRow(x, y, count, row);
    }
};

std::unique_ptr<GShader> GCreateProxyShader(Triangle tri, GShader* sh) {
    return std::unique_ptr<GShader>(new ProxyShader(tri, sh));
}