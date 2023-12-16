#include <iostream>
#include <vector>

#ifndef HW5_COLOUR_H
#define HW5_COLOUR_H


using namespace std;


class Colour {
private:
    unsigned char r, g, b;

public:
    static const int GRAY_TONES = 0;
    static const int RGB = 1;

    Colour(unsigned char r, unsigned char g, unsigned char b);

    Colour();

    unsigned char getMin();

    unsigned char getMax();

    static int getMode(const string &tag) {
        if (tag == "P5") {
            return Colour::GRAY_TONES;
        } else if (tag == "P6") {
            return RGB;
        } else {
            return -1;
        }
    }

    void read(istream &in, int mode);

    void write(ostream &out, int mode);

    void inverse();

    void fitInterval(int minVal, float range);

    void fitInterval(vector<unsigned char> &newValues);

    static unsigned char sat(int val);
};


#endif //HW5_COLOUR_H
