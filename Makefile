#
# This file is just a wrapper to find and run GNU Make
#
MAKE=`(type gmake || type gnumake || type make) 2>/dev/null | cut -d\  -f3`

auto: all

*: .PHONY
	@echo $(MAKE) -f GNUmakefile $@ 
	@$(MAKE) -f GNUmakefile $@ 

.DEFAULT: .PHONY
	@echo $(MAKE) -f GNUmakefile $@ 
	@$(MAKE) -f GNUmakefile $@ 


