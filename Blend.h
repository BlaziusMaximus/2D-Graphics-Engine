#include "GPaint.h"
#include "GPixel.h"
#include <iostream>

class Blend {
private:
    const static unsigned int opt1 = 32896;
public:
    static GPixel blend(GPixel& src, GPixel& dst, GBlendMode mode) {
        pixel pSrc(
            GPixel_GetA(src),
            GPixel_GetR(src),
            GPixel_GetG(src),
            GPixel_GetB(src)
        );

        pixel pDst(
            GPixel_GetA(dst),
            GPixel_GetR(dst),
            GPixel_GetG(dst),
            GPixel_GetB(dst)
        );

        pixel pRes(0,0,0,0);
        unsigned int opt0, opt2;

        switch (mode) {
            case(GBlendMode::kClear):
                break;
            case(GBlendMode::kSrc):
                return src;
            case(GBlendMode::kDst):
                return dst;
            case(GBlendMode::kSrcOver):
                if (pSrc.a == 255) {
                    pRes.ARGB(255, pSrc.r, pSrc.g, pSrc.b);
                } else {
                    opt0 = (255-pSrc.a)*257;
                    pRes.ARGB(
                        pSrc.a + ((opt0*pDst.a+opt1)>>16),
                        pSrc.r + ((opt0*pDst.r+opt1)>>16),
                        pSrc.g + ((opt0*pDst.g+opt1)>>16),
                        pSrc.b + ((opt0*pDst.b+opt1)>>16)
                    );
                }
                break;
            case(GBlendMode::kDstOver):
                if (pDst.a == 255) {
                    pRes.ARGB(255, pDst.r, pDst.g, pDst.b);
                } else {
                    opt0 = (255-pDst.a)*257;
                    pRes.ARGB(
                        pDst.a + ((opt0*pSrc.a+opt1)>>16),
                        pDst.r + ((opt0*pSrc.r+opt1)>>16),
                        pDst.g + ((opt0*pSrc.g+opt1)>>16),
                        pDst.b + ((opt0*pSrc.b+opt1)>>16)
                    );
                }
                break;
            case(GBlendMode::kSrcIn):
                if (pDst.a != 0) {
                    opt0 = pDst.a*257;
                    pRes.ARGB(
                        (opt0*pSrc.a+opt1)>>16,
                        (opt0*pSrc.r+opt1)>>16,
                        (opt0*pSrc.g+opt1)>>16,
                        (opt0*pSrc.b+opt1)>>16
                    );
                }
                break;
            case(GBlendMode::kDstIn):
                if (pSrc.a != 0) {
                    opt0 = pSrc.a*257;
                    pRes.ARGB(
                        (opt0*pDst.a+opt1)>>16,
                        (opt0*pDst.r+opt1)>>16,
                        (opt0*pDst.g+opt1)>>16,
                        (opt0*pDst.b+opt1)>>16
                    );
                }
                break;
            case(GBlendMode::kSrcOut):
                if (pDst.a != 255) {
                    opt0 = (255-pDst.a)*257;
                    pRes.ARGB(
                        (opt0*pSrc.a+opt1)>>16,
                        (opt0*pSrc.r+opt1)>>16,
                        (opt0*pSrc.g+opt1)>>16,
                        (opt0*pSrc.b+opt1)>>16
                    );
                }
                break;
            case(GBlendMode::kDstOut):
                if (pSrc.a != 255) {
                    opt0 = (255-pSrc.a)*257;
                    pRes.ARGB(
                        (opt0*pDst.a+opt1)>>16,
                        (opt0*pDst.r+opt1)>>16,
                        (opt0*pDst.g+opt1)>>16,
                        (opt0*pDst.b+opt1)>>16
                    );
                }
                break;
            case(GBlendMode::kSrcATop):
                if (pSrc.a == 255 && pDst.a == 0) {
                } else if (pSrc.a == 255) {
                    opt0 = pDst.a*257;
                    pRes.ARGB(
                        (opt0*pSrc.a+opt1)>>16,
                        (opt0*pSrc.r+opt1)>>16,
                        (opt0*pSrc.g+opt1)>>16,
                        (opt0*pSrc.b+opt1)>>16
                    );
                } else if (pDst.a == 0) {
                    opt2 = (255-pSrc.a)*257;
                    pRes.ARGB(
                        (opt2*pDst.a+opt1)>>16,
                        (opt2*pDst.r+opt1)>>16,
                        (opt2*pDst.g+opt1)>>16,
                        (opt2*pDst.b+opt1)>>16
                    );
                } else {
                    opt0 = pDst.a*257;
                    opt2 = (255-pSrc.a)*257;
                    pRes.ARGB(
                        ((opt0*pSrc.a+opt1)>>16) + ((opt2*pDst.a+opt1)>>16),
                        ((opt0*pSrc.r+opt1)>>16) + ((opt2*pDst.r+opt1)>>16),
                        ((opt0*pSrc.g+opt1)>>16) + ((opt2*pDst.g+opt1)>>16),
                        ((opt0*pSrc.b+opt1)>>16) + ((opt2*pDst.b+opt1)>>16)
                    );
                }
                break;
            case(GBlendMode::kDstATop):
                if (pSrc.a == 0 && pDst.a == 255) {
                } else if (pSrc.a == 0) {
                    opt2 = (255-pDst.a)*257;
                    pRes.ARGB(
                        (opt2*pSrc.a+opt1)>>16,
                        (opt2*pSrc.r+opt1)>>16,
                        (opt2*pSrc.g+opt1)>>16,
                        (opt2*pSrc.b+opt1)>>16
                    );
                } else if (pDst.a == 255) {
                    opt0 = pSrc.a*257;
                    pRes.ARGB(
                        (opt0*pDst.a+opt1)>>16,
                        (opt0*pDst.r+opt1)>>16,
                        (opt0*pDst.g+opt1)>>16,
                        (opt0*pDst.b+opt1)>>16
                    );
                } else {
                    opt0 = pSrc.a*257;
                    opt2 = (255-pDst.a)*257;
                    pRes.ARGB(
                        ((opt0*pDst.a+opt1)>>16) + ((opt2*pSrc.a+opt1)>>16),
                        ((opt0*pDst.r+opt1)>>16) + ((opt2*pSrc.r+opt1)>>16),
                        ((opt0*pDst.g+opt1)>>16) + ((opt2*pSrc.g+opt1)>>16),
                        ((opt0*pDst.b+opt1)>>16) + ((opt2*pSrc.b+opt1)>>16)
                    );
                }
                break;
            case(GBlendMode::kXor):
                if (pSrc.a == 255 && pDst.a == 255) {
                } else if (pSrc.a == 255) {
                    opt2 = (255-pDst.a)*257;
                    pRes.ARGB(
                        (opt2*pSrc.a+opt1)>>16,
                        (opt2*pSrc.r+opt1)>>16,
                        (opt2*pSrc.g+opt1)>>16,
                        (opt2*pSrc.b+opt1)>>16
                    );
                } else if (pDst.a == 255) {
                    opt0 = (255-pSrc.a)*257;
                    pRes.ARGB(
                        (opt0*pDst.a+opt1)>>16,
                        (opt0*pDst.r+opt1)>>16,
                        (opt0*pDst.g+opt1)>>16,
                        (opt0*pDst.b+opt1)>>16
                    );
                } else {
                    opt0 = 255-pSrc.a;
                    opt2 = 255-pDst.a;
                    pRes.ARGB(
                        ((opt0*pDst.a + opt2*pSrc.a)*257 + opt1)>>16,
                        ((opt0*pDst.r + opt2*pSrc.r)*257 + opt1)>>16,
                        ((opt0*pDst.g + opt2*pSrc.g)*257 + opt1)>>16,
                        ((opt0*pDst.b + opt2*pSrc.b)*257 + opt1)>>16
                    );
                }
                break;
        }
        
        if (pRes.a < pRes.b || pRes.a < pRes.r || pRes.a < pRes.g) {
            // printf("mode: %d\n", mode);
        }
        // pRes.print();

        return GPixel_PackARGB(pRes.a, pRes.r, pRes.g, pRes.b);
    }
private:
    struct pixel {
        unsigned int a, r, g, b;

        pixel(int _a, int _r, int _g, int _b) : a(_a),r(_r),g(_g),b(_b) {}

        void ARGB(int _a, int _r, int _g, int _b) {
            a = _a; r = _r; g = _g; b = _b;
        }

        void print() {
            printf("ARGB:(%d, %d, %d, %d)\n", a, r, g, b);
        }

        bool operator!=(const pixel p) const {
            return !(a==p.a && r==p.r && g==p.g && b==p.b);
        }
    };
};
