#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include "encode.h"
#include "types.h"
#include "common.h"


/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3; 
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if( (strstr(argv[2], ".bmp")) != NULL)
    {
        encInfo->src_image_fname = argv[2];
    }
    else
    {
        printf("Error : invalid input file\n");
        return e_failure;
    }
   // check argv[2] is having .bmp as extn
    //    valid => store it in the structure (encInfo->src_image_fname = argv[2])
    //    Not present => return e_failure;
    
    if(strstr(argv[3], ".txt") != NULL)
    {
       encInfo->secret_fname = argv[3];
       strcpy( encInfo->extn_secret_file , strstr(argv[3], ".txt"));
    }
    else
    {
        printf("Error : invalid secretfile\n");
        return e_failure;
    }

    //check argv[3] is having .txt
    //    valid => store
    //    Not present => return e_failure;
    if(argv[4] != NULL)
    {
        if(strstr(argv[4] , ".bmp") != NULL)
        {
            encInfo->stego_image_fname = argv[4];
            
        }
        else
        {
            printf("Error : invalid o/pfile name\n");
            return e_failure;
        }
    }
    else
    {
        encInfo->stego_image_fname = "stego.bmp";
    }

    //check argv[4] is passed or Not
    //    passed => check argv[4] is having .bmp as extn => Not present => return e_failure;
    //    Not passed => store "stego.bmp" in the structure as output file name

    return e_success;
}
//uint get_image_size_for_bmp(FILE *fptr_src_image)
//{

  //  fseek(encInfo->fptr_src_image, 0, SEEK_END);
    //return ftell(encInfo->fptr_src_image);
//}

Status check_capacity(EncodeInfo *encInfo)
{
    uint size =  get_image_size_for_bmp(encInfo->fptr_src_image);

    fseek(encInfo->fptr_secret, 0, SEEK_END);

    int data_size = ftell(encInfo->fptr_secret);

    encInfo->size_secret_file = data_size;

    rewind(encInfo->fptr_secret);

    int size_info = strlen(MAGIC_STRING) + sizeof(int) + strlen(encInfo->extn_secret_file) + sizeof(int) + data_size;

    if(size > size_info*8)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char arr[54];

    rewind(fptr_src_image);

    fread(arr, 54, 1, fptr_src_image);

    fwrite(arr, 54, 1, fptr_stego_image);

    return e_success;
}

void encode_byte_to_lsb( char *arr, char ch)
{
    for(int i = 0; i < 8; i++)
    {
        arr[i] = (arr[i] & (~1)) | ((ch>>i) & 1);
    }
}


void encode_string_to_image( char *str,int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char arr[8];
    for(int i = 0; i < size; i++)
    {
        //Logic to encode single charecter
        fread(arr, 8, 1, fptr_src_image);
        encode_byte_to_lsb(arr, str[i]);
        fwrite(arr, 8, 1, fptr_stego_image);
    }

}

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    encode_string_to_image(MAGIC_STRING, strlen(MAGIC_STRING), encInfo->fptr_src_image, encInfo->fptr_stego_image);
    return e_success;
}

void encode_int_to_image(int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{

    char arr[32];
    fread(arr, 32, 1, fptr_src_image);

    for(int i = 0; i < 32; i++)
    {
        arr[i] = (arr[i] & (~1)) | ((size>>i) & 1);
    }

    fwrite(arr, 32, 1, fptr_stego_image);
}

Status encode_secret_file_extn_size(int extn_size, EncodeInfo *encInfo)
{
    
    encode_int_to_image(extn_size, encInfo->fptr_src_image, encInfo->fptr_stego_image);
    return e_success;

}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    encode_string_to_image(encInfo->extn_secret_file, strlen(encInfo->extn_secret_file), encInfo->fptr_src_image, encInfo->fptr_stego_image);
    return e_success;
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    encode_int_to_image( file_size, encInfo->fptr_src_image, encInfo->fptr_stego_image);
    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char arr[encInfo->size_secret_file+1];

    fread(arr, encInfo->size_secret_file, 1, encInfo->fptr_secret);

    arr[encInfo->size_secret_file] = 0;

    encode_string_to_image(arr, encInfo->size_secret_file, encInfo->fptr_src_image, encInfo->fptr_stego_image);

    return e_success;

}


Status copy_remaining_img_data(FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char ch;

    while (fread(&ch, 1, 1, fptr_src_image) == 1)
    {
        fwrite(&ch, 1, 1, fptr_stego_image);
    }

    return e_success;
}
Status do_encoding(EncodeInfo *encInfo)
{
    if(open_files(encInfo) == e_failure) 
    {
        printf("error : one or more files cannot be opened\n");
        return e_failure;
    }// print error and return e_failure

    if(check_capacity(encInfo) == e_failure) // print error and return e_failure
    {
        printf("Error : no enough space in image to encode\n");
        return e_failure;
    }

    if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
    {
        printf("Error : couldnot copy the header info\n");
        return e_failure;
    }

    if(encode_magic_string(MAGIC_STRING, encInfo) == e_failure)
    {
        printf("Encoding magic string failed\n");
        return e_failure;
    }

    if(encode_secret_file_extn_size(strlen(encInfo->extn_secret_file), encInfo) == e_failure)
    {
        printf("Secret file extsize  encoding failed\n");
        return e_failure;
    }

    if(encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_failure)
    {
        printf("secret file extn encoding failed\n");
        return e_failure;
    }

    if(encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_failure)
    {
        printf("Encoding secret file size failed\n");
        return e_failure;
    }

    if(encode_secret_file_data(encInfo) == e_failure)
    {
        printf("Data encoding failed\n");
        return e_failure;
    }

    if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
    {
        printf("Remaining data encoding failed\n");
        return e_failure;
    }


    return e_success;

}
