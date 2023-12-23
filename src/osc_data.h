#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "utils_general.h"
#include "utils_math.h"
#include "structs_osc.h"


int get_arg_value_size(void* pArgData, char argType) {
    switch (argType) {
        case 'i': { return sizeof(int32_t); }
        case 'f': { return sizeof(float); }
        case 'b': { return sizeof(char); }
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

void init_osc_message(OscMessage* pOscMsg) {
    pOscMsg->address = (char*) malloc(1);
    pOscMsg->address[0] = '\0';
    pOscMsg->args = NULL;
    pOscMsg->size = 0;
    pOscMsg->argsCount = 0;
}

void free_osc_arg(OscArg* pOscArg) {
    if (pOscArg->data != NULL) free(pOscArg->data);
    pOscArg->size = 0;
    pOscArg->sizeWPad = 0;
}

void free_osc_message(OscMessage* pOscMsg) {
    if (pOscMsg->args != NULL) free(pOscMsg->args);
    if (pOscMsg->address != NULL) free(pOscMsg->address);
    pOscMsg->address = NULL;
    pOscMsg->args = NULL;
    pOscMsg->size = 0;
    pOscMsg->argsCount = 0;
}

// Copy the address char* into the struct and already pad it
void set_osc_message_address(OscMessage* pOscMsg, char* address) {
    int addressStrSize = strlen(address) * sizeof(char)+1;
    int addressStrSizePadded = calculate_size_with_padding(addressStrSize);
    pOscMsg->address = (char*) malloc(addressStrSizePadded);
    memset(pOscMsg->address, '\0', addressStrSizePadded);
    memcpy(pOscMsg->address, address, addressStrSize);
}

void set_osc_arg_value(OscArg* pOscArg, void* pValue, char type) {
    pOscArg->size = get_arg_value_size(pValue, type);
    pOscArg->sizeWPad = calculate_size_with_padding(pOscArg->size);
    pOscArg->data = malloc(pOscArg->sizeWPad);
    pOscArg->type = type;

    memset(pOscArg->data, '\0', pOscArg->sizeWPad);
    memcpy(pOscArg->data, pValue, pOscArg->size);

    // OSC args data is basically uint32 big endian on most types so we reverse
    // their byte order bc most computers are little endian
    if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) {
        switch (pOscArg->type) {
            case 'b':
            case 'f':
            case 'i': {
                *((uint32_t*) pOscArg->data) = __builtin_bswap32(*((uint32_t*) pOscArg->data));
                break;
            }
        }
    }
}

void add_arg_to_osc_message(OscMessage* pMessage, OscArg* pOscArg) {
    pOscArg->msgDataIdx = pMessage->size;
    pMessage->size += pOscArg->sizeWPad;

    OscArg* newOscArgArray = (OscArg*) malloc(++pMessage->argsCount * sizeof(OscArg));
    newOscArgArray[pMessage->argsCount-1] = *pOscArg;
    
    if (pMessage->args != NULL) {
        memset(newOscArgArray, '\0', (pMessage->argsCount-1) * sizeof(OscArg));
        memcpy(newOscArgArray, pMessage->args, (pMessage->argsCount-1) * sizeof(OscArg));
        free(pMessage->args);
    }

    pMessage->args = (OscArg*) malloc(pMessage->argsCount * sizeof(OscArg));
    memcpy(pMessage->args, newOscArgArray, pMessage->argsCount * sizeof(OscArg));
}

char* assemble_osc_message_args(OscMessage* pMsg, char* fullArgs) {
    int i;
    for (i = 0; i < pMsg->argsCount; i++) {
        OscArg pOscArg = pMsg->args[i];
        memcpy(&fullArgs[pOscArg.msgDataIdx], pOscArg.data, pOscArg.sizeWPad);
    }
    return fullArgs;
}

char* assemble_osc_message_type_tags(OscMessage* pMsg, char* typeTagsStr) {
    typeTagsStr[0] = ',';

    int i;
    for (i = 0; i < pMsg->argsCount; i++) {
        typeTagsStr[i+1] = pMsg->args[i].type;
    }
    return typeTagsStr;
}

}
