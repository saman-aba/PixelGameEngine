#include "GaluEngine.h"

GaluEngine::GaluEngine()
{
    _sAppName = L"Galu Engine";
}

bool GaluEngine::OnUserCreate()
{
    meshCube.tris = {
        {0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,   1.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f,  1.0f, 1.0f, 0.0f,   1.0f, 0.0f, 0.0f},
        
        {1.0f, 0.0f, 0.0f,  1.0f, 1.0f, 0.0f,   1.0f, 1.0f, 1.0f},
        {1.0f, 0.0f, 0.0f,  1.0f, 1.0f, 1.0f,   1.0f, 0.0f, 1.0f},
        
        {1.0f, 0.0f, 1.0f,  1.0f, 1.0f, 1.0f,   0.0f, 1.0f, 1.0f},
        {1.0f, 0.0f, 1.0f,  0.0f, 1.0f, 1.0f,   0.0f, 0.0f, 1.0f},
        
        {0.0f, 0.0f, 1.0f,  0.0f, 1.0f, 1.0f,   0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 1.0f,  0.0f, 1.0f, 0.0f,   0.0f, 0.0f, 0.0f},
        
        {0.0f, 1.0f, 0.0f,  0.0f, 1.0f, 1.0f,   1.0f, 1.0f, 1.0f},
        {0.0f, 1.0f, 0.0f,  1.0f, 1.0f, 1.0f,   1.0f, 1.0f, 0.0f},
        
        {1.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 0.0f},
        {1.0f, 0.0f, 1.0f,  0.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f}

    };

    float fNear = 0.1f;
    float fFar = 1000.0f;
    float fFov = 90.0f;
    float fAspectRatio = (float)ScreenHeight() / (float)ScreenWidth();
    float fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * 3.14159f);

    matProj.m[0][0] = fAspectRatio * fFovRad;
    matProj.m[1][1] = fFovRad;
    matProj.m[2][2] = fFar / (fFar - fNear);
    matProj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
    matProj.m[2][3] = 1.0f;
    matProj.m[3][3] = 0.0f;

    return true;
}

bool GaluEngine::OnUserUpdate(float fElapsedTime)
{

    Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);
    for (auto tri : meshCube.tris)
    {
        triangle triProjected, triTranslated;

        triTranslated = tri;
        triTranslated.p[0].z = tri.p[0].z + 3.0f;
        triTranslated.p[1].z = tri.p[1].z + 3.0f;
        triTranslated.p[2].z = tri.p[2].z + 3.0f;

        MultiplyMatrixVector(triTranslated.p[0], triProjected.p[0], matProj);
        MultiplyMatrixVector(triTranslated.p[0], triProjected.p[0], matProj);
        MultiplyMatrixVector(triTranslated.p[0], triProjected.p[0], matProj);

        triProjected.p[0].x += 1.0f; triProjected.p[0].y += 1.0f;
        triProjected.p[1].x += 1.0f; triProjected.p[1].y += 1.0f;
        triProjected.p[2].x += 1.0f; triProjected.p[2].y += 1.0f;

        triProjected.p[0].x *= 0.5f * (float)ScreenWidth();
        triProjected.p[0].y *= 0.5f * (float)ScreenHeight();

        triProjected.p[1].x *= 0.5f * (float)ScreenWidth();
        triProjected.p[1].y *= 0.5f * (float)ScreenHeight();

        triProjected.p[2].x *= 0.5f * (float)ScreenWidth();
        triProjected.p[2].y *= 0.5f * (float)ScreenHeight();

        DrawTriangle(triProjected.p[0].x, triProjected.p[0].y,
            triProjected.p[1].x, triProjected.p[1].y,
            triProjected.p[2].x, triProjected.p[2].y,
            PIXEL_SOLID, FG_WHITE);
    }
    return true;
}

bool GaluEngine::OnUserDestroy()
{
    return false;
}
