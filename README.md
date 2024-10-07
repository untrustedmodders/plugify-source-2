# Source2 SDK Plugin

## Overview

The **Source2 SDK** is a specialized plugin for the Plugify plugin system, designed to facilitate the development of plugins for the Source 2 engine. With this SDK, developers can create, manage, and deploy custom plugins that enhance gameplay and user experience in Source 2 games.

[Come and join our Discord](https://discord.gg/rX9TMmpang)

### Prerequisites

- Plugify Framework Installed
- [C++ Language Module](https://www.mono-project.com/docs/about-mono/compatibility/)
- [PolyHook Plugin](https://github.com/untrustedmodders/plugify-polyhook)

### Installation

#### Option 1: Install via Plugify Plugin Manager

You can install the Source2 SDK using the Plugify plugin manager by running the following command:

```bash
plugify install cs2sdk
```

#### Option 2: Manual Installation

1. Clone this repository:

    ```bash
    git clone https://github.com/untrustedmodders/plugify-source-2.git
    cd plugify-source-2
    git submodule update --init --recursive
    ```

2. Build the plugin:

    ```bash
    mkdir build && cd build
    cmake ..
    cmake --build .
    ```
   
## Documentation

For a complete list of functions, parameters, and examples, please visit our documentation site at: [Plugify Source 2 Documentation](https://untrustedmodders.github.io/plugify-source-2/).

## Contributing

We welcome contributions from the community! If you’d like to contribute, please fork the repository and submit a pull request. Ensure to follow the contribution guidelines outlined in the repository.

## License

This project is licensed under the GPLv3 License - see the [LICENSE](LICENSE) file for details.

## Support

For support and inquiries, please open an issue in the GitHub repository or contact us through our community channels.

---

Happy coding and enjoy creating amazing plugins for Source 2!
