#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <stdbool.h>
#include "limace.h"
#include "tai.h"

#define DEBOGAGE

#ifdef DEBOGAGE
#define DEBUG fprintf(stderr,"Fichier %s, ligne %d\n",__FILE__,__LINE__);
#else
#define DEBUG
#endif

#define AFAIRE(fonction) fprintf(stderr,"Corps de la fonction " #fonction \
                                        " à écrire.\n" ); \
												 return NULL;
#define AFAIRE2(fonction) fprintf(stderr,"Corps de la fonction " #fonction \
                                        " à écrire.\n" ); \
												 return 0;

/*
 * Conversion d'une image couleur en une image de niveaux de gris
 * Entrée : image initiale en couleur
 * Sortie : image de niveaux de gris résultat
 */
Image RGB2Gray(Image Im)
{
  const unsigned char** redcomp = (const unsigned char**) ImGetR(Im);
  const unsigned char** greencomp = (const unsigned char**) ImGetG(Im);
  const unsigned char** bluecomp = (const unsigned char**) ImGetB(Im);

  const int rowCount = (const int) ImNbRow(Im);
  const int columnCount = (const int) ImNbCol(Im);

  Image result = ImAlloc(GrayLevel, rowCount, columnCount);
  unsigned char** graycomp = ImGetI(result);

  for(int i = 0; i < rowCount; ++i) {
    for(int j = 0; j < columnCount; ++j) {
      graycomp[i][j] = round(
        0.299*redcomp[i][j]
        + 0.587*greencomp[i][j]
        + 0.114*bluecomp[i][j]
      );
    }
  }

  return result;
}


/*
 * Binarisation d'une image de niveaux de gris par seuillage global
 * Entrées : image de niveaux de gris initiale
             seuil (niveau de gris)
 * Sortie : image binaire
 */
Image Binarization(Image Im, unsigned char Threshold)
{
  const unsigned char** graycomp = (const unsigned char**) ImGetI(Im);

  const int rowCount = (const int) ImNbRow(Im);
  const int columnCount = (const int) ImNbCol(Im);

  Image result = ImAlloc(BitMap, rowCount, columnCount);
  unsigned char** resultGraycomp = ImGetI(result);

  for(int i = 0; i < rowCount ; ++i) {
    for(int j = 0; j < columnCount ; ++j) {
      resultGraycomp[i][j] = (graycomp[i][j] < Threshold) ? 0 : 1;
    }
  }

  return result;
}


/*
 * Inversion d'une image
 * Entrée : image initiale (binaire, niveaux de gris ou couleur)
 * Sortie : image résultat
 */
Image Inversion(Image Im)
{
  const ImageType imageType = (const ImageType) ImType(Im);

  const int rowCount = (const int) ImNbRow(Im);
  const int columnCount = (const int) ImNbCol(Im);

  Image result = ImAlloc(imageType, rowCount, columnCount);

  if(imageType == Color) {
    const unsigned char** redcomp = (const unsigned char**) ImGetR(Im);
    const unsigned char** greencomp = (const unsigned char**) ImGetG(Im);
    const unsigned char** bluecomp = (const unsigned char**) ImGetB(Im);

    unsigned char** resultRedcomp = ImGetR(result);
    unsigned char** resultGreencomp = ImGetG(result);
    unsigned char** resultBluecomp = ImGetB(result);
    
    for(int i = 0; i < rowCount; ++i) {
      for(int j = 0; j < columnCount; ++j) {
        resultRedcomp[i][j] = 255 - redcomp[i][j];
        resultGreencomp[i][j] = 255 - greencomp[i][j];
        resultBluecomp[i][j] = 255 - bluecomp[i][j];
      }
    }
  }
  else {
    const unsigned char** graycomp = (const unsigned char**) ImGetI(Im);
    unsigned char** resultGraycomp = ImGetI(result);
    
    if(imageType == GrayLevel) {
      for(int i = 0; i < rowCount; ++i) {
        for(int j = 0; j < columnCount; ++j) {
          resultGraycomp[i][j] = 255 - graycomp[i][j];
        }
      }
    }
    else if(imageType == BitMap) {
      for(int i = 0; i < rowCount; ++i) {
        for(int j = 0; j < columnCount; ++j) {
          resultGraycomp[i][j] = 1 - graycomp[i][j];
        }
      }
    }
  }

  return result;
}


