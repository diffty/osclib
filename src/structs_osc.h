typedef struct OscArg {
    char type;
    void* data;
    int size;
    int sizeWPad;
    int msgDataIdx;
} OscArg;


typedef struct OscMessage {
    char* address;
    OscArg* args;
    int size;
    unsigned int argsCount;
} OscMessage;
