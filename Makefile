ALL:  camera_cal.exe camera_cal_init.exe camera_cal_init.o terminal_server.o mainh.o
#camera_cal.exe
SOME: 
INC = -I/usr/include/opencv4 #-I/usr/include/eigen3 #-I/usr/include/gtk-4.0 -I/usr/include/glib-2.0 #-I/usr/include/opencv4/opencv2 #/usr/include
CC=g++

camera_cal.exe: camera_cal.cpp camera_cal.h ../../mainh.h mainh.o camera_cal_init.o  terminal_server.o
	$(CC) $(INC)  -Wall -g -o camera_cal.exe camera_cal.cpp camera_cal_init.o terminal_server.o mainh.o `pkg-config --cflags --libs opencv4` -Wno-unused-variable -Wno-unused-but-set-variable -lopencv_highgui

camera_cal_init.o: camera_cal_init.cpp camera_cal.h ../../mainh.h  terminal_server.h terminal_server.c 
	$(CC) $(INC) -Wall -g -c camera_cal_init.cpp   `pkg-config --cflags --libs opencv4` -Wno-unused-variable -Wno-unused-but-set-variable -lopencv_highgui

camera_cal_init.exe: camera_cal_init.cpp camera_cal.h ../../mainh.h mainh.o terminal_server.h terminal_server.c terminal_server.o
	$(CC) $(INC) -DINIT_MAIN_TEST -Wall -g -o camera_cal_init.exe camera_cal_init.cpp terminal_server.o mainh.o `pkg-config --cflags --libs opencv4` -Wno-unused-variable -Wno-unused-but-set-variable -lopencv_highgui

terminal_server.o: ../../mainh.h mainh.o terminal_server.h terminal_server.c
	$(CC) $(INC) -Wall -g -c terminal_server.c -Wno-unused-variable -Wno-unused-but-set-variable 

mainh.o: ../../mainh.h ../../mainh.c
	$(CC)  -g -Wall  -c -o mainh.o ../../mainh.c -lm
	
