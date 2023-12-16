#include <iostream>
#include <string>
#include <chrono>
#include <functional>
#include "PNMImage.h"


using namespace std;


void run(const function<void()>& f, int threads) {
    auto start = chrono::steady_clock::now();
    f();
    auto end = chrono::steady_clock::now();
    auto res = chrono::duration_cast<chrono::milliseconds>(end - start).count();
    printf("Time (%i thread(s)): %i ms\n", threads, res);
}


int main(int argc, char *argv[]) {
    if (argc != 1 + 4) {
        printf("Bad number of arguments. Expected 4, got %d\n", argc - 1);
        return 1;
    }
    int threadsCount;
    try {
        threadsCount = stoi(argv[1]);
    } catch (invalid_argument &exc) {
        printf("Bad number of threads. Expected integer, got %s\n", argv[1]);
        return 1;
    } catch (out_of_range &exc) {
        printf("Number of threads cannot be represented as int: %s\n", argv[1]);
        return 1;
    }
    string inputFile = string(argv[2]);
    string outputFile = string(argv[3]);
    float delta = atof(argv[4]);

    PNMImage img(inputFile);
    if (!img.isOk()) {
        printf("An error happened while reading img %s\n", inputFile.c_str());
        return 1;
    }


    run([&]() {
        img.autoContrastOmpNoFloat(delta, threadsCount);
    }, threadsCount);

    if (img.writeImage(outputFile)) {
        return 1;
    }
    return 0;
}
