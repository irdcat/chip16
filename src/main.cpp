#include "Application.hpp"

#if defined(_WIN32)

#include <Windows.h>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    Application app;
    app.run(0, nullptr);
    return 0;
}

#else

int main(int argc, char ** argv)
{
    Application app;
    app.run(argc, argv);
    return 0;
}

#endif