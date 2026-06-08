#include "MyApp.h"
#include <Stonefish/core/Robot.h>
#include <Stonefish/actuators/Actuator.h>
#include <Stonefish/graphics/IMGUI.h>
#include <Stonefish/utils/SystemUtil.hpp>
#include <SDL.h>
#include <algorithm>

MyApp::MyApp(std::string dataDirPath, sf::RenderSettings s, sf::HelperSettings h, MySimulationManager *sim)
    : GraphicalSimulationApp("tmr-simulator", dataDirPath, s, h, sim),
      guiSurge_(0), guiSway_(0), guiHeave_(0), guiYaw_(0), thrustScale_(1.0)
{
}

sf::Thruster *MyApp::getThruster(const std::string &name)
{
    auto *robot = getSimulationManager()->getRobot("VectoredAUV");
    if (!robot)
        return nullptr;
    return dynamic_cast<sf::Thruster *>(robot->getActuator(name));
}

void MyApp::ProcessInputs()
{
    if (!diagPrinted_)
    {
        diagPrinted_ = true;
        auto *robot = getSimulationManager()->getRobot("VectoredAUV");
        if (robot)
        {
            cInfo("Robot 'VectoredAUV' found");
            for (size_t i = 0;; ++i)
            {
                auto *a = robot->getActuator(i);
                if (!a)
                    break;
                cInfo("  Actuator[%zu] name='%s' type=%d", i, a->getName().c_str(), (int)a->getType());
            }
        }
        else
            cWarning("Robot 'VectoredAUV' NOT FOUND");
    }

    auto set = [&](const std::string &name, sf::Scalar val)
    {
        auto *t = getThruster(name);
        if (t)
            t->setSetpoint(val);
    };

    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    sf::Scalar cmd_surge = 0, cmd_sway = 0, cmd_heave = 0, cmd_yaw = 0;
    bool kbdActive = false;

    static bool keyDiagPrinted = false;
    if (!keyDiagPrinted && (keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_R] || keys[SDL_SCANCODE_F] || keys[SDL_SCANCODE_T] || keys[SDL_SCANCODE_G] || keys[SDL_SCANCODE_SPACE]))
    {
        keyDiagPrinted = true;
        cInfo("Keyboard input detected");
    }

    if (keys[SDL_SCANCODE_UP])
    {
        cmd_surge = 1.0;
        kbdActive = true;
    }
    if (keys[SDL_SCANCODE_DOWN])
    {
        cmd_surge = -1.0;
        kbdActive = true;
    }
    if (keys[SDL_SCANCODE_LEFT])
    {
        cmd_sway = 1.0;
        kbdActive = true;
    }
    if (keys[SDL_SCANCODE_RIGHT])
    {
        cmd_sway = -1.0;
        kbdActive = true;
    }
    if (keys[SDL_SCANCODE_R])
    {
        cmd_heave = 1.0;
        kbdActive = true;
    }
    if (keys[SDL_SCANCODE_F])
    {
        cmd_heave = -1.0;
        kbdActive = true;
    }
    if (keys[SDL_SCANCODE_T])
    {
        cmd_yaw = 1.0;
        kbdActive = true;
    }
    if (keys[SDL_SCANCODE_G])
    {
        cmd_yaw = -1.0;
        kbdActive = true;
    }
    if (keys[SDL_SCANCODE_SPACE])
    {
        kbdActive = true;
    }

    if (!kbdActive)
    {
        cmd_surge = guiSurge_;
        cmd_sway = guiSway_;
        cmd_heave = guiHeave_;
        cmd_yaw = guiYaw_;
    }

    const char *names[8] = {"VectoredAUV/thr_flb", "VectoredAUV/thr_frb", "VectoredAUV/thr_blb", "VectoredAUV/thr_brb",
                            "VectoredAUV/thr_flt", "VectoredAUV/thr_frt", "VectoredAUV/thr_blt", "VectoredAUV/thr_brt"};

    if (cmd_surge == 0 && cmd_sway == 0 && cmd_heave == 0 && cmd_yaw == 0)
    {
        for (int i = 0; i < 8; ++i)
            set(names[i], 0);
        return;
    }

    // Allocation: each DOF assigns ±1 to thruster groups based on force direction
    // clang-format off
    struct { sf::Scalar surge, sway, heave, yaw; } alloc[8] = {
        {-1, -1, -1, -1},  // flb
        {-1,  1, -1,  1},  // frb
        { 1, -1, -1,  1},  // blb
        { 1,  1, -1, -1},  // brb
        { 1,  1,  -1,  1},  // flt
        {1,  -1,  -1,  -1},  // frt
        { -1, 1,  -1,  -1},  // blt
        { -1, -1, -1, 1},  // brt
    };
    // clang-format on

    sf::Scalar s = thrustScale_;
    sf::Scalar sp[8];
    for (int i = 0; i < 8; ++i)
    {
        sf::Scalar raw_sp = (alloc[i].surge * cmd_surge + alloc[i].sway * cmd_sway + alloc[i].heave * cmd_heave + alloc[i].yaw * cmd_yaw);
        raw_sp = std::max(static_cast<sf::Scalar>(-1.0), std::min(static_cast<sf::Scalar>(1.0), raw_sp));
        sp[i] = s * raw_sp;
        set(names[i], sp[i]);
    }
}

void MyApp::DoHUD()
{
    GraphicalSimulationApp::DoHUD();
    auto *gui = getGUI();

    sf::Uid uid;
    uid.owner = 0;

    uid.item = 0;
    thrustScale_ = gui->DoSlider(uid, 600.f, 30.f, 200.f, 0.0, 1.0, thrustScale_, "Thrust Scale");

    uid.item = 1;
    guiSurge_ = gui->DoSlider(uid, 600.f, 60.f, 200.f, -1.0, 1.0, guiSurge_, "Surge");

    uid.item = 2;
    guiSway_ = gui->DoSlider(uid, 600.f, 90.f, 200.f, -1.0, 1.0, guiSway_, "Sway");

    uid.item = 3;
    guiHeave_ = gui->DoSlider(uid, 600.f, 120.f, 200.f, -1.0, 1.0, guiHeave_, "Heave");

    uid.item = 4;
    guiYaw_ = gui->DoSlider(uid, 600.f, 150.f, 200.f, -1.0, 1.0, guiYaw_, "Yaw");

    gui->DoLabel(600.f, 190.f,
                 "--- Keyboard ---\n"
                 " Arrows : Surge/Sway\n"
                 " R/F    : Heave (up/down)\n"
                 " T/G    : Yaw (CW/CCW)\n"
                 " SPACE  : Stop\n"
                 " H      : Toggle HUD");
}
