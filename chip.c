#include "chip.h"

int chip_load_rom(Chip8 *chip, char *rom_path)
{
    FILE *rom = fopen(rom_path, "rb");
    assert(rom != NULL);

    fseek(rom, 0, SEEK_END);
    long rom_size = ftell(rom);
    rewind(rom);

    char *rom_buffer = (char *)malloc(sizeof(char) * rom_size);
    assert(rom_buffer != NULL);

    size_t result = fread(rom_buffer, sizeof(char), (size_t)rom_size, rom);
    assert(result == (size_t)rom_size);

    if ((4096 - START_ADDRESS) > rom_size)
    {
        for (int i = 0; i < rom_size; ++i)
        {
            chip->memory[i + START_ADDRESS] = (uint8_t)rom_buffer[i];
        }
    }
    else
    {
        perror("ROM too large");

        return 1;
    }

    fclose(rom);
    free(rom_buffer);

    return 0;
}

// OPCODES:

// 00E0 - CLS
void chip_op00E0(Chip8 *chip)
{
    memset(chip->video, 0, sizeof(chip->video));
}

// 00EE - RET
void chip_op00EE(Chip8 *chip)
{
    chip->sp--;
    chip->pc = chip->stack[chip->sp];
}

// 1nnn - JP addr
void chip_op1nnn(Chip8 *chip)
{
    uint16_t address = chip->opcode & 0x0FFFu;
    chip->pc = address;
}

// 2nnn - CALL addr
void chip_op2nnn(Chip8 *chip)
{
    uint16_t address = chip->opcode & 0x0FFFu;
    chip->stack[chip->sp] = chip->pc;
    chip->sp++;
    chip->pc = address;
}

// 3xkk - SE Vx, byte
void chip_op3xkk(Chip8 *chip)
{
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
    uint8_t byte = chip->opcode & 0x00FFu;

    if (chip->registers[Vx] == byte)
    {
        chip->pc += 2;
    }
}

// 4xkk - SNE Vx, byte
void chip_op4xkk(Chip8 *chip)
{
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
    uint8_t byte = chip->opcode & 0x00FFu;

    if (chip->registers[Vx] != byte)
    {
        chip->pc += 2;
    }
}

// 5xy0 - SE Vx, Vy
void chip_op5xy0(Chip8 *chip)
{
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (chip->opcode & 0x00F0u) >> 4u;

    if (chip->registers[Vx] == chip->registers[Vy])
    {
        chip->pc += 2;
    }
}

// 6xkk - LD Vx, byte
void chip_op6xkk(Chip8 *chip)
{
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
    uint8_t byte = chip->opcode & 0x00FFu;

    chip->registers[Vx] = byte;
}

// 7xkk - ADD Vx, byte
void chip_op7xkk(Chip8 *chip)
{
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
    uint8_t byte = chip->opcode & 0x00FFu;

    chip->registers[Vx] += byte;
}

// 8xy0 - LD Vx, Vy
void chip_op8xy0(Chip8 *chip)
{
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (chip->opcode & 0x00F0u) >> 4u;

    chip->registers[Vx] = chip->registers[Vy];
}

// 8xy1 - OR Vx, Vy
void chip_op8xy1(Chip8 *chip)
{
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (chip->opcode & 0x00F0u) >> 4u;

    chip->registers[Vx] |= chip->registers[Vy];
}

// 8xy2 - AND Vx, Vy
void chip_op8xy2(Chip8 *chip)
{
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (chip->opcode & 0x00F0u) >> 4u;

    chip->registers[Vx] &= chip->registers[Vy];
}

// 8xy3 - XOR Vx, Vy
void chip_op8xy3(Chip8 *chip)
{
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (chip->opcode & 0x00F0u) >> 4u;

    chip->registers[Vx] ^= chip->registers[Vy];
}

// 8xy4 - ADD Vx, Vy
void chip_op8xy4(Chip8 *chip)
{
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (chip->opcode & 0x00F0u) >> 4u;
    uint16_t sum = chip->registers[Vx] + chip->registers[Vy];

    if (sum > 255u)
    {
        chip->registers[0xF] = 1;
    }
    else
    {
        chip->registers[0xF] = 0;
    }

    chip->registers[Vx] = sum & 0xFFu;
}

// 8xy5 - SUB Vx, Vy
void chip_op8xy5(Chip8 *chip)
{
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (chip->opcode & 0x00F0u) >> 4u;

    if (chip->registers[Vx] > chip->registers[Vy])
    {
        chip->registers[0xF] = 1;
    }
    else
    {
        chip->registers[0xF] = 0;
    }

    chip->registers[Vx] -= chip->registers[Vy];
}

