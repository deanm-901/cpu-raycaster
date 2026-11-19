#ifndef CONSOLE_H
#define CONSOLE_H

#include <SDL2/SDL.h>
#include "app.h"
#include "render.h"
#include "colors.h"

#define CONSOLE_STRING_COMMAND_MAX_CHAR 64
#define CONSOLE_MAX_COMMANDS 16

typedef struct _Console {
    char current_command[CONSOLE_STRING_COMMAND_MAX_CHAR];
    char saved_commands[CONSOLE_MAX_COMMANDS][CONSOLE_STRING_COMMAND_MAX_CHAR];
    int saved_commands_amm;
    int b_text_input;
    Rect rect;
} Console;

#define Console_SetDefault(X) X = (Console){.rect=(Rect){.w=280, .h=210, .x=0, .y=210, .color=C_BLACK}};

void Console_ReadTextInput(Console* console, SDL_Event input);
void Console_Render(App* app, Console* console);
void Console_Update(Console* console);

#endif