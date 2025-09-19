#define _GNU_SOURCE

#include "../include/includes.h"
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>


typedef struct ProgramParams {
	char* name;
	pid_t pid;
} ProgramParams;

//void foo2(ProgramParams* pr, const int arr_len)
//{
//	for (int i = 0; i < arr_len; i++)
//	{
//		pr[i].pid = fork();
//		if (pr[i].pid == -1)
//		{
//			printf("it's -ERROR\n");
//			exit(1);
//		}
//		else if (pr[i].pid == 0)
//		{
//			execl(pr[i].name, pr[i].name, nullptr);
//			exit(0);
//		}
//	}
//}

void foo1(const char *const name, char** argv, char** envp, pid_t* pid)
{
	printf("foo1(), name: %s, argv0: %s\n", name, argv[0]);
	*pid = fork();
	if (*pid == -1)
	{
		printf("it's -ERROR\n");
		exit(1);
	}
	else if (*pid == 0)
	{
		errno = 0;
		int x = execvpe(name, argv, nullptr);
		printf("x = %d, %s\n", x, strerror(errno));
		exit(0);
	}
}


char** foo(const uint8_t arg_count, ...)
{
	char** result = malloc(sizeof(*result) * arg_count);

	va_list arg;
	va_start(arg, arg_count);
	char* buh3;

	for (uint8_t i = 0; i < arg_count; i++)
	{
		buh3 = va_arg(arg, char*);
		result[i] = malloc(sizeof(**result) * strlen(buh3));
		memcpy(result[i], buh3, sizeof(**result) * strlen(buh3));
	}

	va_end(arg);

	return result;
}


int main(const int argc, const char *const *const argv)
{
	printf("\033[0m\033[2m▇▇▇\033[100m \e[48;5;235m Hi Friend!! \033[100m \033[0m\033[2m▇▇▇\033[0m\n");

	uint8_t arg_count = 1;
	uint8_t arg2_count = 1;
	uint8_t envp_count = 2;
	char** result = foo(arg_count, "/home/eugene/FILES/Programming/vk_c/tests/test1/build/test2");
	char** result2 = foo(arg2_count, "/home");
	char** envp = foo(envp_count, "-", ".");


	pid_t pid = 0;
	int get_libs_rv = 0;
	foo1("./get_libs", result, nullptr, &pid);
	if(waitpid(pid, &get_libs_rv, 0) == -1)
	{
		printf("starter: get_libs wait error, rv: %d\n", get_libs_rv);
	}

	pid_t pid2 = 0;
	foo1("./find_lib", result2, nullptr, &pid);

	int rv = 0;
	for (int i = 0; i < 4; i++)
	{
		if(waitpid(-1, &rv, 0) == -1)
		{
			printf("wait error\n");
		}
	}

	for (uint8_t i = 0; i < arg_count; i++)
	{
		free(result[i]);
	}
	free(result);

	printf("it's over..\n");

	return 0;
}
