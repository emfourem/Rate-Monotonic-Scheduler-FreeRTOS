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
   4. **Install the FreeRTOS version 202212.01**: [FreeRTOS](https://www.freertos.org/a00104.html)
   6. **Download and install Make for Windows**: [MAKE](https://gnuwin32.sourceforge.net/downlinks/make.php)
   7. **Install MinGW version 11.0.0**: [MinGW](https://www.mingw-w64.org/downloads/#msys2)
   8. Add these paths to "Path" User Variable:
      - For make: C:\Program Files (x86)\GnuWin32\bin
      - Fot qemu: C:\Program Files\qemu
      - For the debugger used into the demo: C:\Program Files (x86)\Arm GNU Toolchain arm-none-eabi\13.2 Rel1\bin
         - Note: the paths are related to the installation on Windows, check that they are the same on your pc or if they are different, change them appropriately.
   10. Open the folder "C:\yourPath\FreeRTOSv202212.01\FreeRTOSv202212.01\FreeRTOS\Demo\CORTEX_MPS2_QEMU_IAR_GCC" as VSCode Project.
   11. Navigate to the .vscode folder.
   12. Open the file "launch.json" and change the “miDebuggerPath” to the path to the new debugger in        the GNU Arm Embedded Toolchain downloaded earlier. The new debugger will be “GNU Arm Embedded          Toolchain/10 2021.10/bin/arm-none-eabi-gdb.exe”.
       <p align="center">
          <img src="images/launch.png" alt="Edit launch file" width="80%" />
         </p>
   13. Now open the “Run and debug” (Step 1) section and click on "Start Debugging" (or click F5 - Step 2).
       <p align="center">
          <img src="images/demoDebug.png" alt="Start Debugging" width="80%" />
         </p>
   15. A window will ask you whether to proceed to the next step and then run the demo. Complete Step 1 first and then Step 2 and the Demo will start.
       <p align="center">
          <img src="images/demoStart.png" alt="Start Demo" width="80%" />
         </p>
       
