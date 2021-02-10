#include "include/GShader.h"
#include "include/GBitmap.h"
#include "include/GMatrix.h"

class BitmapShader : public GShader {
private:
    GBitmap fDevice;
    GMatrix fMatrix;
    GMatrix fInverse;
    GShader::TileMode fMode;
    float fInvWidth, fInvHeight;
public:
    BitmapShader(const GBitmap& device, const GMatrix& localMatrix, GShader::TileMode mode) {
        fDevice.alloc(device.width(), device.height(), device.rowBytes());
        for (int y = 0; y < device.height(); y++) {
            for (int x = 0; x < device.width(); x++) {
                GPixel pix = *device.getAddr(x,y);
                *fDevice.getAddr(x,y) = pix;
                // printf("%d vs %d\n", *fDevice.getAddr(x,y), addr);
            }
        }
        fMatrix = localMatrix;

        fMode = mode;

        fInvWidth = 1./(float)device.width();
        fInvHeight = 1./(float)device.height();
    }

    bool isOpaque() {
        return fDevice.isOpaque();
    }

    bool setContext(const GMatrix& ctm) {
        return (ctm*fMatrix).invert(&fInverse);
    }

    void shadeRow(int x, int y, int count, GPixel row[]) override {
        if (fDevice.width() <= 0 || fDevice.height() <= 0) { return; }

        GPoint* pt = (GPoint*)malloc(sizeof(GPoint));

        for (int i = 0; i < count; i++) {
            *pt = {x + i + 0.5f, y + 0.5f};
            fInverse.mapPoints(pt, 1);

            int srcX, srcY;
            if (fMode == GShader::TileMode::kClamp) {
                srcX = GRoundToInt(pt->x());
                srcY = GRoundToInt(pt->y());
            } else if (fMode == GShader::TileMode::kRepeat) {
                float fSrcX = pt->x() * fInvWidth;
                fSrcX -= floorf(fSrcX);
                srcX = fSrcX * fDevice.width();

                float fSrcY = pt->y() * fInvHeight;
                fSrcY -= floorf(fSrcY);
                srcY = fSrcY * fDevice.height();
            } else if (fMode == GShader::TileMode::kMirror) {
                float fSrcX = pt->x() * fInvWidth;
                if ((int)floorf(fSrcX)%2 == 0) { fSrcX -= floorf(fSrcX); }
                else { fSrcX = 1. - (fSrcX-floorf(fSrcX)); }
                srcX = fSrcX * fDevice.width();

                float fSrcY = pt->y() * fInvHeight;
                if ((int)floorf(fSrcY)%2 == 0) { fSrcY -= floorf(fSrcY); }
                else { fSrcY = 1. - (fSrcY-floorf(fSrcY)); }
                srcY = fSrcY * fDevice.height();

                // srcX = GRoundToInt(abs(pt->x())) % fDevice.width();
                // if (GRoundToInt(abs(pt->x())) / fDevice.width() == 1) { srcX = fDevice.width() - srcX; }
                // if (pt->x() < 0) { srcX = fDevice.width() - srcX; }

                // srcY = GRoundToInt(abs(pt->y())) % fDevice.height();
                // if (GRoundToInt(abs(pt->y())) / fDevice.height() == 1) { srcY = fDevice.height() - srcY; }
                // if (pt->y() < 0) { srcY = fDevice.height() - srcY; }
            }

            srcX = std::max(0, std::min(fDevice.width() - 1, srcX));
            srcY = std::max(0, std::min(fDevice.height() - 1, srcY));

            row[i] = *(fDevice.getAddr(srcX, srcY));
        }

        free(pt);
    }
};

std::unique_ptr<GShader> GCreateBitmapShader(const GBitmap& device, const GMatrix& localMatrix, GShader::TileMode mode) {
    if (!device.pixels()) { return nullptr; }

    GMatrix new_localMatrix = GMatrix::Concat(localMatrix, GMatrix());

    return std::unique_ptr<GShader>(new BitmapShader(device, new_localMatrix, mode));
}
