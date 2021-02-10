/*
 *  Copyright 2020 Mike Reed
 */

#ifndef GFinal_DEFINED
#define GFinal_DEFINED

#include "GCanvas.h"
#include "GPath.h"
#include "GShader.h"

class GFinal {
public:
    virtual ~GFinal() {}

    /**
     *  Return a radial-gradient shader.
     *
     *  This is a shader defined by a circle with center and a radius.
     *  The array of colors are evenly distributed between the center (color[0]) out to
     *  the radius (color[count-1]). Beyond the radius, it respects the TileMode.
     */
    virtual std::unique_ptr<GShader> createRadialGradient(GPoint center, float radius,
                                                          const GColor colors[], int count,
                                                          GShader::TileMode mode) {
        return nullptr;
    }

    enum CapType {
        kButt,      // no cap on the line
        kSquare,    // square cap extending width/2
        kRound,     // round cap with radius = width/2
    };

    /**
     *  Add contour(s) to the specified path that will draw a line from p0 to p1 with the specified
     *  width and CapType. Note that "width" is the distance from one side of the stroke to the
     *  other, ala its thickness.
     */
    virtual void addLine(GPath* path, GPoint p0, GPoint p1, float width, CapType) {}
};

std::unique_ptr<GFinal> GCreateFinal();

#endif
