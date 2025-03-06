# FUNDAMENTOS DE PROCESSAMENTO DE IMAGENS

# Andrei Pochmann Koenich - Matrícula 00308680

# Makefile

# Variáveis de flags de compilação e de diretórios
CXXFLAGS := -g -Wall -Wno-unknown-pragmas $(shell pkg-config --cflags opencv4) -o
LDFLAGS := $(shell pkg-config --libs opencv4)

FILES := 	Sources/processadorImagens.cpp \
			Sources/menu.cpp \
			Sources/operacoesPrincipais.cpp \
			Sources/operacoesCores.cpp \
			Sources/histograma.cpp \
			Sources/kernel.cpp

# Comando para compilar e gerar o executável
link:
	g++ $(CXXFLAGS) processadorImagens $(FILES) $(LDFLAGS)

# Comando padrão do Makefile
.DEFAULT_GOAL := link
