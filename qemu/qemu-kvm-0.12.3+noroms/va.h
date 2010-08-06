/*
 * Include file for Video Acceleration support.
 *
 * Author: pat
 *
 */

#ifndef _va_h
#define _va_h

#ifdef __cplusplus
extern "C" {
#endif

struct SDL_mutex;
struct SDL_Surface;
struct AVCodecContext;
struct AVFrame;

typedef struct {
    struct SDL_mutex* mutex;
    struct AVCodecContext* avctx;
    struct AVFrame* frame;
    struct {
        int l, t, w, h;
    } clientRect;
    int needDraw;
    int playing;
} VAContext;

VAContext* va_getContext(void);
void va_init(void);
void va_do_drawing(struct SDL_Surface* real_screen, int width, int height);

#ifdef __cplusplus
}
#endif

#endif /* _va_h */

