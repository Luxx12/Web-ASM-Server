#include <stdlib.h>

#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <fstream>

#include <cstdio>
#include <cstdlib>

#include <Eigen/Dense>

#define SIZE 160

#define EMPTY 0
#define EDGE 1
#define VERTEX 2

#define PI 3.14159265358979323846

const double MAGIC_MAX = (1.4 / 2.0);

template <typename T>
inline int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

double toRad(double deg) {
    return (deg * (PI / 180.0));
}

Eigen::Matrix<double, 4, 4> rodriguesRotMat(double theta) {
    Eigen::Matrix<double, 4, 4> R;

    double A = 1 + (2*std::cos(theta));
    double B = 1 - (std::cos(theta)) + (sqrt(3) * std::sin(theta));
    double C = 1 - (std::cos(theta)) - (sqrt(3) * std::sin(theta));

    // Implementation for Rodrigues' rotation formula

    R <<  A, C, B, 0,
          B, A, C, 0,
          C, B, A, 0,
          0, 0, 0, 3;

    R *= 1.0/3.0;

    return R;

}

Eigen::Matrix<double, 4, 4> perspectiveProject(double d) {
    Eigen::Matrix<double, 4, 4> P;

    P << 1, 0, 0, 0,
         0, 1, 0, 0,
         0, 0, 1, 0,
         0, 0, 1.0/d, 0;

    return P;
}

Eigen::Matrix<double, 4, 4> rotate2D(double phi) {
    Eigen::Matrix<double, 4, 4> Q;

    Q << std::cos(phi), -std::sin(phi), 0, 0,
        -std::sin(phi),  std::cos(phi), 0, 0,
        0,             0,     1, 0,
        0,             0,     0, 1;

    return Q;
}

Eigen::Matrix<int, 2, 8> viewportTransform(double d, const Eigen::Matrix<double, 2, 8>& original) {
    Eigen::Matrix<double, 2, 8> V = original;

    V /= (MAGIC_MAX * d);

    for (int i = 0; i < V.cols(); i++) {
        V.col(i).x() = (SIZE / 2.0) * (V.col(i).x() + 1);
        V.col(i).y() = (SIZE / 2.0) * (V.col(i).y() + 1);
    }

    Eigen::Matrix<int, 2, 8> V_int = V.array().round().cast<int>().matrix();
    V_int = V_int.cwiseMax(0).cwiseMin(SIZE - 1);

    return V_int;
}

void LineAlgorithm(Eigen::Matrix<int, SIZE, SIZE>& tsM, int x1, int y1, int x2, int y2) {

    if (x1 == x2 && y1 == y2) {
        tsM(y1, x1) = EDGE;
        return;
    }

    int ichange = 0;

    int dX = abs(x2 - x1);
    int dY = abs(y2 - y1);

    int x = x1; int y = y1;

    int s1 = sgn(x2 - x1);
    int s2 = sgn(y2 - y1);

    if (dY > dX) {
        int T = dX;
        dX = dY;
        dY = T;
        ichange = 1;
    }

    int e = (2 * dY) - dX;
    int i = 1;

    while (i <= dX) {
        if (tsM(y, x) != VERTEX) tsM(y, x) = EDGE;

        while(e >= 0) {
            if(ichange) {
                x = x + s1;
            } else {
                y = y + s2;
            }
            e = e - (2 * dX);
        }

        if (ichange) {
            y = y + s2;
        } else {
            x = x + s1;
        }

        e = e + (2 * dY);

        i++;
    }

    if (tsM(y, x) != VERTEX) tsM(y, x) = EDGE;
}

Eigen::Matrix<int, SIZE, SIZE> triStateMatrix(Eigen::Matrix<int, 2, 8>& verticeCoords, std::vector<int>& V1, std::vector<int>& V2) {
    Eigen::Matrix<int, SIZE, SIZE> tsMatrix;
    tsMatrix.setZero();

    int tsX, tsY;

    for (int i = 0; i < verticeCoords.cols(); i++ ) {
        tsX = verticeCoords.col(i).x();
        tsY = verticeCoords.col(i).y();

        tsMatrix(tsY, tsX) = VERTEX;
    }

    // call Bresenham line algorithm to fill edges

    for (int i = 0; i < 12; i++) {
        LineAlgorithm(tsMatrix, verticeCoords.col(V1[i]).x(), verticeCoords.col(V1[i]).y(), verticeCoords.col(V2[i]).x(), verticeCoords.col(V2[i]).y());
    }

    return tsMatrix;
}

