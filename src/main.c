#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "osc_data.h"


void main() {
    OscMessage message;
    init_osc_message(&message);
    

    OscArg intArg1;
    init_osc_arg(&intArg1);
    int arg1 = 69;
    set_osc_arg_value(&intArg1, &arg1, 'i');
    print_memory_block_hex(intArg1.data, intArg1.size);

    OscArg intArg2;
    init_osc_arg(&intArg2);
    int arg2 = 1337;
    set_osc_arg_value(&intArg2, &arg2, 'i');
    print_memory_block_hex(intArg2.data, intArg2.size);

    char arg3[] = "test!! keveune";
    OscArg strArg3;
    init_osc_arg(&strArg3);
    set_osc_arg_value(&strArg3, &arg3, 's');
    print_memory_block_hex(strArg3.data, strArg3.size);
    
    OscArg floatArg4;
    init_osc_arg(&floatArg4);
    float arg4 = 2.0;
    set_osc_arg_value(&floatArg4, &arg4, 'f');
    print_memory_block_hex(floatArg4.data, floatArg4.size);


    add_arg_to_osc_message(&message, &intArg1);
    print_memory_block_hex(message.args[0].data, intArg1.size);

    add_arg_to_osc_message(&message, &intArg2);
    print_memory_block_hex(message.args[1].data, intArg2.size);

    add_arg_to_osc_message(&message, &strArg3);
    print_memory_block_hex(message.args[2].data, strArg3.size);

    add_arg_to_osc_message(&message, &floatArg4);
    print_memory_block_hex(message.args[3].data, floatArg4.size);

    int i;
    char* wholeMessage = (char*) malloc(message.size);
    for (i = 0; i < message.argsCount; i++) {
        OscArg pOscArg = message.args[i];
        memcpy(&wholeMessage[pOscArg.msgDataIdx], pOscArg.data, pOscArg.sizeWPad);
    }

    print_memory_block_hex(wholeMessage, message.size);
}
