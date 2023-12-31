#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "osc_data.h"


int main() {
    OscMessage message;
    init_osc_message(&message);
    set_osc_message_address(&message, "/test/ouida");

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


    char* fullArgs = (char*) malloc(message.size);
    assemble_osc_message_args(&message, fullArgs);
    print_memory_block_hex(fullArgs, message.size);

    int typeTagsStrPadSize = calculate_size_with_padding(message.argsCount + 2);
    char* typeTags = (char*) malloc(typeTagsStrPadSize);
    memset(typeTags, '\0', typeTagsStrPadSize);
    assemble_osc_message_type_tags(&message, typeTags);

    printf("%s\n", typeTags);
    print_memory_block_hex(typeTags, typeTagsStrPadSize);

    printf("%s\n", message.address);


    int msgAddrSize = calculate_size_with_padding(strlen(message.address) + 1);
    
    int wholeMsgSize = msgAddrSize
                       + typeTagsStrPadSize
                       + message.size;

    char* wholeMsg = (char*) malloc(wholeMsgSize);
    memset(wholeMsg, '\0', wholeMsgSize);

    // TODO faire une routine pour assembler des bytestream comme ça
    memcpy(&wholeMsg[0], message.address, msgAddrSize);
    memcpy(&wholeMsg[msgAddrSize], typeTags, typeTagsStrPadSize);
    memcpy(&wholeMsg[msgAddrSize + typeTagsStrPadSize], fullArgs, message.size);
    
    free(fullArgs);
    free(typeTags);

    int i;
    for (i = 0; i < wholeMsgSize; i++) {
        printf("%c", wholeMsg[i]);
    }

    struct sockaddr_in dest;

    int sck;
    if ((sck = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("Error opening socket!!\n");
        exit(1);
    }

    memset(&dest, 0, sizeof(dest));

    dest.sin_family = AF_INET;
    dest.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    dest.sin_port = htons(7001);

    sendto(sck, wholeMsg, wholeMsgSize, 0, &dest, sizeof(dest));

    close(sck);

    free_osc_arg(&intArg1);
    free_osc_arg(&intArg2);
    free_osc_arg(&strArg3);
    free_osc_arg(&floatArg4);

    free_osc_message(&message);

    
    OscMessage newMsg = make_osc_message("/test/ouida", "iisf", &arg1, &arg2, &arg3, &arg4);
    fullArgs = (char*) malloc(newMsg.size);
    assemble_osc_message_args(&newMsg, fullArgs);
    print_memory_block_hex(fullArgs, newMsg.size);

    for (i = 0; i < newMsg.size; i++) {
        printf("%c", fullArgs[i]);
    }
}
