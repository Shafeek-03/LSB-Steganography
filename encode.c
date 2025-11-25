#include <stdio.h>
#include <string.h>
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
    printf("Opening required files\n");
    return e_success;
}


Status do_encoding(EncodeInfo *encInfo)
{
    if(open_files(encInfo) == e_failure)
	return e_failure;   
    else if(check_capacity(encInfo) == e_failure)
	return e_failure;
    else if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
	return e_failure;
    else if(encode_magic_string(MAGIC_STRING, encInfo) == e_failure)
    {
	printf("Magic string encoding error!\n");
	return e_failure;
    }
    else if(encode_file_extn_size(strlen(encInfo->extn_secret_file), encInfo) == e_failure)
    {
	printf("file extension size encoding error!\n");
	return e_failure;
    }
    else if(encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_failure)
    {
	printf("Secret file extension encoding error!\n");
	return e_failure;
    }
    else if(encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_failure)
    {
	printf("Error in secret file size encoding!\n");
	return e_failure;
    }
    else if(encode_secret_file_data(encInfo) == e_failure)
    {
	printf("Error in secret file data encoding!\n");
	return e_failure;
    }
    else if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
    {
	printf("Error in copy remainging data.\n");
	return e_failure;
    }
    else
    {
	return e_success;
    
    }		

}


Status check_capacity(EncodeInfo *encInfo)
{
     printf("Checking for beautiful.bmp capacity to handle secret.txt\n");
     encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);            // take the source image size
     int magic_string = strlen(MAGIC_STRING);                                              // magic string length
     int extension = strlen(encInfo->extn_secret_file);                                    // length of secret file extension 
     encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);                      // secret data size
     uint Encoding_things = (magic_string + sizeof(int) + extension + sizeof(int) + encInfo->size_secret_file) * 8;   // Total bytes need to encode the data
     if(Encoding_things <= encInfo->image_capacity)
     {
	 printf("Found OK\n");
	 return e_success;
     }
     else
     {
	 printf("There is not enough capacity.\n");
	 return e_failure;
     }
}


uint get_file_size(FILE *fptr)                   // get secret file size 
{
    fseek(fptr, 0, SEEK_END);                    // Move the file offset to the EOF 
    return ftell(fptr);                          // Return the file offset position
}


Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)           // copy the header file from source image to new image
{
    printf("Copying Image Header\n");
    rewind(fptr_src_image);                              // move the file offset to 0th index, because cursor is now in 25th position
    char arr[54];         
    fread(arr, sizeof(arr), 1, fptr_src_image);          // Read the 54 bytes of header from source image
    if(fwrite(arr, sizeof(arr), 1, fptr_dest_image))	    // and write 54 bytes to new image
    {
	printf("Done\n");
        return e_success;
    }
    else
    {
	printf("Header file copy error!\n");
	return e_failure;
    }


}


Status encode_magic_string(char *magic_string, EncodeInfo *encInfo)          // Encode magic string
{
    printf("Encoding Magic String Signature\n");
    if(encode_data_to_image(magic_string, strlen(magic_string), encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
	printf("Done\n");
	return e_success;
    }
    else
	return e_failure;
}


Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)     // Encode character data
{
    for(int i = 0; i < size; i++)           // Run a loop of size times
    {
	char arr1[8];                       // Each character need 8 byte of data from source file
  	fread(arr1, sizeof(arr1), 1, fptr_src_image);
	encode_byte_to_lsb(data[i], arr1);
	fwrite(arr1, 8, 1, fptr_stego_image);        // copy the encoded data to the new image
    }
    return e_success;
}


Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for(int j = 0; j < 8; j++)                                  // Perfrom encoding logic, one character has 8 bits
    {
        image_buffer[j] = (image_buffer[j] & ~1) | (data >>  (7 - j) & 1);  // one bit is encoded to the lsb bit of 1 byte 
    }
    return e_success;
}


Status encode_file_extn_size(int size, EncodeInfo *encInfo)
{
    printf("Encoding secret.txt File Extenstion size\n");
    char arr2[32];
    fread(arr2, sizeof(arr2), 1, encInfo->fptr_src_image); // Read the 32 bytes from source image to encode integer data (4 byte => 4*8 = 32 bytes needed)
    encode_int_to_lsb(size, arr2);                         // call the integer encoding function
    if(fwrite(arr2, sizeof(arr2), 1, encInfo->fptr_stego_image))     // write 32 bytes to new image file 
    {
        printf("Done\n");
        return e_success;
    }
    else
    {
	printf("Encode file extension size error!\n");
	return e_failure;
    }
}


Status encode_int_to_lsb(int size, char *image_buffer)
{
    for(int i = 0; i < 32; i++)                                              // Perfrom encoding logic, one integer has 32 bits
    {
        image_buffer[i] = (image_buffer[i] & ~1) | (size >> (31 - i) & 1);  // one bit is encoded to the lsb bit of 1 byte
    }
    return e_success;
}


Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{
    printf("Encoding secret.txt File Extenstion\n");
    if(encode_data_to_image(file_extn, strlen(file_extn), encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
	printf("Done\n");
	return e_success;
    }
    else
	return e_failure;
}


Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    printf("Encoding secret.txt File Size\n");
    char arr2[32];
    fread(arr2, sizeof(arr2), 1, encInfo->fptr_src_image);   // Read the 32 bytes from source image to encode integer data (4 byte => 4*8 = 32 bytes needed)
    encode_int_to_lsb(file_size, arr2);                      // call the integer encoding function
    if(fwrite(arr2, sizeof(arr2), 1, encInfo->fptr_stego_image))     // write 32 bytes to new image file 
    {
        printf("Done\n");
	return e_success;
    }
    else
    {
	printf("Encode file extension size error!\n");
	return e_failure;
    }
}


Status encode_secret_file_data(EncodeInfo *encInfo)
{
    printf("Encoding secret.txt File Data\n");
    rewind(encInfo->fptr_secret);
    char secret_data[encInfo->size_secret_file];
    fread(secret_data, 1, encInfo->size_secret_file, encInfo->fptr_secret);
    if(encode_data_to_image(secret_data, encInfo->size_secret_file, encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
	printf("Done\n");
	return e_success;
    }
    else
	return e_failure;
}


Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    printf("Copying Left Over Data\n");
    char c;
    while(fread(&c, 1, 1, fptr_src) == 1)
    {
 	 fwrite(&c, 1, 1, fptr_dest);
    }
    printf("Done\n");
    return e_success;
}
