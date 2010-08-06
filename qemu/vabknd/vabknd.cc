#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/rtc.h>
#include <linux/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <SDL.h>
#include <SDL_thread.h>
typedef int64_t UINT64_C;
extern "C"{
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
}
#include <iostream>
#include "connection.h"
#include "MessageConnection.h"
#include "MessageReader.h"
#include "va.h"
#include "TcpListener.h"

using namespace va;
using namespace std;

VAContext* vactx;
SDL_Thread *chh_tid;
SDL_Thread *decode_tid;
#define CH_PORT 8198
#define DECODE_PORT 8199

int changeHandler_thread(void *v)
{
	vactx->clientRect.l = 0;
	vactx->clientRect.t = 0;
	vactx->clientRect.w = 1024;
	vactx->clientRect.h = 768;

	TcpListener listener(CH_PORT);
	while(1) try{
		MessageConnection conn(listener.acceptConnection());
		while(1){
			Message m = conn.readMessage();
    			int header  = m.getHeader();
			if(header!=7){	
				printf("error header: %d\n",header);
				continue;
			}
			MessageReader mr = MessageReader(m);
			int left = mr.readInt32();
			int width = mr.readInt32();
			int top = mr.readInt32();
			int height = mr.readInt32();
			printf("change handler thread accept:\nleft:%d width:%d top:%d height:%d\n",left,width,top,height);
			SDL_LockMutex(vactx->mutex);
			vactx->clientRect.l = left;
			vactx->clientRect.t = top;
			vactx->clientRect.w = width;
			vactx->clientRect.h = height;
			SDL_UnlockMutex(vactx->mutex);
	 }
	}catch(std::exception& e) {
        	std::cerr << e.what() << std::endl;
    	}


}

void codec_init(int i)
{
 	CodecID cid;
    	switch(i){
		case 5: {cid =  CODEC_ID_H264;
			break;}
		case 8: {cid = CODEC_ID_MPEG4;
			break;}
                case 7: {cid = CODEC_ID_MPEG2VIDEO;
                        break;}
		default:{printf("Unsupported format\n");
			throw cid;}
    	}
    
	AVCodec* codec = avcodec_find_decoder(cid);	
    	
	if (!codec) {
        	fprintf(stderr, "codec not found\n");
        	throw cid;
    	}
    	if(vactx->avctx) {
		av_free(vactx->avctx);
		vactx->avctx = NULL;
    	}
    	
	vactx->avctx = avcodec_alloc_context();
    	if (avcodec_open(vactx->avctx, codec) < 0) {
        	fprintf(stderr, "could not open codec\n");
        	throw cid;
    	}
    	 	
}

int decode_thread(void *v)
{
    avcodec_init();
    avcodec_register_all();
    /*if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER|SDL_INIT_EVENTTHREAD)){
	fprintf(stderr,"Couldn't init SDL %s \n",SDL_GetError());	
	exit(1);
    }*/
    int i=0;
    int got,len=0;
    TcpListener listener(DECODE_PORT);	
    while (1) try {
	MessageConnection conn(listener.acceptConnection());
	while(1){
    		Message m = conn.readMessage();
    		int header  = m.getHeader();
    		printf("header: %d\nsize:%d\n",header,m.getLength());
    		if(header==1){
			int cid;
			memcpy(&cid,m.getContentPtr(),sizeof(int));
			printf("init Codec : %d\n",cid);
			SDL_LockMutex(vactx->mutex);
			codec_init(cid);
			SDL_UnlockMutex(vactx->mutex);
			continue;
		}
                //continue;
		if(header!=4){
			continue;
		}
   	 	AVPacket avpkt;
   	 	av_init_packet(&avpkt);
    		avpkt.data = (uint8_t*)(const_cast<char*>(m.getContentPtr()+4));
    		avpkt.size = m.getLength()-sizeof(int);
		SDL_LockMutex(vactx->mutex);
                //printf("Here 1\n");
    		len = avcodec_decode_video2(vactx->avctx,vactx->frame,&got,&avpkt);   
                //printf("Here 2\n");
    		if(len<0){
			fprintf(stderr,"Error while decoding frame %d\n",i);
			SDL_UnlockMutex(vactx->mutex);
			continue;
	    	}
	    	if(got){
			vactx->playing = 1;
			vactx->needDraw = 1;
		}else{
			printf("didn't get\n");
		}
		SDL_UnlockMutex(vactx->mutex);
		av_free_packet(&avpkt);
                //printf("Here 3\n");
	}
    }catch(std::exception& e) {
        	std::cerr << e.what() << std::endl;
        vactx->playing = 0;
    }
}

VAContext* va_getContext(void) 
{
    return vactx;
}

void va_init(void)
{
    	cout << "Init called." << endl;
	vactx = new VAContext;
        memset(vactx, 0, sizeof(VAContext));
    	vactx->mutex = SDL_CreateMutex();
	vactx->playing = 0;
	vactx->needDraw = 0;
        vactx->frame = avcodec_alloc_frame();
	chh_tid = SDL_CreateThread(changeHandler_thread,NULL);
	decode_tid = SDL_CreateThread(decode_thread,NULL);
}

