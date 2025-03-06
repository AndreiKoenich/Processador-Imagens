#include "../Headers/histograma.hpp"
#include "../Headers/kernel.hpp"
#include "../Headers/operacoesCores.hpp"
#include "../Headers/operacoesPrincipais.hpp"
#include "../Headers/constantes.hpp"
#include "../Headers/cvui.h"

using namespace cv;
using namespace std;

void histograma(Mat outImg) /* Funcao para imprimir o histograma da imagem manipulada. */
{
	Mat auxImg(outImg.rows, outImg.cols, CV_8UC3, Scalar(0));

	copiaImagem(outImg, auxImg);

	float canais[DIMENSAORGB];
	luminancia(auxImg, canais); /* Aplica a operacao de luminancia na imagem. */

	double intensidades[INTENSIDADESIMAGEM] = { 0 };

	Mat janela_histograma(LARGURAJANELAHISTOGRAMA, ALTURAJANELAHISTOGRAMA, CV_8UC3, Scalar(255, 255, 255));

	calculaHistograma(auxImg, intensidades); /* Funcao para calcular os valores do vetor do histograma da imagem. */
	normalizaHistograma(intensidades); /* Normaliza todos os valores do histograma, antes da impressao. */
	montaHistograma(janela_histograma, intensidades); /* Funcao para desenhar o histograma na janela correspondente. */

	imshow(NOMEHISTOGRAMA, janela_histograma); /* Exibe a janela contendo o histograma. */
}

