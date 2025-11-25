#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "decode.h"

int main(int argc, char *argv[])
{
    EncodeInfo encInfo;
    DecodeInfo decInfo;
    uint img_size;
    if(argc <= 2 || argc > 5)
    {
	    usage();
	    return 0;
    }
    if(check_operation_type(argv) == e_encode)
    {
	
        printf("Welcome to stegnography!!\n");
	if(read_and_validate_encode_args(argc, argv, &encInfo) == e_success)
	{
		// do the encode
		printf("## Encoding Procedure Started ##\n");
		if(do_encoding(&encInfo) == e_success)
			printf("Encoding done successfully\n");
		else
			printf("Encoding error!!\n");
	}
	else
	{
		usage();
		return 0;
	}

    }
    else if(check_operation_type(argv) == e_decode)
    {
	if(read_and_validate_decode_args(argc, argv, &decInfo) == e_success)
	{
		printf("## Decoding Procedure Started ##\n");
		// do the decode
		if(do_decoding(&decInfo) == e_success)
			printf("Decoding done successfully\n");
		else
			printf("Decoding error!!\n");
	}
	else
	{
		usage();
		return 0;
	}
    }
    else
    {
	usage();
	return 0;
    }





    // Test get_image_size_for_bmp3
/*  img_size = get_image_size_for_bmp(encInfo.fptr_src_image);
    printf("INFO: Image size = %u\n", img_size);
*/
    return 0;
}

OperationType check_operation_type(char *argv[])
{
	if(strcmp(argv[1], "-e") == 0)
	{
		return e_encode;
	}
	else if(strcmp(argv[1], "-d") == 0)
	{
		return e_decode;
	}
	else
	{
		return e_unsupported;

	}
}

Status read_and_validate_encode_args(int argc, char *argv[], EncodeInfo *encInfo)
{
	if(argc >= 4 && argc <= 5)
	{
		if(strcmp(strstr(argv[2], ".bmp"), ".bmp") == 0)
		{
			encInfo -> src_image_fname = argv[2];
		}
		else
		{
			printf("Error, it is not a .bmp file.\n");
			return e_failure;
		}
		char *ptr1 = strstr(argv[3], ".txt");
		if(ptr1 == NULL)
		{
			ptr1 = strstr(argv[3], ".c");
			if(ptr1 == NULL)
			{
				ptr1 = strstr(argv[3], ".sh");
				if(ptr1 == NULL)
				{
					printf("Wrong secret file extension.\n");
					return e_failure;
				}
				else if(strcmp(ptr1, ".sh") == 0)
                		{
                        		encInfo -> secret_fname = argv[3];
                        		strcpy(encInfo->extn_secret_file, ptr1);
                		}
                		else
                		{
                	        	printf("Error!, File extension is wrong.\n");
                        		return e_failure;
                		}

			}
			else if(strcmp(ptr1, ".c") == 0)
                	{
                        	encInfo -> secret_fname = argv[3];
                        	strcpy(encInfo->extn_secret_file, ptr1);
                	}
                	else
                	{
                	        printf("Error!, File extension is wrong.\n");
                        	return e_failure;
                	}
		}
		else if(strcmp(ptr1, ".txt") == 0)
		{
			encInfo -> secret_fname = argv[3];
			strcpy(encInfo->extn_secret_file, ptr1);
		}
		else
		{
			printf("Error!, File extension is wrong.\n");
			return e_failure;
		}

		if(argv[4] == NULL)
		{
			encInfo -> stego_image_fname = "stego.bmp";
			printf("Output File not mentioned, creating stego.bmp as default\n");
			return e_success;
		}
		else
		{			
			if(strcmp(strstr(argv[4], ".bmp"), ".bmp") == 0)
			{
				printf("new bmp file created.\n");
				encInfo -> stego_image_fname = argv[4];
				return e_success;
			}
			else
			{
				printf("Error, it is not a .bmp file.\n");
				return e_failure;
			}	
		}
	}
	else
	{
		printf("Error!, Not enough arguments.\n");
		return e_failure;
	}	
}


Status read_and_validate_decode_args(int argc, char *argv[], DecodeInfo *decInfo)
{
        if(argc >= 3 && argc <= 4)
        {
                if(strcmp(strstr(argv[2], ".bmp"), ".bmp") == 0)
                {
                        decInfo -> out_image_fname = argv[2];
                }
                else
                {
                        printf("Error, it is not a .bmp file.\n");
                        return e_failure;
                }

                if(argv[3] == NULL)
                {
			printf("Output File not mentioned. Creating output_secret as default\n");
                        decInfo -> secret_fname = "output_secret";
                        return e_success;
                }
                else
                {
			char *ptr1 = strchr(argv[3], '.');
                	if(ptr1 == NULL)
                	{
                        	decInfo -> secret_fname = argv[3];
                        	return e_success;
                	}
                	else
                	{
				ptr1 = '\0';
				decInfo -> secret_fname = ptr1;
                        	return e_success;
                	}
                }
        }
        else
        {
                printf("Error!, Not enough arguments.\n");
                return e_failure;
        }
}

void usage()
{
	printf("Usage\n");
        printf("For encoding : ./a.out -e <. bmp file> <. txt file> [output file]\n");
        printf("For decoding : ./a.out -d <. bmp file> [output file]\n");
}
