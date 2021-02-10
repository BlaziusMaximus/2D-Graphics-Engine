#include "include/GShader.h"
#include "include/GMatrix.h"
#include "include/GPoint.h"
#include "myUtils.h"

class ComposeShader : public GShader {
private:
    GShader* shader1;
    GShader* shader2;
public:
    ComposeShader(GShader* s1, GShader* s2) {
        shader1 = s1;
        shader2 = s2;
    }

    bool isOpaque() override {
        return shader1->isOpaque() && shader2->isOpaque();
    }

    bool setContext(const GMatrix& ctm) override {
        return shader1->setContext(ctm) && shader2->setContext(ctm);
    }

    void shadeRow(int x, int y, int count, GPixel row[]) override {
        GPixel tmp[count];
        shader1->shadeRow(x, y, count, tmp);
        shader2->shadeRow(x, y, count, row);

        for (int i = 0; i < count; i++) {
            row[i] = GPixel_PackARGB(
                (GPixel_GetA(row[i])*GPixel_GetA(tmp[i])*257)>>16,
                (GPixel_GetR(row[i])*GPixel_GetR(tmp[i])*257)>>16,
                (GPixel_GetG(row[i])*GPixel_GetG(tmp[i])*257)>>16,
                (GPixel_GetB(row[i])*GPixel_GetB(tmp[i])*257)>>16
            );
        }
    }
};

std::unique_ptr<GShader> GCreateComposeShader(GShader* s1, GShader* s2) {
    return std::unique_ptr<GShader>(new ComposeShader(s1, s2));
}
