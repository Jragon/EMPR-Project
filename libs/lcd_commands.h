#pragma once

// taken from section ten of the PCF2119x driver datasheet

/*
    CO - 0 - last control byte
       1 - another control byte follows
    RS - 0 - instruction register selected
       1 - data register selected
*/
#define LCDCMD_ControlByte(CO, RS) (0x00 | (CO << 7) | (RS << 6))

/*
    DL - 0 - 4bit mode
       1 - 8bit mode // n.b. doesn't matter for i2c operation
    M  - 0 - 1 line x 32 chars
       1 - 2 line x 16 chars // this is the one for our display ;)
    SL - 0 - idk use 0
    H  - 0 - standard instruction set
       1 - extended instruction set
*/
#define LCDCMD_FunctionSet(DL, M, SL, H) (0x20 | (DL << 4) | (M << 2) | (SL << 1) | H)
#define LCDCMD_StandardFunctionSet LCDCMD_FunctionSet(1, 1, 0, 0)
#define LCDCMD_ExtendedFunctionSet LCDCMD_FunctionSet(1, 1, 0, 1)

/*********************
    Standard Commands
 *********************/

/*
    writes the blank character code (0x20) into all DDRAM addresses
*/
#define LCDCMD_ClearDisplay 0x01

/*
    cursor goes back to top left
*/
#define LCDCMD_ReturnHome 0x02

/*
    I_D - 0 - RAM address decrements: cursor moves to the left
      - 1 - RAM address increments: cursor moves to the right
    S   - 0 - display does not shift
      - 1 - display shifts // appears as the whole display moving
*/
#define LCDCMD_EntryModeSet(I_D, S) (0x04 | I_D << 1 | S)

/*
    D - 0 - display off
      1 - display on
    C - 0 - cursor off
      1 - curson on
    B - 0 - char blink off
      1 - char blink on
*/
#define LCDCMD_DisplayCtl(D, C, B) (0x08 | D << 2 | C << 1 | B << 1)

/*
    SC - 0 - move cursor
       1 - shift display
    RL - 0 - left
       1 - right
*/
#define LCDCMD_CursDispShift(SC, RL) (0x10 | SC << 3 | RL << 2)

/*
    ACG - 6 bits - set CGRAM address
*/
#define LCDCMD_SetCGRAM(ACG) (0x40 | (ACG & 0x3F))

/*
    ADD - 7 bits - set DDRAM address
*/
#define LCDCMD_SetDDRAM(ADD) (0x80 | (ADD & 0x7F))

/*********************
    Extended Commands
 *********************/

/*
    P - 0 - left to right
      1 - right to left
    Q - 0 - top to bottom
      1 - bottom to top
*/
#define LCDCMD_DispConf(P, Q) (0x04 | P << 1 | Q)

/*
    TC - 0 to 3 - sets temp coeff value
    default 0
*/
#define LCDCMD_TempCtl(TC) (0x10 | TC)
#define LCDCMD_DefaultTempCtl LCDCMD_TempCtl(0)

/*
    S - 0 to 2 - sets voltage multiplier stages from 1 to 3
    default 2
*/
#define LCDCMD_HVGen(S) (0x40 | S)
#define LCDCMD_DefaultHVGen LCDCMD_HVGen(2)

/*
    V - 0 - set register Va
      1 - set register Vb
    D - 6 bits - data for Va or Vb
    default Va: 0x1F
*/
#define LCDCMD_VLCDSet(V, D) (0x80 | V << 6 | (D & 0x3F))
#define LCDCMD_DefaultVLCDSet LCDCMD_VLCDSet(0, 0x1F)
