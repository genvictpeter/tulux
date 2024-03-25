#ifndef _YX_DRV_GPIO_H_
#define _YX_DRV_GPIO_H_

typedef enum {
	GPIO_DIR_OUT	= 0,
	GPIO_DIR_IN,
}GPIO_DIR_TYPE_E;

typedef enum {
	GPIO_EDGE_NONE	= 0,
	GPIO_EDGE_RISING,
	GPIO_EDGE_FALLING,
	GPIO_EDGE_BOTH,
}GPIO_EDGE_TYPE_E;


int yx_drv_gpio_init(int n,GPIO_DIR_TYPE_E type);
int yx_drv_gpio_edge_set(int n,GPIO_EDGE_TYPE_E type);
int yx_drv_gpio_get_value_byfd(int fd); 
int yx_drv_gpio_set_value_byfd(int fd,INT8U value);
int yx_drv_gpio_get_value(int n);
int yx_drv_gpio_set_value(int n,INT8U value);
#endif
