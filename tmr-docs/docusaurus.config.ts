import {themes as prismThemes} from 'prism-react-renderer';
import type {Config} from '@docusaurus/types';
import type * as Preset from '@docusaurus/preset-classic';

const config: Config = {
  title: 'TMR Simulator',
  tagline: 'Vectored AUV simulation using Stonefish physics',
  favicon: 'img/favicon.ico',

  future: {
    v4: true,
  },

  url: 'https://torontomarinereb.org',
  baseUrl: '/',

  organizationName: 'Toronto-Marine-Robotics',
  projectName: 'tmr-core',

  onBrokenLinks: 'throw',

  i18n: {
    defaultLocale: 'en',
    locales: ['en'],
  },

  presets: [
    [
      'classic',
      {
        docs: {
          sidebarPath: './sidebars.ts',
          editUrl:
            'https://github.com/Toronto-Marine-Robotics/tmr-core/blob/main/tmr-docs/',
        },
        theme: {
          customCss: './src/css/custom.css',
        },
      } satisfies Preset.Options,
    ],
  ],

  themeConfig: {
    image: 'img/docusaurus-social-card.jpg',
    colorMode: {
      respectPrefersColorScheme: true,
    },
    navbar: {
      title: 'TMR Simulator',
      items: [
        {
          type: 'docSidebar',
          sidebarId: 'docsSidebar',
          position: 'left',
          label: 'Docs',
        },
        {
          href: 'https://github.com/Toronto-Marine-Robotics/tmr-core',
          label: 'GitHub',
          position: 'right',
        },
      ],
    },
    footer: {
      style: 'dark',
      links: [
        {
          title: 'Docs',
          items: [
            {
              label: 'Introduction',
              to: '/docs/intro',
            },
            {
              label: 'Build Instructions',
              to: '/docs/build',
            },
            {
              label: 'Controls',
              to: '/docs/controls',
            },
            {
              label: 'Architecture',
              to: '/docs/architecture',
            },
            {
              label: 'Scenarios',
              to: '/docs/scenarios',
            },
          ],
        },
        {
          title: 'More',
          items: [
            {
              label: 'GitHub',
              href: 'https://github.com/Toronto-Marine-Robotics/tmr-core',
            },
          ],
        },
      ],
      copyright: `Copyright © ${new Date().getFullYear()} Toronto Marine Robotics.`,
    },
    prism: {
      theme: prismThemes.github,
      darkTheme: prismThemes.dracula,
    },
  } satisfies Preset.ThemeConfig,
};

export default config;
