# L3 Informatique - Informatique graphique, traitement et analyse d'images
# Bases du traitement et de l'analyse d'images
#
# Pour lancer les compilations, tapez :
# make
# Pour effacer tous les fichiers objets et executables, tapez :
# make clean

# Ne pas modifier ce fichier, sauf si l'option -Wpedantic n'est pas acceptée.
# Dans ce cas, la remplacer par -pedantic

CC=gcc
CFLAGS=-Wall -Wextra -std=c11 -Wpedantic
LDLIBS=-lm
RM=rm -f

# Fichiers objets
OBJECTS=limace.o \
        erreurs.o \
        tai.o \
				comparison.o \
				rgb2gray.o \
				binarization.o \
				histogram.o \
				hist2im.o \
				otsu.o \
				hist2cumhist.o \
				applut.o \
				histspecif.o \
				inversion.o \
				erosion.o \

# Fichiers exécutables
EXE=comparison \
    rgb2gray \
		binarization \
		histogram \
		hist2im \
		otsu \
		hist2cumhist \
		applut \
		histspecif \
		inversion \
		erosion \

# Pour mettre à jour tous les exécutables
.PHONY: all
all: $(EXE)

# Dépendances non implicites des exécutables
OBJ=limace.o erreurs.o tai.o
comparison: $(OBJ)
rgb2gray: $(OBJ)
binarization: $(OBJ)
histogram: $(OBJ)
hist2im: $(OBJ)
otsu: $(OBJ)
hist2cumhist: $(OBJ)
applut: $(OBJ)
histspecif: $(OBJ)
inversion: $(OBJ)
erosion: $(OBJ)

# Dépendances non implicites des objets
HEAD=limace.h erreurs.h tai.h
tai.o: $(HEAD)
comparison.o: $(HEAD)
rgb2gray.o: $(HEAD)
binarization.o: $(HEAD)
histogram.o: $(HEAD)
hist2im.o: $(HEAD)
otsu.o: $(HEAD)
hist2cumhist.o: $(HEAD)
applut.o: $(HEAD)
histspecif.o: $(HEAD)
inversion.o: $(HEAD)
erosion.o: $(HEAD)

# Pour effacer les objets et les exécutables
clean:
	$(RM) $(OBJECTS) $(EXE)
