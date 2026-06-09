#ifndef SIMULATION_MANAGER_H
#define SIMULATION_MANAGER_H

#include <Stonefish/core/SimulationManager.h>
#include <memory>

class Telemetry;

class MySimulationManager : public sf::SimulationManager
{
public:
    MySimulationManager(sf::Scalar stepsPerSecond);
    ~MySimulationManager();
    void BuildScenario() override;
    void SimulationStepCompleted(sf::Scalar timeStep) override;

private:
    std::unique_ptr<Telemetry> telemetry_;
};

#endif
