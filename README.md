#Etch-NDS

A historical archive for a project I created prior to any formal programming education.

![Alt text](/screenshots/etchtitle2.png?raw=true "Etch Title")
![Alt text](/screenshots/colorselector3.png?raw=true "16/24 bit color selector")
![Alt text](/screenshots/toolbarpic.png?raw=true "Toolbar")
![Alt text](/screenshots/fbfinaletch.png?raw=true "Filebrowser")


##Original Readme:

Etch Alpha Build by BassAceGold

###Installation
- copy Etch.nds to anywhere on card
- copy etch.ini to the same folder as Etch.nds
- ensure picture directory (as specified in etch.ini) exists
- ttf fonts go in directory as specified in etch.ini

###Current Features Supported:
- Creation and Saving of 24,16 and 8 bit bitmaps
- Creation and Saving of Etch Project File (epf) formats (24,16 and 8 bit)
- creation and saving of images up to 1280x1024 (without external ram)
- layers
- external ram and ram overclocking support (ez flash, supercard, etc..)
- customizable options, controls and program colors via ini file
- zooming with configurable grids
- ttf font support in keyboard menu with Anti-Aliasing
- dynamic tool bar
- mini view on top screen
- standard tools (pencil, bucket, eraser, line and shape tools, color picker)
- editable 8bit color palette when in 8bit mode
- full file browser which can delete and compress files

###Not finished or working Features
- R button is not customizable, it is reserved for Debug
- keyboard menu is incomplete
- need to optimize font rendering in kb menu
- selector tools
- Layer drawing is still basic, more time is needed to add the advanced features of layer drawing (alpha blending handling is missing so although you're drawing below a layer with alpha it won't show up)

Planned Features
-copy, paste, rotate, cut for selector tools
-options menu
-importation and exportation of 8bit palettes for 8bit mode
-auto load images via an ini file
-sprite/bg animation testing mode -this will probably be a feature for the release after the initial public release
-font size, color and positioning options in keyboard menu
 

 
