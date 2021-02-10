#pragma once

#include "GPixel.h"
#include "GColor.h"
#include "GPoint.h"
#include "GRect.h"
#include "GShader.h"

GPixel getPixel(const GColor& color);

GRect getBoundingRect(const GPoint points[], int count);

struct Edge {
    int yMin, yMax;
    float m, b;
    float currX;
    int wind;

    Edge(float _yMin, float _yMax, float _m, float _b, float _currX)
        : yMin(_yMin), yMax(_yMax), m(_m), b(_b), currX(_currX) { wind = 0; }
    Edge(float _yMin, float _yMax, float _m, float _b, float _currX, int _wind)
        : yMin(_yMin), yMax(_yMax), m(_m), b(_b), currX(_currX), wind(_wind) {}
};

struct Vertex {
    GPoint pt;
    GColor col;
    GPoint tex;
    bool hasColor = false;
    bool hasTexture = false;

    Vertex() {};
    Vertex(GPoint _pt)
        : pt(_pt) {}
    Vertex(GPoint _pt, GColor _col, GPoint _tex)
        : pt(_pt), col(_col), tex(_tex) {}
    void setColor(GColor _col) {
        col = _col;
        hasColor = true;
    }
    void setTexture(GPoint _tex) {
        tex = _tex;
        hasTexture = true;
    }
};

struct Triangle {
    Vertex v0, v1, v2;

    Triangle() {};
    Triangle(Vertex _v0, Vertex _v1, Vertex _v2)
        : v0(_v0), v1(_v1), v2(_v2) {}

    bool hasColors() { return v0.hasColor && v1.hasColor && v2.hasColor; }
    bool hasTextures() { return v0.hasTexture && v1.hasTexture && v2.hasTexture; }
};

void ptsToVerts(const GPoint points[], int count, Vertex verts[]);

GColor colorClamp(GColor c);

std::unique_ptr<GShader> GCreateTriColorShader(Triangle tri);
std::unique_ptr<GShader> GCreateProxyShader(Triangle tri, GShader* sh);
std::unique_ptr<GShader> GCreateComposeShader(GShader* s1, GShader* s2);
std::unique_ptr<GShader> GCreateRadialGradientShader(GPoint center, float radius,
                                                        const GColor colors[], int count,
                                                        GShader::TileMode mode);

GColor operator*(const float& c, const GColor& color);
GColor operator+(const GColor& color1, const GColor& color2);
