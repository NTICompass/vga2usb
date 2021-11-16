# $Id: Makefile 52005 2020-11-02 10:31:32Z adrozdov $

ifeq ($(src),)
src := $(shell pwd)
endif
include $(src)/Config.mak

obj-$(CONFIG_VIDEO_VGA2USB)	:= vga2usb.o

obj-binary := vga2usb_bins.o
obj-binary-pathed	:= $(patsubst %,$(src)/%,$(obj-binary))

vga2usb-objs = drvwrapper.o drvwrapper_lib.o $(obj-binary)
#LDFLAGS += $(obj-binary-pathed)
#vga2usb-objs = drvwrapper.o


# If we find Rules.make, we can assume we're using the old 2.4 style building
OLDMAKE=$(wildcard $(KDIR)/Rules.make)

vga2usb-module := vga2usb.ko

default: $(FSRC)
	touch $(src)/.$(obj-binary).cmd
	$(MAKE) -C $(KDIR) M=$(src) modules

clean:
	-rm -f vga2usb.o vga2usb.ko drvwrapper.o drvwrapper_lib.o

distclean:
	-rm -f *\.o *\.ko *\.mod* \.depend \.*\.o\.flags \.*\.o\.cmd \.*\.ko\.cmd


install: default
	$(MAKE) -C $(KDIR) M=$(src) modules_install
	[ -d "/lib/udev/rules.d" ] && install -m 0644 extra/40-epiphan.rules /lib/udev/rules.d/
	depmod -a

install_nodepmod: default
	$(MAKE) -C $(KDIR) M=$(src) modules_install

# Load vga2usb module and its prerequisites
comma:= ,
empty:=
space:= $(empty) $(empty)
depline  = $(shell /sbin/modinfo $(vga2usb-module) | grep depends:)
depcomma = $(strip $(subst depends:,,$(depline)))
deplist  = $(subst $(comma),$(space),$(depcomma))
modprobe = sudo /sbin/modprobe $(mod);
load:   default
	$(foreach mod,$(deplist),$(modprobe))
	sudo /sbin/insmod $(vga2usb-module)

unload:
	-sudo /sbin/rmmod vga2usb

reload:
	make unload
	sleep 1
	make load

dot:= .
modverline = $(shell awk '/\#define V2U_VERSION_/ {print $$3}' ../../../../include/v2u_version.h)
modversion  = $(subst $(space),$(dot),$(modverline))
kver = $(shell echo ${KDIR} | awk '{ gsub("^[^0-9]*", ""); print }' | awk '{ gsub("/.*", ""); print }')
distro: default
	rm -fr .tmp_versions .*.cmd Module.symvers Module.markers modules.order vga2usb.mod.* drvwrapper_lib.o drvwrapper.o vga2usb.o
	tar -cjvf ../vga2usb-$(modversion)-$(kver).tbz --exclude  \*.tbz --exclude CVS --exclude .svn --exclude \*~ . ../../extra/40-epiphan.rules

