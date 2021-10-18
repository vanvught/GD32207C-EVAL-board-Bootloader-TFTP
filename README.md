# GD32F20x-Bootloader-TFTP

This bootloader will install your application by means of the TFTP protocol. There is no need to change your application code. 
Per default DHCP is used for obtaining the ip-address.

The bootloader can be installed with the tools supplied by GigaDevice -> [http://www.gd32mcu.com/en/download/7?kw=GD32F2](http://www.gd32mcu.com/en/download/7?kw=GD32F2)

The limitation for the firmware file to be uploaded is given by the RAM available. For example, the MCU on GD32207C-EVAL is the [GD32F207VCT6](https://www.gigadevice.com/microcontroller/gd32f207vct6/). With the 128K RAM we have the firmware file size limit of 104K.

There are 2 places for this configuration:

File: `gd32f20x_flash.ld`

	_Min_Heap_Size  = 0x1A800; /* 106K required amount of heap  */
	_Min_Stack_Size = 0x00400; /* 1K required amount of stack */
	
	/* Specify the memory areas */
	MEMORY
	{
	  FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 256K
	  RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 128K
	}

File: `spiflashinstall.h`

	#elif defined (GD32)
	# define FIRMWARE_MAX_SIZE 	0x1A000		// 104K
	# define OFFSET_UIMAGE		0x007000	// 28K

The 2K difference is needed for other `new` (`malloc`) within the bootloader.

The change to be made in your application is in the file `gd32f20x_flash.ld`. 

	/* Specify the memory areas */
	MEMORY
	{
	  FLASH (rx)      : ORIGIN = 0x08007000, LENGTH = 512K
	  RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 128K
	}
	
The `FLASH ORIGIN` must match the `OFFSET_UIMAGE` from the bootloader file `spiflashinstall.h`

The direcroty `GD32F20x_Demo_Suites_V2.2.1` contains the `gd32f20x.bin` for each demo. There is also the shell script `do-tftp.sh` included. 
Usage: `do-tftp.sh <ip-address>` 

A ready to compile `GD32F20x_Demo_Suites_V2.2.1` can be found here -> [https://github.com/vanvught/GD32207C-EVAL-board](https://github.com/vanvught/GD32207C-EVAL-board)

The code for the bootloader is a fork from [https://github.com/vanvught/rpidmx512](https://github.com/vanvught/rpidmx512). In order to reduce the memory footprint, some functions are not available. 