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

#include <opencv2\opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <conio.h>
#include <stdbool.h>
#include "cvui.h"

#define NOMEJANELAORIGINAL "ORIGINAL IMAGE" /* Constantes dos nomes das janelas. */
#define NOMEJANELANOVA "NEW IMAGE"
#define NOMEINTERFACE "USER INTERFACE"
#define NOMEHISTOGRAMA "HISTOGRAM"
#define JANELA_ANTESEQUAL "BEFORE EQUALIZATION"
#define JANELA_HISTORIGINAL "ORIGINAL HISTOGRAM"
#define JANELA_DEPOISEQUAL "AFTER EQUALIZATION"
#define JANELA_HISTEQUALIZADO "EQUALIZED HISTOGRAM"

#define DIMENSAORGB 3
#define DIMENSAOKERNEL 9
#define INTENSIDADESIMAGEM 256
#define NORMALIZACAOHISTOGRAMA 300
#define LARGURABARRAS 4
#define TAMANHOFONTE 0.55
#define ESC 27
#define R 2
#define G 1
#define B 0

#define LARGURABOTAO 150		/* Constantes dos elementos da interface. */
#define ALTURABOTAO 30
#define LARGURAINTERFACE 400
#define ALTURAINTERFACE 860
#define LARGURAJANELAHISTOGRAMA 512
#define ALTURAJANELAHISTOGRAMA 512
#define PULO_CORES 1
#define PULO_CONTRASTE 0.1
#define PULO_BRILHO 0.5
#define PULO_PESOKERNEL 0.0625
#define PULO_ZOOM 1

#define X_INTERFACE 1350	/* Constantes das coordenadas das janelas. */
#define Y_INTERFACE 100
#define X_KERNEL 1350
#define Y_KERNEL 650

#define X_ORIGINAL 200	
#define Y_ORIGINAL 200
#define X_NOVA 820
#define Y_NOVA 200

#define X_NAOEQUALIZADO 100
#define Y_NAOEQUALIZADO 30
#define X_EQUALIZADO 750
#define Y_EQUALIZADO 30

#define FORMATOIMAGEMSAIDA ".jpg"

using namespace cv;
using namespace std;

void copia_imagem(Mat inImg, Mat& outImg) /* Funcao para copiar uma imagem de entrada para uma imagem de saida. */
{
	Mat auxImg(inImg.rows, inImg.cols, CV_8UC3, Scalar(0));

	for (int i = 0; i < inImg.rows; i++) /* Copia os conteudos de cada pixel da imagem de entrada para a imagem de saida. */
		for (int j = 0; j < inImg.cols; j++)
			auxImg.at<Vec3b>(i, j) = inImg.at<Vec3b>(i, j);

	outImg = auxImg;
}

void flip_horizontal(Mat outImg) /* Funcao para realizar o espelhamento horizontal da imagem. */
{
	Mat auxImg(outImg.rows, outImg.cols, CV_8UC3, Scalar(0));
	copia_imagem(outImg, auxImg);

	for (int i = 0; i < outImg.rows; i++) /* Realiza permutacoes entre pixels para espelhar a imagem. */
		for (int j = 0; j < outImg.cols; j++)
			outImg.at<Vec3b>(i, j) = auxImg.at<Vec3b>(i, outImg.cols - j - 1);

	cv::imshow(NOMEJANELANOVA, outImg); /* Exibe a imagem na sua tela correspondente, apos as modificacoes. */
}

void flip_vertical(Mat outImg) /* Funcao para realizar o espelhamento vertical da imagem. */
{
	Mat auxImg(outImg.rows, outImg.cols, CV_8UC3, Scalar(0));
	copia_imagem(outImg, auxImg);

	for (int i = 0; i < outImg.rows; i++) /* Realiza permutacoes entre pixels para espelhar a imagem. */
		for (int j = 0; j < outImg.cols; j++)
			outImg.at<Vec3b>(i, j) = auxImg.at<Vec3b>(outImg.rows - i - 1, j);

	cv::imshow(NOMEJANELANOVA, outImg); /* Exibe a imagem na sua tela correspondente, apos as modificacoes. */
}

float tom_cinza(float canais[]) /* Funcao para determinar qual sera o tom cinza de um pixel da imagem. */
{
	float l = (0.299 * canais[R]) + (0.587 * canais[G]) + (0.114 * canais[B]);
	return l;
}

void luminancia_aux(float canais[]) /* Funcao para atribuir o tom cinza aos canais RGB de um pixel. */
{
	float canal_cinza = tom_cinza(canais);
	canais[R] = canal_cinza;
	canais[G] = canal_cinza;
	canais[B] = canal_cinza;
}

void luminancia(Mat outImg, float canais[]) /* Funcao para aplicar efeito de luminancia em uma imagem. */
{
	for (int i = 0; i < outImg.rows; i++) /* Iteracao para percorrer todos os pixels da imagem, aplicando luminancia. */
		for (int j = 0; j < outImg.cols; j++)
		{
			canais[R] = outImg.at<cv::Vec3b>(i, j)[R]; /* Obtem os canais RGB de cada pixel da imagem. */
			canais[G] = outImg.at<cv::Vec3b>(i, j)[G];
			canais[B] = outImg.at<cv::Vec3b>(i, j)[B];

			luminancia_aux(canais); /* Atribui tom cinza aos canais RGB de um pixel. */

			outImg.at<cv::Vec3b>(i, j)[R] = canais[R];
			outImg.at<cv::Vec3b>(i, j)[G] = canais[G];
			outImg.at<cv::Vec3b>(i, j)[B] = canais[B];
		}
}

float maior_tom(Mat outImg) /* Acha o maior tom de um pixel da imagem. */
{
	float maior = 0, aux;
	float canais[DIMENSAORGB];

	for (int i = 0; i < outImg.rows; i++) /* Iteracao para percorrer todos os pixels da imagem, aplicando luminancia. */
		for (int j = 0; j < outImg.cols; j++)
		{
			canais[R] = outImg.at<cv::Vec3b>(i, j)[R]; /* Obtem os canais RGB de cada pixel da imagem. */
			canais[G] = outImg.at<cv::Vec3b>(i, j)[G];
			canais[B] = outImg.at<cv::Vec3b>(i, j)[B];

			luminancia_aux(canais); /* Atribui tom cinza aos canais RGB de um pixel. */

			if (i == 0 && j == 0) /* Atribui um valor inicial para a variavel, a fim de iniciar comparacoes. */
				maior = canais[B];

			aux = canais[B];

			if (aux > maior) /* Analisa se o tom encontrado e maior do que o anterior. */
				maior = aux;
		}

	return maior;
}

