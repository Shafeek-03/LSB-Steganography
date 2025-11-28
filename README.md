# Image Steganography in C (BMP)

This project is a simple **image steganography** tool written in C.

It can:
- **Encode** secret text inside a BMP image.
- **Decode** the hidden text back from the stego image.

The project uses the **LSB (Least Significant Bit)** technique on BMP images.

---

## Features

- Supports BMP images.
- Hides data from a text file (e.g, `secret.txt`).
- Command-line interface for encoding and decoding.
- For encoding:
    ./stego -e <input.bmp> <secret.txt> [output_stego.bmp]
- For decoding:
    ./stego -d <stego.bmp> [output_decoded.txt]
- Example files:
  - `beautiful.bmp` – sample cover image
  - `secret.txt` – sample secret message

---

## Build Instructions

    
You need a C compiler (like `gcc`).

Compile:

```bash
gcc encode.c decode.c test_encode.c -o stego


