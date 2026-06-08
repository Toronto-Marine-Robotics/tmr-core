#ifndef MY_APP_H
#define MY_APP_H

#include <Stonefish/core/GraphicalSimulationApp.h>
#include <Stonefish/actuators/Thruster.h>
#include "SimulationManager.h"

class MyApp : public sf::GraphicalSimulationApp
{
public:
    MyApp(std::string dataDirPath, sf::RenderSettings s, sf::HelperSettings h, MySimulationManager* sim);

    void DoHUD() override;
    void ProcessInputs() override;

private:
    sf::Thruster* getThruster(const std::string& name);

    sf::Scalar guiSurge_, guiSway_, guiHeave_, guiYaw_;
    sf::Scalar thrustScale_;
    bool diagPrinted_ = false;
};

#endif
