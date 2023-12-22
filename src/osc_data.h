#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "utils_general.h"
#include "utils_math.h"
#include "structs_osc.h"


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

void init_osc_arg(OscArg* pOscArg) {
    pOscArg->data = (void*) NULL;
    pOscArg->msgDataIdx = -1;
    pOscArg->size = -1;
    pOscArg->type = '0';
}

void set_osc_arg_value(OscArg* pOscArg, void* pValue, char type) {
    pOscArg->size = get_arg_value_size(pValue, type);
    pOscArg->sizeWPad = calculate_size_with_padding(pOscArg->size);
    pOscArg->data = malloc(pOscArg->sizeWPad);
    pOscArg->type = type;
    memset(pOscArg->data, '\0', pOscArg->sizeWPad);
    memcpy(pOscArg->data, pValue, pOscArg->size);
}

void init_osc_message(OscMessage* pOscMsg) {
    pOscMsg->address = (char*) malloc(1);
    pOscMsg->address[0] = '\0';
    pOscMsg->args = NULL;
    pOscMsg->size = 0;
    pOscMsg->argsCount = 0;
}

void add_arg_to_osc_message(OscMessage* pMessage, OscArg* pOscArg) {
    OscArg* newOscArgArray = (OscArg*) malloc(++pMessage->argsCount * sizeof(OscArg));

    pOscArg->msgDataIdx = pMessage->size;
    pMessage->size += pOscArg->sizeWPad;

    newOscArgArray[pMessage->argsCount-1] = *pOscArg;
    
    if (pMessage->args != NULL) {
        memset(newOscArgArray, '\0', (pMessage->argsCount-1) * sizeof(OscArg));
        memcpy(newOscArgArray, pMessage->args, (pMessage->argsCount-1) * sizeof(OscArg));
        free(pMessage->args);
    }

    pMessage->args = (OscArg*) malloc(pMessage->argsCount * sizeof(OscArg));
    memcpy(pMessage->args, newOscArgArray, pMessage->argsCount * sizeof(OscArg));

}
