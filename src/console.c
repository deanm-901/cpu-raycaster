#include "console.h"
#include "colors.h"
#include <string.h>

static void Console_FireCurrentCommand(Console* console) {
    memcpy(console->saved_commands[console->saved_commands_amm++], console->current_command, CONSOLE_STRING_COMMAND_MAX_CHAR);
    
    console->current_command[0] = '\0';
}

void Console_ReadTextInput(Console* console, SDL_Event input) {
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
    renderRect(app, console->rect);
    renderText(app, getImageData("font2.png"), 5, 8, console->current_command, console->rect.x+2, console->rect.y+console->rect.h-8-2, 1, C_WHITE);

    // TODO
}

void Console_Update(Console* console) {

}