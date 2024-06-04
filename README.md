#Instructions for building a demo
1. **Install Visual Studio Code**: [VSCode](https://code.visualstudio.com/download)
   - Download the VSCode version for **Windows (x64)**.
   - Manually run the installation file (.exe) if it does not start automatically.
   - Accept terms of use.
   - Choose the installation folder. The default selected folder is recommended.
   - Remember to check the “Add to Path” box to add the path to VSCode in the environment variables.
     <p align="center">
       <img src="images/vscodePath.png" alt="Install VSCode" width="80%" />
      </p>
 2. **Download QEMU**: [QEMU](https://qemu.weilnetz.de/w64/)
    - Enter the folder called "2023/".
    - Click on “qemu-w64-setup-20231224.exe.” The download will start automatically.
    - After the installation is complete, add the path to QEMU in the **system** variables.
      - Note: the path is the installation folder chosen during the installation phase.
      - Open "Environment Variables".
      - Click on “New” in the System Variables.
      - Add variable name and path to QEMU.
        <p align="center">
          <img src="images/qemuPath.png" alt="QEMU System Variable" width="80%" />
         </p>
   3. **Download and install the GNU Arm Embedded Toolchain**: [Arm GNU Toolchain](https://developer.arm.com/downloads/-/gnu-rm)
      - Install the version "gcc-arm-none-eabi-10.3-2021.10-win32.exe".
      - Choose the installation folder. The default selected folder is recommended.
      - Remember to check the “Add to Path” box to add the path to GNU in the environment variables.
        <p align="center">
          <img src="images/gnuPath.png" alt="GNU User Variable" width="80%" />
         </p>

     4. **Install the FreeRTOS version 202212.01**:[FreeRTOS](https://www.freertos.org/a00104.html)
     5. **Download and install Make for Windows**: [MAKE](https://gnuwin32.sourceforge.net/downlinks/make.php)
      6. 
