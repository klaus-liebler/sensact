/*
 *
 * 		Atollic TrueSTUDIO Minimal System calls file
 *
 * 		For more information about which c-functions
 * 		need which of these lowlevel functions
 * 		please consult the Newlib libc-manual
 *
 * */
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include <stm32f4xx_hal.h>

#undef errno
extern int errno;

register char * stack_ptr asm("sp");

char *__env[1] = { 0 };
char **environ = __env;

void initialise_monitor_handles(void);
int _getpid(void);
int _kill(int pid, int sig);
void _exit (int status);
int _write(int file, const void *ptr, size_t len);
caddr_t _sbrk(int incr);
int _close(int file);
int _fstat(int file, struct stat *st);
int _isatty(int file);
int _lseek(int file, int ptr, int dir);
int _read(int file, char *ptr, int len);
int _open(char *path, int flags, ...);
int _wait(int *status);
int _unlink(char *name);
int _times(struct tms *buf);
int _stat(char *file, struct stat *st);
int _link(char *old, char *new);
int _fork(void);
int _execve(char *name, char **argv, char **env);

void initialise_monitor_handles(void)
{
}

int _getpid(void)
{
	return 1;
}

int _kill(int pid, int sig)
{
	(void)(pid);
	(void)(sig);
	errno = EINVAL;
	return -1;
}

void _exit (int status)
{
	_kill(status, -1);
	while (1) {}		/* Make sure we hang here */
}

int _write (int file, const void *buf, size_t count)
{
	(void)(file);
	size_t i;

   for (i = 0; i < count; i++)
   {
		while (!(USART1->SR & USART_SR_TXE))
			;
		USART1->DR = (unsigned char)*(uint8_t*)buf;
		(uint8_t*)buf++;
   }

	/* Implement your write code here, this is used by puts and printf for example */
	return count;
}


caddr_t _sbrk(int incr)
{
	extern char end asm("end");
	static char *heap_end;
	char *prev_heap_end;

	if (heap_end == 0)
		heap_end = &end;

	prev_heap_end = heap_end;
	if (heap_end + incr > stack_ptr)
	{
//		write(1, "Heap and stack collision\n", 25);
//		abort();
		errno = ENOMEM;
		return (caddr_t) -1;
	}

	heap_end += incr;

	return (caddr_t) prev_heap_end;
}

int _close(int file)
{
	(void)(file);
	return -1;
}


int _fstat(int file, struct stat *st)
{
	(void)(file);
	st->st_mode = S_IFCHR;
	return 0;
}

int _isatty(int file)
{
	(void)(file);
	return 1;
}

int _lseek(int file, int ptr, int dir)
{
	(void)(file);
	(void)(ptr);
	(void)(dir);
	return 0;
}

int _read(int file, char *ptr, int len)
{
	(void)(file);
	(void)(ptr);
	(void)(len);
	return 0;
}

int _open(char *path, int flags, ...)
{
	(void)(path);
	(void)(flags);
	/* Pretend like we always fail */
	return -1;
}

int _wait(int *status)
{
	(void)(status);
	errno = ECHILD;
	return -1;
}

int _unlink(char *name)
{
	(void)(name);
	errno = ENOENT;
	return -1;
}

int _times(struct tms *buf)
{
	(void)(buf);
	return -1;
}

int _stat(char *file, struct stat *st)
{
	(void)(file);
	st->st_mode = S_IFCHR;
	return 0;
}

int _link(char *old, char *new)
{
	(void)(old);
	(void)(new);
	errno = EMLINK;
	return -1;
}

int _fork(void)
{
	errno = EAGAIN;
	return -1;
}

int _execve(char *name, char **argv, char **env)
{
	(void)(name);
	(void)(argv);
	(void)(env);
	errno = ENOMEM;
	return -1;
}
