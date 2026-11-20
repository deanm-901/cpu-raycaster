#ifndef CONSOLE_H
#define CONSOLE_H

#include <SDL2/SDL.h>
#include "app.h"
#include "render.h"
#include "colors.h"

#define CONSOLE_STRING_COMMAND_MAX_CHAR 64
#define CONSOLE_MAX_COMMANDS 16

#define CONSOLE_MAX_ARGV 16

typedef struct _ConsoleArg {
    int argc;
    char* argv[CONSOLE_MAX_ARGV];
} ConsoleArg;

typedef struct _Console {
    char current_command[CONSOLE_STRING_COMMAND_MAX_CHAR];
    char log[CONSOLE_MAX_COMMANDS][CONSOLE_STRING_COMMAND_MAX_CHAR];
    int log_amm;
    int b_visible;
    Rect rect;
} Console;

#define Console_SetDefault(X) X = (Console){.rect=(Rect){.w=280, .h=10*(CONSOLE_MAX_COMMANDS)+2, .x=0, .y=210, .color=C_DRKGRY}};

void Console_ReadTextInput(Console* console, SDL_Event input);
void Console_Render(App* app, Console* console);
void Console_Update(Console* console);
void Console_Println(Console* console, const char* str);
void ConsoleCommands_Init();

#endif