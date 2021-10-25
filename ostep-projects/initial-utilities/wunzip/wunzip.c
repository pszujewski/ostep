#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void unzip(char *path);

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        printf("wunzip: file1 [file2 ...]\n");
        exit(1);
    }

    for (int i = 1; i < argc; i++)
    {
        unzip(argv[i]);
    }

    return 0;
}

void unzip(char *path)
{
    FILE *fp = fopen(path, "rb");

    if (fp == NULL)
    {
        printf("wzip: cannot open file\n");
        exit(1);
    }

    fseek(fp, 0, SEEK_END);
    int eof = ftell(fp);

    fseek(fp, 0, SEEK_SET);
    int current = ftell(fp);

    while (current < eof)
    {
        int countBuffer[1];
        char charBuffer[1];

        /** 
         * in fread() below:
         * In effect, the second paramter defines the size of 1 "chunk"
         * and the third paramter defines how many chunks to read into the given
         * buffer. Be careful, the buffer type size in bytes should always equal the 
         * size of a chunk being read.  
         * 
         * fread always advances the file pointer to the first point past the bytes it just read. 
         * So if 4 bytes are read, the file pointer will be advanced 4 bytes after the read. 
        */

        fread(countBuffer, sizeof(int), 1, fp); // An int is 4 bytes
        fread(charBuffer, sizeof(char), 1, fp); // A char is 1 byte

        int count = countBuffer[0];
        char c = (char)charBuffer[0];

        for (int i = 0; i < count; i++)
        {
            printf("%c", c);
        }

        current = ftell(fp);
    }

    fclose(fp);
}
