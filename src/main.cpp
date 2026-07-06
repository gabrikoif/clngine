#include "engine.hpp"

int main()
{
    Engine engine;

    if (!engine.initialize()) return -1;
    engine.createAndLoad();

    engine.run();

    engine.shutdown();
}