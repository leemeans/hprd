#include <stdlib.h>
#include <unistd.h>

#include "util.h"
#include "dl_help.h"
#include "input_dev.h"
#include "frame_buffer.h"
#include "dev_templete.h"

struct input_object *input_dev_init(void)
{
	int ret;
	struct input_dev_ops *dev_ops;
	struct input_object *input_obj;

	input_obj = calloc(1, sizeof(struct input_object));

	dev_ops = (struct input_dev_ops *)load_lib_data("src2/input_dev/xcb_input/libxcb_input.so", "dev_ops");

	if(!dev_ops)
	{
		char path_tmp[255];
		getcwd(path_tmp, 255);
		log_error("load xcb_input.so fail. dir:%s\n", path_tmp);
		exit(-1);
	}

	ret = dev_ops->init(input_obj);
	if(ret == 0)
	{
		input_obj->ops = dev_ops;
	}else{
		log_error("xcb_input.so init fail.");
		exit(-1);
	}
	
	return input_obj;
}

DEV_GET_INFO(input, input_dev_ops)
DEV_MAP_FB(input, input_dev_ops)
DEV_UNMAP_FB(input, input_dev_ops)
DEV_GET_FB(input, input_dev_ops)
DEV_PUT_FB(input, input_dev_ops)
DEV_RELEASE(input, input_dev_ops)

int input_main_loop(struct input_object *input_obj)
{
	struct input_dev_ops *dev_ops;

	if(!input_obj)
		return -1;

	dev_ops = (struct input_dev_ops *)input_obj->ops;
	if(dev_ops)
	{
		if(dev_ops->event_loop)
			return dev_ops->event_loop(input_obj);
	}

	return -1;
}