std::string stringFrame(const Eigen::Matrix<int, SIZE, SIZE>& triStateMatrix) {

    std::string frame;

    for (int row = 0; row < SIZE; ++row) {
        for (int col = 0; col < SIZE; ++col) {
            char ch;
            switch (triStateMatrix(row, col)) {
                case EMPTY: ch = ' '; break;
                case EDGE: ch = '#'; break;
                case VERTEX: ch = '@'; break;
            }
            frame += ch;
            frame += ch;
        }
        frame += '\n';
    }

    return frame;
}

extern "C" int getFrame(double theta, double phi, double d) { // to be called from the asm file

    std::vector<int> V1 = {0, 0, 0, 1, 1, 2, 2, 3, 4, 4, 5, 6};
    std::vector<int> V2 = {1, 2, 4, 3, 5, 3, 6, 7, 5, 6, 7, 7};

    Eigen::Matrix<double, 4, 8> cubeVertices;

    cubeVertices.col(0) << -1, -1, -1, 1;
    cubeVertices.col(1) << 1, -1, -1, 1;
    cubeVertices.col(2) << -1, 1, -1, 1;
    cubeVertices.col(3) << 1, 1, -1, 1;
    cubeVertices.col(4) << -1, -1, 1, 1;
    cubeVertices.col(5) << 1, -1, 1, 1;
    cubeVertices.col(6) << -1, 1, 1, 1;
    cubeVertices.col(7) << 1, 1, 1, 1;

    Eigen::Matrix<double, 4, 8> transformed = cubeVertices;
    transformed = rodriguesRotMat(toRad(theta)) * cubeVertices;

    for (int i = 0; i < transformed.cols(); i++) {
        transformed.col(i).z() = transformed.col(i).z() + 3;
    }

    transformed = perspectiveProject(d) * transformed;

    for (int i = 0; i < transformed.cols(); i++) {
        transformed.col(i) *= (d / transformed.col(i).z());
    }

    transformed = rotate2D(toRad(phi)) * transformed;

    Eigen::Matrix<double, 2, 8> projVertices = transformed.block(0, 0, 2, 8);
    Eigen::Matrix<int, 2, 8> viewTransformed = viewportTransform(d, projVertices);

    Eigen::Matrix<int, SIZE, SIZE> tsMatrix = triStateMatrix(viewTransformed, V1, V2);

    std::string cubeString = stringFrame(tsMatrix);
    std::ofstream outputFile("output.txt");

    if (outputFile.is_open()) {

        outputFile << cubeString;
        outputFile.close();

        std::cout << "File exported successfully." << std::endl;
        return 0;
    } else {
        std::cerr << "Error: Could not open or create the file." << std::endl;
        return 1;
    }

    return 1;
}

/*
int main() {

    double theta = 0.0;
    double phi = 0.0;
    double d = 2.0;

    std::vector<int> V1 = {0, 0, 0, 1, 1, 2, 2, 3, 4, 4, 5, 6};
    std::vector<int> V2 = {1, 2, 4, 3, 5, 3, 6, 7, 5, 6, 7, 7};

    Eigen::Matrix<double, 4, 8> cubeVertices;

    cubeVertices.col(0) << -1, -1, -1, 1;
    cubeVertices.col(1) << 1, -1, -1, 1;
    cubeVertices.col(2) << -1, 1, -1, 1;
    cubeVertices.col(3) << 1, 1, -1, 1;
    cubeVertices.col(4) << -1, -1, 1, 1;
    cubeVertices.col(5) << 1, -1, 1, 1;
    cubeVertices.col(6) << -1, 1, 1, 1;
    cubeVertices.col(7) << 1, 1, 1, 1;

    std::cout << "\x1b[2J\x1b[H";

    while(1) {
        Eigen::Matrix<double, 4, 8> transformed = cubeVertices;
        transformed = rodriguesRotMat(toRad(theta)) * cubeVertices;

        for (int i = 0; i < transformed.cols(); i++) {
            transformed.col(i).z() = transformed.col(i).z() + 3;
        }

        transformed = perspectiveProject(d) * transformed;

        for (int i = 0; i < transformed.cols(); i++) {
            transformed.col(i) *= (d / transformed.col(i).z());
        }

        transformed = rotate2D(toRad(phi)) * transformed;

        Eigen::Matrix<double, 2, 8> projVertices = transformed.block(0, 0, 2, 8);
        Eigen::Matrix<int, 2, 8> viewTransformed = viewportTransform(d, projVertices);

        Eigen::Matrix<int, SIZE, SIZE> tsMatrix = triStateMatrix(viewTransformed, V1, V2);

        std::cout << "\x1b[H" << stringFrame(tsMatrix) << std::flush;
        theta += 0.09;
    }
}
*/
