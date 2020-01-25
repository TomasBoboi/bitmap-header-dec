#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>

typedef struct{
	uint16_t	file_type;
	uint32_t	file_size;
	uint16_t	reserved1;
	uint16_t	reserved2;
	uint32_t	pixel_offset;
} bitmap_file_header;

typedef struct{
	uint32_t	header_size;
	int32_t		bitmap_width;
	int32_t		bitmap_height;
	uint16_t	color_planes;
	uint16_t	color_depth;
	uint32_t	compression_method;
	uint32_t	image_size;
	int32_t		horizontal_resolution;
	int32_t		vertical_resolution;
	uint32_t	color_palette;
	uint32_t	important_colors;
} BITMAPINFOHEADER;

void lite2bige_16(uint16_t *n)
{
	uint8_t hi = (*n) >> 8;
	uint8_t lo = (*n) & 0xFF;
	*n = (lo << 8) | hi;
}

void lite2bige_32(uint32_t *n)
{
	uint16_t hi = (*n) >> 16;
	uint16_t lo = (*n) & 0x0000FFFF;
	
	lite2bige_16(&hi);
	lite2bige_16(&lo);
	
	*n = (lo << 16) | hi;
}

void err()
{
	printf("Error reading from input file\n");
	exit(EXIT_FAILURE);
}

void print_file_header(bitmap_file_header bfh)
{
	printf("+--------------------------------------------------------------------+\n");
	printf("|  OFFSET_HEX  |  OFFSET_DEC  |  SIZE  |    VALUE    |  DESCRIPTION  |\n");
	printf("|      %02X      |      %02u      |   %luB   |    %c%c       |  File Type    |\n",0x00,0x00,sizeof(bfh.file_type),bfh.file_type >> 8,bfh.file_type & 0xFF);
	printf("|      %02X      |      %02u      |   %luB   |    %-8u |  File Size    |\n",0x02,0x02,sizeof(bfh.file_size),bfh.file_size);
	printf("|      %02X      |      %02u      |   %luB   |    %-8u |  Reserved_1   |\n",0x06,0x06,sizeof(bfh.reserved1),bfh.reserved1);
	printf("|      %02X      |      %02u      |   %luB   |    %-8u |  Reserved_2   |\n",0x08,0x08,sizeof(bfh.reserved2),bfh.reserved2);
	printf("|      %02X      |      %02u      |   %luB   |    %-8u |  Pixel Offset |\n",0x10,0x10,sizeof(bfh.pixel_offset),bfh.pixel_offset);
	printf("+--------------------------------------------------------------------+\n");
}

void print_info_header(BITMAPINFOHEADER bih)
{
	printf("+--------------------------------------------------------------------------------------------------------------------+\n");
	printf("|  OFFSET_HEX  |  OFFSET_DEC  |  SIZE  |    VALUE    |                          DESCRIPTION                          |\n");
	printf("|      %02X      |      %02u      |   %luB   |    %-8u |  The size of this header, in bytes                            |\n",0x0E,0x0E,sizeof(bih.header_size),bih.header_size);
	printf("|      %02X      |      %02u      |   %luB   |    %-8d |  The bitmap width, in pixels                                  |\n",0x12,0x12,sizeof(bih.bitmap_width),bih.bitmap_width);
	printf("|      %02X      |      %02u      |   %luB   |    %-8d |  The bitmap height, in pixels                                 |\n",0x16,0x16,sizeof(bih.bitmap_height),bih.bitmap_height);
	printf("|      %02X      |      %02u      |   %luB   |    %-8u |  The number of color planes                                   |\n",0x1A,0x1A,sizeof(bih.color_planes),bih.color_planes);
	printf("|      %02X      |      %02u      |   %luB   |    %-8u |  The number of bits per pixel                                 |\n",0x1C,0x1C,sizeof(bih.color_depth),bih.color_depth);
	printf("|      %02X      |      %02u      |   %luB   |    %-8u |  The compression method being used                            |\n",0x1E,0x1E,sizeof(bih.compression_method),bih.compression_method);
	printf("|      %02X      |      %02u      |   %luB   |    %-8u |  The raw image size, in bytes                                 |\n",0x22,0x22,sizeof(bih.image_size),bih.image_size);
	printf("|      %02X      |      %02u      |   %luB   |    %-8d |  The horizontal resolution of the image, in pixels per metre  |\n",0x26,0x26,sizeof(bih.horizontal_resolution),bih.horizontal_resolution);
	printf("|      %02X      |      %02u      |   %luB   |    %-8d |  The vertical resolution of the image, in pixels per metre    |\n",0x2A,0x2A,sizeof(bih.vertical_resolution),bih.vertical_resolution);
	printf("|      %02X      |      %02u      |   %luB   |    %-8u |  The number of colors in the color palette                    |\n",0x2E,0x2E,sizeof(bih.color_palette),bih.color_palette);
	printf("|      %02X      |      %02u      |   %luB   |    %-8u |  The number of important colors used                          |\n",0x32,0x32,sizeof(bih.important_colors),bih.important_colors);
	printf("+--------------------------------------------------------------------------------------------------------------------+\n");
}

