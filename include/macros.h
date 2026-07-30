#ifndef MACROS_H
#define MACROS_H 1


// macro signum function
#define sign(x) \
    ({ __typeof__ (x) _x = (x); \
        _x>0 ? 1 : _x<0 ? -1 : 0; })
// macro max function
#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })
#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _b : _a; })
//for keybinds
#define ctrl(x)           ((x) & 0x1f)

char* trim_whitespaces(char *str);

int is_all_space(char *string);
void shufflePreserveGraphemes(char* string);

#define handleAccents(Form) \
                    case '`': \
                        /* for accents*/\
                        if(config.autoaccent>0){\
                            wasJustTilde=false; \
                            /* double backtick should cancel accent*/\
                            if(!wasJustBacktick){\
                                wasJustBacktick= true;\
                                form_driver(Form, ch);\
                            }\
                            else{\
                                wasJustBacktick= false;\
                            }\
                            break;\
                        }\
                    case '~':\
                        if(config.autoaccent>0){\
                            wasJustBacktick=false;\
                            /* double tilde should cancel it*/\
                            if(!wasJustTilde){\
                                wasJustTilde = true;\
                                form_driver(Form, ch);\
                            }\
                            else{ /* double ~*/\
                                wasJustTilde = false;\
                            }\
                            break;\
                        }\
                    default:\
                        if(ch<128){\
                            if(config.autoaccent>0){\
                                /*handle accent marks*/\
                                if(wasJustBacktick){\
                                    switch(ch){\
                                        case 'a':\
                                            ch = L'á'; break;\
                                        case 'e':\
                                            ch = L'é'; break;\
                                        case 'i':\
                                            ch = L'í'; break;\
                                        case 'o':\
                                            ch = L'ó'; break;\
                                        case 'u':\
                                            ch = L'ú'; break;\
                                        case 'A':\
                                            ch = L'Á'; break;\
                                        case 'E':\
                                            ch = L'É'; break;\
                                        case 'I':\
                                            ch = L'Í'; break;\
                                        case 'O':\
                                            ch = L'Ó'; break;\
                                        case 'U':\
                                            ch = L'Ú'; break;\
                                        default:\
                                            form_driver(Form, REQ_NEXT_CHAR);\
                                            break;\
                                    }\
                                    form_driver(Form, REQ_DEL_PREV);\
                                }\
                                else if(wasJustTilde && ch=='n'){\
                                    ch = L'ñ';\
                                    form_driver(Form, REQ_DEL_PREV);\
                                }\
                                else if(wasJustTilde && ch=='N'){\
                                    ch = L'Ñ';\
                                    form_driver(Form, REQ_DEL_PREV);\
                                }\
                            }\
                            form_driver_w(Form, OK, ch);\
                        }\
                        wasJustBacktick = false;\
                        wasJustTilde = false;\
                        break;

#endif
