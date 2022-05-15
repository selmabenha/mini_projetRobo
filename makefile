
#This is a template to build your own project with the e-puck2_main-processor folder as a library.
#Simply adapt the lines below to be able to compile

# Define project name here
PROJECT = Project

#Define path to the e-puck2_main-processor folder
GLOBAL_PATH = ../../lib/e-puck2_main-processor

#Source files to include
CSRC += ./main.c \
		./audio_processing.c \
		./communications.c \
		./fft.c \
		./tof.c \
		./detect_color.c \
		./create_path.c \
		./motors_pro.c \

#Header folders to include
INCDIR += 

#Jump to the main Makefile
include $(GLOBAL_PATH)/Makefile