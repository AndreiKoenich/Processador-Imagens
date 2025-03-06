#ifndef HISTOGRAMA_HPP
#define HISTOGRAMA_HPP

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

void histograma(Mat outImg); /* Funcao para imprimir o histograma da imagem manipulada. */
void equalizaAux(Mat outImg, double hist_original[], double hist_cumulativo[]); /* Funcao para aplicar a equalizacao em um histograma especifico. */
void equalizaHistograma(Mat outImg); /* Funcao para equalizar e exibir a imagem de saida. */
int mapeiaHistogramas(double intensidade, double histcumulativo_match[]); /* Funcao para fazer o match entre dois histogramas. */
void histogramMatching(Mat outImg); /* Funcao para realizar histogram matching entre duas imagens. */
void normalizaHistograma(double histograma[]); /* Funcao para normalizar todos os valores do histograma. */
void calculaHistograma(Mat outImg, double histograma[]); /* Funcao para calcular os valores do vetor do histograma da imagem. */
void montaHistograma(Mat janela, double intensidades[]); /* Funcao para desenhar o histograma na janela correspondente. */

#endif