/*
 * Calcul de l'histogramme d'une image de niveaux de gris
 * Entrée : image initiale (niveaux de gris)
 * Sortie : histogramme (matrice de int 1 x 256)
 */
Matrix Histogram(Image Im)
{
  const int rowCount = (const int) ImNbRow(Im);
  const int columnCount = (const int) ImNbCol(Im);

  const unsigned char** graycomp = (const unsigned char**) ImGetI(Im);

  Matrix result = MatCAlloc(Int, 1, 256);

  int** resultValues = MatGetInt(result);

  for(int i = 0; i < rowCount; ++i) {
    for(int j = 0; j < columnCount; ++j) {
      resultValues[0][graycomp[i][j]] += 1;
    }
  }

  return result;
}


/*
 * Représentation d'un histogramme sous forme d'une image
 * Entrées : histogramme (matrice de int 1 x 256) et nombre de lignes de
 * l'image résultat (une échelle des niveaux de gris de 25 lignes est ajoutée
 * sous l'histogramme)
 * Sortie : image de niveaux de gris résultat
 */
Image Hist2Im(Matrix Hist, int NbLig)
{
	unsigned char **I;
	int *h,i,j,Max=0,NbCol=256,NbLig2=NbLig+25;
	Image Res;

	if (MatType(Hist)!=Int) return NULL;
  NbLig2=NbLig+25;
	Res=ImCAlloc(GrayLevel,NbLig2,NbCol);
  if (Res==NULL) return NULL;
	h=*MatGetInt(Hist);
	for (j=0;j<NbCol;j++)
		if (h[j]>Max) Max=h[j];
	I=ImGetI(Res);
	for (j=0;j<256;j++)
		for (i=NbLig-1;i>=(NbLig-NbLig*h[j]/Max);i--)
		    I[i][j]=255;
  for (j=0;j<256;j++)
    I[NbLig][j]=0;
  for (i=NbLig+1;i<NbLig2;i++)
    for (j=0;j<256;j++)
      I[i][j]=j;
	return Res;
}

double cost(Matrix H, unsigned char i) {
  const int** Hvalues = (const int**) MatGetInt(H);

  //

  double q1 = 0;
  double q2 = 0;
  
  double u1 = 0;
  double u2 = 0;

  double r1 = 0;
  double r2 = 0;

  // 

  for(int k = 0; k < i; ++k) {
    const int h = Hvalues[0][k];
    q1 += h;
    u1 += k*h;
  }
  if(q1 == 0) q1 = HUGE_VAL;
  u1 = u1 / q1;

  for(int k = i; k < 255+1; ++k) {
    const int h = Hvalues[0][k];
    q2 += h;
    u2 += k*h;
  }
  if(q2 == 0) q2 = HUGE_VAL;
  u2 = u2 / q2;

  // 

  for(int k = 0; k < i; ++k) {
    const int h = Hvalues[0][k];
    const double kdist2u1 = k - u1;
    r1 += h*kdist2u1*kdist2u1;
  }

  for(int k = i; k < 255+1; ++k) {
    const int h = Hvalues[0][k];
    const double kdist2u2 = k - u2;
    r2 += h*kdist2u2*kdist2u2;
  }

  // 

  return r1 + r2;
}

/*
 * Calcul du seuil d'Otsu
 * Entrée : histogramme (matrice de int 1 x 256)
 * Sortie : seuil (niveau de gris) obtenu par la méthode d'Otsu
 */
unsigned char Otsu(Matrix Hist)
{
  unsigned char threshold = 1;
  double min = cost(Hist,1);

  for(int i = 2; i < 254+1; ++i) {
    const double c = cost(Hist,i);

    if(c < min) {
      min = c;
      threshold = i;
    }
  }

  return threshold;
}


