BUILDDIR=build/`uname -sm| sed 's/  */-/g'`

all: dirs
	@echo "Executables are in build/":
	@ls -al build/*/bin/smacqq
	@ln -fs $(BUILDDIR)/bin/smacqq smacqq

platform: .PHONY
	misc/config-env # Make the build directory and its config.mk
	@mkdir -p $(BUILDDIR)
	@ln -sf `pwd`/Makefile.top $(BUILDDIR)/Makefile

%: platform .PHONY
	@echo $(MAKE) -C $(BUILDDIR) $@ 
	@$(MAKE) -C $(BUILDDIR) $@ 

.PHONY:
	@true


