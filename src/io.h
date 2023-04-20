
#include <stdlib.h>

typedef struct MySDL_RWops {
    int cur;
} MySDL_RWops;

MySDL_RWops* MySDL_RWFromFile(const char* filename, const char* mode);
void MySDL_RWread(MySDL_RWops *context, void *ptr, size_t size, size_t maxnum);
void MySDL_RWclose(MySDL_RWops *context);

struct Emulator;
int InitEmu(struct Emulator* emulator);
void TickEmu(struct Emulator* emulator);
void QuitEmu(struct Emulator* emulator);

extern size_t allocated;
extern size_t lastMalloc;

void* mymalloc(size_t size);