ALLDIRS = CgosDrv CgosLib CgosDump CgosMon CgosTest

# Determination of the target system
target = $(shell uname)
sysdir = Lx

ifdef target
 ifeq ("$(target)","QNX")
  sysdir = Qx
 endif
endif

M = for i in $(ALLDIRS); do make -C $$i/$(sysdir)
N = || exit 1; done

ifeq ("$(target)","QNX")
install:
	cp CgosDump/$(sysdir)/cgosdump /usr/bin
	cp CgosMon/$(sysdir)/cgosmon /usr/bin
	cp CgosLib/$(sysdir)/libcgos.so /usr/lib
	cp CgosDrv/$(sysdir)/cgosdrv /usr/lib
	cp CgosTest/$(sysdir)/cgostest /usr/lib
else

default:
	$(M) $(N)

install:
	install -m 755 -o root -g root CgosDump/$(sysdir)/cgosdump /usr/bin
	install -m 755 -o root -g root CgosMon/$(sysdir)/cgosmon /usr/bin
	install -m 755 -o root -g root CgosTest/$(sysdir)/cgostest /usr/bin
	install -m 755 -o root -g root CgosLib/$(sysdir)/libcgos.so /usr/lib
	install -m 644 -o root -g root CgosDrv/$(sysdir)/cgosdrv.ko /lib/modules/`uname -r`/kernel/drivers/misc 
	depmod -a

# copy the udev rules to /lib/udev/rules.d - but only if the file does not already exist
	@if ! [ -e /lib/udev/rules.d/99-cgos.rules ]; \
	then \
		cp 99-cgos.rules /lib/udev/rules.d/99-cgos.rules; \
		echo "Copied CGOS udev rule to /lib/udev/rules.d."; \
	fi
	
# copy the configuration file for module insertion at startup to /usr/lib/modules-load.d - but only if the file does not already exist
	
	@if ! [ -e /usr/lib/modules-load.d ]; \
	then \
		mkdir /usr/lib/modules-load.d; \
	fi
	
	@if ! [ -e /usr/lib/modules-load.d/cgos.conf ]; \
	then \
		cp cgos.conf /usr/lib/modules-load.d/cgos.conf; \
		echo "Added cgos.conf to /usr/lib/modules-load.d for cgosdrv module insertion at system startup."; \
	fi
	
clean:
	$(M) $@ $(N)

endif


