/*
 * Video acceleration updater, used after each frame is drawn.
 */

#include <SDL/SDL.h>
#include <libavcodec/avcodec.h>
#include <stdio.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include "va.h"

static struct {
    SDL_Overlay* overlay;
    SDL_Surface* last_screen;
    int frame;
} state;

static void calcScale(VAContext* context, SDL_Surface* screen, int reswidth, int resheight, int* vl, int* vt, int* vw, int* vh) {
    int videow, videoh, windoww, windowh, nwindoww, nwindowh, nwindowl, nwindowt;
    videow = context->avctx->width;
    videoh = context->avctx->height;
    windoww = context->clientRect.w;
    windowh = context->clientRect.h;
    if (windoww < 0) windoww = 0;
    if (windowh < 0) windowh = 0;
    if (windoww > 2000) windoww = 2000;
    if (windowh > 2000) windowh = 2000;
    if (windoww * videoh > windowh * videow) {
        nwindoww = videow * windowh / videoh;
        nwindoww = nwindoww
            //- nwindoww % 4
            ;
        nwindowh = windowh;
        nwindowl = context->clientRect.l + (windoww - nwindoww) / 2;
        nwindowt = context->clientRect.t + (windowh - nwindowh) / 2;
    } else {
        nwindowh = videoh * windoww / videow;
        nwindoww = windoww
            //- windoww % 4
            ;
        nwindowt = context->clientRect.t + (windowh - nwindowh) / 2;
        nwindowl = context->clientRect.l + (windoww - nwindoww) / 2;
    }
    *vl = nwindowl * screen->w / reswidth;
    *vt = nwindowt * screen->h / resheight;
    *vw = nwindoww * screen->w / reswidth;
    *vh = nwindowh * screen->h / resheight;
}

static int convert(VAContext* context, int width, int height) {
    struct SwsContext *convertCtx;
    AVPicture pict;
    AVCodecContext* avctx;
    AVFrame* picture;
    SDL_Overlay* bmp;
    int success;
    
    bmp = state.overlay;
    SDL_LockYUVOverlay(bmp);

    pict.data[0] = bmp->pixels[0];
    pict.data[1] = bmp->pixels[2];
    pict.data[2] = bmp->pixels[1];

    pict.linesize[0] = bmp->pitches[0];
    pict.linesize[1] = bmp->pitches[2];
    pict.linesize[2] = bmp->pitches[1];

    avctx = context->avctx;
    picture = context->frame;

    convertCtx = sws_getContext(avctx->width, avctx->height, avctx->pix_fmt, width, height,
        PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);

    success = sws_scale(convertCtx, picture->data, picture->linesize, 0, avctx->height, pict.data, pict.linesize);
    //printf("Here\n");
    sws_freeContext(convertCtx);
    SDL_UnlockYUVOverlay(bmp);
    
    return success;
}

static void va_do_drawing_locked(VAContext* context, SDL_Surface* real_screen, int width, int height) {
    SDL_LockMutex(context->mutex);
    if (context->playing) {
        /* Check if there's anything to draw */
        if (!context->needDraw) {
            //printf("%s\n", "Nothing to draw..");
            SDL_UnlockMutex(context->mutex);
            return;
        }
        //printf("Here1\n");
        /* Create and adjust overlay */
        int vl, vt, vw, vh, success;
        calcScale(context, real_screen, width, height, &vl, &vt, &vw, &vh);
        //printf("Here2\n");
        if (state.overlay == 0 || state.overlay->w != vw || state.overlay->h != vh || state.last_screen != real_screen) {
            if (state.overlay) {
                SDL_FreeYUVOverlay(state.overlay);
                state.overlay = 0;
                printf("%s\n", "Destroyed overlay.");
            }
            if (vw < context->avctx->width / 20 || vh < context->avctx->height / 20) {
                /* won't create overlay */
                SDL_UnlockMutex(context->mutex);
                return;
            }
            state.overlay = SDL_CreateYUVOverlay(vw, vh, SDL_YV12_OVERLAY, real_screen);
            //printf("Here3\n");
            if (state.overlay == 0) {
                printf("%s\n", "va_do_drawing_locked: Unable to create YUV overlay!");
                SDL_UnlockMutex(context->mutex);
                return;
            }
            state.last_screen = real_screen;
            printf("%s\n", "Created new overlay.");
        }
        /* Resize the frame into overlay */
        success = convert(context, vw, vh);
        //printf("Here4\n");
        /* Mark the frame as drawn */
        context->needDraw = 0;
        SDL_UnlockMutex(context->mutex);
        /* Draw the frame */
        if (success > 0) {
            SDL_Rect rect;
            rect.x = vl;
            rect.y = vt;
            rect.w = vw;
            rect.h = vh;
            SDL_DisplayYUVOverlay(state.overlay, &rect);
            //printf("Here5\n");
            //printf("Frame %d drawn.\n", ++state.frame);
        } else {
            printf("%s\n", "va_do_drawing_locked: image scaling failure!");
        }
        return;
    }
    SDL_UnlockMutex(context->mutex);
    if (state.overlay) {
        SDL_FreeYUVOverlay(state.overlay);
        state.overlay = 0;
        printf("%s\n", "Destroyed overlay.");
    }
}

void va_do_drawing(SDL_Surface* real_screen, int width, int height) {
    static int time = 0;
    ++time;
    //printf("va_do_drawing, time=%d", time);
    
    VAContext* context = va_getContext();
    if (context == 0) return;

    //printf("Playing: %d, NeedDraw: %d\n", context->playing, context->needDraw);
    va_do_drawing_locked(context, real_screen, width, height);
}