float menor_tom(Mat outImg) /* Acha menor tom da imagem. */
{
	float menor = 0, aux;
	float canais[DIMENSAORGB];

	for (int i = 0; i < outImg.rows; i++) /* Iteracao para percorrer todos os pixels da imagem, aplicando luminancia. */
		for (int j = 0; j < outImg.cols; j++)
		{
			canais[R] = outImg.at<cv::Vec3b>(i, j)[R]; /* Obtem os canais RGB de cada pixel da imagem. */
			canais[G] = outImg.at<cv::Vec3b>(i, j)[G];
			canais[B] = outImg.at<cv::Vec3b>(i, j)[B];

			luminancia_aux(canais); /* Atribui tom cinza aos canais RGB de um pixel. */

			if (i == 0 && j == 0) /* Atribui um valor inicial para a variavel, a fim de iniciar comparacoes. */
				menor = canais[B];

			aux = canais[B];

			if (aux < menor) /* Analisa se o tom encontrado e menor do que o anterior. */
				menor = aux;
		}

	return menor;
}

void quantizacao(Mat inImg, Mat outImg, int n) /* Funcao para aplicar a quantizacao em uma imagem de tom cinza. */
{
	float maior = maior_tom(inImg); /* Determina o maior tom de pixel presenta na imagem. */
	float menor = menor_tom(inImg); /* Determina o menor tom de pixel presente na imagem. */
	float tam_int = (maior - menor + 1);
	float canais[DIMENSAORGB];
	float t_orig, ti, tf, centro;
	int k, achou_intervalo = 1;

	if (n <= 0) /* Casos em que o usuario escolhe um numero de cores negativo. */
		n = 1;

	else if (n >= tam_int) /* Casos em que o numero de cores supera o sucessor da diferença entre o maior e o menor tom da imagem. */
	{
		cvui::imshow(NOMEJANELANOVA, outImg); /* Exibe a imagem na sua tela correspondente, apos as modificacoes. */
		return;
	}

	int tb = roundf(tam_int / n); /* Calculo do tamanho de cada intervalo bin da imagem. */

	for (int i = 0; i < outImg.rows; i++) /* Iteracao para percorrer todos os pixels da imagem, aplicando quantizacao. */
		for (int j = 0; j < outImg.cols; j++)
		{
			canais[R] = outImg.at<cv::Vec3b>(i, j)[R]; /* Obtem os canais RGB de cada pixel da imagem. */
			canais[G] = outImg.at<cv::Vec3b>(i, j)[G];
			canais[B] = outImg.at<cv::Vec3b>(i, j)[B];

			luminancia_aux(canais); /* Atribui tom cinza aos canais RGB de um pixel. */
			t_orig = canais[B];

			k = 0;
			achou_intervalo = 1;

			while (achou_intervalo == 1)
			{
				ti = menor - 0.5 + k * tb;			/* Calculo do limite inferior de um dos intervalos de bin da imagem. */
				tf = menor - 0.5 + (k + 1) * tb;	/* Calculo do limite superior dos intervalos de bin da imagem. */
				k++;

				if (t_orig >= ti && t_orig < tf) /* Verifica qual o intervalo de bin contem o t_orig do pixel analisado. */
				{
					achou_intervalo = 0; /* Indica que o intervalo foi encontrado, para encerrar a iteracao. */
					centro = roundf((ti + tf) / 2); /* Determina o valor inteiro mais proximo do centro do intervalo. */

					outImg.at<cv::Vec3b>(i, j)[R] = centro; /* Altera o tom de cada pixel da imagem. */
					outImg.at<cv::Vec3b>(i, j)[G] = centro;
					outImg.at<cv::Vec3b>(i, j)[B] = centro;
				}
			}
		}

	cvui::imshow(NOMEJANELANOVA, outImg); /* Exibe a imagem na sua tela correspondente, apos as modificacoes. */
}

void salva_imagem(Mat outImg) /* Funcao para salvar a nova imagem em algum diretorio. */
{
	bool exportou;
	string name, pathOut, finalPath;
	ShowWindow(GetConsoleWindow(), SW_SHOW); /* Faz com que o terminal apareca novamente. */

	system("cls");

	cout << "Enter the output image name (no need for extension):" << endl; /* Pede o nome da imagem de saida ao usuario. */
	cin >> name;

	name.append(FORMATOIMAGEMSAIDA); /* Adiciona a extensao .jpg na imagem de saida. */

	cout << "\nEnter the output image directory:" << endl; /* Solicita o diretorio da imagem de saida ao usuario. */
	cin >> pathOut;

	finalPath.append(pathOut);
	finalPath.append("\\");
	finalPath.append(name);

	exportou = imwrite(finalPath, outImg); /* Exporta a imagem de saida ao diretorio informado. */

	if (exportou == false) /* Controle dos casos em que a nova imagem nao pode ser exportada. */
	{
		cout << "\nERROR: Invalid image name and/or directory." << endl;
		cout << "\nExample of a valid directory: \\Users\\Andrei\\Desktop" << endl;
	}

	else
	{
		cout << "\nThe image ";
		cout << name;
		cout << " was succesfully saved." << endl;
	}

	cout << "\nPress any key to continue." << endl;
	_getch() /* Aguarda o usuario pressionar alguma tecla. */;

	ShowWindow(GetConsoleWindow(), SW_HIDE); /* Esconde o terminal novamente. */
}

void controla_canais(double canais[]) /* Verifica se os canais RGB de um pixel de uma imagem estao dentro da faixa permitida. */
{
	for (int i = 0; i < DIMENSAORGB; i++)
	{
		if (canais[i] <= 0) /* Verifica se o valor da intensidade do pixel ultrapassou o limite inferior. */
			canais[i] = 0;

		else if (canais[i] > 255) /* Verifica se o valor da intensidade do pixel ultrapassou o limite superior. */
			canais[i] = 255;
	}
}

void aplica_negativo(Mat outImg) /* Funcao para calcular o negativo da imagem de saida. */
{
	for (int i = 0; i < outImg.rows; i++)	/* Aplica a operacao de negativo em cada pixel da imagem de saida. */
		for (int j = 0; j < outImg.cols; j++)
		{
			outImg.at<cv::Vec3b>(i, j)[R] = (-1 * outImg.at<cv::Vec3b>(i, j)[R]) + 255; /* Multiplica os canais RGB do pixel por -1, e soma com 255. */
			outImg.at<cv::Vec3b>(i, j)[G] = (-1 * outImg.at<cv::Vec3b>(i, j)[G]) + 255;
			outImg.at<cv::Vec3b>(i, j)[B] = (-1 * outImg.at<cv::Vec3b>(i, j)[B]) + 255;
		}

	cv::imshow(NOMEJANELANOVA, outImg); /* Exibe a imagem na sua tela correspondente, apos as modificacoes. */
}

