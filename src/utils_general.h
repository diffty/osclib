#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void print_memory_block_hex(void* pMemBlock, int memBlockSize) {
    int i;
    
    for (i = 0; i < memBlockSize; i++) { 
        printf("%02hhX ", ((unsigned char*) pMemBlock)[i]);
    }
    
    printf("\n");
}
