all: libvabknd.so

libvabknd.so: vabknd.cc ../qemu-kvm-0.12.3+noroms/va.h
	g++ -shared -o libvabknd.so vabknd.cc -I../qemu-kvm-0.12.3+noroms/ -I../../comm/bknd -L../../comm/bknd/Release/ -lbkndcomm -L../../../ffmpeg-checkout-2010-07-16/libavcodec/ -lavcodec -L../../../ffmpeg-checkout-2010-07-16/libavutil -lavutil -lz -lm -lpthread -L../../../ffmpeg-checkout-2010-07-16/libswscale/ -lswscale -I/usr/include/SDL -D_GNU_SOURCE=1 -D_REENTRANT -L/usr/lib -lSDL

install: all
	cp libvabknd.so /usr/lib/

clean:
	rm *.o *.so *~ -f

