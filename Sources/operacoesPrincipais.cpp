#include "../Headers/operacoesPrincipais.hpp"
#include "../Headers/constantes.hpp"
#include "../Headers/cvui.h"

using namespace cv;
using namespace std;

void flipHorizontal(Mat outImg) /* Funcao para realizar o espelhamento horizontal da imagem. */
{
	Mat auxImg(outImg.rows, outImg.cols, CV_8UC3, Scalar(0));
	copiaImagem(outImg, auxImg);

	for (int i = 0; i < outImg.rows; i++) /* Realiza permutacoes entre pixels para espelhar a imagem. */
		for (int j = 0; j < outImg.cols; j++)
			outImg.at<Vec3b>(i, j) = auxImg.at<Vec3b>(i, outImg.cols - j - 1);

	cv::imshow(NOMEJANELANOVA, outImg); /* Exibe a imagem na sua tela correspondente, apos as modificacoes. */
}

void flipVertical(Mat outImg) /* Funcao para realizar o espelhamento vertical da imagem. */
{
	Mat auxImg(outImg.rows, outImg.cols, CV_8UC3, Scalar(0));
	copiaImagem(outImg, auxImg);

	for (int i = 0; i < outImg.rows; i++) /* Realiza permutacoes entre pixels para espelhar a imagem. */
		for (int j = 0; j < outImg.cols; j++)
			outImg.at<Vec3b>(i, j) = auxImg.at<Vec3b>(outImg.rows - i - 1, j);

	cv::imshow(NOMEJANELANOVA, outImg); /* Exibe a imagem na sua tela correspondente, apos as modificacoes. */
}

void salvaImagem(Mat outImg) /* Funcao para salvar a nova imagem em algum diretorio. */
{
	bool exportou;
	string name, pathOut, finalPath;

	cout << "\nEnter the output image name (no need for extension):" << endl; /* Pede o nome da imagem de saida ao usuario. */
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
}

void rotacionaHorario(Mat& outImg) /* Funcao para rotacionar a imagem de saida no sentido horario. */
{
	Mat auxImg(outImg.cols, outImg.rows, CV_8UC3, Scalar(0)); /* Declaracao de imagem auxiliar, com dimensoes invertidas. */

	for (int i = 0; i < outImg.rows; i++) /* Faz a permutacao dos pixels, rotacionando a imagem. */
		for (int j = 0; j < outImg.cols; j++)
			auxImg.at<Vec3b>(j, outImg.rows - i - 1) = outImg.at<Vec3b>(i, j);

	outImg = auxImg; /* Atribui o conteudo da imagem auxiliar para a imagem de saida. */

	cv::imshow(NOMEJANELANOVA, outImg); /* Exibe a imagem na sua tela correspondente, apos as modificacoes. */
}

void rotacionaAntihorario(Mat& outImg) /* Funcao para rotacionar a imagem de saida no sentido antihorario. */
{
	Mat auxImg(outImg.cols, outImg.rows, CV_8UC3, Scalar(0));

	for (int i = 0; i < outImg.rows; i++) /* Faz a permutacao dos pixels, rotacionando a imagem. */
		for (int j = 0; j < outImg.cols; j++)
			auxImg.at<Vec3b>(outImg.cols - j - 1, i) = outImg.at<Vec3b>(i, j);

	outImg = auxImg; /* Atribui o conteudo da imagem auxiliar para a imagem de saida. */
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

void copiaImagem(Mat inImg, Mat& outImg) /* Funcao para copiar uma imagem de entrada para uma imagem de saida. */
{
	Mat auxImg(inImg.rows, inImg.cols, CV_8UC3, Scalar(0));

	for (int i = 0; i < inImg.rows; i++) /* Copia os conteudos de cada pixel da imagem de entrada para a imagem de saida. */
		for (int j = 0; j < inImg.cols; j++)
			auxImg.at<Vec3b>(i, j) = inImg.at<Vec3b>(i, j);

	outImg = auxImg;
}
