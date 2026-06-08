import type {ReactNode} from 'react';
import clsx from 'clsx';
import Heading from '@theme/Heading';

type FeatureItem = {
  title: string;
  description: ReactNode;
};

const FeatureList: FeatureItem[] = [
  {
    title: '8-Thruster Vectored AUV',
    description: (
      <>
        Full 4-DOF control (surge, sway, heave, yaw) via an allocation matrix
        mapping body-frame forces to individual thruster setpoints.
      </>
    ),
  },
  {
    title: 'Stonefish Physics',
    description: (
      <>
        Marine robotics simulator with hydrodynamics, buoyancy, ocean optics,
        and real-time underwater rendering via OpenGL 4.3.
      </>
    ),
  },
  {
    title: 'Keyboard &amp; GUI Control',
    description: (
      <>
        Dual control modes: keyboard for quick testing or GUI sliders for
        precise adjustment. HUD overlay for real-time feedback.
      </>
    ),
  },
];

function Feature({title, description}: FeatureItem) {
  return (
    <div className={clsx('col col--4')}>
      <div className="text--center padding-horiz--md">
        <Heading as="h3">{title}</Heading>
        <p>{description}</p>
      </div>
    </div>
  );
}

export default function HomepageFeatures(): ReactNode {
  return (
    <section className="padding-top--lg padding-bottom--lg">
      <div className="container">
        <div className="row">
          {FeatureList.map((props, idx) => (
            <Feature key={idx} {...props} />
          ))}
        </div>
      </div>
    </section>
  );
}
