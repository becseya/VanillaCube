# VanillaCube
Lightweight IDE based around the STM32Cube Ecosystem

Linux + STM32 Cube + VS Code = :heart:

## Installation

Cube MX code generator:
- Download and install STM32 Cube MX form: https://www.st.com/en/development-tools/stm32cubemx.html
- Place a link pointing to the executable at: `~/.bin/STM32CubeMX`

ST-Link Programmer:
- `sudo apt-get install libusb-1.0`
- Download ST-LINK firmware updater tool from: from: https://www.st.com/en/development-tools/stsw-link007.html
- Follow the instructions, but as a recap:
  - Update firmware by: `sudo java -jar STLinkUpgrade.jar`
  - Update kernel driver rules by: `sudo dpkg -i st-stlink-udev-rules-xxxx-linux-noarch.rpm`
  - Reboot
- Download and install STM32 Cube Prog from: https://www.st.com/en/development-tools/stm32cubeprog.html
- Place a link pointing to the CLI executable at: `~/.bin/STM32_Programmer_CLI`

VanillaCube:
- Create a new git repo for your project
- Add **VanillaCube** as a submodule by: `git submodule add https://github.com/becseya/VanillaCube`
- Run `VanillaCube/new-project.sh`
