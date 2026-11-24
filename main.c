#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEMORY_SIZE 4096
#define SCREEN_SIZE_X 64
#define SCREEN_SIZE_Y 32
#define STACK_SIZE 16
#define VARIABLE_REGISTER_SIZE 16
#define NUMBER_KEYS 16

static uint8_t const FONT[] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

typedef struct {
    /// Current instruction in memory
    uint16_t PC;
    /// Used to point at locations in memory
    uint16_t I;
    /// Remembers where to return after subroutines and functions
    uint16_t Stack[STACK_SIZE];
    /// Stack Pointer, tracks the top of the stack
    uint8_t SP;
    /// Decreases at a rate of 60hz till it reaches 0
    uint8_t delay_timer;
    /// Decreases at a rate of 60hz till it reaches 0
    uint8_t sound_timer;
    /// Variable registers
    uint8_t V[VARIABLE_REGISTER_SIZE];
    ///
    uint8_t Memory[MEMORY_SIZE];
    /// The Size of the display screen {X,Y} on or off
    bool Display[SCREEN_SIZE_X][SCREEN_SIZE_Y];
    /// Key pressed
    bool Keys[NUMBER_KEYS];
}machine_state;

/// Debugs to terminal
static void debug_termianl(machine_state *s) {
    for (int y = 0; y < SCREEN_SIZE_Y; y++) {
        for (int x = 0; x < SCREEN_SIZE_X; x++) {
            if (s->Display[x][y]) {
                printf("#");
            }
            else {
                printf(" ");
            }
        }
        printf("\n");
    }
}

/// Initializes the machine state to 0 except for PC and stores the font in memory
static void init_machine_state(machine_state *s) {
    memset(s, 0, sizeof(*s));
    s->PC = 0x200;
    memcpy(&s->Memory[0x050], FONT, sizeof(FONT));
}

/// Opens the ROM file and stores it in memory
static void open_file(machine_state *s) {
    // *TODO* Hard coded file system for now replace with user input
    FILE *f = fopen("/home/ghosthi/CLionProjects/chip8/ROMS/1-chip8-logo.ch8", "rb");
    if (f == NULL) {
        perror("fopen");
        exit(1);
    }

    fread(&s->Memory[0x200], 1, MEMORY_SIZE - 0x200, f);
    fclose(f);
}

int main(void) {
    machine_state state;
    init_machine_state(&state);
    open_file(&state);

        while (true){
            // FETCH
            uint8_t M1 = state.Memory[state.PC];
            uint8_t M2 = state.Memory[state.PC + 1];
            uint16_t opcode = M1 << 8 | M2;
            state.PC += 2;

            // DECODE
            uint8_t n1 = (opcode & 0xF000) >> 12;
            uint8_t X = (opcode & 0x0F00) >> 8;
            uint8_t Y = (opcode & 0x00F0) >> 4;
            uint8_t N = opcode & 0x000F;
            uint8_t NN = opcode & 0x00FF;
            uint16_t NNN = opcode & 0x0FFF;

            // EXECUTE
            switch (n1) {
                case 0x0:
                    if (opcode == 0x00E0) {
                        memset(state.Display, 0, sizeof(state.Display));
                    }
                    break;
                case 0x1:
                    state.PC = NNN;
                    break;
                case 0x6:
                    state.V[X] = NN;
                    break;
                case 0x7:
                    state.V[X] += NN;
                    break;
                case 0xA:
                    state.I = NNN;
                    break;
                case 0xD:{
                    uint8_t x = state.V[X]%SCREEN_SIZE_X;
                    uint8_t y = state.V[Y]%SCREEN_SIZE_Y;
                    state.V[0xF] = 0;

                    for (int row = 0; row < N; row++) {
                        uint8_t sprite_byte = state.Memory[state.I + row];

                        int y_pos = y + row;
                        if (y_pos >= SCREEN_SIZE_Y) {
                            break;
                        }

                        for (int col = 0; col < 8; col++) {
                            bool sprite_bit = sprite_byte & (0x80 >> col);

                            int x_pos = x + col;

                            if (x_pos >= SCREEN_SIZE_X) {
                                break;
                            }

                            if (sprite_bit) {
                                bool active = state.Display[x_pos][y_pos];
                                if (active) {
                                    state.V[0xF] = 1;
                                }
                                state.Display[x_pos][y_pos] = !state.Display[x_pos][y_pos];
                            }
                        }
                    }

                    debug_termianl(&state);

                    break;}

                default:
                    printf("%04X\n", opcode);
                    break;
            }
    }
}

