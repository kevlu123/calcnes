#include "io.h"
#include "utils.h"
#include "emulator.h"
#include <stdlib.h>

extern const char FILE_DATA[];

MySDL_RWops* MySDL_RWFromFile(const char* filename, const char* mode) {
    MySDL_RWops* rw = (MySDL_RWops*)malloc(sizeof(MySDL_RWops));
    if (rw == NULL) {
        return NULL;
    }
    rw->cur = 0;
    return rw;
}

void MySDL_RWread(MySDL_RWops *context, void *ptr, size_t size, size_t maxnum) {
    int i;
    for (i = 0; i < size * maxnum; i++) {
        ((char*)ptr)[i] = FILE_DATA[context->cur++];
    }
}

void MySDL_RWclose(MySDL_RWops *context) {
    free(context);
}

void LOG(enum LogLevel logLevel, const char* fmt, ...) {
}

//static uint64_t PERIOD;
static uint16_t TURBO_SKIP;

int InitEmu(struct Emulator* emulator) {
    char* genie = NULL;
    if (load_file("", genie, &emulator->mapper))
        return 1;
    emulator->type = emulator->mapper.type;
    if(emulator->type == NTSC) {
        //PERIOD = 1000000000 / NTSC_FRAME_RATE;
        TURBO_SKIP = NTSC_FRAME_RATE / NTSC_TURBO_RATE;
    }else{
        //PERIOD = 1000000000 / PAL_FRAME_RATE;
        TURBO_SKIP = PAL_FRAME_RATE / PAL_TURBO_RATE;
    }

    init_mem(emulator);
    init_ppu(emulator);
    init_cpu(emulator);

    return 0;
}

void TickEmu(struct Emulator* emulator) {
    struct JoyPad* joy1 = &emulator->mem.joy1;
    struct JoyPad* joy2 = &emulator->mem.joy2;
    struct PPU* ppu = &emulator->ppu;
    struct c6502* cpu = &emulator->cpu;

    if(ppu->frames % TURBO_SKIP == 0) {
        turbo_trigger(joy1);
        turbo_trigger(joy2);
    }

    //if(!emulator->pause)
    {
        // if ppu.render is set a frame is complete
        if(emulator->type == NTSC) {
            while (!ppu->render) {
                execute_ppu(ppu);
                execute_ppu(ppu);
                execute_ppu(ppu);
                execute(cpu);
            }
        }else{
            // PAL
            uint8_t check = 0;
            while (!ppu->render) {
                execute_ppu(ppu);
                execute_ppu(ppu);
                execute_ppu(ppu);
                check++;
                if(check == 5) {
                    // on the fifth run execute an extra ppu clock
                    // this produces 3.2 scanlines per cpu clock
                    execute_ppu(ppu);
                    check = 0;
                }
                execute(cpu);
            }
        }
        ppu->render = 0;
    }
}

void QuitEmu(struct Emulator* emulator) {
    free_mapper(&emulator->mapper);
}

extern size_t allocated = 0;
extern size_t lastMalloc = 0;

void* mymalloc(size_t size) {
    allocated += size;
    lastMalloc = size;
    return malloc(size);
}
