#include menu.h
extern char hist[];
static struct {
    char *question = "Available Fonts are:";
    MLINE m[] = {
        {"1. Courier",action,&hist},
        {"2. Courier bold",action,&hist},
        {"3. Line printer compressed",action,&hist};
    };
    int eos = NULL;
} m24;
static struct {
    char *question = "Skip over perforation:";
    MLINE m[] = {
        {"1. Off",action,&hist},
        {"2. On",action,&hist};
    };
    int eos = NULL;
} m23;
static struct {
    char *question = "Select page size.\n\
Remember all except letter size must be hand fed.";
    MLINE m[] = {
        {"1. Executive",action,&hist},
        {"2. Letter",action,&hist},
        {"3. Legal",action,&hist},
        {"4. A4",action,&hist},
        {"5. Monarch (envelope)",action,&hist},
        {"6. Commercial 10 (envelope)",action,&hist},
        {"7. International DL (envelope)",action,&hist},
        {"8. International C5 (envelope)",action,&hist};
    };
    int eos = NULL;
} m22;
static struct {
    char *question = "Choose one.";
    MLINE m[] = {
        {"1. Portrait(vertical)",action,&hist},
        {"2. Landscape(horizontal)",action,&hist};
    };
    int eos = NULL;
} m21;
extern int copies();
static struct {
    char *question = "Which do you want to change?";
    MLINE m[] = {
        {"1. Page orientation",menu,&m21},
        {"2. Page size",menu,&m22},
        {"3. Skip over perforation",menu,&m23},
        {"4. Font",menu,&m24},
        {"5. Number of copies",copies,0};
    };
    int eos = NULL;
} m2;
extern int display(),finish(),exit();
struct {
    char *question = "Select One:";
    MLINE m[] = {
        {"1. Display current printer setup",display,0},
        {"2. Alter printer setup",menu,&m2},
        {"3. Write printer control codes and exit",finish,0},
        {"4. Just exit",exit,0};
    };
    int eos = NULL;
} m;
nu,&m22},
        {"3. Skip over perforation",menu,&m23},
        {"4. Font",menu,&m24},
        {"5. Number of copies",copi