void muda_contraste(Mat outImg, double gain) /* Funcao para mudar o contraste de cada pixel da imagem de saida. */
{
	double gain_aux = gain;
	double canais[DIMENSAORGB];

	if (gain <= 0)			/* Mantem o valor do contraste a ser aplicado dentro da faixa aceitavel. */
		gain_aux = 0.1;

	else if (gain > 255)	/* Mantem o valor do contraste a ser aplicado dentro da faixa aceitavel. */
		gain_aux = 255;

	for (int i = 0; i < outImg.rows; i++)	/* Aplica a alteracao de contraste em cada pixel da imagem de saida. */
		for (int j = 0; j < outImg.cols; j++)
		{
			canais[R] = outImg.at<cv::Vec3b>(i, j)[R] * gain_aux;
			canais[G] = outImg.at<cv::Vec3b>(i, j)[G] * gain_aux;
			canais[B] = outImg.at<cv::Vec3b>(i, j)[B] * gain_aux;

			controla_canais(canais);	/* Verifica se o resultado da operacao esta dentro da faixa aceitavel. */

			outImg.at<cv::Vec3b>(i, j)[R] = canais[R];
			outImg.at<cv::Vec3b>(i, j)[G] = canais[G];
			outImg.at<cv::Vec3b>(i, j)[B] = canais[B];
		}

	cv::imshow(NOMEJANELANOVA, outImg); /* Exibe a imagem na sua tela correspondente, apos as modificacoes. */
}

void muda_brilho(Mat outImg, double brilho) /* Funcao para mudar o brilho de cada pixel da imagem de saida. */
{
	double brilho_aux = brilho;
	double canais[DIMENSAORGB];

	if (brilho < -255)		/* Mantem o valor do brilho a ser aplicado dentro da faixa aceitavel. */
		brilho_aux = -255;

	else if (brilho > 255)	/* Mantem o valor do brilho a ser aplicado dentro da faixa aceitavel. */
		brilho_aux = 255;

	for (int i = 0; i < outImg.rows; i++) /* Aplica a alteracao de brilho em cada pixel da imagem de saida. */
		for (int j = 0; j < outImg.cols; j++)
		{
			canais[R] = outImg.at<cv::Vec3b>(i, j)[R] + brilho_aux;
			canais[G] = outImg.at<cv::Vec3b>(i, j)[G] + brilho_aux;
			canais[B] = outImg.at<cv::Vec3b>(i, j)[B] + brilho_aux;

			controla_canais(canais);	/* Verifica se o resultado da operacao esta dentro da faixa aceitavel. */

			outImg.at<cv::Vec3b>(i, j)[R] = canais[R];
			outImg.at<cv::Vec3b>(i, j)[G] = canais[G];
			outImg.at<cv::Vec3b>(i, j)[B] = canais[B];
		}

	cv::imshow(NOMEJANELANOVA, outImg); /* Exibe a imagem na sua tela correspondente, apos as modificacoes. */
}

void rotaciona_horario(Mat& outImg) /* Funcao para rotacionar a imagem de saida no sentido horario. */
{
	Mat auxImg(outImg.cols, outImg.rows, CV_8UC3, Scalar(0)); /* Declaracao de imagem auxiliar, com dimensoes invertidas. */

	for (int i = 0; i < outImg.rows; i++) /* Faz a permutacao dos pixels, rotacionando a imagem. */
		for (int j = 0; j < outImg.cols; j++)
			auxImg.at<Vec3b>(j, outImg.rows - i - 1) = outImg.at<Vec3b>(i, j);

	outImg = auxImg; /* Atribui o conteudo da imagem auxiliar para a imagem de saida. */

	cv::imshow(NOMEJANELANOVA, outImg); /* Exibe a imagem na sua tela correspondente, apos as modificacoes. */
}

void rotaciona_antihorario(Mat& outImg) /* Funcao para rotacionar a imagem de saida no sentido antihorario. */
{
	Mat auxImg(outImg.cols, outImg.rows, CV_8UC3, Scalar(0));

	for (int i = 0; i < outImg.rows; i++) /* Faz a permutacao dos pixels, rotacionando a imagem. */
		for (int j = 0; j < outImg.cols; j++)
			auxImg.at<Vec3b>(outImg.cols - j - 1, i) = outImg.at<Vec3b>(i, j);

	outImg = auxImg; /* Atribui o conteudo da imagem auxiliar para a imagem de saida. */
	cv::imshow(NOMEJANELANOVA, outImg); /* Exibe a imagem na sua tela correspondente, apos as modificacoes. */
}

void normaliza_histograma(double histograma[]) /* Funcao para normalizar todos os valores do histograma. */
{
	double max = 0;

	for (int i = 0; i < INTENSIDADESIMAGEM; i++) /* Acha o maior valor do histograma. */
		if (histograma[i] > max)
			max = histograma[i];

	for (int i = 0; i < INTENSIDADESIMAGEM; i++) /* Iteracao para normalizar os valores do histograma. */
		histograma[i] = (histograma[i] / max) * (NORMALIZACAOHISTOGRAMA);
}

void calcula_histograma(Mat outImg, double histograma[]) /* Funcao para calcular os valores do vetor do histograma da imagem. */
{
	int indice;

	for (int i = 0; i < outImg.rows; i++) /* Calcula a frequencia de cada tom da imagem. */
		for (int j = 0; j < outImg.cols; j++)
		{
			indice = (int)outImg.at<cv::Vec3b>(i, j)[0];
			histograma[indice]++; /* Incrementa a frequencia de um determinado tom da imagem. */
		}
}

void monta_histograma(Mat janela, double intensidades[]) /* Funcao para desenhar o histograma na janela correspondente. */
{
	int i = 0;

	for (i = 0; i < INTENSIDADESIMAGEM; i++) /* Iteracoes para desenhar o histograma na nova janela, com varios retangulos adjacentes. */
		for (int k = 0; k < LARGURABARRAS; k++)
			rectangle(janela, Point(128 + i + k, 383), Point(128 + i, 383 - intensidades[i]), Scalar(255, 191, 0), FILLED);

	line(janela, Point(115, 385), Point(128 + i, 385), Scalar(0, 0, 0), 1, LINE_8); /* Desenha o eixo das abcissas. */

	putText(janela, "0", Point(125, 400), FONT_HERSHEY_COMPLEX_SMALL, TAMANHOFONTE, Scalar(0, 0, 0)); /* Desenha as legendas dos valores no eixo. */
	putText(janela, "50", Point(175, 400), FONT_HERSHEY_COMPLEX_SMALL, TAMANHOFONTE, Scalar(0, 0, 0));
	putText(janela, "100", Point(220, 400), FONT_HERSHEY_COMPLEX_SMALL, TAMANHOFONTE, Scalar(0, 0, 0));
	putText(janela, "150", Point(270, 400), FONT_HERSHEY_COMPLEX_SMALL, TAMANHOFONTE, Scalar(0, 0, 0));
	putText(janela, "200", Point(320, 400), FONT_HERSHEY_COMPLEX_SMALL, TAMANHOFONTE, Scalar(0, 0, 0));
	putText(janela, "250", Point(370, 400), FONT_HERSHEY_COMPLEX_SMALL, TAMANHOFONTE, Scalar(0, 0, 0));
}

