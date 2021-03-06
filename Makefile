./bin/ReadRoot: ./main/ReadRoot.C
	g++ `root-config --cflags --libs` -lMinuit -L/software/STCF/OSCAR/2.0.0/sniper/InstallArea/Linux-x86_64/lib -L/home/zhouh/STCF_Oscar2.0.0/InstallArea/Linux-x86_64/lib -L/software/STCF/OSCAR/2.0.0/offline/InstallArea/Linux-x86_64/lib -I/software/STCF/OSCAR/2.0.0/offline/InstallArea/include -I/home/zhouh/STCF_Oscar2.0.0/InstallArea/include -I/home/txiao/STCF_Oscar2.0.0/share/pi+/test2/include -lBaseEvent -lEDMUtil -lStcfMCEvent $^ -o $@
./bin/HoughTracker: ./main/main_hough.cpp ./include/Hough.h
	g++ `root-config --cflags --libs` -std=c++14 -lMinuit -L/software/STCF/OSCAR/2.0.0/sniper/InstallArea/Linux-x86_64/lib -L/home/zhouh/STCF_Oscar2.0.0/InstallArea/Linux-x86_64/lib -L/software/STCF/OSCAR/2.0.0/offline/InstallArea/Linux-x86_64/lib -I/software/STCF/OSCAR/2.0.0/offline/InstallArea/include -I/home/zhouh/STCF_Oscar2.0.0/InstallArea/include -I/home/txiao/STCF_Oscar2.0.0/share/pi+/test2/include -lBaseEvent -lEDMUtil -lStcfMCEvent -O2 $^ -o $@
./bin/LocalTracker: ./main/main_local.cpp ./include/Hough.h
	g++ `root-config --cflags --libs` -std=c++14 -lMinuit -L/software/STCF/OSCAR/2.0.0/sniper/InstallArea/Linux-x86_64/lib -L/home/zhouh/STCF_Oscar2.0.0/InstallArea/Linux-x86_64/lib -L/software/STCF/OSCAR/2.0.0/offline/InstallArea/Linux-x86_64/lib -I/software/STCF/OSCAR/2.0.0/offline/InstallArea/include -I/home/zhouh/STCF_Oscar2.0.0/InstallArea/include -I/home/txiao/STCF_Oscar2.0.0/share/pi+/test2/include -lBaseEvent -lEDMUtil -lStcfMCEvent -O2 $^ -o $@
clean:
	rm -f ./bin/*
