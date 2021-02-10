#include <stdlib.h>
#include <time.h>
#include <vector>
#include <stack>
#include <algorithm>

#include "GCanvas.h"
#include "GBitmap.h"
#include "GShader.h"
#include "GPath.h"
#include "myUtils.h"
#include "Clip.h" 
#include "ScanConverter.h"

class myCanvas : public GCanvas {
public:
    const int width;
    const int height;

    myCanvas(const GBitmap& device)
        : width(device.width()), height(device.height()), fDevice(device), CTM(GMatrix())
            {}
    
    void save() override {
        CTM_saves.push(CTM);
    }

    void restore() override {
        assert(CTM_saves.size() > 0);
        CTM = CTM_saves.top();
        CTM_saves.pop();
    }

    void concat(const GMatrix& matrix) override {
        CTM = CTM * matrix;
    }

    void drawPaint(const GPaint& paint) override {
        drawRect(GRect::MakeXYWH(0,0,fDevice.width(),fDevice.height()), paint);
    }

    void drawRect(const GRect& rect, const GPaint& paint) override {
        GShader* shader = paint.getShader();
        if (shader != nullptr && !shader->setContext(CTM)) { return; }
        // shader->isOpaque();

        GPoint pts[4] = {
            GPoint::Make(rect.fLeft, rect.fTop),
            GPoint::Make(rect.fRight, rect.fTop),
            GPoint::Make(rect.fRight, rect.fBottom),
            GPoint::Make(rect.fLeft, rect.fBottom)
        };
        CTM.mapPoints(pts, 4);
  
        // drawConvexPolygon(pts, 4, paint);
 
        GIRect new_rect = GRect::MakeLTRB(pts[0].x(), pts[0].y(), pts[1].x(), pts[2].y()).round();
        new_rect.fLeft =   std::max(new_rect.fLeft,   0);
        new_rect.fTop =    std::max(new_rect.fTop,    0);
        new_rect.fRight =  std::min(new_rect.fRight,  fDevice.width());
        new_rect.fBottom = std::min(new_rect.fBottom, fDevice.height());

        ScanConverter::rectScan(new_rect, fDevice, paint);
    } 
 
    void drawConvexPolygon(const GPoint pts[], int count, const GPaint& paint) override {
        if (count < 3) { return; }

        GShader* shader = paint.getShader();
        if (shader != nullptr && !shader->setContext(CTM)) { return; }

        GPoint new_pts[count];
        CTM.mapPoints(new_pts, pts, count);
        std::vector<Vertex> verts;
        for (int i = 0; i < count; i++) {
            verts.push_back(Vertex(new_pts[i]));
        }

        // clip and sort
        std::vector<Edge> edges = Clip::clip(verts, fDevice.width(), fDevice.height());
        if (edges.size() < 2) { return; }
        std::sort(edges.begin(), edges.end(), myCanvas::edgeComp);

        ScanConverter::convexScan(edges, count, fDevice, paint);
    }