void histograma(Mat outImg) /* Funcao para imprimir o histograma da imagem manipulada. */
{
	Mat auxImg(outImg.rows, outImg.cols, CV_8UC3, Scalar(0));

	copia_imagem(outImg, auxImg);

	float canais[DIMENSAORGB];
	luminancia(auxImg, canais); /* Aplica a operacao de luminancia na imagem. */

	double intensidades[INTENSIDADESIMAGEM] = { 0 };

	Mat janela_histograma(LARGURAJANELAHISTOGRAMA, ALTURAJANELAHISTOGRAMA, CV_8UC3, Scalar(255, 255, 255));

	calcula_histograma(auxImg, intensidades); /* Funcao para calcular os valores do vetor do histograma da imagem. */
	normaliza_histograma(intensidades); /* Normaliza todos os valores do histograma, antes da impressao. */
	monta_histograma(janela_histograma, intensidades); /* Funcao para desenhar o histograma na janela correspondente. */

	imshow(NOMEHISTOGRAMA, janela_histograma); /* Exibe a janela contendo o histograma. */
}

void equaliza_aux(Mat outImg, double hist_original[], double hist_cumulativo[]) /* Funcao para aplicar a equalizacao em um histograma especifico. */
{
	calcula_histograma(outImg, hist_original);

	int total_pixels = outImg.cols * outImg.rows;
	double alpha = 255.0 / total_pixels;

	hist_cumulativo[0] = (hist_original[0] * alpha);	/* Calculo do histograma cumulativo. */

	for (int i = 1; i < INTENSIDADESIMAGEM; i++)
		hist_cumulativo[i] = (hist_cumulativo[i - 1] + (hist_original[i] * alpha)); /* Calculo do histograma cumulativo. */

	for (int i = 0; i < outImg.rows; i++) /* Iteracoes para obter a nova imagem de saida. */
		for (int j = 0; j < outImg.cols; j++)
		{
			outImg.at<cv::Vec3b>(i, j)[R] = hist_cumulativo[outImg.at<cv::Vec3b>(i, j)[R]];
			outImg.at<cv::Vec3b>(i, j)[G] = hist_cumulativo[outImg.at<cv::Vec3b>(i, j)[G]];
			outImg.at<cv::Vec3b>(i, j)[B] = hist_cumulativo[outImg.at<cv::Vec3b>(i, j)[B]];
		}
}

void equaliza_histograma(Mat outImg) /* Funcao para equalizar e exibir a imagem de saida. */
{
	float canais[DIMENSAORGB] = { 0 };
	double hist_original[INTENSIDADESIMAGEM] = { 0 };
	double hist_cumulativo[INTENSIDADESIMAGEM] = { 0 };
	double hist_novo[INTENSIDADESIMAGEM] = { 0 };

	Mat auxImg(outImg.rows, outImg.cols, CV_8UC3, Scalar(0));
	copia_imagem(outImg, auxImg);

	luminancia(auxImg, canais);
	luminancia(outImg, canais);

	equaliza_aux(outImg, hist_original, hist_cumulativo);
	normaliza_histograma(hist_original); /* Realiza a normalizacao do histograma apos a equalizacao. */

	destroyWindow(NOMEJANELAORIGINAL);	/* Fecha a janela comum contendo a imagem original. */
	destroyWindow(NOMEJANELANOVA);		/* Fecha a janela comum contendo a imagem modificada antes da equalizacao. */

	if (cv::getWindowProperty("HISTOGRAM", WND_PROP_VISIBLE) == 1) /* Fecha a janela com o histograma comum, se necessario. */
		destroyWindow("HISTOGRAM");

	Mat janelahistograma_original(LARGURAJANELAHISTOGRAMA, ALTURAJANELAHISTOGRAMA, CV_8UC3, Scalar(255, 255, 255));
	monta_histograma(janelahistograma_original, hist_original); /* Funcao para desenhar o histograma original na janela correspondente. */

	imshow(JANELA_ANTESEQUAL, auxImg); /* Imprime a imagem original e o seu histograma abaixo. */
	imshow(JANELA_HISTORIGINAL, janelahistograma_original);
	moveWindow(JANELA_ANTESEQUAL, X_NAOEQUALIZADO, Y_NAOEQUALIZADO);
	moveWindow(JANELA_HISTORIGINAL, X_NAOEQUALIZADO, Y_NAOEQUALIZADO + 420);

	Mat janelahistograma_novo(LARGURAJANELAHISTOGRAMA, ALTURAJANELAHISTOGRAMA, CV_8UC3, Scalar(255, 255, 255));
	calcula_histograma(outImg, hist_novo); /* Calcula os valores do vetor do novo histograma da imagem, ja equalizada. */
	normaliza_histograma(hist_novo); /* Realiza a normalizacao do histograma apos a equalizacao. */
	monta_histograma(janelahistograma_novo, hist_novo); /* Funcao para desenhar o histograma equalizado na janela correspondente. */

	imshow(JANELA_DEPOISEQUAL, outImg); /* Imprime a imagem equalizada e o seu histograma abaixo. */
	imshow(JANELA_HISTEQUALIZADO, janelahistograma_novo);
	moveWindow(JANELA_DEPOISEQUAL, X_EQUALIZADO, Y_EQUALIZADO);
	moveWindow(JANELA_HISTEQUALIZADO, X_EQUALIZADO, Y_EQUALIZADO + 420);
}

int mapeia_histogramas(double intensidade, double histcumulativo_match[]) /* Funcao para fazer o match entre dois histogramas. */
{
	int indice_menor = 0;
	double diferenca, menor_diferenca;

	menor_diferenca = abs(histcumulativo_match[0] - intensidade);

	for (int i = 1; i < INTENSIDADESIMAGEM; i++)
	{
		diferenca = abs(histcumulativo_match[i] - intensidade); /* Calcula a diferenca entre as intensidades consideradas. */

		if (diferenca < menor_diferenca)
		{
			indice_menor = i; /* Atualiza o indice correspondente a intensidade mais proxima. */
			menor_diferenca = diferenca;
		}
	}

	return indice_menor; /* Retorna o ponto de match para o novo histograma. */
}

