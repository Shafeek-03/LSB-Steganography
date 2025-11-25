#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */


/*
 * Get File pointers for i/p and o/p files
 * Inputs: out Image file, Secret file name without the extension
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_image_files(DecodeInfo *decInfo)
{
    // encoded Image file
    decInfo->fptr_out_image = fopen(decInfo->out_image_fname, "r");
    // Do Error handling
    if (decInfo->fptr_out_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->out_image_fname);

        return e_failure;
    }
    // No failure return e_success
    return e_success;
}


Status do_decoding(DecodeInfo *decInfo)
{
    if(open_image_files(decInfo) == e_failure)
        return e_failure;
    else if(skip_bmp_header(decInfo->fptr_out_image) == e_failure)
        return e_failure;
    else if(decode_magic_string(decInfo) == e_failure)
    {
        printf("Magic string decoding error!\n");
        return e_failure;
    }
    else if(decode_file_extn_size(decInfo) == e_failure)
    {
        printf("file extension size decoding error!\n");
        return e_failure;
    }
    else if(decode_secret_file_extn(decInfo) == e_failure)
    {
        printf("Secret file extension decoding error!\n");
        return e_failure;
    }
    else if(decode_secret_file_size(decInfo) == e_failure)
    {
        printf("Error in secret file size decoding!\n");
        return e_failure;
    }
    else if(decode_secret_file_data(decInfo) == e_failure)
    {
        printf("Error in secret file data decoding!\n");
        return e_failure;
    }
    else
    {
        return e_success;

    }

}


Status skip_bmp_header(FILE *fptr_out_image)           // skip the header file from encoded image(output)
{
	fseek(fptr_out_image, 54, SEEK_SET);
	return e_success;
}


Status decode_magic_string(DecodeInfo *decInfo)          // Decode magic string
{
    int i;
    char *str = malloc(sizeof(MAGIC_STRING));
    for(i = 0; i < strlen(MAGIC_STRING); i++)
    {
	fread(decInfo->image_data, 8, 1, decInfo->fptr_out_image);
	str[i] = decode_byte_from_lsb(decInfo);
    }
    str[i] = '\0';
    if(strcmp(MAGIC_STRING, str) == 0)
    {
	printf("Magic string is %s\n", str);
	return e_success;
    }
    else
	return e_failure;

}


char decode_byte_from_lsb(DecodeInfo *decInfo)
{
    char ch = 0;
    for(int j = 0; j < 8; j++)                               // Perfrom decoding logic, one character has 8 bits
    {
        ch = ch << 1 | (decInfo->image_data[j] & 1);         // one bit is decoded from the lsb bit of 1 byte image data
    }
    return ch;
}


Status decode_file_extn_size(DecodeInfo *decInfo)
{
    char arr2[32];
    fread(arr2, sizeof(arr2), 1, decInfo->fptr_out_image); // Read the 32 bytes from source image to encode integer data (4 byte => 4*8 = 32 bytes needed)
    decInfo->file_extn_size = decode_int_from_lsb(arr2);             // call the integer decoding function
    printf("file extension size = %d\n", decInfo->file_extn_size);
    return e_success;
}


uint decode_int_from_lsb(char *image_buffer)
{
    unsigned int size = 0;
    for(int i = 0; i < 32; i++)                                              // Perfrom decoding logic, one integer has 32 bits
    {
        size = (size << 1 | image_buffer[i] & 1);                 // one bit is decoded from the lsb bit of 1 byte encoded image data
    }
    return size;
}


Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    int i;
    char str[decInfo->file_extn_size + 1];
    for(i = 0; i < decInfo->file_extn_size; i++)
    {
        fread(decInfo->image_data, 8, 1, decInfo->fptr_out_image);
        str[i] = decode_byte_from_lsb(decInfo);
    }
    str[i] = '\0';
    printf("output file extension : %s\n", str);
    char dec_secret_file_name[50];
    strcpy(dec_secret_file_name, decInfo->secret_fname);
    strcat(dec_secret_file_name, str);
    decInfo->secret_fname = dec_secret_file_name;
    decInfo->fptr_dec_secret = fopen(decInfo->secret_fname, "w");
    if(decInfo->fptr_dec_secret == NULL)
    {
	perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->secret_fname);

        return e_failure;
    }
    else
    {
	printf("New secret file name : %s\n", decInfo->secret_fname);
        return e_success;
    }
}


Status decode_secret_file_size(DecodeInfo *decInfo)
{
    char arr2[32];
    fread(arr2, sizeof(arr2), 1, decInfo->fptr_out_image);   // Read the 32 bytes from source image to encode integer data (4 byte => 4*8 = 32 bytes needed)
    decInfo->size_secret_file = decode_int_from_lsb(arr2);                      // call the integer encoding function
    printf("secret file size = %ld\n", decInfo->size_secret_file);
    return e_success;
}


Status decode_secret_file_data(DecodeInfo *decInfo)
{
    int i;
    char data;
    for(i = 0; i < decInfo->size_secret_file; i++)
    {
        fread(decInfo->image_data, 1, 8, decInfo->fptr_out_image);
        data = decode_byte_from_lsb(decInfo);
        fwrite(&data, 1, 1, decInfo->fptr_dec_secret);
    }
    return e_success;
   
}