int main(int argc, char *argv[])
{
	if(argc != 2)
	{
		printf("Use: %s <filename.bmp>\n",argv[0]);
		exit(EXIT_FAILURE);
	}
	
	
	uint32_t f;
	if((f = open(argv[1],O_RDONLY)) < 0)
	{
		printf("Error opening input file\n");
		exit(EXIT_FAILURE);
	}
	
	uint32_t r;
	bitmap_file_header bfh;
	if((r = read(f,&bfh.file_type,sizeof(bfh.file_type))) < 0) err();
	if((r = read(f,&bfh.file_size,sizeof(bfh.file_size))) < 0) err();
	if((r = read(f,&bfh.reserved1,sizeof(bfh.reserved1))) < 0) err();
	if((r = read(f,&bfh.reserved2,sizeof(bfh.reserved2))) < 0) err();
	if((r = read(f,&bfh.pixel_offset,sizeof(bfh.pixel_offset))) < 0) err();
	
	lite2bige_16(&bfh.file_type);
	lite2bige_16(&bfh.reserved1);
	lite2bige_16(&bfh.reserved2);
	
	print_file_header(bfh);
	
	uint32_t bih_size = bfh.pixel_offset - 14;
	
	printf("\nInformation header type: ");
	switch(bih_size)
	{
		case 12:
			printf("BITMAPCOREHEADER / OS21XBITMAPHEADER\n");
			break;
			
		case 16: case 64:
			printf("OS22XBITMAPHEADER\n");
			break;
			
		case 40:
			printf("BITMAPINFOHEADER\n");
			break;
			
		case 52:
			printf("BITMAPV2INFOHEADER\n");
			break;
			
		case 56:
			printf("BITMAPV3INFOHEADER\n");
			break;
			
		case 108:
			printf("BITMAPV4HEADER\n");
			break;
			
		case 124:
			printf("BITMAPV5HEADER\n");
			break;
			
		default:
			printf("*undefined bitmap information header*\n");
			break;
	}
	
	if(bih_size == 40)
	{
		BITMAPINFOHEADER bih;
		
		if((r = read(f,&bih.header_size,sizeof(bih.header_size))) < 0) err();
		if((r = read(f,&bih.bitmap_width,sizeof(bih.bitmap_width))) < 0) err();
		if((r = read(f,&bih.bitmap_height,sizeof(bih.bitmap_height))) < 0) err();
		if((r = read(f,&bih.color_planes,sizeof(bih.color_planes))) < 0) err();
		if((r = read(f,&bih.color_depth,sizeof(bih.color_depth))) < 0) err();
		if((r = read(f,&bih.compression_method,sizeof(bih.compression_method))) < 0) err();
		if((r = read(f,&bih.image_size,sizeof(bih.image_size))) < 0) err();
		if((r = read(f,&bih.horizontal_resolution,sizeof(bih.horizontal_resolution))) < 0) err();
		if((r = read(f,&bih.vertical_resolution,sizeof(bih.vertical_resolution))) < 0) err();
		if((r = read(f,&bih.color_palette,sizeof(bih.color_palette))) < 0) err();
		if((r = read(f,&bih.important_colors,sizeof(bih.important_colors))) < 0) err();
		
		printf("Compression method: ");
		switch(bih.compression_method)
		{
			case 0:
				printf("BI_RGB\n");
				break;
			case 1:
				printf("BI_RLE8\n");
				break;
			case 2:
				printf("BI_RLE4\n");
				break;
			case 3:
				printf("BI_BITFIELDS\n");
				break;
			case 4:
				printf("BI_JPEG\n");
				break;
			case 5:
				printf("BI_PNG\n");
				break;
			case 6:
				printf("BI_ALPHABITFIELDS\n");
				break;
			case 11:
				printf("BI_CMYK\n");
				break;
			case 12:
				printf("BI_CMYKRLE8\n");
				break;
			case 13:
				printf("BI_CMYKRLE4\n");
				break;
			default:
				printf("*undefined compression method*\n");
				break;
		}
		
		print_info_header(bih);
	}
	
	if(close(f) == -1)
	{
		printf("Error closing input file\n");
		exit(EXIT_FAILURE);
	}
	
	return 0;
}
