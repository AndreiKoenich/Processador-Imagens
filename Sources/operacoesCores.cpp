#include "../Headers/operacoesCores.hpp"
#include "../Headers/constantes.hpp"
#include "../Headers/cvui.h"

using namespace cv;
using namespace std;

float tomCinza(float canais[]) /* Funcao para determinar qual sera o tom cinza de um pixel da imagem. */
{
	float l = (0.299 * canais[R]) + (0.587 * canais[G]) + (0.114 * canais[B]);
	return l;
}

void luminanciaAux(float canais[]) /* Funcao para atribuir o tom cinza aos canais RGB de um pixel. */
{
	float canal_cinza = tomCinza(canais);
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

			luminanciaAux(canais); /* Atribui tom cinza aos canais RGB de um pixel. */

			outImg.at<cv::Vec3b>(i, j)[R] = canais[R];
			outImg.at<cv::Vec3b>(i, j)[G] = canais[G];
			outImg.at<cv::Vec3b>(i, j)[B] = canais[B];
		}
}

float maiorTom(Mat outImg) /* Acha o maior tom de um pixel da imagem. */
{
	float maior = 0, aux;
	float canais[DIMENSAORGB];

	for (int i = 0; i < outImg.rows; i++) /* Iteracao para percorrer todos os pixels da imagem, aplicando luminancia. */
		for (int j = 0; j < outImg.cols; j++)
		{
			canais[R] = outImg.at<cv::Vec3b>(i, j)[R]; /* Obtem os canais RGB de cada pixel da imagem. */
			canais[G] = outImg.at<cv::Vec3b>(i, j)[G];
			canais[B] = outImg.at<cv::Vec3b>(i, j)[B];

			luminanciaAux(canais); /* Atribui tom cinza aos canais RGB de um pixel. */

			if (i == 0 && j == 0) /* Atribui um valor inicial para a variavel, a fim de iniciar comparacoes. */
				maior = canais[B];

			aux = canais[B];

			if (aux > maior) /* Analisa se o tom encontrado e maior do que o anterior. */
				maior = aux;
		}

	return maior;
}

float menorTom(Mat outImg) /* Acha menor tom da imagem. */
{
	float menor = 0, aux;
	float canais[DIMENSAORGB];

	for (int i = 0; i < outImg.rows; i++) /* Iteracao para percorrer todos os pixels da imagem, aplicando luminancia. */
		for (int j = 0; j < outImg.cols; j++)
		{
			canais[R] = outImg.at<cv::Vec3b>(i, j)[R]; /* Obtem os canais RGB de cada pixel da imagem. */
			canais[G] = outImg.at<cv::Vec3b>(i, j)[G];
			canais[B] = outImg.at<cv::Vec3b>(i, j)[B];

			luminanciaAux(canais); /* Atribui tom cinza aos canais RGB de um pixel. */

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
	float maior = maiorTom(inImg); /* Determina o maior tom de pixel presenta na imagem. */
	float menor = menorTom(inImg); /* Determina o menor tom de pixel presente na imagem. */
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

			luminanciaAux(canais); /* Atribui tom cinza aos canais RGB de um pixel. */
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

void controlaCanais(double canais[]) /* Verifica se os canais RGB de um pixel de uma imagem estao dentro da faixa permitida. */
{
	for (int i = 0; i < DIMENSAORGB; i++)
	{
		if (canais[i] <= 0) /* Verifica se o valor da intensidade do pixel ultrapassou o limite inferior. */
			canais[i] = 0;

		else if (canais[i] > 255) /* Verifica se o valor da intensidade do pixel ultrapassou o limite superior. */
			canais[i] = 255;
	}
}

void aplicaNegativo(Mat outImg) /* Funcao para calcular o negativo da imagem de saida. */
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

void mudaContraste(Mat outImg, double gain) /* Funcao para mudar o contraste de cada pixel da imagem de saida. */
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

			controlaCanais(canais);	/* Verifica se o resultado da operacao esta dentro da faixa aceitavel. */

			outImg.at<cv::Vec3b>(i, j)[R] = canais[R];
			outImg.at<cv::Vec3b>(i, j)[G] = canais[G];
			outImg.at<cv::Vec3b>(i, j)[B] = canais[B];
		}

	cv::imshow(NOMEJANELANOVA, outImg); /* Exibe a imagem na sua tela correspondente, apos as modificacoes. */
}

void mudaBrilho(Mat outImg, double brilho) /* Funcao para mudar o brilho de cada pixel da imagem de saida. */
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

			controlaCanais(canais);	/* Verifica se o resultado da operacao esta dentro da faixa aceitavel. */

			outImg.at<cv::Vec3b>(i, j)[R] = canais[R];
			outImg.at<cv::Vec3b>(i, j)[G] = canais[G];
			outImg.at<cv::Vec3b>(i, j)[B] = canais[B];
		}

	cv::imshow(NOMEJANELANOVA, outImg); /* Exibe a imagem na sua tela correspondente, apos as modificacoes. */
}
