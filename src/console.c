#include "console.h"
#include "colors.h"
#include <string.h>

HashMap* consoleCommands;

extern Map map;

void Console_Println(Console* console, const char* str) {
    memcpy(console->log[console->log_amm++], str, CONSOLE_STRING_COMMAND_MAX_CHAR);

    if (console->log_amm >= CONSOLE_MAX_COMMANDS) {
        console->log_amm = CONSOLE_MAX_COMMANDS-1;
        
        for (int i=1;i<CONSOLE_MAX_COMMANDS;i++) {
            memcpy(console->log[i-1], console->log[i], CONSOLE_STRING_COMMAND_MAX_CHAR);
        }
    }
}

static void Console_FireCurrentCommand(Console* console) {
    ConsoleArg arg = {0};

    char print[32] = ">";
    Console_Println(console, strcat(print, console->current_command));

    char* token = strtok(console->current_command, " \t\n");
    while (token != NULL && arg.argc < CONSOLE_MAX_ARGV - 1) {
        arg.argv[arg.argc++] = token;
        token = strtok(NULL, " \t\n");
    }

    HashMapItem* item = HashMap_get(consoleCommands, arg.argv[0]);

    console->current_command[0] = '\0';

    if (item == NULL || item->value.value == NULL) {
        Console_Println(console, "Unknown Command!");
        return;
    }

    void (*func_ptr)(ConsoleArg,Console*) = (void (*)(ConsoleArg, Console*))item->value.value;
    func_ptr(arg, console);

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

    if (input.text.text[0] == '`') return;

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

static void command_clear(ConsoleArg arg, Console* console) {
    memset(console->log, 0, sizeof(console->log));
}

static void command_addtex(ConsoleArg arg, Console* console) {
    if (arg.argc < 3) {
        Console_Println(console, "addtex usage: \"addtex <NAME> <INDEX>\"");
    }
    char* endptr;
    long idx = strtol(arg.argv[2], &endptr, 10);

    if (endptr == arg.argv[2]) {
        Console_Println(console, "addtex usage: \"addtex <NAME> <INDEX>\"");
        return;
    }

    Map_setFaceTexIdx(&map, arg.argv[1], idx);

    Map_initFaceTexData(&map);
}

void ConsoleCommands_Init() {
    consoleCommands = HashMap_init();

    HashMap_set(consoleCommands, "clear", command_clear, sizeof(command_clear));
    HashMap_set(consoleCommands, "addtex", command_addtex, sizeof(command_addtex));

}