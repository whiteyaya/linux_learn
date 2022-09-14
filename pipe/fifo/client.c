/*fifo-client.c */

#include "comm.h"

int main()
{
	//用户端不用再创建fifo
	//直接打开
	int fd = open(MY_FIFO,O_WRONLY);//不需要O_CREAT
	if(fd<0)
	{
		perror("open FIFO");
		return 1;
	}
	//写操作
	char buf[64] = {0};
	while(1)
	{
		buf[0]=0;
		//从标准输入读数据
		ssize_t s = read(0,buf,sizeof(buf)-1);
		if(s>0)
		{
			buf[s]=0;
			//写入管道
			write(fd,buf,strlen(buf));
			printf("发送成功：%s",buf);
		}
		else
		{
			perror("read 0");
			return 2;
		}

	}

	close(fd);
	return 0;
}