// 8xy6 - SHR Vx {, Vy}
void chip_op8xy6(Chip8 *chip)
{
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;

    chip->registers[0xF] = chip->registers[Vx] & 0x1u;
    chip->registers[Vx] >>= 1;
}

// 8xy7 - SUBN Vx, Vy
void chip_op8xy7(Chip8 *chip)
{
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (chip->opcode & 0x00F0u) >> 4u;

    if (chip->registers[Vy] > chip->registers[Vx])
    {
        chip->registers[0xF] = 1;
    }
    else
    {
        chip->registers[0xF] = 0;
    }

    chip->registers[Vx] = chip->registers[Vy] - chip->registers[Vx];
}

// 8xyE - SHL Vx {, Vy}
void chip_op8xyE(Chip8 *chip)
{
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;

    chip->registers[0xF] = chip->registers[Vx] >> 7u;
    chip->registers[Vx] <<= 1;
}

// 9xy0 - SNE Vx, Vy
void chip_op9xy0(Chip8 *chip)
{
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (chip->opcode & 0x00F0u) >> 4u;

    if (chip->registers[Vx] != chip->registers[Vy])
    {
        chip->pc += 2;
    }
}

// Annn - LD I, addr
void chip_opAnnn(Chip8 *chip)
{
    uint16_t address = chip->opcode & 0x0FFFu;
    chip->index = address;
}

// Bnnn - JP V0, addr
void chip_opBnnn(Chip8 *chip)
{
    uint16_t address = chip->opcode & 0x0FFFu;
    chip->pc = chip->registers[0] + address;
}

// Cxkk - RND Vx, byte
void chip_opCxkk(Chip8 *chip)
{
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
    uint8_t byte = chip->opcode & 0x00FFu;

    chip->registers[Vx] = rand() & byte;
}

// Dxyn - DRW Vx, Vy, nibble
void chip_opDxyn(Chip8 *chip)
{
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (chip->opcode & 0x00F0u) >> 4u;
    uint8_t height = chip->opcode & 0x000Fu;

    uint8_t xPos = chip->registers[Vx] % 64;
    uint8_t yPos = chip->registers[Vy] % 32;

    chip->registers[0xF] = 0;

    for (unsigned int row = 0; row < height; row++)
    {
        uint8_t sprite_byte = chip->memory[chip->index + row];

        for (unsigned int col = 0; col < 8; col++)
        {
            uint8_t sprite_pixel = sprite_byte & (0x80u >> col);
            uint32_t *screen_pixel = &chip->video[(yPos + row) * 64 + (xPos + col)];

            if (sprite_pixel)
            {
                if (*screen_pixel == 0xFFFFFFFF)
                {
                    chip->registers[0xF] = 1;
                }

                *screen_pixel ^= 0xFFFFFFFF;
            }
        }
    }
}

// Ex9E - SKP Vx
void chip_opEx9E(Chip8 *chip)
{
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
    uint8_t key = chip->registers[Vx];

    if (chip->keypad[key])
    {
        chip->pc += 2;
    }
}

// ExA1 - SKNP Vx
void chip_opExA1(Chip8 *chip)
{
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
    uint8_t key = chip->registers[Vx];

    if (!chip->keypad[key])
    {
        chip->pc += 2;
    }
}

// Fx07 - LD Vx, DT
void chip_opFx07(Chip8 *chip)
{
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;

    chip->registers[Vx] = chip->delay_timer;
}

// Fx0A - LD Vx, K
void chip_opFx0A(Chip8 *chip)
{
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
    bool key_pressed = false;

    for (int i = 0; i < 16; i++)
    {
        if (chip->keypad[i])
        {
            chip->registers[Vx] = i;
            key_pressed = true;
        }
    }

    if (!key_pressed)
    {
        chip->pc -= 2;
    }
}

// Fx15 - LD DT, Vx
void chip_opFx15(Chip8 *chip)
{
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;

    chip->delay_timer = chip->registers[Vx];
}

// Fx18 - LD ST, Vx
void chip_opFx18(Chip8 *chip)
{
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;

    chip->sound_timer = chip->registers[Vx];
}

// Fx1E - ADD I, Vx
void chip_opFx1E(Chip8 *chip)
{
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;

    chip->index += chip->registers[Vx];
}

// Fx29 - LD F, Vx
void chip_opFx29(Chip8 *chip)
{
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
    uint8_t digit = chip->registers[Vx];

    chip->index = FONTSET_START_ADDRESS + (5 * digit);
}

