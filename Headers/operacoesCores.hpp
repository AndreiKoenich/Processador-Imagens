#ifndef OPERACOESCORES_HPP
#define OPERACOESCORES_HPP

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

float tomCinza(float canais[]); /* Funcao para determinar qual sera o tom cinza de um pixel da imagem. */
void luminanciaAux(float canais[]); /* Funcao para atribuir o tom cinza aos canais RGB de um pixel. */
void luminancia(Mat outImg, float canais[]); /* Funcao para aplicar efeito de luminancia em uma imagem. */
float maiorTom(Mat outImg); /* Acha o maior tom de um pixel da imagem. */
float menorTom(Mat outImg); /* Acha menor tom da imagem. */
void quantizacao(Mat inImg, Mat outImg, int n); /* Funcao para aplicar a quantizacao em uma imagem de tom cinza. */
void controlaCanais(double canais[]); /* Verifica se os canais RGB de um pixel de uma imagem estao dentro da faixa permitida. */
void aplicaNegativo(Mat outImg); /* Funcao para calcular o negativo da imagem de saida. */
void mudaContraste(Mat outImg, double gain); /* Funcao para mudar o contraste de cada pixel da imagem de saida. */
void mudaBrilho(Mat outImg, double brilho); /* Funcao para mudar o brilho de cada pixel da imagem de saida. */

#endif