void histogram_matching(Mat outImg) /* Funcao para realizar histogram matching entre duas imagens. */
{
	Mat matchImg;
	String path;
	float canais[DIMENSAORGB] = { 0 };
	double canais_match[DIMENSAORGB] = { 0 };

	double hist_original[INTENSIDADESIMAGEM] = { 0 };
	double hist_cumulativo[INTENSIDADESIMAGEM] = { 0 };

	double hist_originalmatch[INTENSIDADESIMAGEM] = { 0 };
	double hist_cumulativomatch[INTENSIDADESIMAGEM] = { 0 };

	double hist_matched[INTENSIDADESIMAGEM] = { 0 };

	ShowWindow(GetConsoleWindow(), SW_SHOW); /* Mostra o terminal. */
	system("cls");

	cout << "Enter the image whose histogram will be used for matching. " << endl;
	cout << "Type the image directory and name, including its extension:" << endl; /* Solicita o diretorio completo da imagem para o matching de histograma. */
	cin >> path;

	matchImg = imread(path); /* Realiza a leitura da imagem de entrada, a partir do diretorio fornecido. */

	if (matchImg.empty()) /* Controle dos casos em que nao e possivel abrir a imagem. */
	{
		system("cls");
		cout << "ERROR: Invalid directory." << endl;
		cout << "\nExample of a valid directory: \\Users\\Andrei\\Desktop\\Image.jpg" << endl;
		cout << "\nPress any key to continue." << endl;
		_getch() /* Aguarda o usuario pressionar alguma tecla. */;
		system("cls");
		ShowWindow(GetConsoleWindow(), SW_HIDE); /* Fecha o terminal. */
		return;
	}

	ShowWindow(GetConsoleWindow(), SW_HIDE); /* Fecha o terminal. */

	luminancia(outImg, canais); /* Aplica luminancia na imagem original. */
	luminancia(matchImg, canais); /* Aplica luminancia na imagem de match. */

	calcula_histograma(outImg, hist_original); /* Calcula o histograma da imagem original. */
	equaliza_aux(outImg, hist_original, hist_cumulativo); /* Equaliza o histograma original e calcula o histograma cumulativo. */
	normaliza_histograma(hist_cumulativo); /* Normaliza o histograma cumulativo da imagem original. */

	calcula_histograma(matchImg, hist_originalmatch); /* Calcula o histograma da imagem de match. */
	equaliza_aux(matchImg, hist_originalmatch, hist_cumulativomatch); /* Equaliza o histograma original e calcula o histograma cumulativo. */
	normaliza_histograma(hist_cumulativomatch); /* Normaliza o histograma cumulativo da imagem de match. */

	for (int i = 0; i < INTENSIDADESIMAGEM; i++)
		hist_matched[i] = hist_cumulativo[mapeia_histogramas(hist_cumulativo[i], hist_cumulativomatch)];

	for (int i = 0; i < outImg.rows; i++) /* Iteracoes para obter a nova imagem de saida, a partir do histograma novo. */
		for (int j = 0; j < outImg.cols; j++)
		{
			outImg.at<cv::Vec3b>(i, j)[R] = hist_matched[outImg.at<cv::Vec3b>(i, j)[R]];
			outImg.at<cv::Vec3b>(i, j)[G] = hist_matched[outImg.at<cv::Vec3b>(i, j)[G]];
			outImg.at<cv::Vec3b>(i, j)[B] = hist_matched[outImg.at<cv::Vec3b>(i, j)[B]];
		}

	cv::imshow(NOMEJANELANOVA, outImg); /* Exibe a imagem na sua tela correspondente, apos as modificacoes. */
}

void zoomIn(Mat& outImg) /* Funcao para realizar a operacao de zoom in 2x2. */
{
	int new_height = 2 * outImg.rows - 1; /* Calculo das dimensoes da nova imagem aproximada. */
	int new_width = 2 * outImg.cols - 1;
	int a = 0, b = 0, c = 0, d = 0;

	Mat auxImg(new_height, new_width, CV_8UC3, Scalar(0)); /* Declaracao de imagem auxiliar. */

	while (a < new_height) /* Iteracoes para preencher as linhas pares da imagem auxiliar. */
	{
		while (b < new_width)
		{
			auxImg.at<Vec3b>(a, b)[R] = outImg.at<Vec3b>(c, d)[R];
			auxImg.at<Vec3b>(a, b)[G] = outImg.at<Vec3b>(c, d)[G];
			auxImg.at<Vec3b>(a, b)[B] = outImg.at<Vec3b>(c, d)[B];

			b += 2;
			d++;
		}

		b = 1;
		d = 0;

		while (b < new_width) /* Preenche os pixels com a media dos valores dos pixels horizontalmente adjacentes. */
		{
			auxImg.at<Vec3b>(a, b)[R] = (auxImg.at<Vec3b>(a, b - 1)[R] + auxImg.at<Vec3b>(a, b + 1)[R]) / 2;
			auxImg.at<Vec3b>(a, b)[G] = (auxImg.at<Vec3b>(a, b - 1)[G] + auxImg.at<Vec3b>(a, b + 1)[G]) / 2;
			auxImg.at<Vec3b>(a, b)[B] = (auxImg.at<Vec3b>(a, b - 1)[B] + auxImg.at<Vec3b>(a, b + 1)[B]) / 2;


			b += 2;
		}

		a += 2;
		b = 0;
		c++;
	}

	a = 1;
	b = 0;

	while (a < new_height)  /* Iteracoes para preencher as linhas impares da imagem auxiliar. */
	{
		while (b < new_width) /* Preenche os pixels com a media dos valores dos pixels verticalmente adjacentes. */
		{
			auxImg.at<Vec3b>(a, b)[R] = (auxImg.at<Vec3b>(a - 1, b)[R] + auxImg.at<Vec3b>(a + 1, b)[R]) / 2;
			auxImg.at<Vec3b>(a, b)[G] = (auxImg.at<Vec3b>(a - 1, b)[G] + auxImg.at<Vec3b>(a + 1, b)[G]) / 2;
			auxImg.at<Vec3b>(a, b)[B] = (auxImg.at<Vec3b>(a - 1, b)[B] + auxImg.at<Vec3b>(a + 1, b)[B]) / 2;

			b++;
		}

		a += 2;
		b = 0;
	}

	outImg = auxImg; /* Atribui o conteudo da imagem auxiliar para a imagem de saida. */
	cv::imshow(NOMEJANELANOVA, outImg); /* Exibe a imagem na sua tela correspondente, apos as modificacoes. */
}

void zoomOut(Mat& outImg, int fator_h, int fator_v) /* Funcao para reduzir o tamanho da imagem, conforme os fatores horizontais e verticais. */
{
	int new_width, new_height, total_pixels = 0, auxfator_h = fator_h, auxfator_v = fator_v;
	float somasR = 0, somasG = 0, somasB = 0, mediaR = 0, mediaG = 0, mediaB = 0;

	if (fator_h < 1) /* Verifica se o fator horizontal possui valor aceitavel. */
		auxfator_h = 1;

	if (fator_v < 1) /* Verifica se o fator vertical possui valor aceitavel. */
		auxfator_v = 1;

	new_height = floor(outImg.rows / auxfator_v); /* Determina a dimensao vertical da imagem de saida. */
	new_width = floor(outImg.cols / auxfator_h); /* Determina a dimensao horizontal da imagem de saida. */

	if (new_height <= 0) /* Mantem a dimensao vertical da imagem de saida dentro do limite aceitavel. */
		new_height = 1;

	if (new_width <= 0) /* Mantem a dimensao horizontal da imagem de saida dentro do limite aceitavel. */
		new_width = 1;

	Mat auxImg(new_height, new_width, CV_8UC3, Scalar(0)); /* Declaracao de imagem auxiliar. */

	for (int a = 0; a < outImg.rows; a += auxfator_v) /* Iteracoes para percorrer a imagem de origem, e calcular as medias dos pixels em cada bloco. */
	{
		for (int b = 0; b < outImg.cols; b += auxfator_h)
		{

			somasR = somasG = somasB = total_pixels = 0;
			mediaR = mediaG = mediaB = 0;

			for (int c = 0; c < auxfator_h; c++)
				for (int d = 0; d < auxfator_v; d++)
				{
					if (a + d < outImg.rows && b + c < outImg.cols) /* Armazena a soma de cada um dos tons RGB de um pixel. */
					{
						somasR += outImg.at<Vec3b>(a + d, b + c)[R];
						somasG += outImg.at<Vec3b>(a + d, b + c)[G];
						somasB += outImg.at<Vec3b>(a + d, b + c)[B];
						total_pixels++;
					}
				}

			if (total_pixels != 0)  /* Calcula as medias dos canais RGB para cada pixel do bloco considerado. */
			{
				mediaR = somasR / total_pixels;
				mediaG = somasG / total_pixels;
				mediaB = somasB / total_pixels;
			}

			if (a / auxfator_v < auxImg.rows && b / auxfator_h < auxImg.cols) /* Atribui os valores RGB de cada pixel para a imagem de saida. */
			{
				auxImg.at<Vec3b>(a / auxfator_v, b / auxfator_h)[R] = mediaR;
				auxImg.at<Vec3b>(a / auxfator_v, b / auxfator_h)[G] = mediaG;
				auxImg.at<Vec3b>(a / auxfator_v, b / auxfator_h)[B] = mediaB;
			}
		}
	}

	outImg = auxImg; /* Atribui o conteudo da imagem auxiliar para a imagem de saida. */
	cv::imshow(NOMEJANELANOVA, outImg); /* Exibe a imagem na sua tela correspondente, apos as modificacoes. */
}

