#ifndef DECODE_H
#define DECODE_H

#include<stdio.h>
#include<string.h>
#include "types.h"

#define MAX_SECRET_BUF_SIZE 10
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 10

typedef struct _DecodeInfo
{
    //input stego image file
    char *stego_image_fname;
    FILE *fptr_stego_image;
    char image_data[MAX_IMAGE_BUF_SIZE];
    char extn_stego_file[MAX_FILE_SUFFIX];

    //output file
    int valid_secret_file_name;
    char secret_file_name[50];
    char secret_file_extn[MAX_FILE_SUFFIX];
    char secret_data[MAX_SECRET_BUF_SIZE];

    int secret_file_extn_size;
    FILE *fptr_output;
 int secret_file_size;

}DecodeInfo;

Status open_files_decode(DecodeInfo *decInfo);

Status read_and_validate_decode_args(char **argv, DecodeInfo *decInfo);

Status do_decoding(DecodeInfo *decInfo);

Status decode_magic_string(DecodeInfo *decInfo);

char *decode_string_from_image(int size, FILE *fptr_stego_image);

//retrievs 1 byte from 8 bytes of image
char decode_byte_from_lsb(char *arr);

//reads secret file extn size
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

//convert 32 bit size stored into int
int decode_int_from_lsb(FILE *fptr_stego_image);

//reads extn from image
Status decode_secret_file_extn(DecodeInfo *decInfo);

// combine file name and extn
Status str_cat(DecodeInfo *decInfo);
Status decode_secret_data_size(DecodeInfo *decInfo);
Status open_output_file(DecodeInfo *decInfo);
Status decode_secret_file_data(DecodeInfo *decInfo);
Status close_decode_files(DecodeInfo *decInfo);

#endif 



