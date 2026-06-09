#include "SimulationManager.h"
#include "Telemetry.h"
#include <Stonefish/core/ScenarioParser.h>
#include <Stonefish/core/SimulationApp.h>
#include <Stonefish/utils/SystemUtil.hpp>

MySimulationManager::MySimulationManager(sf::Scalar stepsPerSecond) : SimulationManager(stepsPerSecond)
{
}

MySimulationManager::~MySimulationManager() = default;

void MySimulationManager::BuildScenario()
{
    sf::ScenarioParser parser(this);
    const std::string scenarioPath = sf::GetDataPath() + "/tmr-simulator.scn";

    if (!parser.Parse(scenarioPath))
    {
        for (const auto &message : parser.getLog())
        {
            switch (message.type)
            {
            case sf::MessageType::INFO:
                cInfo("%s", message.text.c_str());
                break;
            case sf::MessageType::WARNING:
                cWarning("%s", message.text.c_str());
                break;
            case sf::MessageType::ERROR:
                cError("%s", message.text.c_str());
                break;
            case sf::MessageType::CRITICAL:
                cCritical("%s", message.text.c_str());
                break;
            }
        }
        cCritical("Scenario parser failed to load %s", scenarioPath.c_str());
    }

    telemetry_ = std::make_unique<Telemetry>();
    if (!telemetry_->Init())
    {
        cWarning("Telemetry initialization failed, continuing without it");
        telemetry_.reset();
    }
    else
    {
        setCallSimulationStepCompleted(true);
    }
}

void MySimulationManager::SimulationStepCompleted(sf::Scalar /*timeStep*/)
{
    if (telemetry_)
    {
        double simTime = getSimulationTime();
        telemetry_->PublishStep(this, simTime);
    }
}
