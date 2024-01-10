#ifndef CHIP_H
#define CHIP_H

#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <sys/time.h>
#include <ncurses.h>
#include <unistd.h>

#define FONTSET_SIZE 80
#define START_ADDRESS 0x200
#define FONTSET_START_ADDRESS 0x50

typedef struct Chip8
{
    uint8_t registers[16];
    uint8_t memory[4096];
    uint16_t index;
    uint16_t pc;
    uint16_t stack[16];
    uint8_t sp;
    uint8_t delay_timer;
    uint8_t sound_timer;
    uint8_t keypad[16];
    uint32_t video[64 * 32];
    uint16_t opcode;
} Chip8;

int chip_load_rom(Chip8 *chip, char *rom_path);

void chip_cycle(Chip8 *chip);

#endif // CHIP_H