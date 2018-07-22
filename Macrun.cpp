#include <Macrun.h>
#include <SD.h>

Macrun::Macrun(const String macroDir): macroDir(macroDir)
{
    reset();
}

void Macrun::execute(String name)
{
    const String filename = macroDir + "/" + name + ".wkm";
    char fname[64];
    strcpy(fname, filename.c_str());

    bool exists = SD.exists(fname);

    if (!exists) {
        return;
    }

    File macro = SD.open(fname);
    
    while (macro.available())
    {
        size_t bytesAvailable = min(macro.available(), READBUFFERSIZE);
		macro.read(readBuffer, bytesAvailable);

        // Pass the butter
        for (size_t byteNo = 0; byteNo < bytesAvailable; byteNo++)
		{
            if (skipline && readBuffer[byteNo] == '\n') {
                skipline = false;
                continue;
            } else if (skipline) {
                continue;
            }

            if (IS_MAC_CTRL_CHAR(readBuffer[byteNo])) {
                // Command type or comma
                if (readBuffer[byteNo] != ',') {
                    typeId = readBuffer[byteNo];
                }
            } else {
                // Terminal case - last character in file
                // TODO - Fix multiple of 512 char issue
                if (byteNo == (bytesAvailable - 1) && bytesAvailable < READBUFFERSIZE) {
                    if (readBuffer[byteNo] != '\n') {
                        macroBuffer[macroBufferIndex] = readBuffer[byteNo];
                        macroBufferIndex++;
                    }
                    dispatchPending();
                    return;
                }

                // Read to macroBuffer, hex, dispatch
                if (readBuffer[byteNo] == '\n') {
                    dispatchPending();
                } else if (isxdigit(readBuffer[byteNo])) {
                    if (macroBufferIndex == MACROBUFFERSIZE) {
                        // We should be at the end. Skip the line
                        skipline = true;
                        macroBufferIndex = 0;
                    }

                    macroBuffer[macroBufferIndex] = readBuffer[byteNo];
                    macroBufferIndex++;
                }
            }
        }
    }

    reset();
}

void Macrun::dispatchPending() {
    macroBuffer[macroBufferIndex] = '\0';
    uint16_t macro = strtol(macroBuffer, NULL, 16);

    switch (typeId) {
        case 'p':
            press(macro);
            break;
        case 'r':
            release(macro);
            break;
        case 't':
            delay(macro);
            break;
    }

    macroBufferIndex = 0;
}

void Macrun::reset() {
    readBufferIndex = 0;
    macroBufferIndex = 0;
}

void Macrun::press(uint16_t macro) {
    Keyboard.press(macro);
}

void Macrun::release(uint16_t macro) {
    Keyboard.release(macro);
}