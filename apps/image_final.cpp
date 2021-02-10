/**
 *  Copyright 2018 Mike Reed
 */

#include "image.h"

#include "GFinal.h"
#include "GCanvas.h"
#include "GBitmap.h"
#include "GColor.h"
#include "GMatrix.h"
#include "GPath.h"
#include "GPoint.h"
#include "GRandom.h"
#include "GRect.h"
#include <string>

static GPoint center(const GRect& r) {
    return { (r.fLeft + r.fRight)*0.5f, (r.fTop + r.fBottom)*0.5f };
}

static void draw_rect_radial(GFinal* fin, GCanvas* canvas, const GRect& r, float radius,
                             const GColor colors[], int count, GShader::TileMode mode) {
    auto sh = fin->createRadialGradient(center(r), radius, colors, count, mode);
    canvas->drawRect(r, GPaint(sh.get()));
}

static void final_radial(GCanvas* canvas) {
    auto fin = GCreateFinal();

    const GColor c0[] = { {1,1,0,0}, {1, 0, 1, 0}, {1,0,0,1} };
    const GColor c1[] = { {1,0,0,0}, {1, 1, 1, 1} };

    struct {
        GPoint              offset;
        float               radius;
        float               scale;
        GShader::TileMode   mode;
        const GColor*       colors;
        int                 count;
    } recs[] = {
        { {-256, -256},  90, 4, GShader::kClamp,  c1, GARRAY_COUNT(c1) },
        { {   0,    0}, 120, 1, GShader::kClamp,  c0, GARRAY_COUNT(c0) },
        { { 256,    0},  60, 1, GShader::kRepeat, c0, GARRAY_COUNT(c0) },
        { {   0,  256},  60, 1, GShader::kMirror, c0, GARRAY_COUNT(c0) },
    };
    for (auto& r : recs) {
        canvas->save();
        canvas->translate(r.offset.fX, r.offset.fY);
        canvas->scale(r.scale, r.scale);
        draw_rect_radial(fin.get(), canvas, GRect::MakeLTRB(0, 0, 256, 256), r.radius,
                         r.colors, r.count, r.mode);
        canvas->restore();
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////

static void draw_stroke(GFinal* fin, GCanvas* canvas, GPoint p0, GPoint p1, float width,
                        GFinal::CapType cap, const GPaint& paint) {
    GPath path;
    fin->addLine(&path, p0, p1, width, cap);
    canvas->drawPath(path, paint);
}

static void pinwheel(GCanvas* canvas, GFinal::CapType cap) {
    auto fin = GCreateFinal();

    const GColor colors[] = {
        {1, 1, 0, 0},
        {1, 1, 1, 0},
        {1, 0, 1, 0},
        {1, 0, 1, 1},
        {1, 0, 0, 1},
        {1, 1, 0, 1},
        {1, 0, 0, 0},
    };
    constexpr int N = GARRAY_COUNT(colors);

    const GPoint pts[] = { {-100, 0}, {100, 0} };

    float width = 60;
    for (int i = 0; i < N; ++i) {
        float angle = float(i * 2 * M_PI / N);
        canvas->save();
        canvas->rotate(angle);
        draw_stroke(fin.get(), canvas, pts[0], pts[1], width, cap, GPaint(colors[i]));
        canvas->restore();
        width -= 60.0f / (N+1);
    }
}

static void final_stroke(GCanvas* canvas) {
    canvas->clear({1,1,1,1});

    constexpr float delta = 140;

    const struct {
        GPoint offset;
        GFinal::CapType cap;
    } recs[] = {
        { {delta, delta}, GFinal::CapType::kButt },
        { {512-delta, 512/2}, GFinal::CapType::kSquare },
        { {delta, 512-delta}, GFinal::CapType::kRound },
    };
    for (auto& r : recs) {
        canvas->save();
        canvas->translate(r.offset.fX, r.offset.fY);
        pinwheel(canvas, r.cap);
        canvas->restore();
    }
}
