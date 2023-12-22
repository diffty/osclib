#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int calculate_size_with_padding(int size) {
    int paddingSize = size % 4;
    if (paddingSize > 0) {
        return size + (4 - paddingSize);
    }
    else {
        return size;
    }
}


// pDstValue memory block is not allocated by the function `read_data` itself
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

            //char* pNewStr = (char*) malloc(strSize * sizeof(char));

            memset(pDstValue, '\0', strSize);
            memcpy(pDstValue, *pDataCursor, strSize);

            //*((char**) pDstValue) = pNewStr;

            *pDataCursor = pDataCursor + strSize + paddingSize;
            break;
        }
    }
}

// The destination message memory block at `pDataIteratorPtr` is NOT allocated
// before it's written into.
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

typedef struct OscMessage {
    char* address;
    void* argsData;
    char* argsTypes;
    int* argsDataIndexes;
    int argsDataSize;
} OscMessage;


int get_arg_value_size(void* pArgData, char argType) {
    switch (argType) {
        case 'i': {
            return sizeof(int32_t);
        }

        case 'f': {
            return sizeof(float);
        }

        case 'b': {
            return sizeof(char);
        }

        case 's': {
            int strSize = 0; while (((char*) pArgData)[strSize++] != '\0');
            return strSize;
        }
    }
}

void add_arg_to_osc_message(OscMessage* pMessage, void* pArgData, char argType) {
    void* oldArgsData = pMessage->argsData;
    void* oldArgsTypes = pMessage->argsTypes;

    int nbArgs = strlen(pMessage->argsTypes);
    int argValueSize = get_arg_value_size(pArgData, argType);

    pMessage->argsData = (void*) malloc(calculate_size_with_padding(pMessage->argsDataSize + argValueSize));
    pMessage->argsTypes = (char*) malloc(calculate_size_with_padding(nbArgs+2));

    memset(pMessage->argsData, '\0', calculate_size_with_padding(pMessage->argsDataSize + argValueSize));

    if (oldArgsData != NULL) {
        memmove(pMessage->argsData, oldArgsData, pMessage->argsDataSize);
    }
    
    memcpy((char*) pMessage->argsData + pMessage->argsDataSize, pArgData, argValueSize);
    memcpy(pMessage->argsTypes, oldArgsTypes, nbArgs);

    pMessage->argsDataSize += calculate_size_with_padding(argValueSize);
    pMessage->argsTypes[nbArgs] = argType;
}

void* get_arg_in_osc_message(OscMessage* pMessage, int argId) {
    void* pDataCursor = pMessage->argsData;
    void* pCurrValue = NULL;

    int i;
    for (i = 0; i <= argId; i++) {
        int dataSize = get_arg_value_size(&pDataCursor, pMessage->argsTypes[i]);
        pCurrValue = (void*) malloc(dataSize);
        read_data(pCurrValue, pMessage->argsTypes[i], &pDataCursor);

        if (i < argId) {
            free(pCurrValue);
        }
    }

    return pCurrValue;
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


int main2() {
    printf("--- MAIN 2 ---\n");
    OscMessage message;

    char address[] = "/test";
    message.address = (char*) malloc(strlen(address) + 1);
    strcpy(message.address, address);
    message.argsData = NULL;
    message.argsTypes = (char*) malloc(1);
    message.argsTypes[0] = '\0';
    message.argsDataSize = 0;

    int arg1 = 69;
    add_arg_to_osc_message(&message, &arg1, 'i');

    float arg2 = 2.0;
    add_arg_to_osc_message(&message, &arg2, 'f');

    char keveune[] = "abdefh";
    add_arg_to_osc_message(&message, &keveune, 's');

    int arg3 = 1337;
    add_arg_to_osc_message(&message, &arg3, 'i');

    printf("%s %d\n", message.argsTypes, message.argsDataSize);
    print_memory_block_hex(message.argsData, message.argsDataSize);

    char* texte = (char*) get_arg_in_osc_message(&message, 2);
    printf("%s\n", texte);

    float* test2 = (float*) get_arg_in_osc_message(&message, 1);
    printf("%f\n", *test2);

    free(texte);
    free(test2);

    print_memory_block_hex(message.argsData, message.argsDataSize);
}


int main() {
    printf("--- MAIN 1 ---\n");
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

    pDataPtr = pData;

    int32_t* v1 = (int32_t*) malloc(get_arg_value_size(pDataPtr, 'i'));
    read_data(v1, 'i', &pDataPtr);

    int32_t* v2 = (int32_t*) malloc(get_arg_value_size(pDataPtr, 'i'));
    read_data(v2, 'i', &pDataPtr);
    
    float* vf = (float*) malloc(get_arg_value_size(pDataPtr, 'f'));
    read_data(vf, 'f', &pDataPtr);

    char* vs = (char*) malloc(get_arg_value_size(pDataPtr, 's'));
    read_data(vs, 's', &pDataPtr);

    printf("%d %d %f %s\n", *v1, *v2, *vf, vs);

    free(pData);
    free(v1);
    free(v2);
    free(vf);
    free(vs);

    print_memory_block_hex(pData, dataSize);

    main2();

    return 0;
}


int main3() {
    int i;
    for (i = 0; i < 20; i++)
        printf("%d %d\n", i, calculate_size_with_padding(i));
    
}
