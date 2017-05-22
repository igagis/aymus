include prorab.mk


this_name := aymus


this_srcs := $(call prorab-src-dir, src)


this_cxxflags := -Wall
this_cxxflags += -Wno-comment #no warnings on nested comments
this_cxxflags += -Wno-format
this_cxxflags += -fstrict-aliasing #strict aliasing!!!
this_cxxflags += -g
this_cxxflags += -std=c++11
this_cxxflags += -O3

ifeq ($(debug), true)
    this_cxxflags += -DDEBUG
endif

ifeq ($(prorab_os),windows)
#    this_ldlibs += -lmingw32 #these should go first, otherwise linker will complain about undefined reference to WinMain
#    this_ldlibs += $(prorab_this_dir)../../src/libaudout$(prorab_lib_extension)
#    this_ldflags += -L/usr/lib -L/usr/local/lib
#    this_ldlibs +=  -lglew32 -lopengl32 -lpng -ljpeg -lz -lfreetype -mwindows

#    this_cxxflags += -I/usr/include -I/usr/local/include

    #WORKAROUND for MinGW bug:
#    this_cxxflags += -D__STDC_FORMAT_MACROS
else ifeq ($(prorab_os),macosx)
#    this_ldlibs += $(prorab_this_dir)../../src/libaudout$(prorab_lib_extension)
    this_ldlibs += -framework AudioUnit
else ifeq ($(prorab_os),linux)
    this_ldlibs += -laudout -lnitki -lpulse-simple -lpogodi -lpulse -lm
endif

this_ldlibs += -lstdc++


$(eval $(prorab-build-app))


define this_rules
test:: $(prorab_this_name)
	@echo running $$^...
	@$$^
endef
$(eval $(this_rules))
