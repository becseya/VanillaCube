# VanillaCube
Lightweight C++ development environment for microcontrollers with a built-in template library.

VS Code + Linux + STM32 Cube + C++ = :heart:

## Installation

#### GNU Arm Embedded Toolchain
- `sudo apt-get install gcc-arm-none-eabi binutils-arm-none-eabi`

#### Cube MX code generator:
- Download and install STM32 Cube MX form: https://www.st.com/en/development-tools/stm32cubemx.html
- Place a link pointing to the executable at: `~/.bin/STM32CubeMX`

#### ST-Link Programmer:
- `sudo apt-get install libusb-1.0`
- Download ST-LINK firmware updater tool from: from: https://www.st.com/en/development-tools/stsw-link007.html
- Follow the instructions, but as a recap:
  - Update firmware by: `sudo java -jar STLinkUpgrade.jar`
  - Update kernel driver rules by: `sudo dpkg -i st-stlink-udev-rules-xxxx-linux-noarch.rpm`
  - Apply changes by rebooting or by: `sudo udevadm control --reload-rules && sudo udevadm trigger`
- Download and install STM32 Cube Prog from: https://www.st.com/en/development-tools/stm32cubeprog.html
- Place a link pointing to the CLI executable at: `~/.bin/STM32_Programmer_CLI`

#### VanillaCube:
- Create a new git repo for your project
- Add **VanillaCube** as a submodule by: `git submodule add https://github.com/becseya/VanillaCube`
- Run `VanillaCube/new-project.sh`
