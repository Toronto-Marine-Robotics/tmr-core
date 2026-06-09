#include "Telemetry.h"

#include <cstring>
#include <cmath>
#include <vector>
#include <fcntl.h>
#include <unistd.h>

static inline bool zenoh_pub_is_empty(const z_owned_publisher_t &p)
{
    for (int i = 0; i < 120; ++i)
        if (p._0[i] != 0)
            return false;
    return true;
}

Telemetry::Telemetry()
    : connected_(false), sampleCounter_(0)
{
    memset(&session_, 0, sizeof(session_));
    memset(&pubDvl_, 0, sizeof(pubDvl_));
    memset(&pubDepth_, 0, sizeof(pubDepth_));
    memset(&pubColor_, 0, sizeof(pubColor_));
    memset(&pubImu_, 0, sizeof(pubImu_));
    memset(&keyDvl_, 0, sizeof(keyDvl_));
    memset(&keyDepth_, 0, sizeof(keyDepth_));
    memset(&keyColor_, 0, sizeof(keyColor_));
    memset(&keyImu_, 0, sizeof(keyImu_));
}

Telemetry::~Telemetry()
{
    Shutdown();
}

void Telemetry::Shutdown()
{
    if (!zenoh_pub_is_empty(pubDvl_))
        z_publisher_drop(z_publisher_move(&pubDvl_));
    if (!zenoh_pub_is_empty(pubDepth_))
        z_publisher_drop(z_publisher_move(&pubDepth_));
    if (!zenoh_pub_is_empty(pubColor_))
        z_publisher_drop(z_publisher_move(&pubColor_));
    if (!zenoh_pub_is_empty(pubImu_))
        z_publisher_drop(z_publisher_move(&pubImu_));
    z_keyexpr_drop(z_keyexpr_move(&keyDvl_));
    z_keyexpr_drop(z_keyexpr_move(&keyDepth_));
    z_keyexpr_drop(z_keyexpr_move(&keyColor_));
    z_keyexpr_drop(z_keyexpr_move(&keyImu_));
    z_session_drop(z_session_move(&session_));
    connected_ = false;
}

bool Telemetry::Init()
{
    z_owned_config_t config;
    std::string configPath = std::string(DATA_DIR) + "/zenoh.json5";
    if (zc_config_from_file(&config, configPath.c_str()) != Z_OK)
    {
        if (z_config_default(&config) != Z_OK)
        {
            cWarning("Zenoh: failed to create config");
            return false;
        }
    }

    if (z_open(&session_, z_config_move(&config), nullptr) != Z_OK)
    {
        cWarning("Zenoh: failed to open session");
        return false;
    }

    z_owned_keyexpr_t key;
    if (z_keyexpr_from_str(&key, ZENOH_DVL_TOPIC) == Z_OK)
        keyDvl_ = key;
    if (z_keyexpr_from_str(&key, ZENOH_DEPTH_TOPIC) == Z_OK)
        keyDepth_ = key;
    if (z_keyexpr_from_str(&key, ZENOH_COLOR_TOPIC) == Z_OK)
        keyColor_ = key;
    if (z_keyexpr_from_str(&key, ZENOH_IMU_TOPIC) == Z_OK)
        keyImu_ = key;

    {
        z_owned_publisher_t pub;
        if (z_declare_publisher(z_session_loan(&session_), &pub,
                                z_keyexpr_loan(&keyDvl_), nullptr) == Z_OK)
            pubDvl_ = pub;
        else
            cWarning("Zenoh: failed to declare DVL publisher");
    }
    {
        z_owned_publisher_t pub;
        if (z_declare_publisher(z_session_loan(&session_), &pub,
                                z_keyexpr_loan(&keyDepth_), nullptr) == Z_OK)
            pubDepth_ = pub;
        else
            cWarning("Zenoh: failed to declare depth publisher");
    }
    {
        z_owned_publisher_t pub;
        if (z_declare_publisher(z_session_loan(&session_), &pub,
                                z_keyexpr_loan(&keyColor_), nullptr) == Z_OK)
            pubColor_ = pub;
        else
            cWarning("Zenoh: failed to declare color publisher");
    }
    {
        z_owned_publisher_t pub;
        if (z_declare_publisher(z_session_loan(&session_), &pub,
                                z_keyexpr_loan(&keyImu_), nullptr) == Z_OK)
            pubImu_ = pub;
        else
            cWarning("Zenoh: failed to declare IMU publisher");
    }

    cInfo("Zenoh session opened, topics: %s, %s, %s, %s",
          ZENOH_DVL_TOPIC, ZENOH_DEPTH_TOPIC, ZENOH_COLOR_TOPIC, ZENOH_IMU_TOPIC);
    connected_ = true;
    return true;
}

