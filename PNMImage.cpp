#include <fstream>
#include <iostream>
#include <cmath>
#include "PNMImage.h"

PNMImage::PNMImage(const string &filename) {
    ifstream in(filename, istream::binary);
    if (!in.is_open()) {
        printf("Failed to open img %s\n", filename.c_str());
        readOk = false;
        return;
    }
    in >> tag;
    if (tag != "P5" && tag != "P6") {
        printf("Unsupported image format. Expected P5 or P6, but got %s\n", tag.c_str());
        readOk = false;
        in.close();
        return;
    }
    int maxChanel;
    in >> width >> height >> maxChanel;
    if (maxChanel != 255) {
        printf("Unsupported pixel range. Expected 255, but got %d\n", maxChanel);
        readOk = false;
        in.close();
        return;
    }
    size = width * height;
    in.get(); // \n
    int mode = Colour::getMode(tag);
    img.resize(size);
    for (int i = 0; i < size; i++) {
        img[i].read(in, mode);
    }
    readOk = in.good();
    in.close();
}

bool PNMImage::isOk() const {
    return readOk;
}

void PNMImage::inverse() {
    for (int i = 0; i < height * width; i++) {
        img[i].inverse();
    }
}

int PNMImage::writeImage(const string &outputName) {
    ofstream out(outputName, ofstream::binary);

    if (!out.is_open()) {
        printf("Failed to open file %s for writing\n", outputName.c_str());
        return 1;
    }

    out << tag << "\n";
    out << width << " " << height << "\n" << "255\n";
    int mode = Colour::getMode(tag);
    for (int i = 0; i < size; i++) {
        img[i].write(out, mode);
    }
    int code = 0;
    if (!out.good()) {
        printf("Bad things happened while writing in file %s\n", outputName.c_str());
        code = 1;
    }
    out.close();
    return code;
}

void PNMImage::autoContrast(float delta) {
    int minColourCount[255], maxColourCount[255];
    int skip = floor((float)size * delta);
    for (int i = 0; i < 255; i++) {
        minColourCount[i] = 0;
        maxColourCount[i] = 0;
    }

    for (int i = 0; i < size; i++) {
        minColourCount[img[i].getMin()]++;
        maxColourCount[img[i].getMax()]++;
    }

    int sum = 0;
    unsigned char firstLook = 0, lastLook = 255;
    for (int i = 0; i < 256 && sum < skip; i++) {
        sum += minColourCount[i];
        firstLook++;
    }
    while (sum == skip && firstLook < 255 && minColourCount[firstLook] == 0) {
        firstLook++;
    }
    sum = 0;
    for (int i = 255; i >= 0 && sum < skip; i--) {
        sum += maxColourCount[i];
        lastLook--;
    }
    while (sum == skip && lastLook > 0 && maxColourCount[lastLook] == 0) {
        lastLook--;
    }
    if (firstLook >= lastLook) {
        return;
    }
    float segLen = (float) (lastLook - firstLook);
    float coef = 255.0f / segLen;

    for (int i = 0; i < size; i++) {
        img[i].fitInterval(firstLook, coef);
    }
}


void PNMImage::autoContrastOmp(float delta, int threads) {
    int gMaxColourCount[256];
    int gMinColourCount[256];
    int skip = floor((float)size * delta);
    float segLen, coef;
    bool needChange = true;

    unsigned char firstLook = 0, lastLook = 255;
    fill(gMaxColourCount, gMaxColourCount + 256, 0);
    fill(gMinColourCount, gMinColourCount + 256, 0);

#pragma omp parallel shared(gMaxColourCount, gMinColourCount, skip, firstLook, lastLook, segLen, coef, needChange) default(none) num_threads(threads)
    {
        int maxColourCount[256], minColourCount[256];
        fill(maxColourCount, maxColourCount + 256, 0);
        fill(minColourCount, minColourCount + 256, 0);
#pragma omp for schedule(static, 64)
        for (int i = 0; i < size; i++) {
            minColourCount[img[i].getMin()]++;
            maxColourCount[img[i].getMax()]++;
        }
        for (int i = 0; i < 256; i++) {
#pragma omp critical (gSum)
            {
                gMaxColourCount[i] += maxColourCount[i];
                gMinColourCount[i] += minColourCount[i];
            }
        }
#pragma omp barrier
#pragma omp master
        {
            int sum = 0;
            for (int i = 0; i < 256 && sum < skip; i++) {
                sum += gMinColourCount[i];
                firstLook++;
            }
            while (sum == skip && firstLook < 255 && gMinColourCount[firstLook] == 0) {
                firstLook++;
            }
            sum = 0;
            for (int i = 255; i >= 0 && sum < skip; i--) {
                sum += gMaxColourCount[i];
                lastLook--;
            }
            while (sum == skip && lastLook > 0 && gMaxColourCount[lastLook] == 0) {
                lastLook--;
            }
            if (firstLook >= lastLook) {
                needChange = false;
            } else {
                segLen = (float) (lastLook - firstLook);
                coef = 255.0f / segLen;
            }
        }
#pragma omp barrier
        if (needChange) {
#pragma omp for schedule(static, 64)
            for (int i = 0; i < size; i++) {
                img[i].fitInterval(firstLook, coef);
            }
        }
    }
}

void PNMImage::autoContrastOmpNoFloat(float delta, int threads) {
    int gMaxColourCount[256];
    int gMinColourCount[256];
    vector<unsigned char> newVals(256);
    int skip = floor((float)size * delta);
    bool needChange = true;

    unsigned char firstLook = 0, lastLook = 255;
    fill(gMaxColourCount, gMaxColourCount + 256, 0);
    fill(gMinColourCount, gMinColourCount + 256, 0);

#pragma omp parallel shared(gMaxColourCount, gMinColourCount, skip, firstLook, lastLook, needChange, newVals) default(none) num_threads(threads)
    {
        int maxColourCount[256], minColourCount[256];
        fill(maxColourCount, maxColourCount + 256, 0);
        fill(minColourCount, minColourCount + 256, 0);
#pragma omp for schedule(static, 64)
        for (int i = 0; i < size; i++) {
            minColourCount[img[i].getMin()]++;
            maxColourCount[img[i].getMax()]++;
        }
        for (int i = 0; i < 256; i++) {
#pragma omp critical (gSum)
            {
                gMaxColourCount[i] += maxColourCount[i];
                gMinColourCount[i] += minColourCount[i];
            }
        }
#pragma omp barrier
#pragma omp master
        {
            int sum = 0;
            for (int i = 0; i < 256 && sum < skip; i++) {
                sum += gMinColourCount[i];
                firstLook++;
            }
            while (sum == skip && firstLook < 255 && gMinColourCount[firstLook] == 0) {
                firstLook++;
            }
            sum = 0;
            for (int i = 255; i >= 0 && sum < skip; i--) {
                sum += gMaxColourCount[i];
                lastLook--;
            }
            while (sum == skip && lastLook > 0 && gMaxColourCount[lastLook] == 0) {
                lastLook--;
            }
            float segLen, coef;
            if (firstLook >= lastLook) {
                needChange = false;
            } else {
                segLen = (float) (lastLook - firstLook);
                coef = 255.0f / segLen;
                for (int i = 0; i < 256; i++) {
                    newVals[i] = Colour::sat((int) floor((float)(i - firstLook) * coef));
                }
            }
        }
#pragma omp barrier
        if (needChange) {
#pragma omp for schedule(static, 64)
            for (int i = 0; i < size; i++) {
                img[i].fitInterval(newVals);
            }
        }
    }
}
