#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include "types.h"
#include "common.h"
#include "decode.h"

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo)
{

if(open_files_decode(decInfo) == e_failure)
{
    printf("Error : files cant be opened\n");
    return e_failure;
}
else
{
    printf("Opened files successfully\n");
}

if (decode_magic_string(decInfo) == e_failure)
{
    printf("Error : magic string not found / mismatched \n");
    return e_failure;
}
else
{
    printf("found magic string succesfully\n");
}

if (decode_secret_file_extn_size(decInfo) == e_failure)
{
    printf("Error : file extn size no found\n");
    return e_failure;
}
else
{
    printf("Decoded file extn size successfully\n");
}

if( decode_secret_file_extn(decInfo) == e_failure)
{
    printf("Error : file extn not found\n");
    return e_failure;
}
else
{
    printf("file extn decoded successfully\n");
}


if (str_cat(decInfo) == e_failure)
{
    printf("output file not created\n");
    return e_failure;
}
else
{
    printf("output file name created successfully\n");
}

if(decode_secret_data_size(decInfo) == e_failure)
{
    printf("Error : Failed to decode secret file size\n");
    return e_failure;
}
printf("Decoded secret file size successfully\n");

if(open_output_file(decInfo) == e_failure)
{
    printf("Error : Unable to create output file\n");
    return e_failure;
}
printf("Output file created successfully\n");

if(decode_secret_file_data(decInfo) == e_failure)
{
    printf("Error : Failed to decode secret data\n");
    return e_failure;
}
printf("Secret data decoded successfully\n");

close_decode_files(decInfo);

return e_success;

}

Status read_and_validate_decode_args(char **argv, DecodeInfo *decInfo)
{
    if (strstr(argv[2], ".bmp") == NULL)
    {
        printf("Error : Invalid input\n");
        return e_failure;
    }

    decInfo->stego_image_fname = argv[2];

    
    if(argv[3] != NULL)
    {
    strtok(argv[3], ".");
    strcpy(decInfo->secret_file_name, argv[3]);
    }
    else
    {
    decInfo->secret_file_name[0] = '\0';
    }

    return e_success;
}
/* Get File pointers for i/p and o/p files */
Status open_files_decode(DecodeInfo *decInfo)
{
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");

    if (decInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);

    	return e_failure;
    }
    fseek(decInfo->fptr_stego_image, 54, SEEK_SET);
    return e_success;
}


char decode_byte_from_lsb( char *arr)
{
    char  ch = 0;
    for(int i = 0; i < 8; i++)
    {
        ch = ch | ((arr[i] & 1) << i);
    }
    return ch;
}


char* decode_string_from_image(int size,  FILE *fptr_stego_image)
{
    char arr[8];
    char *str = malloc(size+1);
    if(str == NULL)
    return NULL;

    // write a function
    for(int i = 0; i < size; i++)
    {
        fread(arr,8, 1, fptr_stego_image);
        str[i] = decode_byte_from_lsb(arr);
    
   }
    str[size] = '\0';

return str;
}


Status decode_magic_string( DecodeInfo *decInfo)
{
 char *str;

str = decode_string_from_image(strlen(MAGIC_STRING),decInfo->fptr_stego_image);
 if(strcmp(str, MAGIC_STRING) == 0)
 {
    printf("Magic string is found \n");
    free(str);
    return e_success;
 }
 else
 {
    printf("Magic string is not found\n");
    free(str);
    return e_failure;
 }
}


int decode_int_from_lsb(FILE *fptr_stego_image)
{
    char arr[32];
    int num = 0;

    fread(arr, 32, 1, fptr_stego_image);

    for(int i = 0; i < 32; i++)
    {
        num |= ((arr[i] & 1) << i);
    }

    return num;
}

Status decode_secret_file_extn_size(DecodeInfo *decInfo)
    {
        
        decInfo->secret_file_extn_size = decode_int_from_lsb(decInfo->fptr_stego_image);
        if(decInfo->secret_file_extn_size != 0 )
        {
            return e_success;
        }
        else
        {
            return e_failure;
        }

    }

/* Encode secret file extenstion */
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
char *temp = decode_string_from_image(decInfo->secret_file_extn_size, decInfo->fptr_stego_image);

if(temp == NULL)
return e_failure;

strcpy(decInfo->secret_file_extn, temp);

free(temp);
return e_success;

}
//add the output file extension
Status str_cat(DecodeInfo *decInfo)
{
    if(strlen(decInfo->secret_file_name) == 0)
    {
        strcpy(decInfo->secret_file_name, "decode_output");
    }

    strcat(decInfo->secret_file_name,
           decInfo->secret_file_extn);

    return e_success;
}
/* Decode secret file size */

Status decode_secret_data_size(DecodeInfo *decInfo)
{
    decInfo->secret_file_size =
        decode_int_from_lsb(decInfo->fptr_stego_image);

    if(decInfo->secret_file_size > 0)
        return e_success;

    return e_failure;
}

Status open_output_file(DecodeInfo *decInfo)
{
    

    decInfo->fptr_output = fopen(decInfo->secret_file_name, "wb");

    if(decInfo->fptr_output == NULL)
    {
        perror("fopen");
        return e_failure;
    }

    return e_success;
}

Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char arr[8];
    char ch;

    for(int i = 0; i < decInfo->secret_file_size; i++)
    {
        fread(arr, 8, 1, decInfo->fptr_stego_image);

        ch = decode_byte_from_lsb(arr);

        fputc(ch, decInfo->fptr_output);
    }

    return e_success;
}


Status close_decode_files(DecodeInfo *decInfo)
{
    if(decInfo->fptr_stego_image)
        fclose(decInfo->fptr_stego_image);

    if(decInfo->fptr_output)
        fclose(decInfo->fptr_output);

    return e_success;
}