/*
 * Calcul de l'histogramme cumulé à partir de l'histogramme
 * Entrée : histogramme (matrice de int 1 x 256)
 * Sortie : histogramme cumulé (matrice de int 1 x 256)
 */
Matrix Hist2CumHist(Matrix Hist)
{
  const int rowCount = (const int) MatNbRow(Hist);
  const int columnCount = (const int) MatNbCol(Hist);

  Matrix result = MatCAlloc(Int, rowCount, columnCount);

  const int** Hvalues = (const int**) MatGetInt(Hist);
  int ** resultValues = MatGetInt(result);

  resultValues[0][0] = Hvalues[0][0];

  for(int i = 0; i < rowCount; ++i) {
    for(int j = 1; j < columnCount; ++j) {
      resultValues[i][j] = Hvalues[i][j] + resultValues[i][j-1];
    }
  }

  return result;
}


/*
 * Application d'une transformation ponctuelle à une image de niveaux de gris
 * Entreés : image initiale (niveaux de gris) et
 * transformation ponctuelle (matrice de int 1 x 256)
 * Sortie : image de niveaux de gris transformée
 */
Image AppLUT(Image Im, Matrix LUT)
{
  const ImageType imageType = (const ImageType) ImType(Im);

  const int rowCount = (const int) ImNbRow(Im);
  const int columnCount = (const int) ImNbCol(Im);

  Image result = ImAlloc(imageType, rowCount, columnCount);

  const unsigned char** originalValues = (const unsigned char**) ImGetI(Im);
  const int** lutValues = (const int**) MatGetInt(LUT);

  unsigned char** resultValues = ImGetI(result);

  for(int i = 0; i < rowCount; ++i) {
    for(int j = 0; j < columnCount; ++j) {
      resultValues[i][j] = lutValues[0][originalValues[i][j]];
    }
  }

  return result;
}


/*
 * Spécification d'histogramme
 * Entrées : histogramme cumulé de l'image et histogramme cumulé desiré
 * (on suppose que le dernier élément des deux histogrammes cumulés sont
 * les mêmes, c'est-à-dire qu'ils décrivent des images contenant le même nombre
 * de pixels)
 * Sortie : transformation ponctuelle (matrice 1 x 256)
 */
Matrix HistSpecif(Matrix CumHist, Matrix DesCumHist)
{
  const int rowCount = (const int) MatNbRow(CumHist);
  const int columnCount = (const int) MatNbCol(CumHist);

  Matrix result = MatCAlloc(Int, rowCount, columnCount);

  const int** HCvalues = (const int**) MatGetInt(CumHist);
  const int** DHCvalues = (const int**) MatGetInt(DesCumHist);
  int** resultValues = MatGetInt(result);

  for(int i = 0; i < rowCount; ++i) {
    for(int j = 0; j < columnCount; ++j) {
      const int c = HCvalues[i][j];

      int nearestk = -1;
      int nearestDist = INT_MAX;
      for(int k = 0; k < columnCount; ++k) {
        const int dist = abs(c - DHCvalues[i][k]);

        if(dist < nearestDist) {
          nearestk = k;
          nearestDist = dist;
        }
      }

      resultValues[i][j] = nearestk;
    }
  }

  return result;
}


/*
 * Vérification de la validité d'une matrice représentant un élément
 * structurant binaire (pour l'érosion, la dilatation, etc.)
 * Entrée : matrice représentant un élément structurant
 * Sortie : 0 si la matrice est valide,
            SE_NOT_ODD si son nombre de lignes ou de colonnes n'est pas impair
            SE_NOT_INT si elle ne contient pas que des entiers
            SE_NOT_BIN si elle ne contient pas que des 0 et des 1
*/
int NotValidBinSE(Matrix StructuringElement)
{
  int **ES,NbLig,NbCol,i,j;

  if (MatType(StructuringElement)!=Int)
    return SE_NOT_INT;
  NbLig=MatNbRow(StructuringElement);
	if ((NbLig%2)!=1)
	  return SE_NOT_ODD;
  NbCol=MatNbCol(StructuringElement);
	if ((NbCol%2)!=1)
	  return SE_NOT_ODD;
  ES=MatGetInt(StructuringElement);
  for (i=0;i<NbLig;i++)
    for (j=0;j<NbCol;j++)
      if (ES[i][j]!=0 && ES[i][j]!=1)
        return SE_NOT_BIN;
  return 0;
}


