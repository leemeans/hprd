#ifndef __FRAME_BUFFER_H__
#define __FRAME_BUFFER_H__

enum FRAMEBUFFER_FORMAT
{
	ARGB8888,
	YUV420P,
	NV12
};

struct fb_info
{
	uint32_t width;
	uint32_t hor_stride;
	uint32_t height;
	uint32_t ver_stride;
	uint32_t format;
	uint16_t bpp;
	uint32_t size;
};

struct raw_buffer
{
	uint32_t width;
	uint32_t hor_stride;
	uint32_t height;
	uint32_t ver_stride;
	uint32_t format;
	uint16_t bpp;
	uint32_t size;
	char * ptr;
};

#endif