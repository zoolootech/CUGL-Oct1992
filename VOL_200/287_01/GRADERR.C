#include <stdio.h>
#include <gds.h>
#include <stdarg.h>

char *GRADERRMSG[] = {
        "Undefined Error Number : %d",
        "invalid environment slot number in EnvSave - %d",
        "invalid environment slot number in EnvRsto - %d",
        "environment slot not used - %d",
        "disk access error in BlockSave",
        "bad block file",
        "file not found - %s",
        "too many frame opened",
        "not sufficient memory to create new frame - %d*%d",
        "insufficient stack size for Fill",
        "PROGRAM ERROR IN %s LINE %d",
        "%s",
        "bad font type - %d",
        "too many font files loaded",
        "bad font file",
        "not sufficient memory to load new font",
        "no more environment slot for Draw",
        "invalid command in Draw - %c%c",
        "wrong number of parameter given to %c%c in Draw",
        "a digit expected after '%' or '&' in Draw",
        "invalid marker number - %d",
        "disk error in reading font file"
};

graderror(err_level, err_nu)
int err_level, err_nu;
{
    va_list arg_ptr;

    va_start(arg_ptr, err_nu);
    if ((err_level >= PRE_ERROR_LEVEL) && (err_level != 10)) {
        (*PRE_ERROR_FUNC)(err_level, err_nu, arg_ptr);
    } else {
        switch(err_level) {
        case 2:
            printf("GRAD warning : ");
            break;
        case 4:
            printf("GRAD error : ");
            break;
        case 6:
            printf("GRAD non-recoverable error : ");
            break;
        case 10:
            printf("GRAD fatal error : ");
            break;
        default:
            printf("Undefine error level %d : ",err_level);
            break;
        }
/*
        printf("error number %d\n",err_nu);
*/
        vprintf(GRADERRMSG[err_nu], arg_ptr);
        printf("\n");
    }
    if ((err_level >= POST_ERROR_LEVEL) && (err_level != 10)) {
        (*POST_ERROR_FUNC)(err_level, err_nu, arg_ptr);
    }
    va_end(arg_ptr);
    switch (err_level) {
    case 2:
        return;
    default:
        settext();
        cleanup(1);
    }
}

