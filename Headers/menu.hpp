#ifndef MENU_HPP
#define MENU_HPP

#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <unistd.h>
#include <termios.h>

using namespace std;
using namespace cv;

void fechaJanelas(); /* Funcao para fechar as janelas relacionadas com a equalizacao da imagem, se necessario. */
void mostraMenu(Mat inImg, Mat outImg); /* Funcao para exibir a interface interativa ao usuario. */

#endif
