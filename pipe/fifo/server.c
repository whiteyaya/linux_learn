/*fifo-server.c */

#include "comm.h"

int main()
{
	umask(0);//权限
	if(mkfifo(MY_FIFO,0666)<0)
	{
		perror("mkfifo");
		return 1;
	}
	//打开管道
	int fd = open(MY_FIFO, O_RDONLY);
	if(fd < 0)
	{
		perror("open FIFO");
		return 2;
	}
	//读操作
	char buf[64] = {0};
	while(1)
	{
		buf[0]=0;
		printf("waiting client.....\n");
		ssize_t s = read(fd,buf,sizeof(buf)-1);
		if(s>0)//读成功
		{
			buf[s] = 0;
			printf("client:%s\n",buf);

			//判断输入进行程序替换
			if(strcmp(buf,"ls")==0)
			{
				
				if(fork()==0)
				{
					//子进程
					execl("/usr/bin/ls","ls",NULL);
					exit(1);
				}
				waitpid(-1,NULL,0);
			}
			else if(strcmp(buf,"train")==0)
			{
				if(fork()==0)
				{
					//子进程
					execl("/usr/bin/sl","sl",NULL);
					exit(1);
				}
				waitpid(-1,NULL,0);
			}
		}
		else if(s == 0)//对方写关闭
		{
			printf("client quit\n");
			return 0;
		}
		else//error
		{
			perror("read");
			return 2;
		}
	}
	close(fd);
	return 0;
}
