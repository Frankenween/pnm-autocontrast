#include <string>
#include <vector>
#include "Colour.h"

#ifndef HW5_PNMREADER_H
#define HW5_PNMREADER_H


using namespace std;


class PNMImage {
private:
    vector<Colour> img;
    bool readOk;
    string tag;
    int width, height, size;
public:
    PNMImage(const string &filename);

    bool isOk() const;

    void inverse();

    void autoContrast(float delta);

    void autoContrastOmp(float delta, int threads);

    void autoContrastOmpNoFloat(float delta, int threads);

    int writeImage(const string &outputName);
};


#endif //HW5_PNMREADER_H
