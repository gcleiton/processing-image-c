#include <stdio.h>
#include <stdlib.h>

#define NULL 0
#define HEADER_SIZE 54
#define BYTES_PER_PIXEL 3

typedef unsigned char BYTE;

typedef struct PIXEL{
    BYTE blue, green, red;
} PIXEL;

typedef struct IMAGE{
    int width;
    int height;
    PIXEL **pixels;
} IMAGE;

int getPadding(int width){
    int padding = 0;
    while((width * 3 + padding) % 4){
        padding++;
    }

    return padding;
}

int getSizePixel(){
    return sizeof(struct PIXEL);
}

IMAGE readImage(char* filename){

    FILE *file;
    BYTE header[HEADER_SIZE];
    char tempPadding[4];
    int padding, sizePixel, i, j;

    IMAGE image = { NULL };

    file = fopen(filename, "rb");

    if(file == NULL){
        printf("| AVISO | Erro ao abrir a imagem.");
        exit(1);
    }

    fread(header, HEADER_SIZE, 1, file);
    
    image.width = (int) header[18];
    image.height = (int) header[22];


    padding = getPadding(image.width);
    sizePixel = getSizePixel();

    struct PIXEL **pixels;

    pixels = (struct PIXEL **)malloc(image.height * sizeof(struct PIXEL*));
    for(i = 0; i < image.height; i++){
        pixels[i] = (struct PIXEL *) malloc(image.width * sizeof(sizePixel));
    }

    if(pixels == NULL){
        printf("| AVISO | Memória insuficiente.");
        fclose(file);
        exit(1);
    }

    for(i = 0; i < image.height; i++){
        for(int j = 0; j < image.width; j++){
            fread(&pixels[i][j], BYTES_PER_PIXEL, 1, file);
        }
        if(padding != 0){
            fread(&tempPadding, padding, 1, file);
        }
    }

    image.pixels = pixels;

    fclose(file);
    
    return image;
}

int main(int argc, char *argv[]){

    IMAGE image = { NULL };
    char *filename = argv[1];

    image = readImage(filename);
    
    int cont = 1;
    for(int i = 0; i < image.height; i++){
        
        for(int j = 0; j < image.width; j++){
            printf("Pixel %d: (%d, %d, %d)\n", cont, image.pixels[i][j].blue, image.pixels[i][j].green, image.pixels[i][j].red );
            cont++;
        }
    }
    return 0;
}