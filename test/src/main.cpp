#define SDL_MAIN_HANDLED
#include "testCPU.h"

int main() {

    // Instantiate Tests
    CpuTests::Test* cpuTests = new CpuTests::Test();

    // Run Tests
    cpuTests->RunAllTests();

    // Tear Down
    delete cpuTests;

    return 0;
}