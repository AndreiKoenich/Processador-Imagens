#ifndef KERNEL_HPP
#define KERNEL_HPP

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

int verificaGaussiano(double kernel[]); /* Verifica se o kernel passado como parametro e gaussiano. */
void convolucao(Mat& outImg, double kernel[], bool clampping);
void inicializaKernel(double kernel[], int flag_kernel); /* Funcao para inicializar os valores do kernel de convolucao. */

#endif
