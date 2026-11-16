#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include "encode.h"
#include "types.h"
#include "decode.h"

OperationType check_operation_type(char *argv[])
{
    if( strcmp(argv[1], "-e") == 0)
    return e_encode;

    else if( strcmp(argv[1], "-d") == 0)
    return e_decode;

    else
    return e_unsupported;
    //check argv[1] is -e  => return e_encode
    //check argv[1] is -d => return e_decode
    //else => return e_unsupported.
}

int main(int argc, char * argv[])
{
    
    

    OperationType res = check_operation_type(argv);

    if(res == e_encode)
    {
        
        EncodeInfo encInfo;
        //validation
        if(read_and_validate_encode_args(argv, &encInfo) == e_success)
        {
            //do encoding
            if(do_encoding(&encInfo) == e_failure)
            {
                printf("Error : encoding failed\n");
                exit(1);
            }
            else
            {
                printf("Encoding done successfully\n");
                exit(1);
            }

        }
        else
        {
            printf("Error:invalid inputs\n");
            exit(1);
        }
    }
    else if(res == e_decode)
    {
        // do decoding
        DecodeInfo decInfo;
      if (read_and_validate_decode_args(argv, &decInfo) == e_failure)
{
    printf("Invalid decode arguments\n");
    return 1;
}
    //store name and manually give extension after decoding from image
     if(do_decoding(&decInfo) == e_success)
     {
        printf("Decoded successfully\n");
    exit(0);
     }
     else
     {
        printf("Decoding failed\n");
        exit(1);
     }
  
    }
    else
    {
        // print error
        // exit
        printf("Error : Invalid operation\n");
        exit(1);
    }
    
    return 0;
}
