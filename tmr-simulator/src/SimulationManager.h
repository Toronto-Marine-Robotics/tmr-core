#ifndef SIMULATION_MANAGER_H
#define SIMULATION_MANAGER_H

#include <Stonefish/core/SimulationManager.h>

class MySimulationManager : public sf::SimulationManager
{
public:
    MySimulationManager(sf::Scalar stepsPerSecond);
    void BuildScenario();
};

#endif
