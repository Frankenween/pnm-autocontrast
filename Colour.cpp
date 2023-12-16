#include "Colour.h"
#include <cmath>

Colour::Colour(unsigned char r, unsigned char g, unsigned char b) : r(r), g(g), b(b) {}

void Colour::read(istream &in, int mode) {
    if (mode == Colour::RGB) {
        r = in.get();
        g = in.get();
        b = in.get();
    } else if (mode == Colour::GRAY_TONES) {
        r = g = b = in.get();
    }
}

void Colour::inverse() {
    r = 255 - r;
    g = 255 - g;
    b = 255 - b;
}

void Colour::write(ostream &out, int mode) {
    if (mode == Colour::RGB) {
        out << r << g << b;
    } else if (mode == Colour::GRAY_TONES) {
        out << r;
    }
}

Colour::Colour() {
    r = g = b = 0;
}

unsigned char Colour::getMin() {
    return min(min(r, g), b);
}

unsigned char Colour::getMax() {
    return max(max(r, g), b);
}

void Colour::fitInterval(int minVal, float coef) {
    r = sat((int) floor((float)(r - minVal) * coef));
    g = sat((int) floor((float)(g - minVal) * coef));
    b = sat((int) floor((float)(b - minVal) * coef));
}

unsigned char Colour::sat(int val) {
    if (val < 0) {
        return 0;
    }
    if (val > 255) {
        return 255;
    }
    return val;
}

void Colour::fitInterval(vector<unsigned char> &newValues) {
    r = newValues[r];
    g = newValues[g];
    b = newValues[b];
}
