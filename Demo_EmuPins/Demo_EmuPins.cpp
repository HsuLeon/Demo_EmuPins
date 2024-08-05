// Demo_EmuPins.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include "pinEmu.h"

int main()
{
    const wchar_t* sharedMemoryName = L"Local\\PinSharedMemory";
    const int maxPinNum = 128;

    CPinEmu pinEmu;
    pinEmu.Create(sharedMemoryName, maxPinNum);
    std::cout << "press e or E to exit...\n";
    std::cout << "press w or W to change pin status...\n";
    std::cout << "press r or R to get pin status...\n";

    std::vector<unsigned char> pinValues;
    for (int i = 0; i < maxPinNum; i++)
    {
        unsigned char pinValue = pinEmu.DigitalRead(i);
        pinValues.push_back(pinValue);
    }
    // assign random seed
    srand(::timeGetTime());

    bool bLoop = true;
    while (true)
    {
        char ch = getchar();
        // skip ctrl char
        if (ch == '\r' || ch == '\n' || ch == '\t')
            continue;

        switch (ch)
        {
        case 'w':
        case 'W':
            {
                int pinId = rand() % maxPinNum;
                // get current status
                unsigned char pinValue = pinEmu.DigitalRead(pinId);
                bool bHigh = pinValue > 0 ? false : true;
                // toggle...
                pinEmu.DigitalWrite(pinId, bHigh);
                // update cache
                pinValues[pinId] = pinEmu.DigitalRead(pinId);

                char pBuf[256];
                sprintf_s(pBuf, 256, "set pin %d to %s", pinId, bHigh ? "high" : "low");
                std::cout << pBuf << std::endl;
            }
            break;
        case 'r':
        case 'R':
            {
                for (int i = 0; i < maxPinNum; i++)
                {
                    unsigned char pinValue = pinEmu.DigitalRead(i);
                    // skip if not changed
                    if (pinValue == pinValues.at(i)) continue;
                    // replace to new value
                    pinValues[i] = pinValue;

                    char pBuf[256];
                    sprintf_s(pBuf, 256, "change pinId %d to %s", i, pinValue > 0 ? "high" : "low");
                    std::cout << pBuf << std::endl;
                }
            }
            break;
        case 'e':
        case 'E':
            bLoop = false;
            break;
        default:
            break;
        }
    }
}