void Telemetry::PublishStep(sf::SimulationManager *sim, double simTime)
{
    auto *robot = sim->getRobot("VectoredAUV");
    if (!robot)
        return;

    auto *dvl = dynamic_cast<sf::DVL *>(robot->getSensor("dvl"));
    if (dvl)
        PublishDVL(dvl, simTime);

    auto *depthCam = dynamic_cast<sf::DepthCamera *>(robot->getSensor("oak_d_pro_w_depth"));
    if (depthCam)
        PublishDepth(depthCam, simTime);

    auto *colorCam = dynamic_cast<sf::ColorCamera *>(robot->getSensor("oak_d_pro_w_color"));
    if (colorCam)
        PublishColor(colorCam, simTime);

    auto *imu = dynamic_cast<sf::IMU *>(robot->getSensor("imu"));
    if (imu)
        PublishIMU(imu, simTime);

    ++sampleCounter_;
}

void Telemetry::PublishDVL(const sf::DVL *dvl, double simTime)
{
    if (zenoh_pub_is_empty(pubDvl_))
        return;

    sf::Sample sample = dvl->getLastSample();
    auto data = sample.getData();
    if (data.size() < 4)
        return;

    struct DvlPayload
    {
        double timestamp;
        float velX, velY, velZ;
        float altitude;
        uint8_t status;
        uint8_t pad[3];
    };

    auto *payload = new DvlPayload();
    payload->timestamp = simTime;
    payload->velX = static_cast<float>(data[0]);
    payload->velY = static_cast<float>(data[1]);
    payload->velZ = static_cast<float>(data[2]);
    payload->altitude = static_cast<float>(data[3]);
    payload->status = (data.size() >= 8) ? static_cast<uint8_t>(data[7]) : 0;
    memset(payload->pad, 0, sizeof(payload->pad));

    z_owned_bytes_t bytes;
    z_bytes_from_buf(&bytes, reinterpret_cast<uint8_t *>(payload), sizeof(*payload), [](void *data, void *)
                     { delete static_cast<DvlPayload *>(data); }, nullptr);

    z_put_options_t opts;
    z_put_options_default(&opts);
    z_put(z_session_loan(&session_), z_keyexpr_loan(&keyDvl_),
          z_bytes_move(&bytes), &opts);
}

void Telemetry::PublishDepth(sf::DepthCamera *cam, double simTime)
{
    if (zenoh_pub_is_empty(pubDepth_))
        return;

    unsigned int resX, resY;
    cam->getResolution(resX, resY);

    void *imgPtr = cam->getImageDataPointer(0);
    if (!imgPtr)
        return;
    GLfloat *depthData = static_cast<GLfloat *>(imgPtr);

    uint32_t w = static_cast<uint32_t>(resX);
    uint32_t h = static_cast<uint32_t>(resY);
    uint32_t numPixels = w * h;
    uint32_t headerSize = sizeof(double) + 2 * sizeof(uint32_t);
    uint32_t dataSize = numPixels * sizeof(int16_t);

    auto *buffer = new std::vector<uint8_t>(headerSize + dataSize);
    memcpy(buffer->data(), &simTime, sizeof(double));
    memcpy(buffer->data() + sizeof(double), &w, sizeof(uint32_t));
    memcpy(buffer->data() + sizeof(double) + sizeof(uint32_t), &h, sizeof(uint32_t));

    int16_t *dst = reinterpret_cast<int16_t *>(buffer->data() + headerSize);
    for (uint32_t i = 0; i < numPixels; ++i)
    {
        float d = depthData[i];
        if (d <= 0.0f || d > 20.0f || std::isnan(d) || std::isinf(d))
            dst[i] = 0;
        else
        {
            float scaled = d * FP_SCALE;
            dst[i] = static_cast<int16_t>(scaled > 32767.0f ? 32767.0f : scaled);
        }
    }

    z_owned_bytes_t bytes;
    z_bytes_from_buf(&bytes, buffer->data(), buffer->size(), [](void *data, void *)
                     { delete static_cast<std::vector<uint8_t> *>(data); }, buffer);

    z_put_options_t opts;
    z_put_options_default(&opts);
    z_put(z_session_loan(&session_), z_keyexpr_loan(&keyDepth_),
          z_bytes_move(&bytes), &opts);
}

