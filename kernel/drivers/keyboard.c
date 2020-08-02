#include <kernel/drivers/keyboard.h> 
#include <libk/asmUtils.h>

#include <stdbool.h>

char keyMap[] = {   ' ', ' ', '1', '2', '3',  '4', '5', '6',  '7', '8', '9', '0',
                    '-', '=', '\b', '\t', 'q',  'w', 'e', 'r',  't', 'y', 'u', 'i',
                    'o', 'p', '[', ']', ' ',  ' ', 'a', 's',  'd', 'f', 'g', 'h',
                    'j', 'k', 'l', ';', '\'', '`', ' ', '\\', 'z', 'x', 'c', 'v',
                    'b', 'n', 'm', ',', '.',  '/', ' ', ' ',  ' ', ' '
                };

char capKeyMap[] = {   ' ', ' ', '!', '@', '#', '$', '%', '^',  '&', '*', '(', ')',
                       '_', '+', '\b', '\t', 'Q', 'W', 'E', 'R',  'T', 'Y', 'U', 'I',
                       'O', 'P', '{', '}', ' ',  ' ', 'A', 'S',  'D', 'F', 'G', 'H',
                       'J', 'K', 'L', ':', '\'', '~', ' ', '\\', 'Z', 'X', 'C', 'V',
                       'B', 'N', 'M', '<', '>',  '?', ' ', ' ',  ' ', ' '
                   };

bool upkey = false;

void keyboardHandlerMain() {
    uint8_t keycode = inb(0x60);
    
    switch(keycode) {
        case 0xaa: // left shift release
            upkey = false;
            break;
        case 0x2a: // left shift press
            upkey = true;
            break;
        default:
            if(keycode <= 128) {
                if(upkey) {
                      
                    return;
                }  
            }
    } 
}
