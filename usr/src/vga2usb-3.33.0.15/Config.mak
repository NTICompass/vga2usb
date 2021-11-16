# Which drivers to build
CONFIG_VIDEO_VGA2USB	:= m

# Where is the target kernel
ifeq ($(KDIR),)
KDIR	:= /lib/modules/$(shell uname -r)/build
endif

#  FC_KERNEL should be defined for Fedora Core based kernels and undefined for stock kernel
#EXTRA_CFLAGS += -DFC_KERNEL

#EXTRA_CFLAGS += -DVGA2USB_EVAL -DVGA2USB_EVAL3
#EXTRA_CFLAGS += -DCONFIG_USB_DEBUG_TRACE=1 -DV2U_DEBUG_TRACE=1
