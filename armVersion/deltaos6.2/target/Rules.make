#
# This file contains rules which are shared between multiple Makefiles.
#

#
# False targets.
#
.PHONY: dummy


#VERSION_FLAGS =     -DMAJOR_VERSION=$(MAJOR_VERSION) \
#                    -DMINOR_VERSION=$(MINOR_VERSION) \
#                    -DBUILD_NUMBER=$(BUILD_NUMBER) \
#                    -DRELEASE_DATE=$(RELEASE_DATE) \
#                    -DEDITION=$(EDITION)
                    
#
# Common rules
#

$(BSP_OBJ_PATH)/%.o: %.s
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -xassembler-with-cpp $(VERSION_FLAGS) -o $@ $<

$(BSP_OBJ_PATH)/%.o: %.S
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(CFLAGS_$@)  $(VERSION_FLAGS) -c -o $@ $<

$(BSP_OBJ_PATH)/%.s: %.c
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(CFLAGS_$@) $(VERSION_FLAGS) -S $< -o $@

$(BSP_OBJ_PATH)/%.i: %.c
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(CFLAGS_$@)  $(VERSION_FLAGS) -E $< > $@

$(BSP_OBJ_PATH)/%.o: %.c
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(CFLAGS_$@)  $(VERSION_FLAGS) -c -o $@ $<

$(BSP_OBJ_PATH)/%.o: %.cxx
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(CFLAGS_$@)  $(VERSION_FLAGS) -c -o $@ $<

$(BSP_OBJ_PATH)/%.o: %.cpp
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(CFLAGS_$@)  $(VERSION_FLAGS) -c -o $@ $<

$(BSP_OBJ_PATH)/%.o: %.cc
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(CFLAGS_$@)  $(VERSION_FLAGS) -c -o $@ $<

$(BSP_OBJ_PATH)/%.o: %.C
	$(CC)  $(CFLAGS) $(EXTRA_CFLAGS) $(CFLAGS_$@)  $(VERSION_FLAGS) -c -o $@ $<

##

%.o: %.s
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -xassembler-with-cpp  -o $@ $<

%.o: %.S
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(CFLAGS_$@)  $(VERSION_FLAGS) -c -o $@ $<

%.s: %.c
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(CFLAGS_$@) $(VERSION_FLAGS) -S $< -o $@

%.i: %.c
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(CFLAGS_$@)  $(VERSION_FLAGS) -E $< > $@

%.o: %.c
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(CFLAGS_$@)  $(VERSION_FLAGS) -c -o $@ $<

%.o: %.cxx
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(CFLAGS_$@)  $(VERSION_FLAGS) -c -o $@ $<

%.o: %.cpp
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(CFLAGS_$@)  $(VERSION_FLAGS) -c -o $@ $<

%.o: %.cc
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(CFLAGS_$@)  $(VERSION_FLAGS) -c -o $@ $<

%.o: %.C
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(CFLAGS_$@)  $(VERSION_FLAGS) -c -o $@ $<


##

## Compile Sub Dirs
_subdir_make : .dummy $(patsubst %, _dir_%, $(SUBDIRS))
$(patsubst %, _dir_%, $(SUBDIRS)) : dummy
	@if [ -f  $(patsubst _dir_%, %, $@)/Makefile ] ; then \
	  $(MAKE) -C $(patsubst _dir_%, %, $@) ;\
	fi

## Clean Sub Dirs
_subdir_clean : .dummy  $(patsubst %, _dir_clean_%, $(SUBDIRS))
$(patsubst %, _dir_clean_%, $(SUBDIRS)) : dummy
	@if [ -f $(patsubst _dir_clean_%, %, $@)/Makefile ] ; then \
	    $(MAKE) -C $(patsubst _dir_clean_%, %, $@) clean ; \
	fi



ifdef L_NAME
$(L_NAME) : $(L_OBJS)
	$(AR) $(AR_FLAGS) $(L_PATH)/$@ $^
endif

%_clean:
	@if [ -f $(patsubst %_clean, %, $@)/Makefile ] ; then \
		$(MAKE) -C $(patsubst %_clean, %, $@) clean; \
	fi

%_make:
	@if [ -f $(patsubst %_make, %, $@)/Makefile ] ; then \
		$(MAKE) -C $(patsubst %_make, %, $@); \
	fi

.dummy:

