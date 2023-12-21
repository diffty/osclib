#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Contrary to `write_data`, pDstValue memory block is allocated by
// the function `read_data` itself. Beware of leaks.
void read_data(void* pDstValue, char valueType, void** pDataCursor) {
    switch (valueType) {
        case 'i': {
            memcpy(pDstValue, *pDataCursor, sizeof(int32_t));
            *pDataCursor = ((int32_t*) *pDataCursor) + 1;
            break;
        }

        case 'f': {
            memcpy(pDstValue, *pDataCursor, sizeof(float));
            *pDataCursor = ((float*) *pDataCursor) + 1;
            break;
        }

        case 'b': {
            memcpy(pDstValue, *pDataCursor, sizeof(char));
            *pDataCursor = ((char*) *pDataCursor) + 1;
            break;
        }

        case 's': {
            char* pStrCursor = (char*) *pDataCursor;
            int strSize = 0; while (pStrCursor[strSize++] != '\0');

            int paddingSize = 3 - (strSize % 4);

            char* pNewStr = (char*) malloc(strSize * sizeof(char));

            memset(pNewStr, '\0', strSize);
            memcpy(pNewStr, *pDataCursor, strSize);

            *((char**) pDstValue) = pNewStr;

            *pDataCursor = pStrCursor + strSize + paddingSize;
            break;
        }
    }
}

// Contrary to `read_data`, the destination message memory block
// at `pDataIteratorPtr` is NOT allocated before it's written into.
void write_data(void* pSrcValue, char valueType, void** pDataBlockPtr) {
    switch (valueType) {
        case 'i': {
            memcpy(*pDataBlockPtr, pSrcValue, sizeof(int32_t));
            *pDataBlockPtr = ((int32_t*) *pDataBlockPtr) + 1;
            break;
        }

        case 'f': {
            memcpy(*pDataBlockPtr, pSrcValue, sizeof(float));
            *pDataBlockPtr = ((float*) *pDataBlockPtr) + 1;
            break;
        }

        case 'b': {
            memcpy(*pDataBlockPtr, pSrcValue, sizeof(char));
            *pDataBlockPtr = ((char*) *pDataBlockPtr) + 1;
            break;
        }

        case 's': {
            int strSize = 0; while (((char*) pSrcValue)[strSize++] != '\0');
            int paddingSize = 3 - (strSize % 4);

            memset((char*) *pDataBlockPtr, '\0', strSize + paddingSize);
            memcpy((char*) *pDataBlockPtr, pSrcValue, strSize);

            *pDataBlockPtr = (char*) *pDataBlockPtr + strSize + paddingSize;
            break;
        }
    }
}

void print_memory_block_hex(void* pMemBlock, int memBlockSize) {
    int i;
    
    for (i = 0; i < memBlockSize; i++) { 
        printf("%02hhX ", ((unsigned char*) pMemBlock)[i]);
    }
    
    printf("\n");
}

int calculate_str_size_with_padding(char* s) {
    int strSize = strlen(s);
    int paddingSize = 3 - (strSize % 4);
    return strSize + paddingSize;
}

// DC FF 00 00 45 00 00 00 00 00 00 40 74 65 73 74 61 69 6E 74 00 00 00 00 

int main() {
    int32_t value = 65500;
    int32_t value2 = 69;
    float value3 = 2.;
    char value4[] = "testainteouiononk";

    int dataSize = sizeof(int32_t) * 2 + sizeof(float) + calculate_str_size_with_padding(value4)+1;
    
    void* pData = malloc(dataSize);
    void* pDataPtr = pData;

    memset(pData, 0, dataSize);

    write_data(&value, 'i', &pDataPtr);
    write_data(&value2, 'i', &pDataPtr);
    write_data(&value3, 'i', &pDataPtr);
    write_data(&value4, 's', &pDataPtr);

    int32_t v1;
    int32_t v2;
    float vf;
    char* vs;

    pDataPtr = pData;

    read_data(&v1, 'i', &pDataPtr);
    read_data(&v2, 'i', &pDataPtr);
    read_data(&vf, 'f', &pDataPtr);
    read_data(&vs, 's', &pDataPtr);

    printf("%d %d %f %s\n", v1, v2, vf, vs);

    print_memory_block_hex(pData, dataSize);

    return 0;
}
