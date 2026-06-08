#include <filesystem>
#include "SimulationManager.h"
#include "MyApp.h"

int main(int argc, char **argv)
{
    sf::RenderSettings s;
    s.windowW = 1280;
    s.windowH = 900;
    sf::HelperSettings h;

    MySimulationManager manager(500.0);
    MyApp app(DATA_DIR, s, h, &manager);
    app.Run(true, true);

    return 0;
}
