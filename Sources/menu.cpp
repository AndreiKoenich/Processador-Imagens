#include "../Headers/menu.hpp"
#include "../Headers/kernel.hpp"
#include "../Headers/histograma.hpp"
#include "../Headers/operacoesCores.hpp"
#include "../Headers/operacoesPrincipais.hpp"
#include "../Headers/constantes.hpp"
#include "../Headers/cvui.h"

using namespace cv;
using namespace std;

void fechaJanelas() /* Funcao para fechar as janelas relacionadas com a equalizacao da imagem, se necessario. */
{
	if (cv::getWindowProperty(JANELA_ANTESEQUAL, WND_PROP_VISIBLE) == 1) /* Verifica se as janelas estao abertas. */
	{
		destroyWindow(JANELA_ANTESEQUAL);
		destroyWindow(JANELA_HISTORIGINAL);
		destroyWindow(JANELA_DEPOISEQUAL);
		destroyWindow(JANELA_HISTEQUALIZADO);
	}
}

void mostraMenu(Mat inImg, Mat outImg) /* Funcao para exibir a interface interativa ao usuario. */
{
	float canais[DIMENSAORGB];
	double kernel[DIMENSAOKERNEL] = { 0 };

	copiaImagem(inImg, outImg); /* Copia o conteudo da imagem de entrada para a imagem de saida, deixando-a pronta para sofrer operacoes. */

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

		if (cvui::button(frame, 40, 30, LARGURABOTAO, ALTURABOTAO, "Restore Image")) /* Realiza a copia da imagem original. */
		{
			fechaJanelas(); /* Fecha as janelas relacionadas com a equalizacao da imagem, se necessario. */
			copiaImagem(inImg, outImg);
			cv::imshow(NOMEJANELAORIGINAL, inImg);
			cv::imshow(NOMEJANELANOVA, outImg);
		}

		if (cvui::button(frame, 40, 75, LARGURABOTAO, ALTURABOTAO, "Grayscale")) /* Aplica o efeito de luminancia. */
		{
			luminancia(outImg, canais);
			cv::imshow(NOMEJANELANOVA, outImg); /* Exibe a imagem na sua tela correspondente, apos as modificacoes. */
		}

		if (cvui::button(frame, 40, 115, LARGURABOTAO, ALTURABOTAO, "Horizontal Flip")) /* Aplica espelhamento horizontal. */
			flipHorizontal(outImg);

		if (cvui::button(frame, 40, 155, LARGURABOTAO, ALTURABOTAO, "Vertical Flip")) /* Aplica espelhamento vertical. */
			flipVertical(outImg);

		if (cvui::button(frame, 40, 195, LARGURABOTAO, ALTURABOTAO, "Rotate +90")) /* Rotaciona a imagem em 90 graus. */
			rotacionaAntihorario(outImg);

		if (cvui::button(frame, 40, 235, LARGURABOTAO, ALTURABOTAO, "Rotate -90")) /* Rotaciona a imagem em -90 graus. */
			rotacionaHorario(outImg);

		if (cvui::button(frame, 40, 275, LARGURABOTAO, ALTURABOTAO, "Negative")) /* Calcula o negativo da imagem. */
			aplicaNegativo(outImg);

		if (cvui::button(frame, 40, 315, LARGURABOTAO, ALTURABOTAO, "Show Histogram")) /* Mostra o histograma da imagem. */
			histograma(outImg);

		if (cvui::button(frame, 40, 355, LARGURABOTAO, ALTURABOTAO, "Equalize Histogram")) /* Faz a equalizacao do histograma da imagem de saida. */
			equalizaHistograma(outImg);

		if (cvui::button(frame, 40, 395, LARGURABOTAO, ALTURABOTAO, "Histogram Matching")) /* Faz o matching entre o histograma da imagem e outra imagem. */
			histogramMatching(outImg);

		if (cvui::button(frame, 40, 435, LARGURABOTAO, ALTURABOTAO, "Save")) /* Salva a imagem de saida. */
			salvaImagem(outImg);

		if (cvui::button(frame, 215, 80, LARGURABOTAO, ALTURABOTAO, "Quantize")) /* Aplica quantizacao. */
			quantizacao(inImg, outImg, colors);

		if (cvui::button(frame, 215, 170, LARGURABOTAO, ALTURABOTAO, "Change Constrast")) /* Altera o contraste da imagem de saida. */
			mudaContraste(outImg, contrast);

		if (cvui::button(frame, 215, 260, LARGURABOTAO, ALTURABOTAO, "Change Brightness")) /* Altera o brilho da imagem de saida. */
			mudaBrilho(outImg, brightness);

		if (cvui::button(frame, 215, 300, LARGURABOTAO, ALTURABOTAO, "Zoom In 2x2")) /* Faz o zoom in 2x2 da imagem. */
			zoomIn(outImg);

		if (cvui::button(frame, 215, 435, LARGURABOTAO, ALTURABOTAO, "Zoom Out")) /* Faz o zoom out da imagem. */
			zoomOut(outImg, hor_factor, ver_factor);

		if (cvui::button(frame, 40, 595, LARGURABOTAO + 175, ALTURABOTAO, "Convolve")) /* Realiza a convolucao da imagem, a partir do kernel escolhido pelo usuario. */
			convolucao(outImg, kernel, clampping);

		if (cvui::button(frame, 40, 635, LARGURABOTAO, ALTURABOTAO, "Gaussian")) /* Inicializa o kernel com o filtro Gaussiano. */
			inicializaKernel(kernel, 1);

		if (cvui::button(frame, 40, 675, LARGURABOTAO, ALTURABOTAO, "High Pass")) /* Inicializa o kernel com o filtro passa alta. */
			inicializaKernel(kernel, 2);

		if (cvui::button(frame, 40, 715, LARGURABOTAO, ALTURABOTAO, "Sobel HX")) /* Inicializa o kernel com o filtro Sobel HX. */
			inicializaKernel(kernel, 3);

		if (cvui::button(frame, 40, 755, LARGURABOTAO, ALTURABOTAO, "Sobel HY")) /* Inicializa o kernel com o filtro Sobel HY. */
			inicializaKernel(kernel, 4);

		if (cvui::button(frame, 215, 635, LARGURABOTAO, ALTURABOTAO, "Laplacian")) /* Inicializa o kernel com o filtro Laplaciano. */
			inicializaKernel(kernel, 5);

		if (cvui::button(frame, 215, 675, LARGURABOTAO, ALTURABOTAO, "Prewitt HX")) /* Inicializa o kernel com o filtro Prewitt HX. */
			inicializaKernel(kernel, 6);

		if (cvui::button(frame, 215, 715, LARGURABOTAO, ALTURABOTAO, "Prewitt HY")) /* Inicializa o kernel com o filtro Prewitt HU. */
			inicializaKernel(kernel, 7);

		if (cvui::button(frame, 215, 755, LARGURABOTAO, ALTURABOTAO, "Clear Kernel")) /* Zera todos os valores dos pesos do kernel de convolucao. */
			inicializaKernel(kernel, 8);

		cvui::checkbox(frame, 160, 795, "Clampping", &clampping); /* Decide se a operacao de convolucao envolvera clampping ou nao. */

		if (cvui::button(frame, 40, 820, LARGURABOTAO + 175, ALTURABOTAO, "Quit")) /* Encerra a execucao do programa. */
			break;

		cvui::update(NOMEINTERFACE);
		cvui::imshow(NOMEINTERFACE, frame);

		if (cv::waitKey(20) == ESC) /* Encerra a execucao do programa, caso o usuario pressione ESC. */
			break;
	}
}
