#include "include/GMatrix.h"


GMatrix::GMatrix() {
    fMat[0] = 1.0; fMat[1] = 0.0; fMat[2] = 0.0;
    fMat[3] = 0.0; fMat[4] = 1.0; fMat[5] = 0.0;
}

GMatrix GMatrix::Translate(float tx, float ty) {
    return GMatrix(
        1.0, 0.0, tx,
        0.0, 1.0, ty
    );
}

GMatrix GMatrix::Scale(float sx, float sy) {
    return GMatrix(
        sx, 0.0, 0.0,
        0.0, sy, 0.0
    );
}

GMatrix GMatrix::Rotate(float radians) {
    return GMatrix(
        cosf(radians), -sinf(radians), 0.0,
        sinf(radians), cosf(radians), 0.0
    );
}

GMatrix GMatrix::Concat(const GMatrix& a, const GMatrix& b) {
    return GMatrix(
        a.fMat[0]*b.fMat[0] + a.fMat[1]*b.fMat[3], a.fMat[0]*b.fMat[1] + a.fMat[1]*b.fMat[4], a.fMat[0]*b.fMat[2] + a.fMat[1]*b.fMat[5] + a.fMat[2],
        a.fMat[3]*b.fMat[0] + a.fMat[4]*b.fMat[3], a.fMat[3]*b.fMat[1] + a.fMat[4]*b.fMat[4], a.fMat[3]*b.fMat[2] + a.fMat[4]*b.fMat[5] + a.fMat[5]
    );
}

bool GMatrix::invert(GMatrix* inverse) const {
    float m11 = this->fMat[0]; float m12 = this->fMat[1]; float m13 = this->fMat[2];
    float m21 = this->fMat[3]; float m22 = this->fMat[4]; float m23 = this->fMat[5];

    // determinant is zero, matrix is non-invertible
    float det = m11*m22 - m12*m21;
    if (det == 0) { return false; }

    *inverse = GMatrix(
         m22/det, -m12/det, -(m13*m22 - m12*m23)/det,
        -m21/det,  m11/det,  (m13*m21 - m11*m23)/det
    );
    return true;
}

void GMatrix::mapPoints(GPoint dst[], const GPoint src[], int count) const {
    assert(this->fMat != nullptr);
    for (int pt = 0; pt < count; pt++) {
        GPoint a = src[pt]; a.fX *= 1.0f;
        float x = src[pt].x()*this->fMat[0] + src[pt].y()*this->fMat[1] + this->fMat[2];
        float y = src[pt].x()*this->fMat[3] + src[pt].y()*this->fMat[4] + this->fMat[5];
        dst[pt].set(x, y);
    }
}
