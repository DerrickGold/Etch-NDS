#---------------------------------------------------------------------------------
# PAlib Project Makefile by Scognito, Tom, crash and fincs
#---------------------------------------------------------------------------------

#---------------------------------------------------------------------------------
# Please uncomment (i.e. delete the '#') only one "ARM7_SELECTED" line. If unsure,
# leave it as is (with ARM7_MP3 uncommented).
#---------------------------------------------------------------------------------

#ARM7_SELECTED := ARM7_MP3
#ARM7_SELECTED := ARM7_MP3_DSWIFI
ARM7_SELECTED := ARM7_MAXMOD_DSWIFI

#---------------------------------------------------------------------------------
# If you're using EFS uncomment "USE_EFS = YES" here.
#---------------------------------------------------------------------------------
#USE_EFS = YES
USE_NITROFS = YES

#---------------------------------------------------------------------------------
# Be sure to change these default banner TEXTs. This is the name your project will
# display in the DS menu (including some flash cards). Create a custom logo.bmp
# icon for your project too!
#---------------------------------------------------------------------------------

TEXT1 := Etch Alpha Build
TEXT2 := http://bag.nfnet.org/
TEXT3 := By BassAceGold

#---------------------------------------------------------------------------------
# TARGET is the name of the file to output
# BUILD is the directory where object files and intermediate files will be placed
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing extra header files
# AUDIO is a list of directories containing audio files for use with Maxmod
# RELEASEPATH is the output directory (Optional)
#---------------------------------------------------------------------------------

TARGET      := $(shell basename $(CURDIR))
BUILD       := build
SOURCES     := source data gfx/bin source/Tremolo source/colorwheel source/freetype-2.3.12/ source/GZip
INCLUDES    := include build data gfx source/freetype-2.3.12/ include/tremor source/GZip/include
AUDIO       := audio
RELEASEPATH := 

#---------------------------------------------------------------------------------
# If you need to change other parts of the make process, see the PA_Makefile:
#---------------------------------------------------------------------------------
# Freetype 2 configuration
FT2PATH 	:=	source/freetype-2.3.12
FT2MODULES 	:= 	ftbase ftsystem ftinit ftglyph ftbitmap \
			truetype sfnt autofit smooth
MAKEFILE_VER := ver1

include $(DEVKITPRO)/PAlib/lib/PA_MakefileFT2Tremolo
