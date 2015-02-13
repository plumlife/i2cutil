#include <stdio.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

char	busname[32] = "/dev/i2c-1";
int 	devaddr = 0x50;

int read_buffer(int fd, char *buf, int len)
{
	if (ioctl(fd, I2C_SLAVE, devaddr) < 0)
	{
	    printf("Failed to acquire bus access and/or talk to slave.\n");
	    exit(1);
	}

	return read(fd, buf, len);
}

int set_addr(int fd, int addr)
{
	char buf[] = {addr / 256, addr % 256};

	if (ioctl(fd, I2C_SLAVE, devaddr) < 0)
	{
	    printf("Failed to acquire bus access and/or talk to slave.\n");
	    exit(1);
	}
	write(fd, buf, 2);
}

int write_buffer(int fd, int addr, char *buffer, int len)
{
	char buf[] = {addr / 256, addr % 256};

	if (ioctl(fd, I2C_SLAVE, devaddr) < 0)
	{
	    printf("Failed to acquire bus access and/or talk to slave.\n");
	    exit(1);
	}
	write(fd, buffer, len);
}

void dump(char *buf, int len)
{
	for( int i = 0; i < len; i += 16 )
	{
		for( int j = 0; j < 16; ++j )
		{
			if( i + j < len )
				printf("%02x ", buf[i + j] & 0x0ff);
			else
				printf("   ");			
		}
		printf("| ");
		for( int j = 0; j < 16; ++j )
		{
			if( i + j < len )
			{
				int c = buf[i + j] & 0x0ff;
				if( c >= ' ' && c <= '~' )
					printf("%c", c);
				else
					printf(".");
			}
		}
		printf("\n");
	}
}

int try_write(int fd)
{
	if (ioctl(fd, I2C_SLAVE, devaddr) < 0)
	{
	    printf("Failed to acquire bus access and/or talk to slave.\n");
	    exit(1);
	}

	char buf[16] = {0, 0x80, 0x41, 0x42, 0x43, 0x44};

	int stat = write(fd, buf, 2);
	printf("write status = %d\n", stat);
	stat = write(fd, buf + 2, 4);
	printf("write status = %d\n", stat);
}

int main(int argc, char **argv)
{
	if( argc > 1 )
	{
		int bus = strtol(argv[1], NULL, 0);

		snprintf(busname, sizeof(busname), "/dev/i2c-%d", bus);
	}

	int		fd = open(busname, O_RDWR);

	if( fd < 0 )
	{
		printf("error opening %s.\n", busname);
		exit(1);
	}

	if( argc > 2 )
		devaddr = strtol(argv[2], NULL, 0);

	try_write(fd);

	char	buf[256];

	set_addr(fd, 0);

	int		stat = read_buffer(fd, buf, sizeof(buf));

	if( stat < 0 )
	{
		printf("read failed\n");
		exit(1);
	}

	dump(buf, stat);
}