int verifica_gaussiano(double kernel[]) /* Verifica se o kernel passado como parametro e gaussiano. */
{
	double gaussiano[DIMENSAOKERNEL] = { 0.0625, 0.125, 0.0625, 0.125, 0.25, 0.125, 0.0625, 0.125, 0.0625 }; /* Gaussiano, filtro passa baixas (produz borradura). */

	for (int i = 0; i < DIMENSAOKERNEL; i++)
		if (kernel[i] != gaussiano[i])
			return 0; /* Retorna zero, indicando que os kernels sao diferentes. */

	return 1; /* Retorna um, indicando que os kernels sao iguais. */
}

void convolucao(Mat& outImg, double kernel[], bool clampping)
{
	float canais[DIMENSAORGB];
	double valor_pixel[DIMENSAORGB];
	Mat auxImg(outImg.rows, outImg.cols, CV_8UC3, Scalar(0)); /* Declaracao de imagem auxiliar. */

	if (!verifica_gaussiano(kernel)) /* Aplica a operacao de luminancia na imagem, se o kernel recebido nao corresponde ao gaussiano. */
		luminancia(outImg, canais);

	for (int i = 1; i < outImg.rows - 1; i++) /* Aplica a convolucao com o kernel rotacionado em 180 graus, em cada pixel da imagem. */
		for (int j = 1; j < outImg.cols - 1; j++)
		{
			/* Realiza os produtos dos pixels da vizinhanca com o kernel rotacionado em 180 graus, em seguida realiza as somas. */
			valor_pixel[R] = outImg.at<cv::Vec3b>(i - 1, j - 1)[R] * kernel[8] + outImg.at<cv::Vec3b>(i - 1, j)[R] * kernel[7] + outImg.at<cv::Vec3b>(i - 1, j + 1)[R] * kernel[6] +
				outImg.at<cv::Vec3b>(i, j - 1)[R] * kernel[5] + outImg.at<cv::Vec3b>(i, j)[R] * kernel[4] + outImg.at<cv::Vec3b>(i, j + 1)[R] * kernel[3] +
				outImg.at<cv::Vec3b>(i + 1, j - 1)[R] * kernel[2] + outImg.at<cv::Vec3b>(i + 1, j)[R] * kernel[1] + outImg.at<cv::Vec3b>(i + 1, j + 1)[R] * kernel[0];
			valor_pixel[G] = outImg.at<cv::Vec3b>(i - 1, j - 1)[G] * kernel[8] + outImg.at<cv::Vec3b>(i - 1, j)[G] * kernel[7] + outImg.at<cv::Vec3b>(i - 1, j + 1)[G] * kernel[6] +
				outImg.at<cv::Vec3b>(i, j - 1)[G] * kernel[5] + outImg.at<cv::Vec3b>(i, j)[G] * kernel[4] + outImg.at<cv::Vec3b>(i, j + 1)[G] * kernel[3] +
				outImg.at<cv::Vec3b>(i + 1, j - 1)[G] * kernel[2] + outImg.at<cv::Vec3b>(i + 1, j)[G] * kernel[1] + outImg.at<cv::Vec3b>(i + 1, j + 1)[G] * kernel[0];
			valor_pixel[B] = outImg.at<cv::Vec3b>(i - 1, j - 1)[B] * kernel[8] + outImg.at<cv::Vec3b>(i - 1, j)[B] * kernel[7] + outImg.at<cv::Vec3b>(i - 1, j + 1)[B] * kernel[6] +
				outImg.at<cv::Vec3b>(i, j - 1)[B] * kernel[5] + outImg.at<cv::Vec3b>(i, j)[B] * kernel[4] + outImg.at<cv::Vec3b>(i, j + 1)[B] * kernel[3] +
				outImg.at<cv::Vec3b>(i + 1, j - 1)[B] * kernel[2] + outImg.at<cv::Vec3b>(i + 1, j)[B] * kernel[1] + outImg.at<cv::Vec3b>(i + 1, j + 1)[B] * kernel[0];

			if (clampping) /* Soma os pixels com 127, se necessario (clampping). */
			{
				valor_pixel[R] += 127;
				valor_pixel[G] += 127;
				valor_pixel[B] += 127;
			}

			controla_canais(valor_pixel); /* Mantem os valores dos pixels entre 0 e 255. */
			auxImg.at<cv::Vec3b>(i, j)[R] = valor_pixel[R]; /* Atribui os canais RGB do pixel para a imagem de saida. */
			auxImg.at<cv::Vec3b>(i, j)[G] = valor_pixel[G];
			auxImg.at<cv::Vec3b>(i, j)[B] = valor_pixel[B];
		}

	outImg = auxImg; /* Atribui o conteudo da imagem auxiliar para a imagem de saida. */
	cv::imshow(NOMEJANELANOVA, outImg); /* Exibe a imagem na sua tela correspondente, apos as modificacoes. */
}

