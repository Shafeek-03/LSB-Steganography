#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

/*
 * Structure to store information required for
 * decoding secret file from output Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{
    /* output Image info */
    char *out_image_fname;
    FILE *fptr_out_image;
    uint image_capacity;
    uint bits_per_pixel;
    char image_data[MAX_IMAGE_BUF_SIZE];

    /* Secret File Info */
    char *secret_fname;
    FILE *fptr_dec_secret;
    char extn_secret_file[MAX_FILE_SUFFIX];
    char secret_data[MAX_SECRET_BUF_SIZE];
    uint file_extn_size;
    long size_secret_file;


} DecodeInfo;


/* Decoding function prototype */


/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(int argc, char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_img_files(DecodeInfo *decInfo);

/* Skip bmp image header */
Status skip_bmp_header(FILE *fptr_out_image);

/* Decode and compare Magic String */
Status decode_magic_string(DecodeInfo *decInfo);

/* Decode secret file extenstion */
Status decode_secret_file_extn(DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* decode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Decode a byte from LSB of image data array */
char decode_byte_from_lsb(DecodeInfo *decInfo);

/* Decode the extension size from output images */
Status decode_file_extn_size(DecodeInfo *decInfo);

/* Decode int (4 byte) into lsb of image data array */
uint decode_int_from_lsb(char *image_buffer);

#endif