    void drawPath(const GPath& path, const GPaint& paint) {
        if (path.countPoints() < 2) { return; }

        GShader* shader = paint.getShader();
        if (shader != nullptr && !shader->setContext(CTM)) { return; }
        
        GPath::Edger edger = GPath::Edger(path);
        std::vector<GPoint> pts;
        std::vector<Edge> edges;
        GPoint lastPoint;

        GPath::Verb verb;
        do {
            GPoint edgePts[4];
            verb = edger.next(edgePts);
            
            if (pts.size() > 0 && lastPoint != edgePts[0] && verb != GPath::Verb::kDone) {
                pts.push_back(pts[0]);
                Vertex vptr[pts.size()]; ptsToVerts(&pts[0], pts.size(), vptr);
                std::vector<Vertex> verts(vptr, vptr+pts.size());
                std::vector<Edge> interEdges = Clip::clip(verts, fDevice.width(), fDevice.height(), true);
                edges.insert(edges.end(), interEdges.begin(), interEdges.end());
                pts.clear();
            }
            lastPoint = edgePts[(int)verb];

            if (verb == GPath::Verb::kLine) {
                CTM.mapPoints(edgePts, 2);
                pts.push_back(edgePts[0]);
                pts.push_back(edgePts[1]);
            } else if (verb == GPath::Verb::kQuad) {
                CTM.mapPoints(edgePts, 3);
                GPoint A = edgePts[0], B = edgePts[1], C = edgePts[2];
                GPoint error = -1./4.*A + 2./4.*B - 1./4.*C;
                float mag_error = sqrt(error.fX*error.fX + error.fY*error.fY);
                float thresh = 0.25f;
                int numSegs = ceil(sqrt(mag_error/thresh));

                for (int i = 0; i < numSegs; i++) {
                    float t = (float)i/(float)numSegs;
                    pts.push_back((1.-t)*(1.-t)*A + 2.*t*(1.-t)*B + t*t*C);
                }
            } else if (verb == GPath::Verb::kCubic) {
                CTM.mapPoints(edgePts, 4);
                GPoint A = edgePts[0], B = edgePts[1], C = edgePts[2], D = edgePts[3];
                GPoint P = -1*A +2*B - C; GPoint Q = -1*B + 2*C - D;
                GPoint error = {std::max(abs(P.fX), abs(Q.fX)), std::max(abs(P.fY), abs(Q.fY))};
                float mag_error = sqrt(error.fX*error.fX + error.fY*error.fY);
                float thresh = 0.25f;
                int numSegs = ceil(sqrt(3./4.*mag_error/thresh));

                for (int i = 0; i < numSegs; i++) {
                    float t = (float)i/(float)numSegs;
                    pts.push_back((1.-t)*(1.-t)*(1.-t)*A + 3.*t*(1.-t)*(1.-t)*B + 3.*t*t*(1.-t)*C + t*t*t*D);
                }
            }
        } while (verb != GPath::Verb::kDone);
        pts.push_back(pts[0]);
        Vertex vptr[pts.size()]; ptsToVerts(&pts[0], pts.size(), vptr);
        std::vector<Vertex> verts(vptr, vptr+pts.size());

        std::vector<Edge> clipEdges = Clip::clip(verts, fDevice.width(), fDevice.height(), true);
        edges.insert(edges.end(), clipEdges.begin(), clipEdges.end());
        if (edges.size() < 2) { return; }
        std::sort(edges.begin(), edges.end(), myCanvas::edgeComp);

        ScanConverter::complexScan(edges, edges.size(), fDevice, paint);
    }

    void drawTriangle(Triangle tri, const GPaint& paint) {
        std::unique_ptr<GShader> sh;
        if (tri.hasColors() && tri.hasTextures() && paint.getShader() != nullptr) {
            auto colorSh = GCreateTriColorShader(tri);
            auto proxySh = GCreateProxyShader(tri, paint.getShader());
            sh = GCreateComposeShader(colorSh.release(), proxySh.release());
            sh->setContext(CTM);
        } else if (tri.hasColors()) {
            sh = GCreateTriColorShader(tri);
            sh->setContext(CTM);
        } else if (tri.hasTextures() && paint.getShader() != nullptr) {
            paint.getShader()->setContext(CTM);
            sh = GCreateProxyShader(tri, paint.getShader());
            sh->setContext(CTM);
        }

        GPoint tmp = tri.v0.pt;
        CTM.mapPoints(&tmp, 1);
        tri.v0.pt = tmp;
        tmp = tri.v1.pt;
        CTM.mapPoints(&tmp, 1);
        tri.v1.pt = tmp;
        tmp = tri.v2.pt;
        CTM.mapPoints(&tmp, 1);
        tri.v2.pt = tmp;

        std::vector<Edge> edges = Clip::clip({tri.v0,tri.v1,tri.v2}, fDevice.width(), fDevice.height());
        if (edges.size() < 2) { return; }
        std::sort(edges.begin(), edges.end(), myCanvas::edgeComp);

        ScanConverter::complexScan(edges, edges.size(), fDevice, GPaint(sh.get()));
    }

