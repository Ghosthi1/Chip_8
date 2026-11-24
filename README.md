# CHIP-8 Emulator (C)

A CHIP-8 interpreter written in C17, built from scratch while following Tobias V. Langhoff's guide,
[*Guide to making a CHIP-8 emulator*](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/).

The emulator implements the core fetch, decode and execute cycle and enough of the instruction set to run
the CHIP-8 logo test ROM. It prints the display to the terminal.

## Output

Running the logo test ROM [`1-chip8-logo.ch8`](https://github.com/Timendus/chip8-test-suite) produces this terminal output:

```

            ##### #                    #          ##
              #     ## #   ##  ###   ### #  #  ##  #
              #   # # # # #  # #  # #  # #  # #
              #   # #   # #### #  # #  # #  #  #
              #   # #   # #    #  # #  # #  #   #
              #   # #   #  ### #  #  ###  ### ##


           #####   ##       ##  #####           #######
          ####### ###      ### #######         ###   ###
         ###   ## ###      ### ###  ###       ###     ##
        ###       ###          ###   ##       ###     ##
        ###  # #  ###       ## ###   ##       ###     ##
        ###       ######   ### ###   ##        ###   ##
        ### #   # #######  ### ###   ## ####    ######
        ###  ###  ###  ### ### ###  ### ####   ###  ###
        ###       ###   ## ### #######        ###    ###
        ###       ###   ## ### ######        ###      ##
        ###       ###   ## ### ###           ###      ##
        ###       ###   ## ### ### # #   ### ###      ##
         ###   ## ###   ## ### ### ###    ## ####    ###
          ####### ###   ## ### ###   #     #  #########
           #####  ###   ## ### ###   # # ###   #######


             ###  ##   ## #       ##      # #    ##
              #  #  # #   ###    #   #  #   ### #  #
              #  ####  #  #       #  #  # # #   ####
              #  #      # #        # #  # # #   #
              #   ### ##   ##    ##   ### #  ##  ###
```

## What's implemented

**Machine state:** a single `machine_state` struct containing:

| Component | Representation |
|---|---|
| Memory | 4096 bytes (`uint8_t[4096]`) |
| Display | 64 × 32 monochrome pixels (`bool[64][32]`) |
| Program counter, index register `I` | `uint16_t` |
| Stack and stack pointer | 16 × `uint16_t`, `uint8_t` |
| Variable registers `V0`–`VF` | 16 × `uint8_t` |
| Delay and sound timers | `uint8_t` |
| Keypad | 16 × `bool` |

**Startup:**
- zero-initializes the machine,
- loads the built-in hex font into memory at `0x050`,
- loads a ROM from disk into memory at `0x200`.

**Instructions (6 of 34):**

| Opcode | Meaning |
|---|---|
| `00E0` | Clear the screen |
| `1NNN` | Jump to address `NNN` |
| `6XNN` | Set `VX` to `NN` |
| `7XNN` | Add `NN` to `VX` (no carry flag) |
| `ANNN` | Set `I` to `NNN` |
| `DXYN` | Draw an `N`-row sprite from memory at `I` to (`VX`, `VY`) with XOR, clipping at the screen edges, and setting `VF` on collision |

Unimplemented opcodes are printed to the terminal as they're encountered.

## Concepts practised

- Structs, `typedef`, and passing structs by pointer (`.` vs `->`)
- Fixed-size arrays, including 2D arrays
- Bit manipulation: masks (`&`), shifts (`<<`, `>>`), and combining bytes (`|`)
- Binary file I/O with `fopen`/`fread`, plus `memcpy`/`memset`
- `switch`-based instruction decoding and block scoping within `case` labels

## Building and running

Requirements: a C17 compiler and CMake.

```sh
cmake -S . -B build
cmake --build build
./build/chip8
```

> **Note:** The ROM path is currently hard-coded in `open_file()` in `main.c`. Change it to point at a `.ch8`
> file on your machine before building. The test ROMs aren't included in this repository; download them from
> the [Timendus CHIP-8 test suite](https://github.com/Timendus/chip8-test-suite).

The emulator runs in an infinite loop, as real CHIP-8 programs end by jumping to themselves. Stop it with Ctrl+C.

## Next steps

- Implement the remaining 28 instructions: subroutines, conditional skips, arithmetic and logic, timers, keypad and memory operations
- Take the ROM path as a command-line argument
- Replace the terminal output with an SDL2 window, keyboard input and 60 Hz timers
- Pass the rest of the Timendus test suite (`3-corax+`, `4-flags`, `5-quirks`, and so on)
