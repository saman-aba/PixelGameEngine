#pragma once
#include "Abstract-Engine.h"
class GaluEngine :
    public ConsoleEngine
{
public:
    GaluEngine();

    virtual bool OnUserCreate();
    
    virtual bool OnUserUpdate(float fElapsedTime);

    virtual bool OnUserDestroy();
    
};