/*
 * Vérification de la validité d'une matrice représentant un élément
 * structurant ternaire (pour la transformation "tout ou rien")
 * Entrée : matrice représentant un élément structurant
 * Sortie : 0 si la matrice est valide,
            SE_NOT_ODD si son nombre de lignes ou de colonnes n'est pas impair
            SE_NOT_INT si elle ne contient pas que des entiers
            SE_NOT_TERN si elle ne contient pas que des 0, des 1 et des 2
*/
int NotValidTernSE(Matrix StructuringElement)
{
  int **ES,NbLig,NbCol,i,j;

  if (MatType(StructuringElement)!=Int)
    return SE_NOT_INT;
  NbLig=MatNbRow(StructuringElement);
	if ((NbLig%2)!=1)
	  return SE_NOT_ODD;
  NbCol=MatNbCol(StructuringElement);
	if ((NbCol%2)!=1)
	  return SE_NOT_ODD;
  ES=MatGetInt(StructuringElement);
  for (i=0;i<NbLig;i++)
    for (j=0;j<NbCol;j++)
      if (ES[i][j]!=0 && ES[i][j]!=1 && ES[i][j]!=2)
        return SE_NOT_TERN;
  return 0;
}

// 

typedef bool (*MatchingOperator)(int imgV, int seV);

Image applyImageOperator(
  Image Im,
  Matrix StructuringElement,
  MatchingOperator matchingOperator,
  const int defaultOutValue
) {
  const ImageType imageType = (const ImageType) ImType(Im);

  const int imageRowCount = (const int) ImNbRow(Im);
  const int imageColumnCount = (const int) ImNbCol(Im);

  const int seRowCount = (const int) MatNbRow(StructuringElement);
  const int seColumnCount = (const int) MatNbCol(StructuringElement);

  const int seCenterRowIndex = seRowCount / 2;
  const int seCenterColumnIndex = seColumnCount / 2;

  Image result = ImAlloc(imageType, imageRowCount, imageColumnCount);

  const unsigned char** imageValues = (const unsigned char**) ImGetI(Im);
  const int** seValues = (const int**) MatGetInt(StructuringElement);
  unsigned char** resultValues = ImGetI(result);

  for(int i = 0; i < imageRowCount; ++i) {
    for(int j = 0; j < imageColumnCount; ++j) {
      bool seMatch = true;

      const int compStartI = i - seCenterRowIndex;
      const int compStartJ = j - seCenterColumnIndex;

      for(int sei = 0; sei < seRowCount && seMatch; ++sei) {
        for(int sej = 0; sej < seColumnCount && seMatch; ++sej) {
          const int compI = compStartI + sei;
          const int compJ = compStartJ + sej;

          int imgV;
          if(
            (0 <= compI && compI < imageRowCount)
            && (0 <= compJ && compJ < imageColumnCount)
          ) imgV = imageValues[compI][compJ];
          else imgV = defaultOutValue;

          const int seV = seValues[sei][sej];
          
          if(matchingOperator(imgV, seV)) seMatch = false;
        }
      }

      //

      resultValues[i][j] = seMatch;
    }
  }

  return result;
}

// 

bool erosionMatchingOperator(int imgV, int seV) {
  return seV && !imgV;
}

/*
 * Érosion d'une image binaire
 * Entrées : image binaire initiale et élément structurant (matrice de int
 * contenant uniquement des 0 et des 1)
 * Sortie : image binaire erodée
 */
Image Erosion(Image Im, Matrix StructuringElement)
{
  return applyImageOperator(Im, StructuringElement, erosionMatchingOperator, 0);
}

