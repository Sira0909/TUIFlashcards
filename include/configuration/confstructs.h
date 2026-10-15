enum ETYPE{
    TYPE_BOOL,
    TYPE_INT,
    TYPE_DOUBLE,
    TYPE_STRING,
    TYPE_OBJECT,
};

typedef struct _keypair{
    enum ETYPE type;
    char* key;
    void* value;
} KEYPAIR;

typedef struct _JSONdata {//not fully json but whatever
    int numpairs;
    KEYPAIR* pairs;
} JSONDATA;