void inicializa_kernel(double kernel[], int flag_kernel) /* Funcao para inicializar os valores do kernel de convolucao. */
{
	switch (flag_kernel) /* Analisa qual kernel sera carregado na interface. */
	{
	case 1:		/* Gaussiano, filtro passa baixas (produz borradura). */
		kernel[0] = kernel[2] = kernel[6] = kernel[8] = 0.0625;
		kernel[1] = kernel[3] = kernel[5] = kernel[7] = 0.125;
		kernel[4] = 0.25;
		break;

	case 2:		/* Passa Altas Genérico(detector de arestas mais sensitivo). */
		kernel[0] = kernel[1] = kernel[2] = kernel[3] = kernel[5] = kernel[6] = kernel[7] = kernel[8] = -1;
		kernel[4] = 8;
		break;

	case 3:		/* Sobel Hx (mais sensível ao gradiente dos tons de cinza na direção horizontal – efeito de relevo) */
		kernel[0] = kernel[6] = -1;
		kernel[1] = kernel[4] = kernel[7] = 0;
		kernel[2] = kernel[8] = 1;
		kernel[3] = -2;
		kernel[5] = 2;
		break;

	case 4:		/* Sobel Hy (mais sensível ao gradiente dos tons de cinza na direção vertical – efeito de relevo) */
		kernel[0] = kernel[2] = -1;
		kernel[1] = -2;
		kernel[3] = kernel[4] = kernel[5] = 0;
		kernel[6] = kernel[8] = 1;
		kernel[7] = 2;
		break;

	case 5:		/* Laplaciano, filtro passa altas (detecta arestas importantes). */
		kernel[1] = kernel[3] = kernel[5] = kernel[7] = -1;
		kernel[0] = kernel[2] = kernel[6] = kernel[8] = 0;
		kernel[4] = 4;
		break;

	case 6:		/* Prewitt Hx (gradiente dos tons de cinza na direção horizontal – efeito de relevo) */
		kernel[0] = kernel[3] = kernel[6] = -1;
		kernel[1] = kernel[4] = kernel[7] = 0;
		kernel[2] = kernel[5] = kernel[8] = 1;
		break;

	case 7:		/* Prewitt Hy (gradiente dos tons de cinza na direção vertical – efeito de relevo) */
		kernel[0] = kernel[1] = kernel[2] = -1;
		kernel[3] = kernel[4] = kernel[5] = 0;
		kernel[6] = kernel[7] = kernel[8] = 1;
		break;

	case 8:		/* Zera todos os componentes do kernel de convolucao. */
		kernel[0] = kernel[1] = kernel[2] = 0;
		kernel[3] = kernel[4] = kernel[5] = 0;
		kernel[6] = kernel[7] = kernel[8] = 0;
		break;
	}
}

void fecha_janelas() /* Funcao para fechar as janelas relacionadas com a equalizacao da imagem, se necessario. */
{
	if (cv::getWindowProperty(JANELA_ANTESEQUAL, WND_PROP_VISIBLE) == 1) /* Verifica se as janelas estao abertas. */
	{
		destroyWindow(JANELA_ANTESEQUAL);
		destroyWindow(JANELA_HISTORIGINAL);
		destroyWindow(JANELA_DEPOISEQUAL);
		destroyWindow(JANELA_HISTEQUALIZADO);
	}
}

