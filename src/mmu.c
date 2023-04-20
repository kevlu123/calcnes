#include <string.h>
#include "mmu.h"
#include "emulator.h"
#include "utils.h"

void init_mem(struct Emulator* emulator){
    Memory* mem = &emulator->mem;
    mem->emulator = emulator;
    mem->mapper = &emulator->mapper;

    memset(mem->RAM, 0, RAM_SIZE);
    init_joypad(&mem->joy1, 0);
    init_joypad(&mem->joy2, 1);
}

uint8_t* get_ptr(Memory* mem, uint16_t address){
    if(address < 0x2000)
        return mem->RAM + (address % 0x800);
    if(address > 0x6000 && address < 0x8000 && mem->mapper->save_RAM != NULL)
        return mem->mapper->save_RAM + (address - 0x6000);
    LOG(ERROR, "Could not access pointer to address 0x%x", address);
    // continuing will only cause a segmentation fault
    //exit(EXIT_FAILURE);
    return 0;
}

void write_mem(Memory* mem, uint16_t address, uint8_t value){

    if(address < RAM_END) {
        mem->RAM[address % RAM_SIZE] = value;
        return;
    }

    // resolve mirrored registers
    if(address < IO_REG_MIRRORED_END)
        address = 0x2000 + (address - 0x2000) % 0x8;

    // handle all IO registers
    if(address < IO_REG_END){
        PPU* ppu = &mem->emulator->ppu;

        switch (address) {
            case PPU_CTRL:
                set_ctrl(ppu, value);
                break;
            case PPU_MASK:
                ppu->mask = value;
                break;
            case PPU_SCROLL:
                set_scroll(ppu, value);
                break;
            case PPU_ADDR:
                set_address(ppu, value);
                break;
            case PPU_DATA:
                write_ppu(ppu, value);
                break;
            case OAM_ADDR:
                set_oam_address(ppu, value);
                break;
            case OAM_DMA:
                dma(ppu, value);
                break;
            case OAM_DATA:
                write_oam(ppu, value);
                break;
            case JOY1:
                write_joypad(&mem->joy1, value);
                write_joypad(&mem->joy2, value);
                break;
            default:
                LOG(DEBUG, "Cannot write to register 0x%X", address);
                break;
        }
        return;
    }

    if(address < 0x6000){
        LOG(DEBUG, "Attempted to write to unavailable expansion ROM");
        return;
    }

    if(address < 0x8000){
        // extended ram
        if(mem->mapper->save_RAM != NULL)
            mem->mapper->save_RAM[address - 0x6000] = value;
        else {
            LOG(DEBUG, "Attempted to write to non existent save RAM");
        }
    }else{
        // PRG
        mem->mapper->write_PRG(mem->mapper, address, value);
    }
}
uint8_t read_mem(Memory* mem, uint16_t address){
    if(address < RAM_END)
        return mem->RAM[address % RAM_SIZE];
    
    // resolve mirrored registers
    if(address < IO_REG_MIRRORED_END)
        address = 0x2000 + (address - 0x2000) % 0x8;

    // handle all IO registers
    if(address < IO_REG_END){
        PPU* ppu = &mem->emulator->ppu;
        switch (address) {
            case PPU_STATUS:
                return read_status(ppu);
            case OAM_DATA:
                return read_oam(ppu);
            case PPU_DATA:
                return read_ppu(ppu);
            case JOY1:
                return read_joypad(&mem->joy1);
            case JOY2:
                return read_joypad(&mem->joy2);
            default:
                LOG(DEBUG, "Cannot read from register 0x%X", address);
                return 0;
        }
    }

    if(address < 0x6000){
        LOG(DEBUG, "Attempted to read from unavailable expansion ROM");
        return 0;
    }

    if(address < 0x8000){
        // save RAM
        if(mem->mapper->save_RAM != NULL)
            return mem->mapper->save_RAM[address - 0x6000];
        else {
            LOG(DEBUG, "Attempted to read from non existent save RAM");
            return 0;
        }

    }else{
        // PRG
        return mem->mapper->read_PRG(mem->mapper, address);
    }
}
