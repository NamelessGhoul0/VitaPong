TITLE_ID = VITAPONG0
TARGET = Pong
OBJS   = src/font.o src/gekihen.o src/main.o src/splash.o \
	src/Nogg/src/util/memory.o \
	src/Nogg/src/util/float-to-int16.o \
	src/Nogg/src/util/decode-frame.o \
	src/Nogg/src/decode/stb_vorbis.o \
	src/Nogg/src/decode/setup.o \
	src/Nogg/src/decode/seek.o \
	src/Nogg/src/decode/packet.o \
	src/Nogg/src/decode/io.o \
	src/Nogg/src/decode/decode.o \
	src/Nogg/src/decode/crc32.o \
	src/Nogg/src/api/version.o \
	src/Nogg/src/api/seek-tell.o \
	src/Nogg/src/api/read-int16.o \
	src/Nogg/src/api/read-float.o \
	src/Nogg/src/api/open-file.o \
	src/Nogg/src/api/open-callbacks.o \
	src/Nogg/src/api/open-buffer.o \
	src/Nogg/src/api/info.o \
	src/Nogg/src/api/close.o \
	src/Media/WavFile.o \
	src/Media/FileBuffer.o \
	src/Media/Audio.o

LIBS = -lvita2d -lSceDisplay_stub -lSceGxm_stub -lSceSysmodule_stub \
	-lSceCtrl_stub -lSceTouch_stub -lSceAudio_stub -lfreetype -lpng \
	-lSceCommonDialog_stub -lz -lm

PREFIX  = arm-vita-eabi
CC      = $(PREFIX)-gcc
CFLAGS  = -Wl,-q -Wall -O3 -I./src/Nogg -I./src/Media
ASFLAGS = $(CFLAGS)

all: $(TARGET).vpk

$(TARGET).vpk: eboot.bin
	vita-mksfoex -s TITLE_ID=$(TITLE_ID) "$(TARGET)" param.sfo
	vita-pack-vpk -s param.sfo -b eboot.bin \
	-a res/icon0.png=sce_sys/icon0.png \
	-a res/template.xml=sce_sys/livearea/contents/template.xml \
	-a res/startup.png=sce_sys/livearea/contents/startup.png \
	-a res/bg0.png=sce_sys/livearea/contents/bg0.png \
	-a res/pong.ogg=pong.ogg $@

eboot.bin: $(TARGET).velf
	vita-make-fself -c -s $< eboot.bin

$(TARGET).velf: $(TARGET).elf
	$(PREFIX)-strip -g $<
	vita-elf-create $< $@

$(TARGET).elf: $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

clean:
	@rm -rf *.velf *.elf *.vpk $(OBJS) param.sfo eboot.bin

vpksend: $(TARGET).vpk
	curl -T $(TARGET).vpk ftp://$(PSVITAIP):1337/ux0:/
	@echo "Sent."

send: eboot.bin
	curl -T eboot.bin ftp://$(PSVITAIP):1337/ux0:/app/$(TITLE_ID)/
	@echo "Sent."