void mostra_menu(Mat inImg, Mat outImg) /* Funcao para exibir a interface interativa ao usuario. */
{
	float canais[DIMENSAORGB];
	double kernel[DIMENSAOKERNEL] = { 0 };

	copia_imagem(inImg, outImg); /* Copia o conteudo da imagem de entrada para a imagem de saida, deixando-a pronta para sofrer operacoes. */

	Mat frame = Mat(ALTURAINTERFACE, LARGURAINTERFACE, CV_8UC3); /* Declaracao do frame que corresponde a janela de interface. */

	namedWindow(NOMEINTERFACE);
	moveWindow(NOMEINTERFACE, X_INTERFACE, Y_INTERFACE); /* Posiciona a janela da interface em local adequado na tela. */
	cvui::init(NOMEINTERFACE);

	int colors = 1;
	double contrast = 1;
	double brightness = 1;
	int hor_factor = 1, ver_factor = 1;
	bool clampping = false;

	while (true) /* Exibicao de todos os botoes do menu. */
	{
		frame = cv::Scalar(49, 52, 49); /* Determina as cores da interface. */

		cvui::text(frame, 270, 30, "Colors");						/* Interface para mudanca no numero de tons a serem utilizados na operacao de quantizacao. */
		cvui::counter(frame, 245, 45, &colors, PULO_CORES);

		cvui::text(frame, 265, 120, "Contrast");					/* Interface para mudanca do valor de ganho a ser utilizado na operacao de mudanca de contraste. */
		cvui::counter(frame, 245, 135, &contrast, PULO_CONTRASTE);

		cvui::text(frame, 260, 210, "Brightness");					/* Interface para mudanca do valor de bias a ser utilizado na operacao de mudanca de brilho. */
		cvui::counter(frame, 245, 225, &brightness, PULO_BRILHO);

		cvui::text(frame, 238, 335, "Horizontal Factor");			/* Interface para mudanca do fator horizontal da operacao de zoom out. */
		cvui::counter(frame, 245, 355, &hor_factor, PULO_ZOOM);

		cvui::text(frame, 247, 385, "Vertical Factor");				/* Interface para mudanca do fator vertical da operacao de zoom out. */
		cvui::counter(frame, 245, 405, &ver_factor, PULO_ZOOM);

		cvui::text(frame, 140, 480, "Convolution Kernel");			/* Interface para mudanca dos pesos do kernel de convolucao. */
		cvui::counter(frame, 55, 500, &kernel[0], PULO_PESOKERNEL);
		cvui::counter(frame, 155, 500, &kernel[1], PULO_PESOKERNEL);
		cvui::counter(frame, 255, 500, &kernel[2], PULO_PESOKERNEL);
		cvui::counter(frame, 55, 530, &kernel[3], PULO_PESOKERNEL);
		cvui::counter(frame, 155, 530, &kernel[4], PULO_PESOKERNEL);
		cvui::counter(frame, 255, 530, &kernel[5], PULO_PESOKERNEL);
		cvui::counter(frame, 55, 560, &kernel[6], PULO_PESOKERNEL);
		cvui::counter(frame, 155, 560, &kernel[7], PULO_PESOKERNEL);
		cvui::counter(frame, 255, 560, &kernel[8], PULO_PESOKERNEL);

		if (cvui::button(frame, 40, 30, LARGURABOTAO, ALTURABOTAO, "Copy")) /* Realiza a copia da imagem original. */
		{
			fecha_janelas(); /* Fecha as janelas relacionadas com a equalizacao da imagem, se necessario. */
			copia_imagem(inImg, outImg);
			cv::imshow(NOMEJANELAORIGINAL, inImg);
			cv::imshow(NOMEJANELANOVA, outImg);
		}

		if (cvui::button(frame, 40, 75, LARGURABOTAO, ALTURABOTAO, "Luminance")) /* Aplica o efeito de luminancia. */
		{
			luminancia(outImg, canais);
			cv::imshow(NOMEJANELANOVA, outImg); /* Exibe a imagem na sua tela correspondente, apos as modificacoes. */
		}

		if (cvui::button(frame, 40, 115, LARGURABOTAO, ALTURABOTAO, "Horizontal Flip")) /* Aplica espelhamento horizontal. */
			flip_horizontal(outImg);

		if (cvui::button(frame, 40, 155, LARGURABOTAO, ALTURABOTAO, "Vertical Flip")) /* Aplica espelhamento vertical. */
			flip_vertical(outImg);

		if (cvui::button(frame, 40, 195, LARGURABOTAO, ALTURABOTAO, "Rotate +90")) /* Rotaciona a imagem em 90 graus. */
			rotaciona_antihorario(outImg);

		if (cvui::button(frame, 40, 235, LARGURABOTAO, ALTURABOTAO, "Rotate -90")) /* Rotaciona a imagem em -90 graus. */
			rotaciona_horario(outImg);

		if (cvui::button(frame, 40, 275, LARGURABOTAO, ALTURABOTAO, "Negative")) /* Calcula o negativo da imagem. */
			aplica_negativo(outImg);

		if (cvui::button(frame, 40, 315, LARGURABOTAO, ALTURABOTAO, "Show Histogram")) /* Mostra o histograma da imagem. */
			histograma(outImg);

		if (cvui::button(frame, 40, 355, LARGURABOTAO, ALTURABOTAO, "Equalize Histogram")) /* Faz a equalizacao do histograma da imagem de saida. */
			equaliza_histograma(outImg);

		if (cvui::button(frame, 40, 395, LARGURABOTAO, ALTURABOTAO, "Histogram Matching")) /* Faz o matching entre o histograma da imagem e outra imagem. */
			histogram_matching(outImg);

		if (cvui::button(frame, 40, 435, LARGURABOTAO, ALTURABOTAO, "Save")) /* Salva a imagem de saida. */
			salva_imagem(outImg);

		if (cvui::button(frame, 215, 80, LARGURABOTAO, ALTURABOTAO, "Quantize")) /* Aplica quantizacao. */
			quantizacao(inImg, outImg, colors);

		if (cvui::button(frame, 215, 170, LARGURABOTAO, ALTURABOTAO, "Change Constrast")) /* Altera o contraste da imagem de saida. */
			muda_contraste(outImg, contrast);

		if (cvui::button(frame, 215, 260, LARGURABOTAO, ALTURABOTAO, "Change Brightness")) /* Altera o brilho da imagem de saida. */
			muda_brilho(outImg, brightness);

		if (cvui::button(frame, 215, 300, LARGURABOTAO, ALTURABOTAO, "Zoom In 2x2")) /* Faz o zoom in 2x2 da imagem. */
			zoomIn(outImg);

		if (cvui::button(frame, 215, 435, LARGURABOTAO, ALTURABOTAO, "Zoom Out")) /* Faz o zoom out da imagem. */
			zoomOut(outImg, hor_factor, ver_factor);

		if (cvui::button(frame, 40, 595, LARGURABOTAO + 175, ALTURABOTAO, "Convolve")) /* Realiza a convolucao da imagem, a partir do kernel escolhido pelo usuario. */
			convolucao(outImg, kernel, clampping);

		if (cvui::button(frame, 40, 635, LARGURABOTAO, ALTURABOTAO, "Gaussian")) /* Inicializa o kernel com o filtro Gaussiano. */
			inicializa_kernel(kernel, 1);

		if (cvui::button(frame, 40, 675, LARGURABOTAO, ALTURABOTAO, "High Pass")) /* Inicializa o kernel com o filtro passa alta. */
			inicializa_kernel(kernel, 2);

		if (cvui::button(frame, 40, 715, LARGURABOTAO, ALTURABOTAO, "Sobel HX")) /* Inicializa o kernel com o filtro Sobel HX. */
			inicializa_kernel(kernel, 3);

		if (cvui::button(frame, 40, 755, LARGURABOTAO, ALTURABOTAO, "Sobel HY")) /* Inicializa o kernel com o filtro Sobel HY. */
			inicializa_kernel(kernel, 4);

		if (cvui::button(frame, 215, 635, LARGURABOTAO, ALTURABOTAO, "Laplacian")) /* Inicializa o kernel com o filtro Laplaciano. */
			inicializa_kernel(kernel, 5);

		if (cvui::button(frame, 215, 675, LARGURABOTAO, ALTURABOTAO, "Prewitt HX")) /* Inicializa o kernel com o filtro Prewitt HX. */
			inicializa_kernel(kernel, 6);

		if (cvui::button(frame, 215, 715, LARGURABOTAO, ALTURABOTAO, "Prewitt HY")) /* Inicializa o kernel com o filtro Prewitt HU. */
			inicializa_kernel(kernel, 7);

		if (cvui::button(frame, 215, 755, LARGURABOTAO, ALTURABOTAO, "Clear Kernel")) /* Zera todos os valores dos pesos do kernel de convolucao. */
			inicializa_kernel(kernel, 8);

		cvui::checkbox(frame, 160, 795, "Clampping", &clampping); /* Decide se a operacao de convolucao envolvera clampping ou nao. */

		if (cvui::button(frame, 40, 820, LARGURABOTAO + 175, ALTURABOTAO, "Quit")) /* Encerra a execucao do programa. */
			break;

		cvui::update(NOMEINTERFACE);
		cvui::imshow(NOMEINTERFACE, frame);

		if (cv::waitKey(20) == ESC) /* Encerra a execucao do programa, caso o usuario pressione ESC. */
			break;
	}
}

void inicia_programa() /* Inicializa a execucao do programa, pedindo os dados iniciais ao usuario. */
{
	Mat inImg;
	string path;

	do
	{
		cout << "IMAGE PROCESSING FUNDAMENTALS - PRACTICAL WORK #1 - ";
		cout << "Andrei Pochmann Koenich" << endl;
		cout << "\nEnter the input image directory and name, including its extension:" << endl; /* Solicita o diretorio completo da imagem de entrada. */
		cin >> path;

		inImg = imread(path); /* Realiza a leitura da imagem de entrada, a partir do diretorio fornecido. */

		if (inImg.empty()) /* Controle dos casos em que nao e possivel abrir a imagem. */
		{
			system("cls");
			cout << "ERROR: Invalid directory." << endl;
			cout << "\nExample of a valid directory: \\Users\\Andrei\\Desktop\\Image.jpg" << endl;
			cout << "\nPress any key to type again." << endl;
			_getch() /* Aguarda o usuario pressionar alguma tecla. */;
			system("cls");
		}

	} while (inImg.empty());

	namedWindow(NOMEJANELAORIGINAL, WINDOW_AUTOSIZE);
	moveWindow(NOMEJANELAORIGINAL, X_ORIGINAL, Y_ORIGINAL); /* Posiciona a janela da imagem original em local adequado na tela. */

	namedWindow(NOMEJANELANOVA, WINDOW_AUTOSIZE);
	moveWindow(NOMEJANELANOVA, X_NOVA, Y_NOVA); /* Posiciona a janela da imagem nova em local adequado na tela. */

	ShowWindow(GetConsoleWindow(), SW_HIDE); /* Fecha o terminal. */

	Mat outImg(inImg.rows, inImg.cols, CV_8UC3, Scalar(0)); /* Declaracao da imagem de saida do programa. */

	cvui::imshow(NOMEJANELAORIGINAL, inImg);
	cvui::imshow(NOMEJANELANOVA, outImg);

	mostra_menu(inImg, outImg); /* Exibe a interface interativa ao usuario. */
}

int main()
{
	inicia_programa();
	return 0;
}



