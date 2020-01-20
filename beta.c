#include <stdio.h>
#include <stdlib.h>

#define NULL 0
#define HEADER_SIZE 54
#define FILE_HEADER_SIZE 14
#define INFO_HEADER_SIZE 40
#define BYTES_PER_PIXEL 3

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;

#pragma pack(push, 1); // permite criar structs sem o preenchimento do gcc
typedef struct BITMAPFILEHEADER{
    WORD bfType;
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffBits;

} BITMAPFILEHEADER;

typedef struct BITMAPINFOHEADER{
    DWORD biSize;
    DWORD biWidth;
    DWORD biHeight;
    WORD biPlanes;
    WORD biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    DWORD biXPelsPerMeter;
    DWORD biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
} BITMAPINFOHEADER;

typedef struct PIXEL{
    BYTE blue, green, red;
} PIXEL;

typedef struct IMAGE{
    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;
    PIXEL **pixels;
} IMAGE;
#pragma pack(pop); //retorna as instruções originais do gcc/memória

int getPadding(int width){
    int padding = 0;
    while((width + padding) % 4){
        padding++;
    }

    return padding;
}

int getSizePixel(){
    return sizeof(struct PIXEL);
}

BITMAPFILEHEADER loadFileHeader(FILE *file){
    BITMAPFILEHEADER fileHeader;
    fseek(file, 0, SEEK_SET);

    fread(&fileHeader.bfType, sizeof(WORD), 1, file);
    fread(&fileHeader.bfSize, sizeof(DWORD), 1, file);
    fread(&fileHeader.bfReserved1, sizeof(WORD), 1, file);
    fread(&fileHeader.bfReserved2, sizeof(WORD), 1, file);
    fread(&fileHeader.bfOffBits, sizeof(DWORD), 1, file);

    return fileHeader;
}

BITMAPINFOHEADER loadInfoHeader(FILE *file){
    BITMAPINFOHEADER infoHeader;
    fseek(file, 14, SEEK_SET);

    fread(&infoHeader.biSize, sizeof(DWORD), 1, file);
    fread(&infoHeader.biWidth, sizeof(DWORD), 1, file);
    fread(&infoHeader.biHeight, sizeof(DWORD), 1, file);
    fread(&infoHeader.biPlanes, sizeof(WORD), 1, file);
    fread(&infoHeader.biBitCount, sizeof(WORD), 1, file);
    fread(&infoHeader.biCompression, sizeof(DWORD), 1, file);
    fread(&infoHeader.biSizeImage, sizeof(DWORD), 1, file);
    fread(&infoHeader.biXPelsPerMeter, sizeof(DWORD), 1, file);
    fread(&infoHeader.biYPelsPerMeter, sizeof(DWORD), 1, file);
    fread(&infoHeader.biClrUsed, sizeof(DWORD), 1, file);
    fread(&infoHeader.biClrImportant, sizeof(DWORD), 1, file);

    return infoHeader;
}

IMAGE readImage(FILE *file){

    char tempPadding[4];
    int width, height, padding, sizePixel, i, j;
    PIXEL **pixels;
    IMAGE image;
    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;

    fileHeader = loadFileHeader(file);
    infoHeader = loadInfoHeader(file);

    image.fileHeader = fileHeader;
    image.infoHeader = infoHeader;

    width = image.infoHeader.biWidth;
    height = image.infoHeader.biWidth;
    //printf("Largura: %d\nAltura: %d\n", width, width);

    padding = getPadding(width);
    
    //printf("Espacamento: %d\n", padding);

    sizePixel = getSizePixel(); 

    //printf("Tamanho do Pixel: %d\n", sizePixel);

    //printf("%d", sizeof(struct PIXEL*));


    pixels = (struct PIXEL **)malloc(width * sizeof(struct PIXEL*));
    for(i = 0; i < width; i++){
        pixels[i] = (struct PIXEL *)malloc(width * sizeof(sizePixel));
    }

    if(pixels == NULL){
        printf("| AVISO | Memória insuficiente.");
        fclose(file);
        exit(1);
    }

    

    for(i = 0; i < width; i++){
        for(j=0; j < width; j++){
            fread(&pixels[i][j], BYTES_PER_PIXEL, 1, file);
        }
        if(padding != 0){
            fread(&tempPadding, padding, 1, file);
        }
    }

    image.pixels = pixels;

    fseek(file, 0, 0);

    return image;
}

int main(int argc, char *argv[]){

    FILE *file;
    char *filename = argv[1];
    IMAGE image = { NULL };
    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;
    int width, height;

    file = fopen(filename, "rb");

    if(file == NULL){
        printf("| AVISO | Erro ao abrir o arquivo.");
    }

    image = readImage(file);
    width = image.infoHeader.biWidth;
    height = image.infoHeader.biHeight;

    printf("IMAGE INFO:\n");
    printf("Width: %d\n", width);
    printf("Height: %d\n\n", height);

    int cont = 1;
    /*for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            printf("Pixel %d: bgr(%d, %d, %d)\n", cont, image.pixels[i][j].blue,
                image.pixels[i][j].green, image.pixels[i][j].red);
            cont++;
        }
    }*/

    rotate180Degrees(file, image.pixels, image.fileHeader, image.infoHeader, width, height);

    fclose(file);
}

void rotate90Degrees(FILE *file, PIXEL **pixels, BITMAPFILEHEADER fileHeader, BITMAPINFOHEADER infoHeader, int width, int height){

}

void rotate180Degrees(FILE *file, PIXEL **pixels, BITMAPFILEHEADER fileHeader, BITMAPINFOHEADER infoHeader, int width, int height){

    FILE *newFile;
    PIXEL **rotatedPixels;
    int sizePixel = getSizePixel();

    rotatedPixels = (struct PIXEL **)malloc(height * sizeof(struct PIXEL*));
    for(int i = 0; i < height; i++){
        rotatedPixels[i] = (struct PIXEL *)malloc(width * sizeof(sizePixel));
    }

    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            rotatedPixels[i][j] = pixels[width-1-i][width-1-j];
        }
    }

    /*int cont = 1;
    for(int i = 0; i < width; i++){
        for(int j = 0; j < width; j++){
            printf("Pixel %d: bgr(%d, %d, %d)\n", cont, rotatedPixels[i][j].blue,
                rotatedPixels[i][j].green, rotatedPixels[i][j].red);
            cont++;
        }
    }*/

    newFile = fopen("new-image-180-degrees.bmp", "wb");
    printf("%d", sizeof(fileHeader));
    
    fwrite(&fileHeader, FILE_HEADER_SIZE, 1, newFile);
    fwrite(&infoHeader, INFO_HEADER_SIZE, 1, newFile);
    
    int padding = getPadding(width);

    PIXEL padd = {0,0,0};

    for(int i = 0; i < height; i++){
        fwrite(rotatedPixels[i], width, 3, newFile);
        for(int j = 0; j < padding; j++){
            fputc(0, newFile);
        }

    }

    fclose(file);
}

