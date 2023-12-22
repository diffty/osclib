

void* iterate_on_read_data(void* pMem, char dataType) {
    void* pNext = NULL;

    int valueSize = 0;

    switch (dataType) {
        case 'i': {
            valueSize = sizeof(int32_t);
            pNext = ((int32_t*) pMem) + 1;
            break;
        }

        case 's': {
            char* pStr = (char*) pMem;
            while (pStr[++valueSize] != '\0');
            pNext = pStr + valueSize;
            break;
        }

        case 'f': {
            valueSize = sizeof(float);
            pNext = ((float*) pMem) + 1;
            break;
        }

        case 'b': {
            valueSize = sizeof(bool);
            pNext = ((bool*) pMem) + 1;
            break;
        }
    }

    return pNext;
}



/*
//char* strData = (char*) iterate_on_read_data();

pCurrData = pData;

// TODO: passer prochain pointeur et pointeur vers une valeur en référence plutôt que faire des retours
// Genre `read_data((void *) pCurrValue, char currValueType, void* pDataIteratorPtr)`

printf("%i ", *((int32_t*) pCurrData));
pCurrData = iterate_on_read_data(pCurrData, 'i');

printf("%i ", *((int32_t*) pCurrData));
pCurrData = iterate_on_read_data(pCurrData, 'i');

printf("%f ", *((float*) pCurrData));
pCurrData = iterate_on_read_data(pCurrData, 'f');

*/
