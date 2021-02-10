#include "ScanConverter.h"

#include "myUtils.h"

void blit(const GPaint& paint, const GBitmap& device, int L_X, int R_X, int scanline);

void ScanConverter::rectScan(GIRect rect, const GBitmap& device, const GPaint& paint) {
    GShader* shader = paint.getShader();
    GBlendMode blendMode = paint.getBlendMode();

    if (shader == nullptr) {
        if (paint.getColor().fA == 0 &&
            blendMode != GBlendMode::kSrc &&
            blendMode != GBlendMode::kClear &&
            blendMode != GBlendMode::kSrcIn &&
            blendMode != GBlendMode::kDstIn &&
            blendMode != GBlendMode::kSrcOut &&
            blendMode != GBlendMode::kDstATop) { return; }
        for (int y = rect.fTop; y < rect.fBottom; y++) {
            GPixel color_pixel = getPixel(paint.getColor());
            for (int x = rect.fLeft; x < rect.fRight; x++) {
                GPixel* addr = device.getAddr(x, y);
                *addr = Blend::blend(color_pixel, *addr, blendMode);
            }
        }
    } else {
        for (int y = rect.fTop; y < rect.fBottom; y++) {
            int rectWidth = rect.fRight-rect.fLeft;
            GPixel row[rectWidth];
            shader->shadeRow(rect.fLeft, y, rectWidth, row);
            for (int x = rect.fLeft; x < rect.fRight; x++) {
                GPixel* addr = device.getAddr(x, y);
                *addr = Blend::blend(row[x-rect.fLeft], *addr, paint.getBlendMode());
            }
        }
    }
}

void ScanConverter::convexScan(std::vector<Edge> edges, int count, const GBitmap& device, const GPaint& paint) {
    Edge active1 = edges[0];
    Edge active2 = edges[1];
    float scanline = active1.yMin + 0.5;
    int active1Max = active1.yMax;
    int active2Max = active2.yMax;
    float m1_times_y = active1.m*scanline;
    float m2_times_y = active2.m*scanline;
    int edgeI = 2;
    for (; scanline < device.height(); scanline+=1.0) {
        if (scanline > active1Max) {
            if (edgeI >= edges.size()) { break; }
            active1 = edges[edgeI++];
            active1Max = active1.yMax;
            m1_times_y = active1.m*scanline;
        }
        if (scanline > active2Max) {
            if (edgeI >= edges.size()) { break; }
            active2 = edges[edgeI++];
            active2Max = active2.yMax;
            m2_times_y = active2.m*scanline;
        }

        int L_X = GRoundToInt(m1_times_y + active1.b);
        int R_X = GRoundToInt(m2_times_y + active2.b);
        m1_times_y += active1.m; m2_times_y += active2.m;
        if (L_X > R_X) {
            int tmp = L_X;
            L_X = R_X;
            R_X = tmp;
        }
        
        blit(paint, device, L_X, R_X, scanline);
    }
}

void ScanConverter::complexScan(std::vector<Edge> edges, int count, const GBitmap& device, const GPaint& paint) {
    int scanline = edges[0].yMin;
    while (count > 0) {
        int index = 0;
        int wind = 0;
        int L_X = 0; int R_X = 0;

        while (index < count && edges[index].yMin <= scanline) {
            int og_w = wind;

            L_X = (wind == 0 ? GRoundToInt(edges[index].currX) : L_X);
            wind += edges[index].wind;
            R_X = (wind == 0 && og_w != 0 ? GRoundToInt(edges[index].currX) : R_X);

            if (L_X < R_X) { blit(paint, device, L_X, R_X, scanline); }

            if (scanline >= edges[index].yMax) { edges.erase(edges.begin()+index); count--; }
            else { edges[index].currX += edges[index].m; index++; }
        }

        scanline++;
        while (index < count && scanline == edges[index].yMin) {
            index += 1;
        }

        std::sort(edges.begin(), edges.begin()+index, [](Edge a, Edge b){
            return (a.currX < b.currX);
        });
    }
}

void blit(const GPaint& paint, const GBitmap& device, int L_X, int R_X, int scanline) {
    if (L_X < 0) { L_X = 0; }
    if (R_X > device.width()) { R_X = device.width(); }

    GShader* shader = paint.getShader();
    if (shader == nullptr) {
        GPixel color_pixel = getPixel(paint.getColor());

        for (int x = L_X; x < R_X; x++) {
            GPixel* addr = device.getAddr(x, scanline);
            *addr = Blend::blend(color_pixel, *addr, paint.getBlendMode());      
        }
    } else {
        int xCount = R_X-L_X;
        GPixel row[xCount];
        shader->shadeRow(L_X, scanline, xCount, row);

        for (int x = L_X; x < R_X; x++) {
            GPixel* addr = device.getAddr(x, scanline);
            *addr = Blend::blend(row[x-L_X], *addr, paint.getBlendMode());
        }
    }
}
