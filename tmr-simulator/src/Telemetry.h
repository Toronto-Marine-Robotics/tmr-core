#ifndef TELEMETRY_H
#define TELEMETRY_H

#include <zenoh.h>

#include <Stonefish/core/SimulationManager.h>
#include <Stonefish/core/Robot.h>
#include <Stonefish/entities/SolidEntity.h>
#include <Stonefish/sensors/scalar/DVL.h>
#include <Stonefish/sensors/scalar/IMU.h>
#include <Stonefish/sensors/vision/DepthCamera.h>
#include <Stonefish/sensors/vision/ColorCamera.h>
#include <Stonefish/sensors/Sample.h>
#include <Stonefish/core/SimulationApp.h>

#include <cstring>
#include <vector>

class Telemetry {
public:
    Telemetry();
    ~Telemetry();

    bool Init();
    void Shutdown();

    void PublishStep(sf::SimulationManager* sim, double simTime);

    bool IsConnected() const { return connected_; }

private:
    bool InitZenoh();
    void ShutdownZenoh();
    void PublishDVL(const sf::DVL* dvl, double simTime);
    void PublishDepth(sf::DepthCamera* cam, double simTime);
    void PublishColor(sf::ColorCamera* cam, double simTime);
    void PublishIMU(const sf::IMU* imu, double simTime);

    static constexpr const char* ZENOH_DVL_TOPIC = "auv/dvl";
    static constexpr const char* ZENOH_DEPTH_TOPIC = "auv/depth";
    static constexpr const char* ZENOH_COLOR_TOPIC = "auv/color";
    static constexpr const char* ZENOH_IMU_TOPIC = "auv/imu";
    static constexpr float FP_SCALE = 1000.0f;

    bool connected_;
    z_owned_session_t session_;
    z_owned_publisher_t pubDvl_;
    z_owned_publisher_t pubDepth_;
    z_owned_publisher_t pubColor_;
    z_owned_publisher_t pubImu_;
    z_owned_keyexpr_t keyDvl_;
    z_owned_keyexpr_t keyDepth_;
    z_owned_keyexpr_t keyColor_;
    z_owned_keyexpr_t keyImu_;
    uint64_t sampleCounter_;
};

#endif
