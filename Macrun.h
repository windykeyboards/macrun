#ifndef Marcun_h
#define Marcun_h

#include <ctype.h>
#include "Arduino.h"
#include <stdlib.h>

#define READBUFFERSIZE 512
#define MACROBUFFERSIZE 7
#define IS_MAC_CTRL_CHAR(c) (c == 't' || c == 'p' || c == 'r' || c == ',')

class Macrun
{
    public:
        Macrun(const String macroDir = "macro");

        void execute(String macroName);
    private:
        String macroDir;
        bool skipline;

        char typeId;
        uint8_t readBufferIndex;
        uint8_t macroBufferIndex;
        
        char readBuffer[READBUFFERSIZE];
        char macroBuffer[MACROBUFFERSIZE + 1];

        void press(uint16_t macro);
        void release(uint16_t macro);
        void dispatchPending();
        void reset();
};
#endif