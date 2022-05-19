/*
 * @(#)bitmap2c.c
 *
 * Copyright 2000, Aaron Ardiri     (mailto:aaron@ardiri.com)
 * All rights reserved.
 * 
 * The contents of this file is confidential and proprietrary in nature 
 * ("Confidential Information"). Redistribution or modification without 
 * prior consent of the original author(s) is prohibited.
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

/**
 * The main-line.
 *
 * @param argc the number of command line arguments.
 * @param argv an pointer to a series of command line argument strings.
 * @return 0 on success, 1 otherwise.
 */
int
main(int argc, char *argv[])
{
  int result = 0;

  // called correctly?
  if (argc > 1) {

    unsigned short i;
    FILE     *outFile = NULL;

    // open the "output" file
    outFile = fopen("bitmap.inc","w");

    // process all data files passed in
    for (i=1; i<argc; i++) {

      FILE           *inFile       = NULL;
      unsigned char  buffer[32768] = { 0 }; // 32K of memory
      unsigned char  outbuf[32768] = { 0 }; // 32K of memory
      unsigned short size, outsize, width, height, j;

      // open the PilRC resource image file
      inFile = fopen(argv[i],"rb");
     
      // does it exist?
      if (inFile != NULL) {

        // lets skip over the "16 byte" header for PilRC output
        fseek(inFile, 0L, SEEK_SET); 

        // read in compress the "bitmap"
	size    = fread(buffer,1,16,inFile);
	outsize = fread(outbuf,1,32768,inFile);
	width  = (unsigned short)(((int)buffer[0] << 8) | (int)buffer[1]);
	height = (unsigned short)(((int)buffer[2] << 8) | (int)buffer[3]);

        fprintf(outFile, "//\n"); 
        fprintf(outFile, "// BITMAP DEFINITION\n"); 
        fprintf(outFile, "// this section is auto-generated, do NOT edit.\n"); 
        fprintf(outFile, "//\n"); 
        fprintf(outFile, "// - Aaron Ardiri, 2000\n"); 
        fprintf(outFile, "\n"); 
        fprintf(outFile, "#define bitmap%02xWidth  %d\n", i-1, width);
        fprintf(outFile, "#define bitmap%02xHeight %d\n", i-1, height);
        fprintf(outFile, "#define bitmap%02xSize   %d\n", i-1, outsize);
        fprintf(outFile, "\n"); 
        fprintf(outFile, "BYTE bitmap%02x[%d] =\n", i-1, outsize);
        fprintf(outFile, "{");
	for (j=0; j<(outsize-1); j++) {
	  if ((j % 8) == 0) fprintf(outFile, "\n  ");
          fprintf(outFile, "0x%02x, ", outbuf[j]);
	}
        fprintf(outFile, "0x%02x\n", outbuf[outsize-1]);
        fprintf(outFile, "};\n\n");

        // close input file
        fclose(inFile);
      }
      else
        printf("ERROR: [%s] FILE NOT FOUND\n", argv[i]);
    }
    printf("\n");

    // close output file
    fclose(outFile);
  }
  else {
    printf("USAGE:\n");
    printf("  font2c { file1.bin file2.bin ... fileN.bin }\n\n");

    result = 1;
  }

  return result;
}
