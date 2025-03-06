/* Nome: Andrei Pochmann Koenich	Matricula: 00308680 */

/* O programa a seguir recebe uma imagem de entrada, cujo diretorio e fornecido pelo usuario,
   e em seguida permite que essa imagem possa ser modificada com os processos de espelhamento
   horizontal, espelhamento vertical, luminancia, quantizacao, convolucao, rotacao, equalizacao,
   calculo de negativo, mudanca de brilho e mudanca de contraste. O programa tambem permite a
   visualizacao do histograma correspondente a imagem modificada. Tambem e possivel salvar a
   imagem modificada. Todas essas funcionalidades sao disponibilizadas ao usuario por meio
   do uso de uma interface. Esse programa utliza as bibliotecas OpenCV para manipulacoes de
   imagens e CVUI para geracao da interface. */

#define CVUI_IMPLEMENTATION
#include "../Headers/cvui.h"

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

#include "../Headers/constantes.hpp"
#include "../Headers/menu.hpp"
#include "../Headers/kernel.hpp"
#include "../Headers/histograma.hpp"
#include "../Headers/constantes.hpp"

using namespace cv;
using namespace std;

void inicia_programa() /* Inicializa a execucao do programa, pedindo os dados iniciais ao usuario. */
{
	Mat inImg;
	string path;

	do
	{
		system("clear");
		cout << "IMAGE PROCESSING FUNDAMENTALS - PRACTICAL WORK" << endl;
		cout << "\nAUTHOR: Andrei Pochmann Koenich" << endl;
		cout << "\nEnter the input image directory and name, including its extension:" << endl; /* Solicita o diretorio completo da imagem de entrada. */
		cin >> path;

		inImg = imread(path); /* Realiza a leitura da imagem de entrada, a partir do diretorio fornecido. */

		if (inImg.empty()) /* Controle dos casos em que nao e possivel abrir a imagem. */
		{
			cout << "ERROR: Invalid directory." << endl;
			cout << "\nExample of a valid directory: /home/ubuntu/image.jpg" << endl;
		}

	} while (inImg.empty());

	namedWindow(NOMEJANELAORIGINAL, WINDOW_AUTOSIZE);
	moveWindow(NOMEJANELAORIGINAL, X_ORIGINAL, Y_ORIGINAL); /* Posiciona a janela da imagem original em local adequado na tela. */

	namedWindow(NOMEJANELANOVA, WINDOW_AUTOSIZE);
	moveWindow(NOMEJANELANOVA, X_NOVA, Y_NOVA); /* Posiciona a janela da imagem nova em local adequado na tela. */

	Mat outImg(inImg.rows, inImg.cols, CV_8UC3, Scalar(0)); /* Declaracao da imagem de saida do programa. */

	cvui::imshow(NOMEJANELAORIGINAL, inImg);
	cvui::imshow(NOMEJANELANOVA, outImg);

	mostraMenu(inImg, outImg); /* Exibe a interface interativa ao usuario. */
}

int main()
{
	inicia_programa();
	return 0;
}

