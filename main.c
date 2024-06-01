#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

// ./bmp --input test2 --output result --max_iter 10

typedef struct string string;
#pragma pack(push)
#pragma pack(1)
struct bmp_image {
    unsigned char tag_1;
    unsigned char tag_2;
    int size;
    short reserve_1;
    short reserve_2;
    int bit_offset;
    unsigned int struct_size;
    int width;
    int height;
    short planes;
    short bit_per_pixel;
    unsigned int compression_type;
    unsigned int byte_size;
    unsigned int x_per_meter;
    unsigned int y_per_meter;
    unsigned int general_color_size;
    unsigned int color_size;
    int color_table[2];
} bmp_image;



unsigned char **readBMP(FILE *file, unsigned char **image) {
    fseek(file, bmp_image.bit_offset, SEEK_SET);
    for (int i = 0; i < bmp_image.height; i++) {
        for (int j = 0; j < bmp_image.width + ((3 * bmp_image.width) % 4); j++) {
            fread(&image[i][j], sizeof(image[i][j]), 1, file);
        }
    }

    fclose(file);
    return image;
}


int main(int argc, char *argv[]) {
    char *input = NULL;
    char *outputs = NULL;
    int max_iter = 9999;
    int dump_freq = 1;
    if (argc > 1) {
        for (int i = 0; i < argc; i++)
            if (strcmp(argv[i], "--input") == 0) {
                input = argv[i + 1];
            } else if (strcmp(argv[i], "--output") == 0) {
                outputs = argv[i + 1];
            } else if (strcmp(argv[i], "--max_iter") == 0) {
                max_iter = atoi(argv[i + 1]);
            } else if (strcmp(argv[i], "--dump_freq") == 0) {
                dump_freq = atoi(argv[i + 1]);
            }
    }
    if (input == NULL || outputs == NULL) {
        printf("Error: wrong input");
        return 0;
    }
    FILE *file;
    char* input2;
    input2 = strcat(input, ".bmp");
    file = fopen(input2, "rb");
    fread(&bmp_image, sizeof(bmp_image), 1, file);
    unsigned char **image = (unsigned char **) malloc(bmp_image.height * sizeof(unsigned char *));
    for (int i = 0; i < bmp_image.height; i++) {
        image[i] = (unsigned char *) malloc((bmp_image.width + (((3 * bmp_image.width) % 4))) * sizeof(unsigned char));
    }

    if (bmp_image.bit_per_pixel == 8) {
        image = readBMP(file, image);
    } else {
        printf("%d Not a 8-bit BMP file", bmp_image.bit_per_pixel);
        return 0;
    }
    for (int i = 0; i < bmp_image.height; i++) {
        for (int j = 0; j < bmp_image.width; j++) {
            printf("%d ", image[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    unsigned char **temp = (unsigned char **) malloc(bmp_image.height * sizeof(unsigned char *));
    for (int i = 0; i < bmp_image.height; i++) {
        temp[i] = (unsigned char *) malloc((bmp_image.width + ((3 * bmp_image.width) % 4)) * sizeof(unsigned char));
    }
    char * filename;
    filename = "output";
    mkdir(outputs, 0777);
    for (int iter=0; iter < max_iter; iter++) {
        int flag= 0;
        char *output = (char *) malloc(100 * sizeof(char));
        strcpy(output,outputs);
        char *iter_char;
        iter_char = (char *) malloc(10 * sizeof(char));
        sprintf(iter_char, "%d", iter);
        output = strcat(output, "/");
        output = strcat(output, filename);
        output = strcat(output, "_");
        output = strcat(output, iter_char);
        output = strcat(output, ".bmp");
        memcpy(*temp, *image, bmp_image.height * (bmp_image.width + ((3 * bmp_image.width) % 4)) * sizeof(unsigned char));
        for (int i = 0; i < bmp_image.height; i++) {
            for (int j = 0; j < bmp_image.width; j++) {
                int k = 0;
                if (temp[i][j] == 0) {
                    if ((i > 0 && j > 0) && (temp[i - 1][j - 1] == 1)) k++;
                    if ((i > 0) && (temp[i - 1][j] == 1)) k++;
                    if ((i > 0 && j < bmp_image.width - 1) && (temp[i - 1][j + 1] == 1)) k++;
                    if ((j > 0) && (temp[i][j - 1] == 1)) k++;
                    if ((j < bmp_image.width - 1) && (temp[i][j + 1] == 1)) k++;
                    if ((i < bmp_image.height - 1 && j > 0) && (temp[i + 1][j - 1] == 1)) k++;
                    if ((i < bmp_image.height - 1) && (temp[i + 1][j] == 1)) k++;
                    if ((i < bmp_image.height - 1 && j < bmp_image.width - 1) && (temp[i + 1][j + 1] == 1)) k++;
                    if (k == 3) image[i][j] = 1;
                } else {
                    if ((i > 0 && j > 0) && (temp[i - 1][j - 1] == 1)) k++;
                    if ((i > 0) && (temp[i - 1][j] == 1)) k++;
                    if ((i > 0 && j < bmp_image.width - 1) && (temp[i - 1][j + 1] == 1)) k++;
                    if ((j > 0) && (temp[i][j - 1] == 1)) k++;
                    if ((j < bmp_image.width - 1)  && (temp[i][j + 1] == 1)) k++;
                    if ((i < bmp_image.height - 1 && j > 0) && (temp[i + 1][j - 1] == 1)) k++;
                    if ((i < bmp_image.height - 1) && (temp[i + 1][j] == 1)) k++;
                    if ((i < bmp_image.height - 1 && j < bmp_image.width - 1) && (temp[i + 1][j + 1] == 1)) k++;
                    if (k != 2 && k != 3) image[i][j] = 0;
                }
            }
        }
        FILE *file1;
        sleep(dump_freq);
        for (int i = 0; i < bmp_image.height; i++) {
            for (int j = 0; j < bmp_image.width; j++) {
                if (image[i][j] != temp[i][j]) {
                    flag=1;
                }
            }
        }
        if (flag == 0) {
            printf("No changes in %d iteration", iter);
            return 0;
        }
        file1 = fopen(output, "wb");
        fwrite(&bmp_image, sizeof(bmp_image), 1, file1);

        for (int i = 0; i < bmp_image.height; i++) {
            for (int j = 0; j < bmp_image.width + ((3 * bmp_image.width) % 4); j++) {
                fwrite(&image[i][j], sizeof(image[i][j]), 1, file1);
            }
        }
        fclose(file1);
        for (int i = 0; i < bmp_image.height; i++) {
            for (int j = 0; j < bmp_image.width; j++) {
                printf("%d ", image[i][j]);
            }
            printf("\n");
        }
        printf("\n");
    }
    free(image);
    return 0;
}