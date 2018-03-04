
#include "terminal.h"
#include <conio.h>
#include <string.h>

int stage;
unsigned char bytestr[20];
int bs_idx;

#define STAGE_OPEN 0
#define STAGE_ESC 1
#define STAGE_CSI 2

void resetState() {
  stage = STAGE_OPEN;
  bs_idx = 0;
}

void initTerminal() {
  resetState();
}

int getParamA(int def) {
  int i = 0;
  while(i<bs_idx && bytestr[i] != ';') {
    i++;
  }
  if (i == 0) {
    return def;
  }
  bytestr[i] = 0; // null terminate string
  return atoi(bytestr);
}

int getParamB(int def) {
  int i = 0;
  while(i<bs_idx && bytestr[i] != ';') {
    i++;
  }
  // we should be pointing to beginning of param B in buffer.
  if (i == 0 || i == bs_idx) {
    return def;
  }
  unsigned char* paramb = bytestr + i;
  if (paramb[0] == ';') {
    paramb++;
  }
  i = 0;
  while(i<bs_idx && paramb[i] != ';') {
    i++;
  }
  paramb[i] = 0;
  return atoi(paramb);
}

void cursorUp(int lines) {
  int y = wherey() - lines;
  if (y < 0) {
    y = 0;
  }
  gotoy(y);
}

void cursorDown(int lines) {
  int y = wherey() + lines;
  unsigned char scx;
  unsigned char scy;
  screensize(&scx, &scy);
  scy--;
  if (y > scy) {
    y = scy;
  }
  gotoy(y);
}

void cursorRight(int cols) {
  int x = wherex() + cols;
  unsigned char scx;
  unsigned char scy;
  screensize(&scx, &scy);
  scx--;
  if (x > scx) {
    x = scx;
  }
  gotox(x);
}

void cursorLeft(int cols) {
  int x = wherex() + cols;
  if (x < 0) {
    x = 0;
  }
  gotox(x);
}

void cursorGoto(int x, int y) {
  unsigned char scx;
  unsigned char scy;
  screensize(&scx, &scy);
  scx--;
  scy--;
  if (x > scx) {
    x = scx;
  } else if (x < 0) {
    x = 0;
  }
  if (y > scy) {
    y = scy;
  } else if (y < 0) {
    y = 0;
  }

  gotoxy(x, y);
}

void doCommand(unsigned char c) {
  // Note, ANSI cursor locations (1,1) upper left corner.
  switch (c) {
    case 'A': // cursor UP, 1 param, default 1
      cursorUp(getParamA(1));
      break;
    case 'B': // cursor DOWN, 1 param, default 1
      cursorDown(getParamA(1));
      break;
    case 'C': // cursor RIGHT, 1 param, default 1
      cursorRight(getParamA(1));
      break;
    case 'D': // cursor LEFT, 1 param, default 1
      cursorLeft(getParamA(1));
      break;
    case 'E': // cursor next line, 1 param, default 1
      cursorDown(getParamA(1));
      gotox(0);
      break;
    case 'F': // cursor prev line, 1 param, default 1
      cursorUp(getParamA(1));
      gotox(0);
      break;
    case 'G': // set cursor column, 1 param, default 1
      cursorGoto(getParamA(1),wherey() + 1);
      break;
    case 'H': // set position, 2 param, defaults 1, 1
    case 'f': // synonym
      cursorGoto(getParamB(1), getParamA(1));
      break;
    case 'J': // erase in display, 1 param, default 0
      break;
    case 'K': // erase in line, 1 param, default 0
      break;
    case 'S': // scroll up lines, 1 param, default 1
      break;
    case 'T': // scroll down lines, 1 param, default 1
      break;
    case 'm': // color (SGR), n params
      break;
  }
}

void terminalDisplay(unsigned char c) {
  if (stage == STAGE_OPEN) {
    if (c == 27) {
      stage = STAGE_ESC;
    } else {
      cputc(c);
    }
  } else if (stage == STAGE_ESC) {
    if (c == '[') {
      // command begins
      stage = STAGE_CSI;
      bs_idx = 0;
    } else {
      stage = STAGE_OPEN;
      cputc(27);
      cputc(c);
    }
  } else if (stage == STAGE_CSI) { 
    if (c >= 0x40 && c <= 0x7E) {
      // command complete
      doCommand(c);
      stage = STAGE_OPEN;
    } else if (c >= 0x30 && c <= 0x3F) {
      // capture params. 
      bytestr[bs_idx] = c;
      bs_idx++;
    } else {
      // this is basically an error state... ignoring the command.
      stage = STAGE_OPEN;
    }
  }
}
