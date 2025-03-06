#include "../Headers/kernel.hpp"
#include "../Headers/operacoesCores.hpp"
#include "../Headers/constantes.hpp"
#include "../Headers/cvui.h"

using namespace cv;
using namespace std;

int verificaGaussiano(double kernel[]) /* Verifica se o kernel passado como parametro e gaussiano. */
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

	if (!verificaGaussiano(kernel)) /* Aplica a operacao de luminancia na imagem, se o kernel recebido nao corresponde ao gaussiano. */
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

			controlaCanais(valor_pixel); /* Mantem os valores dos pixels entre 0 e 255. */
			auxImg.at<cv::Vec3b>(i, j)[R] = valor_pixel[R]; /* Atribui os canais RGB do pixel para a imagem de saida. */
			auxImg.at<cv::Vec3b>(i, j)[G] = valor_pixel[G];
			auxImg.at<cv::Vec3b>(i, j)[B] = valor_pixel[B];
		}

	outImg = auxImg; /* Atribui o conteudo da imagem auxiliar para a imagem de saida. */
	cv::imshow(NOMEJANELANOVA, outImg); /* Exibe a imagem na sua tela correspondente, apos as modificacoes. */
}

void inicializaKernel(double kernel[], int flag_kernel) /* Funcao para inicializar os valores do kernel de convolucao. */
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
