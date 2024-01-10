#include "chip.h"
#include "render.h"

#include <signal.h>

void handle_signal()
{
    endwin();
    exit(0);
}

int main(int argc, char **argv)
{
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    if (argc != 3)
    {
        printf("Usage: c8 <file> <speed>");
        return 1;
    }

    uint8_t fontset[FONTSET_SIZE] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0,
        0x20, 0x60, 0x20, 0x20, 0x70,
        0xF0, 0x10, 0xF0, 0x80, 0xF0,
        0xF0, 0x10, 0xF0, 0x10, 0xF0,
        0x90, 0x90, 0xF0, 0x10, 0x10,
        0xF0, 0x80, 0xF0, 0x10, 0xF0,
        0xF0, 0x80, 0xF0, 0x90, 0xF0,
        0xF0, 0x10, 0x20, 0x40, 0x40,
        0xF0, 0x90, 0xF0, 0x90, 0xF0,
        0xF0, 0x90, 0xF0, 0x10, 0xF0,
        0xF0, 0x90, 0xF0, 0x90, 0x90,
        0xE0, 0x90, 0xE0, 0x90, 0xE0,
        0xF0, 0x80, 0x80, 0x80, 0xF0,
        0xE0, 0x90, 0x90, 0x90, 0xE0,
        0xF0, 0x80, 0xF0, 0x80, 0xF0,
        0xF0, 0x80, 0xF0, 0x80, 0x80};

    Chip8 chip;
    assert(chip_load_rom(&chip, argv[1]) == 0);

    memset(chip.video, 0, sizeof(chip.video));
    chip.pc = START_ADDRESS;

    for (size_t i = 0; i < FONTSET_SIZE; i++)
    {
        chip.memory[FONTSET_START_ADDRESS + i] = fontset[i];
    }

    srand(time(NULL));

    Render render;
    initscr();
    render_init(&render);

    bool quit = false;

    struct timeval last_cycle, current_time;
    gettimeofday(&last_cycle, NULL);

    while (!quit)
    {
        quit = render_input(chip.keypad);

        gettimeofday(&current_time, NULL);
        double elapsed_time = (current_time.tv_sec - last_cycle.tv_sec) +
                              (current_time.tv_usec - last_cycle.tv_usec) / 1000000.0;

        double speed = atof(argv[2]) * 100.0;

        if (elapsed_time >= (1.0 / speed))
        {
            chip_cycle(&chip);
            gettimeofday(&last_cycle, NULL);
        }

        render_update(&render, &chip);

        usleep(1000);
    }

    endwin();

    return 0;
}