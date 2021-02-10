#pragma once

#include <vector>
#include "Blend.h"
#include "include/GBitmap.h"
#include "include/GShader.h"

class GIRect;
struct Edge;

class ScanConverter {
public:
    static void rectScan(GIRect rect, const GBitmap& device, const GPaint& paint);
    static void convexScan(std::vector<Edge> edges, int count, const GBitmap& device, const GPaint& paint);
    static void complexScan(std::vector<Edge> edges, int count, const GBitmap& device, const GPaint& paint);
};