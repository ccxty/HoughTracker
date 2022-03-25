./bin/ReadRoot: ReadRoot.C
	g++ `root-config --cflags --libs` -lMinuit -L/software/STCF/OSCAR/2.0.0/sniper/InstallArea/Linux-x86_64/lib -L/home/zhouh/STCF_Oscar2.0.0/InstallArea/Linux-x86_64/lib -L/software/STCF/OSCAR/2.0.0/offline/InstallArea/Linux-x86_64/lib -I/software/STCF/OSCAR/2.0.0/offline/InstallArea/include -I/home/zhouh/STCF_Oscar2.0.0/InstallArea/include -I/home/txiao/STCF_Oscar2.0.0/share/pi+/test2/include -lBaseEvent -lEDMUtil -lStcfMCEvent $^ -o $@
./bin/HoughFinder: ./src/main.C 
	g++ `root-config --cflags --libs` -lMinuit -L/software/STCF/OSCAR/2.0.0/sniper/InstallArea/Linux-x86_64/lib -L/home/zhouh/STCF_Oscar2.0.0/InstallArea/Linux-x86_64/lib -L/software/STCF/OSCAR/2.0.0/offline/InstallArea/Linux-x86_64/lib -I/software/STCF/OSCAR/2.0.0/offline/InstallArea/include -I/home/zhouh/STCF_Oscar2.0.0/InstallArea/include -I/home/txiao/STCF_Oscar2.0.0/share/pi+/test2/include -lBaseEvent -lEDMUtil -lStcfMCEvent $^ -o $@
./bin/Test: ./src/test.C
	g++ `root-config --cflags --libs` -lMinuit -L/software/STCF/OSCAR/2.0.0/sniper/InstallArea/Linux-x86_64/lib -L/home/zhouh/STCF_Oscar2.0.0/InstallArea/Linux-x86_64/lib -L/software/STCF/OSCAR/2.0.0/offline/InstallArea/Linux-x86_64/lib -I/software/STCF/OSCAR/2.0.0/offline/InstallArea/include -I/home/zhouh/STCF_Oscar2.0.0/InstallArea/include -I/home/txiao/STCF_Oscar2.0.0/share/pi+/test2/include -lBaseEvent -lEDMUtil -lStcfMCEvent $^ -o $@
./bin/Test2: ./src/test2.C
	g++ `root-config --cflags --libs` -lMinuit -L/software/STCF/OSCAR/2.0.0/sniper/InstallArea/Linux-x86_64/lib -L/home/zhouh/STCF_Oscar2.0.0/InstallArea/Linux-x86_64/lib -L/software/STCF/OSCAR/2.0.0/offline/InstallArea/Linux-x86_64/lib -I/software/STCF/OSCAR/2.0.0/offline/InstallArea/include -I/home/zhouh/STCF_Oscar2.0.0/InstallArea/include -I/home/txiao/STCF_Oscar2.0.0/share/pi+/test2/include -lBaseEvent -lEDMUtil -lStcfMCEvent $^ -o $@
./bin/HoughFinder_noise: ./src/main2.C
	g++ `root-config --cflags --libs` -lMinuit -L/software/STCF/OSCAR/2.0.0/sniper/InstallArea/Linux-x86_64/lib -L/home/zhouh/STCF_Oscar2.0.0/InstallArea/Linux-x86_64/lib -L/software/STCF/OSCAR/2.0.0/offline/InstallArea/Linux-x86_64/lib -I/software/STCF/OSCAR/2.0.0/offline/InstallArea/include -I/home/zhouh/STCF_Oscar2.0.0/InstallArea/include -I/home/txiao/STCF_Oscar2.0.0/share/pi+/test2/include -lBaseEvent -lEDMUtil -lStcfMCEvent $^ -o $@
./bin/HoughFinder_multi: ./src/main3.C
	g++ `root-config --cflags --libs` -lMinuit -L/software/STCF/OSCAR/2.0.0/sniper/InstallArea/Linux-x86_64/lib -L/home/zhouh/STCF_Oscar2.0.0/InstallArea/Linux-x86_64/lib -L/software/STCF/OSCAR/2.0.0/offline/InstallArea/Linux-x86_64/lib -I/software/STCF/OSCAR/2.0.0/offline/InstallArea/include -I/home/zhouh/STCF_Oscar2.0.0/InstallArea/include -I/home/txiao/STCF_Oscar2.0.0/share/pi+/test2/include -lBaseEvent -lEDMUtil -lStcfMCEvent $^ -o $@
