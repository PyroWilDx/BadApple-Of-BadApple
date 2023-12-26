//
// Created by pyrow on 18/12/2023.
//

#include <iostream>
#include <cassert>
#include <random>
#include "Utils.hpp"
#include "BadApple.hpp"

std::random_device rd;
std::mt19937 gen(rd());

cv::Size Utils::targetSize = cv::Size(0, 0);
const char *Utils::orglBAPath = "res/BadApple.mp4";
char Utils::displayC;
double Utils::changeProb = 1. / BA_FPS;

// Every Video : 30 FPS
const std::vector<VideoInfo> Utils::butBAPaths = {
        {"res/ActuallyPlayable.mp4",            -96},
        {"res/AiGenerated.mp4",                 0},
        {"res/AiGeneratedCharacters.mp4",       1},
        {"res/Apple.mp4",                       0},
        {"res/BlackMIDI.mp4",                   0},
        {"res/C++.mp4",                         0},
        {"res/C1-65A.mp4",                      38},
        {"res/Chess.mp4",                       4},
        {"res/ConwaysLifeGame.mp4",             -553},
        {"res/Cs-Go.mp4",                       0},
        {"res/Desmos.mp4",                      0},
        {"res/Discord.mp4",                     -114},
        {"res/Flick.mp4",                       8},
        {"res/FourierSeries.mp4",               -772},
        {"res/FourierTransform.mp4",            -362},
        {"res/Gameboy.mp4",                     -20},
        {"res/Laser.mp4",                       -36},
        {"res/LaserShow.mp4",                   -284},
        {"res/LaserTeslaCoil.mp4",              10},
        {"res/MarioCoins.mp4",                  -256},
        {"res/MiddleEurope.mp4",                20},
//        {"res/MinecraftApple.mp4", -2},
        {"res/MinecraftChiseledBookshelf.mp4",  16},
        {"res/MinecraftCopper.mp4",             162},
        {"res/MinecraftLava.mp4",               0},
        {"res/MinecraftPinkPetal.mp4",          2},
        {"res/MinecraftSheep.mp4",              0},
        {"res/MinecraftTrapdoor.mp4",           -479},
        {"res/MineSweeper.mp4",                 0},
        {"res/MK-90.mp4",                       -262},
        {"res/MolecularDynamicsSimulation.mp4", -30},
        {"res/OnlyMotion.mp4",                  0},
        {"res/Oscilloscope.mp4",                2},
        {"res/Nes8Bit.mp4",                     -926},
//        {"res/PauseCantSee.mp4", 0},
        {"res/PeoplePlayground.mp4",            46},
        {"res/PhosphorScreen.mp4",              0},
        {"res/PrimeNumber.mp4",                 0},
        {"res/QrCode.mp4",                      0},
        {"res/RPlace.mp4",                      26},
        {"res/RocketLeague.mp4",                -262},
        {"res/SingleLine.mp4",                  0},
        {"res/StopMotion.mp4",                  0},
        {"res/StraightLines.mp4",               0},
        {"res/TerminalVLC.mp4",                 0},
        {"res/Terraria.mp4",                    -1602},
        {"res/VolumeSlice.mp4",                 0},
        {"res/Water.mp4",                       2},
        {"res/WindowsScreensaver.mp4",          -22},
        {"res/WindowsTaskManager.mp4",          0},
        {"res/WindowsVirus.mp4",                0},
        {"res/YoutubeHomepage.mp4",             791},
};

void Utils::myAssert(bool cond, const char *errMsg) {
    if (!cond) {
        std::cout << errMsg << std::endl;
        assert(cond);
    }
}

double Utils::getRandomFloat() {
    std::uniform_real_distribution<double> randReal(0., 1.);
    return randReal(gen);
}

int Utils::getRandomInt(int maxExcluded) {
    std::uniform_int_distribution<> randInt(0, maxExcluded - 1);
    return randInt(gen);
}

void Utils::fillMatrixRandom(int **array, int n, int m, int maxExcluded) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            array[i][j] = getRandomInt(maxExcluded);
        }
    }
}

bool Utils::isRectangleSameValue(bool **matrix, int x, int y, int w, int h) {
    bool value = matrix[y][x];
    for (int i = x; i < x + w; i++) {
        for (int j = y; j < y + h; j++) {
            if (matrix[j][i] != value) return false;
        }
    }
    return true;
}

void Utils::buildQuadTreeFromMatrixRec(QuadTree *quadTree, bool **matrix,
                                       int x, int y, int w, int h) {
    quadTree->rect.x = x;
    quadTree->rect.y = y;
    quadTree->rect.w = w;
    quadTree->rect.h = h;

    int wHalf = w / 2;
    int hHalf = h / 2;

    if (isRectangleSameValue(matrix, x, y, w, h)
        || wHalf < QT_PRECISION_W || hHalf < QT_PRECISION_H) {
        quadTree->value = matrix[y][x];
        quadTree->topLeft = nullptr;
        quadTree->topRight = nullptr;
        quadTree->botLeft = nullptr;
        quadTree->botRight = nullptr;
        return;
    }

    quadTree->value = false;
    quadTree->topLeft = new QuadTree;
    quadTree->topRight = new QuadTree;
    quadTree->botLeft = new QuadTree;
    quadTree->botRight = new QuadTree;
    int wCompensation = (w % 2 == 0) ? 0 : 1;
    int hCompensation = (h % 2 == 0) ? 0 : 1;
    buildQuadTreeFromMatrixRec(quadTree->topLeft, matrix,
                               x, y,
                               wHalf, hHalf);
    buildQuadTreeFromMatrixRec(quadTree->topRight, matrix,
                               x + wHalf, y,
                               wHalf + wCompensation, hHalf);
    buildQuadTreeFromMatrixRec(quadTree->botLeft, matrix,
                               x, y + hHalf,
                               wHalf, hHalf + hCompensation);
    buildQuadTreeFromMatrixRec(quadTree->botRight, matrix,
                               x + wHalf, y + hHalf,
                               wHalf + wCompensation, hHalf + hCompensation);
}

QuadTree *Utils::getQuadTreeFromMatrix(bool **matrix, int w, int h) {
    auto *quadTree = new QuadTree;
    buildQuadTreeFromMatrixRec(quadTree, matrix, 0, 0, w, h);
    return quadTree;
}

void Utils::destroyQuadTree(QuadTree *quadTree) {
    if (quadTree == nullptr) return;

    destroyQuadTree(quadTree->topLeft);
    destroyQuadTree(quadTree->topRight);
    destroyQuadTree(quadTree->botLeft);
    destroyQuadTree(quadTree->botRight);

    delete quadTree;
}

void Utils::addImgToImg(cv::Mat &src, cv::Mat &addImg, int x, int y, int w, int h) {
    cv::Mat cpyAddImg;
    cv::resize(addImg, cpyAddImg, cv::Size(w, h));
//    cv::cvtColor(cpyAddImg, cpyAddImg, cv::COLOR_BGR2BGRA); // Alpha
//    if (w <= 4 || h <= 4) {
//        int totalAlpha = 0;
//        int nAlpha = 0;
//        for (int j = 0; j < cpyAddImg.rows; j++) {
//            for (int i = 0; i < cpyAddImg.cols; i++) {
//                totalAlpha += BadApple::imgGrayOrglBA.at<uchar>(y + j, x + i);
//                nAlpha++;
//            }
//        }
//        double meanAlpha = (double) totalAlpha / (double) nAlpha;
//    }
    cpyAddImg.copyTo(src(cv::Rect(cv::Point(x, y), cpyAddImg.size())));
}
