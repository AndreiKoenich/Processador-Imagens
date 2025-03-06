#ifndef OPERACOESPRINCIPAIS_HPP
#define OPERACOESPRINCIPAIS_HPP

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
void salvaImagem(Mat outImg); /* Funcao para salvar a nova imagem em algum diretorio. */
void rotacionaHorario(Mat& outImg); /* Funcao para rotacionar a imagem de saida no sentido horario. */
void rotacionaAntihorario(Mat& outImg); /* Funcao para rotacionar a imagem de saida no sentido antihorario. */
void zoomIn(Mat& outImg); /* Funcao para realizar a operacao de zoom in 2x2. */
void zoomOut(Mat& outImg, int fator_h, int fator_v); /* Funcao para reduzir o tamanho da imagem, conforme os fatores horizontais e verticais. */
void copiaImagem(Mat inImg, Mat& outImg); /* Funcao para copiar uma imagem de entrada para uma imagem de saida. */
void flipHorizontal(Mat outImg); /* Funcao para realizar o espelhamento horizontal da imagem. */
void flipVertical(Mat outImg); /* Funcao para realizar o espelhamento vertical da imagem. */

#endif
