#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <stdarg.h>
#include <fcntl.h>
#include <termios.h>
#include <time.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h> 
#include "def.h"
#include "yx_drv_gpio.h"
#include <errno.h>


//检查Linux GPIO节点是否存在，成功返回0
static int yx_drv_gpio_check(int n)  
{  
	char path[64];  
	int fd;
	snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d", n); 
	fd = open(path, O_RDONLY);  
	if (fd <= 0) {
		return -1;
	}  
	close(fd);  
	return 0;  
}

//创建Linux GPIO节点
static int yx_drv_gpio_export(int n)  
{  
	int handle;
	char exportStr[6]={0};  
	int ret;

	handle = open("/sys/class/gpio/export",O_WRONLY);
	if(handle == -1)
	{
		printf("%s:%d  gpio=%d error=%d\n",__FUNCTION__,__LINE__,n,errno);	  
		return -1;
	} 
	snprintf(exportStr, sizeof(exportStr), "%d", n); 
	//printf("%s %d\n",exportStr,strlen(exportStr));
	ret = write(handle,exportStr,strlen(exportStr));//ret
	close(handle);
	return ret;  
}

//检查并配置GPIO方向
static int yx_drv_gpio_direction_init(int n,GPIO_DIR_TYPE_E value)  
{  
	char path[64];  
	char value_str[4]={0}; 
	int handle;  
	int ret=0;
	memset(value_str,0,4);
	snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/direction", n); 
	//printf("%s\n",path); 
	handle = open(path, O_RDWR);  
	if (handle <= 0) {  
		printf("%s:%d  gpio=%d error=%d\n",__FUNCTION__,__LINE__,n,errno);	  
		return -1;  
	}  

	lseek(handle,0,0);

    if (value == GPIO_DIR_OUT) {
		ret = write(handle,"out",3);
	} else  {
		ret = write(handle,"in",2);
	}		
	close(handle);
	return 0;  
}

int yx_drv_gpio_edge_set(int n,GPIO_EDGE_TYPE_E type)  
{  
	char path[64];  
	char value_str[4]={0}; 
	int handle;  
	int ret=0;
	memset(value_str,0,4);
	snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/edge", n); 
	//printf("%s\n",path); 
	handle = open(path, O_RDWR);  
	if (handle <= 0) {  
		printf("%s:%d  gpio=%d error=%d\n",__FUNCTION__,__LINE__,n,errno);	  
		return -1;  
	}  

	lseek(handle,0,0);

    if(type == GPIO_EDGE_NONE) {
		ret = write(handle,"none",4);
    } else if (type == GPIO_EDGE_RISING) {
		ret = write(handle,"rising",6);
    }  else if (type == GPIO_EDGE_FALLING) {
		ret = write(handle,"falling",7);
    }  else if (type == GPIO_EDGE_BOTH) {
		ret = write(handle,"both",4);
	}  
	close(handle);
	return 0;  
}


// 
//GPIO初始化
//int value 0 ==out  1==in
int yx_drv_gpio_init(int n,GPIO_DIR_TYPE_E type)  
{
	int ret;
	char path[64];	
	int fd;  
	//1.检查是否存在
    if (n == -1) {
          return -1;
    }
	ret = yx_drv_gpio_check(n);
	if (ret == -1) {
		ret = yx_drv_gpio_export(n);
		if(ret == -1)
			return ret;
	}
	//3.检查并配置GPIO
	if (yx_drv_gpio_direction_init(n,type) != -1) {
		snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", n); 
		fd = open(path, O_RDWR);  
		return fd;
	}
	return -1;
}


//获取GPIO值
int yx_drv_gpio_get_value_byfd(int fd) 
{  
	char path[64];  
	char value_str[3]={0};  
	int ret;
	if (fd <= 0) {  
		return -1;  
	}  
	ret = lseek(fd, 0, SEEK_SET);

	if (read(fd, value_str, 1) < 0) {  
		printf("%s:%d  error=%d\n",__FUNCTION__,__LINE__,errno);	  
		return -1;  
	}  
	if (value_str[0] == '0') {
		return 0;
	} else if(value_str[0] == '1') {
		return 1;
	} else {
		return -1;
	}
 }


 int yx_drv_gpio_set_value_byfd(int fd,INT8U value) 
 {	
	int ret;
	char value_str[3]={0};  
	if (fd <= 0) {	
		return -1;  
	}	
	lseek(fd, 0, SEEK_SET);
	
	if (value == 0) {
		ret = write(fd,"0",1);
	} else {
		ret = write(fd,"1",1);
	}
	return 0;
}
 //获取GPIO值
 int yx_drv_gpio_get_value(int n) 
 {	
	 char path[64];  
	 char value_str[3]={0};  
	 int fd;  
	 snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", n); 
	 //printf("%s\n",path); 
	 fd = open(path, O_RDWR);  
	 if (fd <= 0) 
	 {	
		 printf("%s:%d	gpio=%d error=%d\n",__FUNCTION__,__LINE__,n,errno);    
		 return -1;  
	 }	
 
	 if (read(fd, value_str, 1) < 0) 
	 {	
		 printf("%s:%d	error=%d\n",__FUNCTION__,__LINE__,errno);	   
		 return -1;  
	 }	
	 close(fd);  
	 if(value_str[0] == '0')
		 return 0;
	 else if(value_str[0] == '1')
		 return 1;
	 else
		 return -1;
  }
 
 
  int yx_drv_gpio_set_value(int n,INT8U value) 
  {  
	 int ret;
	 char path[64];  
	 char value_str[3]={0};  
	 int fd;  
	 snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", n); 
	 //printf("%s\n",path); 
	 fd = open(path, O_RDWR);  
	 if (fd <= 0) 
	 {	 
		 printf("%s:%d	 gpio=%d error=%d\n",__FUNCTION__,__LINE__,n,errno);	
		 return -1;  
	 }	 
	 if (value == 0) {
		 ret = write(fd,"0",1);
	 } else {
		 ret = write(fd,"1",1);
	 }
	 close(fd);
	 return 0;
 }

