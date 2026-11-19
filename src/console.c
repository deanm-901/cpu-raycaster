#include "console.h"
#include "colors.h"
#include <string.h>

static void Console_FireCurrentCommand(Console* console) {
    ConsoleArg arg;

    char* token = strtok(console->current_command, " \t\n");
    while (token != NULL && arg.argc < CONSOLE_MAX_ARGV - 1) {
        arg.argv[arg.argc++] = token;
        token = strtok(NULL, " \t\n");
    }

    // TODO: arg.argv[ar]

    memcpy(console->log[console->log_amm++], console->current_command, CONSOLE_STRING_COMMAND_MAX_CHAR);
    
    console->current_command[0] = '\0';
}

void Console_ReadTextInput(Console* console, SDL_Event input) {
    if (input.type == SDL_KEYDOWN && input.key.keysym.scancode == SDL_SCANCODE_GRAVE) console->b_visible = !console->b_visible;

    if (!console->b_visible) return;

    if (input.type == SDL_QUIT) exit(0);

    if (input.type == SDL_KEYDOWN) {
        if (input.key.keysym.scancode == SDL_SCANCODE_BACKSPACE) {
            size_t len = strnlen(console->current_command, CONSOLE_STRING_COMMAND_MAX_CHAR);
            len = (len>1)?len:1;
            console->current_command[len - 1] = '\0';
        }
        if (input.key.keysym.scancode == SDL_SCANCODE_RETURN) {
            Console_FireCurrentCommand(console);
        }
    }

    if (input.type != SDL_TEXTINPUT) return;

    strncat(console->current_command, input.text.text, CONSOLE_STRING_COMMAND_MAX_CHAR);
}

void Console_Render(App* app, Console* console) {
    if (!console->b_visible) return;

    renderRect(app, console->rect);
    renderText(app, getImageData("font2.png"), 5, 8, console->current_command, console->rect.x+2, console->rect.y+console->rect.h-10, 1, C_WHITE);

    for (int i=0; i<console->log_amm; i++) {
        renderText(app, getImageData("font2.png"), 5, 8, console->log[i], console->rect.x+2, console->rect.y+console->rect.h-10-((console->log_amm-i)*10), 1, C_GRAY);
    }
}

void Console_Update(Console* console) {

}