TOOLCHAIN := $(OO_PS4_TOOLCHAIN)
INTDIR := build

CXX := clang++
LD := ld.lld

LIBS := -lc -lkernel -lc++ -lSceUserService -lSceVideoOut -lSceAudioOut -lScePad -lSceSysmodule -lSDL2

CXXFLAGS := --target=x86_64-pc-freebsd12-elf -fPIC -funwind-tables -c -isysroot $(TOOLCHAIN) -isystem $(TOOLCHAIN)/include -isystem $(TOOLCHAIN)/include/c++/v1

LDFLAGS := -m elf_x86_64 -pie --script $(TOOLCHAIN)/link.x --eh-frame-hdr -L$(TOOLCHAIN)/lib $(LIBS) $(TOOLCHAIN)/lib/crt1.o

all: eboot.bin

$(INTDIR):
	mkdir -p $(INTDIR)

$(INTDIR)/main.o: main.cpp | $(INTDIR)
	$(CXX) $(CXXFLAGS) -o $@ $<

eboot.bin: $(INTDIR)/main.o
	$(LD) $< -o $(INTDIR)/snake.elf $(LDFLAGS)
	$(TOOLCHAIN)/bin/linux/create-fself -in=$(INTDIR)/snake.elf -out=$(INTDIR)/snake.oelf --eboot eboot.bin --paid 0x3800000000000011

clean:
	rm -rf $(INTDIR) eboot.bin