    void drawMesh(const GPoint verts[], const GColor colors[], const GPoint texs[], int count, const int indices[], const GPaint& paint) {
        int n = 0;
        for (int i = 0; i < count; i++) {
            Triangle tri = Triangle(
                Vertex(verts[indices[n+0]]),
                Vertex(verts[indices[n+1]]),
                Vertex(verts[indices[n+2]])
            );
            if (colors != nullptr) {
                tri.v0.setColor(colors[indices[n+0]]);
                tri.v1.setColor(colors[indices[n+1]]);
                tri.v2.setColor(colors[indices[n+2]]);
            }
            if (texs != nullptr && paint.getShader() != nullptr) {
                tri.v0.setTexture(texs[indices[n+0]]);
                tri.v1.setTexture(texs[indices[n+1]]);
                tri.v2.setTexture(texs[indices[n+2]]);
            }

            drawTriangle(tri, paint);

            n+=3;
        }
    }

    void drawQuad(const GPoint verts[4], const GColor colors[4], const GPoint texs[4], int level, const GPaint& paint) {
        for (int i = 0; i < level+1; i++) {
            for (int j = 0; j < level+1; j++) {
                GPoint A = verts[0], B = verts[1], C = verts[2], D = verts[3];
                float u = (float)j / (float)(level+1), v = (float)i / (float)(level+1);
                float u_1 = (float)(j+1) / (float)(level+1), v_1 = (float)(i+1) / (float)(level+1);
                Vertex v0 = Vertex((1-u)*(1-v)*A + u*(1-v)*B + u*v*C + v*(1-u)*D);
                Vertex v1 = Vertex((1-u_1)*(1-v)*A + u_1*(1-v)*B + u_1*v*C + v*(1-u_1)*D);
                Vertex v2 = Vertex((1-u_1)*(1-v_1)*A + u_1*(1-v_1)*B + u_1*v_1*C + v_1*(1-u_1)*D);
                Vertex v3 = Vertex((1-u)*(1-v_1)*A + u*(1-v_1)*B + u*v_1*C + v_1*(1-u)*D);
                if (colors != nullptr) {
                    GColor A = colors[0], B = colors[1], C = colors[2], D = colors[3];
                    v0.setColor((1-u)*(1-v)*A + u*(1-v)*B + u*v*C + v*(1-u)*D);
                    v1.setColor((1-u_1)*(1-v)*A + u_1*(1-v)*B + u_1*v*C + v*(1-u_1)*D);
                    v2.setColor((1-u_1)*(1-v_1)*A + u_1*(1-v_1)*B + u_1*v_1*C + v_1*(1-u_1)*D);
                    v3.setColor((1-u)*(1-v_1)*A + u*(1-v_1)*B + u*v_1*C + v_1*(1-u)*D);
                }
                if (texs != nullptr && paint.getShader() != nullptr) {
                    GPoint A = texs[0], B = texs[1], C = texs[2], D = texs[3];
                    v0.setTexture((1-u)*(1-v)*A + u*(1-v)*B + u*v*C + v*(1-u)*D);
                    v1.setTexture((1-u_1)*(1-v)*A + u_1*(1-v)*B + u_1*v*C + v*(1-u_1)*D);
                    v2.setTexture((1-u_1)*(1-v_1)*A + u_1*(1-v_1)*B + u_1*v_1*C + v_1*(1-u_1)*D);
                    v3.setTexture((1-u)*(1-v_1)*A + u*(1-v_1)*B + u*v_1*C + v_1*(1-u)*D);
                }
                Triangle tri1 = Triangle(v0, v1, v3);
                Triangle tri2 = Triangle(v1, v2, v3);

                drawTriangle(tri1, paint);
                drawTriangle(tri2, paint);
            }
        }
    }

private:
    const GBitmap fDevice;