void equalizaAux(Mat outImg, double hist_original[], double hist_cumulativo[]) /* Funcao para aplicar a equalizacao em um histograma especifico. */
{
	calculaHistograma(outImg, hist_original);

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

void equalizaHistograma(Mat outImg) /* Funcao para equalizar e exibir a imagem de saida. */
{
	float canais[DIMENSAORGB] = { 0 };
	double hist_original[INTENSIDADESIMAGEM] = { 0 };
	double hist_cumulativo[INTENSIDADESIMAGEM] = { 0 };
	double hist_novo[INTENSIDADESIMAGEM] = { 0 };

	Mat auxImg(outImg.rows, outImg.cols, CV_8UC3, Scalar(0));
	copiaImagem(outImg, auxImg);

	luminancia(auxImg, canais);
	luminancia(outImg, canais);

	equalizaAux(outImg, hist_original, hist_cumulativo);
	normalizaHistograma(hist_original); /* Realiza a normalizacao do histograma apos a equalizacao. */

	destroyWindow(NOMEJANELAORIGINAL);	/* Fecha a janela comum contendo a imagem original. */
	destroyWindow(NOMEJANELANOVA);		/* Fecha a janela comum contendo a imagem modificada antes da equalizacao. */

	if (cv::getWindowProperty("HISTOGRAM", WND_PROP_VISIBLE) == 1) /* Fecha a janela com o histograma comum, se necessario. */
		destroyWindow("HISTOGRAM");

	Mat janelahistograma_original(LARGURAJANELAHISTOGRAMA, ALTURAJANELAHISTOGRAMA, CV_8UC3, Scalar(255, 255, 255));
	montaHistograma(janelahistograma_original, hist_original); /* Funcao para desenhar o histograma original na janela correspondente. */

	imshow(JANELA_ANTESEQUAL, auxImg); /* Imprime a imagem original e o seu histograma abaixo. */
	imshow(JANELA_HISTORIGINAL, janelahistograma_original);
	moveWindow(JANELA_ANTESEQUAL, X_NAOEQUALIZADO, Y_NAOEQUALIZADO);
	moveWindow(JANELA_HISTORIGINAL, X_NAOEQUALIZADO, Y_NAOEQUALIZADO + 420);

	Mat janelahistograma_novo(LARGURAJANELAHISTOGRAMA, ALTURAJANELAHISTOGRAMA, CV_8UC3, Scalar(255, 255, 255));
	calculaHistograma(outImg, hist_novo); /* Calcula os valores do vetor do novo histograma da imagem, ja equalizada. */
	normalizaHistograma(hist_novo); /* Realiza a normalizacao do histograma apos a equalizacao. */
	montaHistograma(janelahistograma_novo, hist_novo); /* Funcao para desenhar o histograma equalizado na janela correspondente. */

	imshow(JANELA_DEPOISEQUAL, outImg); /* Imprime a imagem equalizada e o seu histograma abaixo. */
	imshow(JANELA_HISTEQUALIZADO, janelahistograma_novo);
	moveWindow(JANELA_DEPOISEQUAL, X_EQUALIZADO, Y_EQUALIZADO);
	moveWindow(JANELA_HISTEQUALIZADO, X_EQUALIZADO, Y_EQUALIZADO + 420);
}

int mapeiaHistogramas(double intensidade, double histcumulativo_match[]) /* Funcao para fazer o match entre dois histogramas. */
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

void histogramMatching(Mat outImg) /* Funcao para realizar histogram matching entre duas imagens. */
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

	cout << "Enter the image whose histogram will be used for matching. " << endl;
	cout << "Type the image directory and name, including its extension:" << endl; /* Solicita o diretorio completo da imagem para o matching de histograma. */
	cin >> path;

	matchImg = imread(path); /* Realiza a leitura da imagem de entrada, a partir do diretorio fornecido. */

	if (matchImg.empty()) /* Controle dos casos em que nao e possivel abrir a imagem. */
	{
		cout << "ERROR: Invalid directory." << endl;
		cout << "\nExample of a valid directory: /home/ubuntu/photo.jpg" << endl;
		return;
	}

	luminancia(outImg, canais); /* Aplica luminancia na imagem original. */
	luminancia(matchImg, canais); /* Aplica luminancia na imagem de match. */

	calculaHistograma(outImg, hist_original); /* Calcula o histograma da imagem original. */
	equalizaAux(outImg, hist_original, hist_cumulativo); /* Equaliza o histograma original e calcula o histograma cumulativo. */
	normalizaHistograma(hist_cumulativo); /* Normaliza o histograma cumulativo da imagem original. */

	calculaHistograma(matchImg, hist_originalmatch); /* Calcula o histograma da imagem de match. */
	equalizaAux(matchImg, hist_originalmatch, hist_cumulativomatch); /* Equaliza o histograma original e calcula o histograma cumulativo. */
	normalizaHistograma(hist_cumulativomatch); /* Normaliza o histograma cumulativo da imagem de match. */

	for (int i = 0; i < INTENSIDADESIMAGEM; i++)
		hist_matched[i] = hist_cumulativo[mapeiaHistogramas(hist_cumulativo[i], hist_cumulativomatch)];

	for (int i = 0; i < outImg.rows; i++) /* Iteracoes para obter a nova imagem de saida, a partir do histograma novo. */
		for (int j = 0; j < outImg.cols; j++)
		{
			outImg.at<cv::Vec3b>(i, j)[R] = hist_matched[outImg.at<cv::Vec3b>(i, j)[R]];
			outImg.at<cv::Vec3b>(i, j)[G] = hist_matched[outImg.at<cv::Vec3b>(i, j)[G]];
			outImg.at<cv::Vec3b>(i, j)[B] = hist_matched[outImg.at<cv::Vec3b>(i, j)[B]];
		}

	cv::imshow(NOMEJANELANOVA, outImg); /* Exibe a imagem na sua tela correspondente, apos as modificacoes. */
}

void normalizaHistograma(double histograma[]) /* Funcao para normalizar todos os valores do histograma. */
{
	double max = 0;

	for (int i = 0; i < INTENSIDADESIMAGEM; i++) /* Acha o maior valor do histograma. */
		if (histograma[i] > max)
			max = histograma[i];

	for (int i = 0; i < INTENSIDADESIMAGEM; i++) /* Iteracao para normalizar os valores do histograma. */
		histograma[i] = (histograma[i] / max) * (NORMALIZACAOHISTOGRAMA);
}

void calculaHistograma(Mat outImg, double histograma[]) /* Funcao para calcular os valores do vetor do histograma da imagem. */
{
	int indice;

	for (int i = 0; i < outImg.rows; i++) /* Calcula a frequencia de cada tom da imagem. */
		for (int j = 0; j < outImg.cols; j++)
		{
			indice = (int)outImg.at<cv::Vec3b>(i, j)[0];
			histograma[indice]++; /* Incrementa a frequencia de um determinado tom da imagem. */
		}
}

void montaHistograma(Mat janela, double intensidades[]) /* Funcao para desenhar o histograma na janela correspondente. */
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