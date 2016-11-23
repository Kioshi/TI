// TI.cpp : Defines the entry point for the console application.
//
#include <thread>
#include <iostream>
#include <map>
#include <functional>
#include <atomic>
#include <chrono>
#include <vector>
#include <conio.h>

enum eStates
{
    OFF,
    ORANGE,
    S1,
    S12,
    S2,
    S23,
    S3,
    S34,
    S4,
    S41,
    S1P,
    S12P,
    S2P,
    S23P,
    S3P,

    STATES_COUNT = 8
};

enum eInputs
{
    TIMER,
    BUS
};

typedef uint32_t uint32;
#define DIFF 100
#define IN_MILISECONDS 1000

enum eTimers
{
    OFFLINE_TIMER = 1 * IN_MILISECONDS,
    COLOR_CHANGE = 4 * IN_MILISECONDS,
    NORMAL_WAIT = 45 * IN_MILISECONDS,
    FAST_WAIT = 30 * IN_MILISECONDS
};

void onInput(eInputs input, eStates& state, uint32& timer)
{
    if (input == TIMER)
    {
        switch (state)
        {
            case OFF:
                timer = OFFLINE_TIMER;
                state = ORANGE;
                return;
            case ORANGE:
                timer = OFFLINE_TIMER;
                state = OFF;
                return;
        }

        if (state == S41)
            state = S1;
        else if (state == S3P)
            state = S34;
        else
            state = eStates(state + 1);

        switch (state)
        {
            case S1:
            case S2:
            case S3:
            case S4:
                timer = NORMAL_WAIT;
                break;
            case S1P:
            case S2P:
            case S3P:
                timer = FAST_WAIT;
                break;
            default:
                timer = COLOR_CHANGE;
                break;
        }
    }
    else
    {
        if (state <= S3 && state >= S1)
            state = eStates(state + STATES_COUNT);
    }
}

enum eSemapthor
{
    OFFLINE,
    GREEN,
    YELLOW,
    RED,
    RELLOW
};

std::map<eStates, std::vector<eSemapthor>> semaphores =
{
    { OFF,{ OFFLINE,OFFLINE,OFFLINE,OFFLINE,OFFLINE,OFFLINE,OFFLINE,OFFLINE,OFFLINE,OFFLINE,OFFLINE,OFFLINE,OFFLINE,OFFLINE } },
    { ORANGE,{ YELLOW,YELLOW,YELLOW,YELLOW,YELLOW,YELLOW,YELLOW,OFFLINE,OFFLINE,OFFLINE,OFFLINE,OFFLINE,OFFLINE,OFFLINE } },
    { S1,{ RED,RED,GREEN,RED,RED,GREEN,GREEN,GREEN,RED,GREEN,RED,RED,GREEN,GREEN } },
    { S12,{ RED,RED,GREEN,RELLOW,RED,YELLOW,YELLOW,RED,RED,RED,RED,RED,RED,RED } },
    { S2,{ RED,RED,GREEN,GREEN,RED,RED,RED,RED,RED,RED,RED,GREEN,RED,RED } },
    { S23,{ RELLOW,RELLOW,YELLOW,YELLOW,RED,RED,RED,RED,RED,RED,RED,GREEN,RED,RED } },
    { S3,{ GREEN,GREEN,RED,RED,RED,RED,RED,RED,GREEN,RED,RED,GREEN,RED,RED } },
    { S34,{ YELLOW,YELLOW,RED,RELLOW,RED,RED,RED,RED,GREEN,RED,RED,GREEN,RED,RED } },
    { S4,{ RED,RED,RED,GREEN,RED,RED,RED,RED,GREEN,RED,GREEN,GREEN,RED,RED } },
    { S41,{ RED,RELLOW,RED,GREEN,RELLOW,RELLOW,RED,RED,RED,RED,RED,RED,RED,RED } },
};

void printSemaphores(eStates state)
{
    std::cout << "\t";
    for (eSemapthor s : semaphores[state])
    {
        switch (s)
        {
        case OFFLINE:
            std::cout << "  ";
            break;
        case GREEN:
            std::cout << "G ";
            break;
        case YELLOW:
            std::cout << "Y ";
            break;
        case RED:
            std::cout << "R ";
            break;
        case RELLOW:
            std::cout << "YR";
            break;
        default:
            break;
        }
        std::cout << " ";
    }
}

void print(uint32 timer, eStates state)
{
    std::cout << "\r";
    std::cout << timer / IN_MILISECONDS << "\t";
    switch (state >= S1P ? state - STATES_COUNT : state)
    {
    case OFF: 
        std::cout << "OFF";
        break;
    case ORANGE:
        std::cout << "ORG";
        break;
    case S1:
        std::cout << "S1";
        break;
    case S2:
        std::cout << "S2";
        break;
    case S3:
        std::cout << "S3";
        break;
    case S4:
        std::cout << "S4";
        break;
    case S12:
        std::cout << "S12";
        break;
    case S23:
        std::cout << "S23";
        break;
    case S34:
        std::cout << "S34";
        break;
    case S41:
        std::cout << "S41";
        break;
    }
    if (state >= S1P)
        std::cout << "P";

    printSemaphores(state >= S1P ? eStates(state - STATES_COUNT) : state);

}

std::atomic<char> c = 0;

int main()
{
    uint32 timer = 0;
    eStates state = OFF;

    std::cout << "Controlls:" << std::endl;
    std::cout << " e\t- exit" << std::endl;
    std::cout << " space\t- set timer to 0" << std::endl;
    std::cout << " b\t- simulate bus sensor input" << std::endl;
    std::cout << " s\t- start normal cycle" << std::endl;
    std::cout << " k\t- start fail cycle" << std::endl;

    std::thread inputThread([]()
    {
        do
        {
            c = _getch();
        } while (c != 'e');
    });


    while (c != 'e')
    {
        switch (c)
        {
            case ' ':
                timer = 1;
                break;
            case 'k':
                timer = 0;
                state = OFF;
                break;
            case 's':
                timer = NORMAL_WAIT;
                state = S1;
                break;
            case 'b':
                onInput(BUS, state, timer);
                break;
        }
        c = 0;

        if (timer)
        {
            if (timer <= DIFF)
                onInput(TIMER, state, timer);
            else
                timer -= DIFF;
        }

        print(timer, state);
        std::this_thread::sleep_for(std::chrono::milliseconds(DIFF));
    }

    inputThread.join();

    return 0;
}

