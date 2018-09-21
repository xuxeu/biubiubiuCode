D: 
cd \  
cd platform_ppc\host\bin  
mount -f D:\platform_ppc / 
cd \  
cd platform_ppc\target\deltaos\tools\monitor\.make 
set path=D:\platform_ppc\host\deltaos\gnu\x86-win32\binD:\platform_ppc\host\bin;D:\platform_ppc\target\deltaos\tools\monitor\.make;D:\platform_ppc\target\deltaos\lib\x86\pentium\little;%path% 
copy /Y D:\platform_ppc\target\deltaos\tools\monitor\makefile D:\platform_ppc\target\deltaos\tools\monitor\.make 
cd \ 
cd D:/platform_ppc/host/bin 
make.exe -k -C D:/platform_ppc/target/deltaos/tools/monitor/.make -f D:/platform_ppc/target/deltaos/tools/monitor/makefile all 
