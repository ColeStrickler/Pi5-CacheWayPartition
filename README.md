# Cache Way partitioning on the Raspberry Pi 5
The Raspberry Pi 5 allows for architecturally partitioning the L3 cache ways into 4 distinct groups using the Arm DynamIQ Shared Unit. Follow the setup guide to get started


## Setup
- Step 1: Download the Raspberry Pi 5 fork of Arm Trusted Firmware https://github.com/raspberrypi/arm-trusted-firmware
- Step 2: Apply the firmware patch --> Inside the firmware repo: "patch -p1 < /path/to/way-partition.patch"
- Step 3: Build the firmware binary --> "CROSS_COMPILE=aarch64-linux-gnu- make PLAT=rpi5"
- Step 4: Copy the firmware binary(bl31.bin) to /boot/
- Step 5: Add the following line to /boot/firmware/cmdline/config.txt --> "armstub=bl31.bin"
- Step 6: Reboot the Raspberry Pi5 to ensure that everything went smoothly
- Step 7: Use the Makefile to compile the driver and the usermode utility
- Step 8: Install the driver --> "sudo insmod way-part-control.ko"
- Step 8: Use the usermode utility to communicate with the driver to write the MSR registers that control cache partitioning


## MSR Registers
The user mode utility allows for writing the following registers
- CLUSERPARTC --> allocates cache ways private to a schemeID
- CLUSTERSTASHSID --> used to set the schemeID for stash transactions read over the interconnect
- CLUSTERTHREADSIDOVR --> used to set masking bits for delegating who has control over a particular set of schemeIDs
- CLUSTERACPSID --> used to set the schemeID for ACP transactions
- CLUSTERTHREADSID --> used to set the schemeID for the current thread/CPU


For more information on these registers and precise use please see: https://developer.arm.com/documentation/100453/0400/functional-description/l3-cache/l3-cache-partitioning?lang=en