// Fx33 - LD B, Vx
void chip_opFx33(Chip8 *chip)
{
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;
    uint8_t value = chip->registers[Vx];

    chip->memory[chip->index + 2] = value % 10;
    value /= 10;

    chip->memory[chip->index + 1] = value % 10;
    value /= 10;

    chip->memory[chip->index] = value % 10;
}

// Fx55 - LD [I], Vx
void chip_opFx55(Chip8 *chip)
{
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;

    for (uint8_t i = 0; i <= Vx; i++)
    {
        chip->memory[chip->index + i] = chip->registers[i];
    }
}

// Fx65 - LD Vx, [I]
void chip_opFx65(Chip8 *chip)
{
    uint8_t Vx = (chip->opcode & 0x0F00u) >> 8u;

    for (uint8_t i = 0; i <= Vx; i++)
    {
        chip->registers[i] = chip->memory[chip->index + i];
    }
}

// returns true if pc should be incremented
bool chip_execute_opcode(Chip8 *chip, uint16_t opcode)
{
    switch (opcode & 0xF000u)
    {
    case 0x0000:
        switch (opcode & 0x000F)
        {
        case 0x0000:
            chip_op00E0(chip);
            return true;
        case 0x000E:
            chip_op00EE(chip);
            return true;
        }

        return true;

    case 0x1000:
        chip_op1nnn(chip);
        // jmp shouldnt increment pc
        return false;

    case 0x2000:
        chip_op2nnn(chip);
        return false;

    case 0x3000:
        chip_op3xkk(chip);
        return true;

    case 0x4000:
        chip_op4xkk(chip);
        return true;

    case 0x5000:
        chip_op5xy0(chip);
        return true;

    case 0x6000:
        chip_op6xkk(chip);
        return true;

    case 0x7000:
        chip_op7xkk(chip);
        return true;

    case 0x8000:
        switch (opcode & 0x000F)
        {
        case 0x0000:
            chip_op8xy0(chip);
            return true;

        case 0x0001:
            chip_op8xy1(chip);
            return true;

        case 0x0002:
            chip_op8xy2(chip);
            return true;

        case 0x0003:
            chip_op8xy3(chip);
            return true;

        case 0x0004:
            chip_op8xy4(chip);
            return true;

        case 0x0005:
            chip_op8xy5(chip);
            return true;

        case 0x0006:
            chip_op8xy6(chip);
            return true;

        case 0x0007:
            chip_op8xy7(chip);
            return true;

        case 0x000E:
            chip_op8xyE(chip);
            return true;
        }

        return true;

    case 0x9000:
        chip_op9xy0(chip);
        return true;

    case 0xA000:
        chip_opAnnn(chip);
        return true;

    case 0xB000:
        chip_opBnnn(chip);
        return true;

    case 0xC000:
        chip_opCxkk(chip);
        return true;

    case 0xD000:
        chip_opDxyn(chip);
        return true;

    case 0xE000:
        switch (opcode & 0x00FF)
        {
        case 0x009E:
            chip_opEx9E(chip);
            return true;

        case 0x00A1:
            chip_opExA1(chip);
            return true;
        }

        return true;

    case 0xF000:
        switch (opcode & 0x00FF)
        {
        case 0x0007:
            chip_opFx07(chip);
            return true;

        case 0x000A:
            chip_opFx0A(chip);
            return true;

        case 0x0015:
            chip_opFx15(chip);
            return true;

        case 0x0018:
            chip_opFx18(chip);
            return true;

        case 0x001E:
            chip_opFx1E(chip);
            return true;

        case 0x0029:
            chip_opFx29(chip);
            return true;

        case 0x0033:
            chip_opFx33(chip);
            return true;

        case 0x0055:
            chip_opFx55(chip);
            return true;

        case 0x0065:
            chip_opFx65(chip);
            return true;
        }
    }

    printf("Unknown opcode: 0x%X\n", opcode);
    return true;
}

void chip_cycle(Chip8 *chip)
{
    chip->opcode = (chip->memory[chip->pc] << 8u) | chip->memory[chip->pc + 1];

    bool should_increment = chip_execute_opcode(chip, chip->opcode);

    if (should_increment)
    {
        chip->pc += 2;
    }

    if (chip->delay_timer > 0)
    {
        chip->delay_timer--;
    }

    if (chip->sound_timer > 0)
    {
        if (chip->sound_timer == 1)
        {
            printf("beep...\n");
        }

        chip->sound_timer--;
    }
}