void Telemetry::PublishColor(sf::ColorCamera *cam, double simTime)
{
    if (zenoh_pub_is_empty(pubColor_))
        return;

    unsigned int resX, resY;
    cam->getResolution(resX, resY);

    void *imgPtr = cam->getImageDataPointer(0);
    if (!imgPtr)
        return;
    GLubyte *rgbaData = static_cast<GLubyte *>(imgPtr);

    uint32_t w = static_cast<uint32_t>(resX);
    uint32_t h = static_cast<uint32_t>(resY);
    uint32_t numPixels = w * h;
    uint32_t headerSize = sizeof(double) + 2 * sizeof(uint32_t);
    uint32_t dataSize = numPixels * 4;

    auto *buffer = new std::vector<uint8_t>(headerSize + dataSize);
    memcpy(buffer->data(), &simTime, sizeof(double));
    memcpy(buffer->data() + sizeof(double), &w, sizeof(uint32_t));
    memcpy(buffer->data() + sizeof(double) + sizeof(uint32_t), &h, sizeof(uint32_t));
    memcpy(buffer->data() + headerSize, rgbaData, dataSize);

    z_owned_bytes_t bytes;
    z_bytes_from_buf(&bytes, buffer->data(), buffer->size(), [](void *data, void *)
                     { delete static_cast<std::vector<uint8_t> *>(data); }, buffer);

    z_put_options_t opts;
    z_put_options_default(&opts);
    z_put(z_session_loan(&session_), z_keyexpr_loan(&keyColor_),
          z_bytes_move(&bytes), &opts);
}

void Telemetry::PublishIMU(const sf::IMU *imu, double simTime)
{
    if (zenoh_pub_is_empty(pubImu_))
        return;

    sf::Sample sample = imu->getLastSample();
    auto data = sample.getData();
    if (data.size() < 9)
        return;

    struct ImuPayload
    {
        double timestamp;
        float roll, pitch, yaw;
        float angVelX, angVelY, angVelZ;
        float linAccX, linAccY, linAccZ;
    };

    auto *payload = new ImuPayload();
    payload->timestamp = simTime;
    payload->roll = static_cast<float>(data[0]);
    payload->pitch = static_cast<float>(data[1]);
    payload->yaw = static_cast<float>(data[2]);
    payload->angVelX = static_cast<float>(data[3]);
    payload->angVelY = static_cast<float>(data[4]);
    payload->angVelZ = static_cast<float>(data[5]);
    payload->linAccX = static_cast<float>(data[6]);
    payload->linAccY = static_cast<float>(data[7]);
    payload->linAccZ = static_cast<float>(data[8]);

    z_owned_bytes_t bytes;
    z_bytes_from_buf(&bytes, reinterpret_cast<uint8_t *>(payload), sizeof(*payload), [](void *data, void *)
                     { delete static_cast<ImuPayload *>(data); }, nullptr);

    z_put_options_t opts;
    z_put_options_default(&opts);
    z_put(z_session_loan(&session_), z_keyexpr_loan(&keyImu_),
          z_bytes_move(&bytes), &opts);
}
