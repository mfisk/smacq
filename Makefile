BUILDDIR=build/`uname -sm| sed 's/  */-/g'`

all: platform dirs
	@echo "Executables are in build/":
	@ls -al build/*/bin/smacqq
	@ln -fs build/*/bin/smacqq smacqq

platform:
	misc/config-env # Make the build directory and its config.mk
	@mkdir -p $(BUILDDIR)
	@ln -sf `pwd`/Makefile.top $(BUILDDIR)/Makefile

%: .ALWAYS
	@echo $(MAKE) -C $(BUILDDIR) $@ 
	@$(MAKE) -C $(BUILDDIR) $@ 

.ALWAYS:
	@true
