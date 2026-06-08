# Scenarios

## Scenario files

The simulator loads an XML scenario file from the data directory. The main
scenario is `tmr-simulator.scn` in `tmr-simulator/data/`.

## Structure

A scenario file defines the environment, materials, looks, and entities:

```xml
<scenario>
    <environment>
        <ned latitude="0.0" longitude="0.0"/>
        <ocean />
        <atmosphere>
            <sun azimuth="25.0" elevation="55.0"/>
        </atmosphere>
    </environment>
    ...
</scenario>
```

## Adding a thruster

Each thruster is an `<actuator>` element with `type="thruster"`:

```xml
<actuator name="thr_example" type="thruster">
    <link name="base_link"/>
    <origin xyz="0.3 0.2 -0.15" rpy="0.0 0.7854 3.9270"/>
    <specs max_setpoint="314.16" normalized_setpoint="true"/>
    <propeller diameter="0.1" right="true">
        <mesh filename="propeller.obj" scale="1.0"/>
        <material name="Aluminium"/>
        <look name="PropellerColor"/>
    </propeller>
    <rotor_dynamics type="first_order">
        <time_constant value="0.1"/>
    </rotor_dynamics>
    <thrust_model type="fluid_dynamics">
        <thrust_coeff forward="0.14" reverse="0.12"/>
        <torque_coeff value="0.015"/>
    </thrust_model>
</actuator>
```

After adding thrusters to the scenario, the allocation matrix in `MyApp.cpp`
must be updated to match the new thruster geometry.

## Customizing the AUV

The robot parameters (mass, inertia, hydrodynamics) are set in the `<robot>`
element:

```xml
<robot name="VectoredAUV" algorithm="general" fixed="false">
    <base_link name="base_link" type="box">
        <dimensions xyz="0.6 0.4 0.3"/>
        <material name="Aluminium"/>
        <mass>
            <value>14.8</value>
            <inertia xyz="0.3 0 0 0 0.4 0 0 0 0.4"/>
        </mass>
        <hydrodynamics>
            <center_of_buoyancy xyz="0.0 0.0 0.05"/>
            <damping_linear xyz="10.0 12.0 15.0" rpy="4.0 4.0 5.0"/>
            <damping_quadratic xyz="25.0 30.0 35.0" rpy="8.0 8.0 10.0"/>
        </hydrodynamics>
    </base_link>
</robot>
```
