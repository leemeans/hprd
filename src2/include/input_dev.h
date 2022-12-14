#ifndef __INPUT_DEV_H__
#define __INPUT_DEV_H__

#include "frame_buffer.h"

struct input_objct
{
	void *priv;
};

struct input_dev_ops
{
	char * name;
	int (* init)(struct input_objct *obj);
	int (* get_info)(struct input_objct *obj, struct fb_info *info);
	int (* map_buffer)(struct input_objct *obj, struct raw_buffer *buf);
	int (* put_buffer)(struct input_objct *obj, struct raw_buffer *buf);
	struct raw_buffer *(* get_buffer)(struct input_objct *obj);
	int (* unmap_buffer)(struct input_objct *obj, struct raw_buffer *buf);
	int (* event_loop)(struct input_objct *obj);
	int (* release)(struct input_objct *obj);
};

#endif