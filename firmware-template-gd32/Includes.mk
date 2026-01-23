$(info "Includes.mk")

INCLUDES:=-I./include -I../common/include -I../include 
INCLUDES+=-I../firmware-template-gd32/include
INCLUDES+=-I../CMSIS/Core/Include
INCLUDES+=-I../lib-gd32/${FAMILY}/${FAMILY_UC}_standard_peripheral/Include
INCLUDES+=-I../lib-gd32/${FAMILY}/CMSIS/GD/${FAMILY_UC}/Include
INCLUDES+=-I../lib-gd32/include
INCLUDES+=$(addprefix -I,$(EXTRA_INCLUDES))

ifeq ($(findstring ENABLE_USB_HOST,$(DEFINES)), ENABLE_USB_HOST)
	USB_HOST=1
endif
ifeq ($(findstring ENABLE_USB_HOST,$(MAKE_FLAGS)), ENABLE_USB_HOST)
	USB_HOST=1
endif

ifeq ($(findstring ENABLE_USB_DEVICE,$(DEFINES)), ENABLE_USB_DEVICE)
	USB_DEVICE=1
endif
ifeq ($(findstring ENABLE_USB_DEVICE,$(MAKE_FLAGS)), ENABLE_USB_DEVICE)
	USB_DEVICE=1
endif

ifeq ($(findstring ENABLE_USB_HOST,$(DEFINES)), ENABLE_USB_HOST)
	USB_HOST_MSC=1
endif
ifeq ($(findstring ENABLE_USB_HOST,$(MAKE_FLAGS)), ENABLE_USB_HOST)
	USB_HOST_MSC=1
endif

ifdef USB_HOST
	INCLUDES+=-I../lib-gd32/device/usb
	INCLUDES+=-I../lib-hal/device/usb/host/gd32
endif

ifeq ($(findstring gd32f20x,$(FAMILY)), gd32f20x)
	ifdef USB_HOST
		INCLUDES+=-I../lib-gd32/${FAMILY}/GD32F20x_usbfs_library/driver/Include
		INCLUDES+=-I../lib-gd32/${FAMILY}/GD32F20x_usbfs_library/host/core/Include
		INCLUDES+=-I../lib-gd32/${FAMILY}/GD32F20x_usbfs_library/ustd/common
		ifdef USB_HOST_MSC
			INCLUDES+=-I../lib-gd32/${FAMILY}/GD32F20x_usbfs_library/host/class/msc/Include
			INCLUDES+=-I../lib-gd32/${FAMILY}/GD32F20x_usbfs_library/ustd/class/msc
		endif
	endif
endif

ifeq ($(findstring gd32f4xx,$(FAMILY)), gd32f4xx)
	INCLUDES+=-I../lib-gd32/${FAMILY}/GD32F4xx_usb_library/ustd/common
 	INCLUDES+=-I../lib-gd32/${FAMILY}/GD32F4xx_usb_library/driver/Include
  ifdef USB_HOST	
		INCLUDES+=-I../lib-gd32/${FAMILY}/GD32F4xx_usb_library/host/core/Include
		ifdef USB_HOST_MSC
			INCLUDES+=-I../lib-gd32/${FAMILY}/GD32F4xx_usb_library/host/class/msc/Include
			INCLUDES+=-I../lib-gd32/${FAMILY}/GD32F4xx_usb_library/ustd/class/msc
		endif
	endif
  ifdef USB_DEVICE
   	INCLUDES+=-I../gd32_emac_debug_usb-midi/include
  
  	INCLUDES+=-I../lib-gd32/${FAMILY}/GD32F4xx_usb_library/device/class/hid/Include
  	INCLUDES+=-I../lib-gd32/${FAMILY}/GD32F4xx_usb_library/device/core/Include
	  INCLUDES+=-I../lib-gd32/${FAMILY}/GD32F4xx_usb_library/ustd/class/hid
  endif
endif

ifeq ($(findstring gd32h7xx,$(FAMILY)), gd32h7xx)
	ifdef USB_HOST	
		INCLUDES+=-I../lib-gd32/${FAMILY}/GD32H7xx_usbhs_library/driver/Include
		INCLUDES+=-I../lib-gd32/${FAMILY}/GD32H7xx_usbhs_library/host/core/Include
		INCLUDES+=-I../lib-gd32/${FAMILY}/GD32H7xx_usbhs_library/ustd/common
		ifdef USB_HOST_MSC
			INCLUDES+=-I../lib-gd32/${FAMILY}/GD32H7xx_usbhs_library/host/class/msc/Include
			INCLUDES+=-I../lib-gd32/${FAMILY}/GD32H7xx_usbhs_library/ustd/class/msc
		endif
	endif
endif

ifdef USB_HOST_MSC
		INCLUDES+=-I../lib-hal/ff14b/source
endif