#include "GPath.h"

#include "GMatrix.h"
#include "GMath.h"

GPath& GPath::addRect(const GRect& rect, Direction dir) {
    this->moveTo(rect.fLeft, rect.fTop);

    switch (dir) {
    case kCW_Direction:
        this->lineTo(rect.fRight, rect.fTop)
             .lineTo(rect.fRight, rect.fBottom)
             .lineTo(rect.fLeft, rect.fBottom);
        // .lineTo(rect.fLeft, rect.fTop);
        break;
    case kCCW_Direction:
        this->lineTo(rect.fLeft, rect.fBottom)
             .lineTo(rect.fRight, rect.fBottom)
             .lineTo(rect.fRight, rect.fTop);
        // .lineTo(rect.fLeft, rect.fTop);
        break;
    }
    return *this;
}

GPath& GPath::addPolygon(const GPoint pts[], int count) {
    if (count < 3) { return *this; }

    this->moveTo(pts[0]);

    for (int i = 1; i < count; i++) {
        this->lineTo(pts[i]);
    }

    return *this;
}

GPath& GPath::addCircle(GPoint center, float radius, GPath::Direction direction) {
                                 /***/GPoint pts[16];/***/
                          /******************|******************/
                    /***************/pts[4]={0,   1};/***************/
                /****************************|****************************/
             pts[5]={-tan(M_PI/8),  1};/*****|*****/pts[3]={tan(M_PI/8),   1};
          /**********************************|**********************************/
         pts[6]={-sqrt(2)/2,  sqrt(2)/2};/***|***/pts[2]={sqrt(2)/2,   sqrt(2)/2};
        /************************************|************************************/
      pts[7]={-1,tan(M_PI/8)};/**************|*************/pts[1]={1,  tan(M_PI/8)};
     /***************************************|***************************************/
    pts[8]={-1,0};/*-------------------------+--------------------------*/pts[0]={1,0};
     /***************************************|***************************************/
      pts[9]={-1,-tan(M_PI/8)};/*************|*************/pts[15]={1,-tan(M_PI/8)};
        /************************************|************************************/
         pts[10]={-sqrt(2)/2,-sqrt(2)/2};/***|***/pts[14]={sqrt(2)/2, -sqrt(2)/2};
          /**********************************|**********************************/
             pts[11]={-tan(M_PI/8),-1};/*****|*****/pts[13]={tan(M_PI/8), -1};
                /****************************|****************************/
                    /***************/pts[12]={0, -1};/***************/
                          /******************|******************/
                                 /***********|***********/
    
    GMatrix matrix = GMatrix::Concat(GMatrix::Translate(center.fX,center.fY), GMatrix::Scale(radius,radius));
    matrix.mapPoints(pts, 16);

    this->moveTo(pts[0]);
    for (int i = 0; i < 8; i++) {
        if (direction == GPath::kCCW_Direction) {
            this->quadTo(pts[i*2+1], i!=7 ? pts[i*2+2] : pts[0]);
        } else if (direction == GPath::kCW_Direction) {
            this->quadTo(pts[15-i*2], pts[14-i*2]);
        }
    }

    return *this;
}

GRect GPath::bounds() const {
    int count = this->fPts.size();
    if (count == 0) { return GRect::MakeXYWH(0,0,0,0); }

    GPoint min = {INFINITY,INFINITY}, max = {0,0};
    for (int i = 0; i < count; i++) {
        min.set(
            fPts[i].fX < min.fX ? fPts[i].fX : min.fX,
            fPts[i].fY < min.fY ? fPts[i].fY : min.fY
        );
        max.set(
            fPts[i].fX > max.fX ? fPts[i].fX : max.fX,
            fPts[i].fY > max.fY ? fPts[i].fY : max.fY
        );
    }

    return GRect::MakeLTRB(min.fX, min.fY, max.fX, max.fY);
}

void GPath::transform(const GMatrix& matrix) {
    matrix.mapPoints(this->fPts.data(), this->countPoints());
}

void GPath::ChopQuadAt(const GPoint src[3], GPoint dst[5], float t) {
    dst[0] = src[0]; dst[4] = src[2];

    dst[1] = (1-t)*src[0] + t*src[1];
    dst[3] = (1-t)*src[1] + t*src[2];

    dst[2] = (1-t)*dst[1] + t*dst[3];
}

void GPath::ChopCubicAt(const GPoint src[4], GPoint dst[7], float t) {
    dst[0] = src[0]; dst[6] = src[3];

    dst[1] = (1-t)*src[0] + t*src[1];
    dst[5] = (1-t)*src[2] + t*src[3];

    GPoint tmp = (1-t)*src[1] + t*src[2];
    dst[2] = (1-t)*dst[1] + t*tmp;
    dst[4] = (1-t)*tmp + t*dst[5];

    dst[3] = (1-t)*dst[2] + t*dst[4];
}