    GMatrix CTM;
    std::stack<GMatrix> CTM_saves;

    static bool edgeComp(Edge e1, Edge e2) {
        if (e1.yMin < e2.yMin) { return true; }
        if (e1.yMin > e2.yMin) { return false; }
        if (e1.currX < e2.currX) { return true; }
        if (e1.currX > e2.currX) { return false; }
        return e1.m < e2.m;
    }
};

std::unique_ptr<GCanvas> GCreateCanvas(const GBitmap& device) {
    if (!device.pixels()) {
        return nullptr;
    }
    return std::unique_ptr<GCanvas>(new myCanvas(device));
}

std::string GDrawSomething(GCanvas* canvas, GISize dim) {
    canvas->clear({1,.07,.07,.1});
    
    //draw walls
    GPoint wall1_verts[4] = {
        {  0,   0 },
        { 20,   0 },
        { 20, 160 },
        {  0, 195 }
    };
    GColor wall1_colors[4] = {
        { .9, .58, .58, .74 },
        { .9, .61, .61, .79 },
        { .9, .24, .24, .38 },
        { .9, .29, .29, .44 }
    };
    canvas->drawQuad(wall1_verts, wall1_colors, nullptr, 3, GPaint());
    GBitmap brick;
    brick.readFromFile("brick.png");
    const float brick_w = brick.width();
    const float brick_h = brick.height();
    auto brick_shader = GCreateBitmapShader(brick, GMatrix(), GShader::kMirror);
    GPoint wall2_verts[4] = {
        {  20,   0 },
        { 200,   0 },
        { 200, 160 },
        {  20, 160 }
    };
    GColor wall2_colors[4] = {
        { .8, .54, .54, .69 },
        { .8, .56, .56, .72 },
        { .8, .21, .21, .41 },
        { .8, .24, .24, .39 }
    };
    GPoint wall2_texs[4] = {
        {       0,       0 },
        { brick_w,       0 },
        { brick_w, brick_h },
        {       0, brick_h }
    };
    canvas->drawQuad(wall2_verts, wall2_colors, wall2_texs, 4, GPaint(brick_shader.get()));
    GPoint wall3_verts[4] = {
        { 200,   0 },
        { 256,   0 },
        { 256, 210 },
        { 200, 160 },
    };
    GColor wall3_colors[4] = {
        { .8, .54, .54, .70 },
        { .8, .56, .57, .74 },
        { .8, .21, .21, .35 },
        { .8, .26, .26, .41 }
    };
    canvas->drawQuad(wall3_verts, wall3_colors, nullptr, 3, GPaint());

    // draw pa1
    canvas->translate(210, 15);
    GBitmap pa1;
    pa1.readFromFile("pa1.png");
    const float pa1_w = pa1.width();
    const float pa1_h = pa1.height();
    auto pa1_shader = GCreateBitmapShader(pa1, GMatrix(), GShader::kMirror);
    GPoint pa1_verts[4] = {
        {  0,  6 },
        { 40,  0 },
        { 40, 84 },
        {  0, 65 }
    };
    GPoint pa1_texs[4] = {
        {     0,     0 },
        { pa1_w,     0 },
        { pa1_w, pa1_h },
        {     0, pa1_h }
    };
    canvas->drawQuad(pa1_verts, nullptr, pa1_texs, 3, GPaint(pa1_shader.get()));
    canvas->translate(-210, -15);

    // draw pa2
    canvas->translate(210, 90);
    GBitmap pa2;
    pa2.readFromFile("pa2.png");
    const float pa2_w = pa2.width();
    const float pa2_h = pa2.height();
    auto pa2_shader = GCreateBitmapShader(pa2, GMatrix(), GShader::kMirror);
    GPoint pa2_verts[4] = {
        {  0,  0 },
        { 40, 20 },
        { 40, 90 },
        {  0, 59 }
    };
    GPoint pa2_texs[4] = {
        {     0,     0 },
        { pa2_w,     0 },
        { pa2_w, pa2_h },
        {     0, pa2_h }
    };
    canvas->drawQuad(pa2_verts, nullptr, pa2_texs, 3, GPaint(pa2_shader.get()));
    canvas->translate(-210, -90);

    // draw pa3
    canvas->translate(30, 15);
    GBitmap pa3;
    pa3.readFromFile("pa3.png");
    const float pa3_w = pa3.width();
    const float pa3_h = pa3.height();
    auto pa3_shader = GCreateBitmapShader(pa3, GMatrix(), GShader::kMirror);
    GPoint pa3_verts[4] = {
        {   0,   0 },
        { 155,   0 },
        { 155, 120 },
        {   0, 120 }
    };
    GPoint pa3_texs[4] = {
        {     0,     0 },
        { pa3_w,     0 },
        { pa3_w, pa3_h },
        {     0, pa3_h }
    };
    canvas->drawQuad(pa3_verts, nullptr, pa3_texs, 3, GPaint(pa3_shader.get()));
    canvas->translate(-30, -15);

    // draw pillar
    GPoint pillar_verts[4] = {
        {  90,   0 },
        { 150,   0 },
        { 150, 210 },
        {  90, 210 } 
    };
    GColor pillar_colors[4] = {
        { 1., .52, .52, .69 },
        { 1., .48, .48, .66 },
        { 1., .18, .18, .31 },
        { 1., .30, .30, .48 }
    };
    canvas->drawQuad(pillar_verts, pillar_colors, nullptr, 3, GPaint());
    GPoint pillar_verts2[4] = {
        {  82,   0 },
        {  90,   0 },
        {  90, 210 },
        {  82, 176 } 
    };
    GColor pillar_colors2[4] = {
        { 1., .63, .63, .81 },
        { 1., .59, .60, .79 },
        { 1., .31, .31, .47 },
        { 1., .36, .36, .55 }
    };
    canvas->drawQuad(pillar_verts2, pillar_colors2, nullptr, 3, GPaint());

    // draw pa4
    canvas->translate(90, 20);
    GBitmap pa4;
    pa4.readFromFile("pa4.png");
    const float pa4_w = pa4.width();
    const float pa4_h = pa4.height();
    auto pa4_shader = GCreateBitmapShader(pa4, GMatrix(), GShader::kMirror);
    GPoint pa4_verts[4] = {
        {  4,  0 },
        { 56,  0 },
        { 56, 52 },
        {  4, 52 }
    };
    GPoint pa4_texs[4] = {
        {     0,     0 },
        { pa4_w,     0 },
        { pa4_w, pa4_h },
        {     0, pa4_h }
    };
    canvas->drawQuad(pa4_verts, nullptr, pa4_texs, 3, GPaint(pa4_shader.get()));
    canvas->translate(-90, -20);

    // draw pa5
    canvas->translate(90, 80);
    GBitmap pa5;
    pa5.readFromFile("pa5.png");
    const float pa5_w = pa5.width();
    const float pa5_h = pa5.height();
    auto pa5_shader = GCreateBitmapShader(pa5, GMatrix(), GShader::kMirror);
    GPoint pa5_verts[4] = {
        {  4,  0 },
        { 56,  0 },
        { 56, 52 },
        {  4, 52 }
    };
    GPoint pa5_texs[4] = {
        {     0,     0 },
        { pa5_w,     0 },
        { pa5_w, pa5_h },
        {     0, pa5_h }
    };
    canvas->drawQuad(pa5_verts, nullptr, pa5_texs, 3, GPaint(pa5_shader.get()));
    canvas->translate(-90, -80);

    return "NULL";
}
             