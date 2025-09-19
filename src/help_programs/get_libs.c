#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include <string.h>

#include <sys/wait.h>

#include "../../../shm_lib/include/shm_api.h"


int main(const int argc, const char *const *const argv)
{
	printf("get_libs started, arg0: %s\n", argv[0]);
	void *const pShmLib = dlopen("./lib_SharedMemoryLibrary.so", RTLD_LAZY);
	const shmOpen_fp shm_simple_open = dlsym(pShmLib, "shm_simple_open");
	const shmWrite_fp shm_write = dlsym(pShmLib, "shm_write");
	const shmTerminate_fp shm_close = dlsym(pShmLib, "shm_close");
	const shmTerminate_fp shm_destroy = dlsym(pShmLib, "shm_destroy");

	SharedMemoryInfo shmInfo = {};
	shmInfo.shmByteSize = 4;

	errno = 0;
	if (setenv("LD_TRACE_LOADED_OBJECTS", "true", 1) == -1)
	{
		fprintf(stderr, "\033[31msetenv failed, errno: %s\n", strerror(errno));
		return -1;
	}

	const pid_t pid = fork();

	if (pid == -1)
	{
		fprintf(stderr, "\033[31mfork failed, errno: %s\n", strerror(errno));
		return -1;
	}
	else if (pid == 0)
	{
		if (freopen("/tmp/out_file_last.txt", "w", stdout) == NULL)
		{
			fprintf(stderr, "\033[31mfreopen failed, errno: %s\n", strerror(errno));
			exit(1);
		}
		if (execl(argv[0], argv[0], nullptr) == -1)
		{
			fclose(stdout);
			fprintf(stderr, "\033[31mexecl failed, errno: %s\n", strerror(errno));
			exit(1);
		}
		exit(1);// на всякий
	}
	else
	{
		int rv = 0;
		waitpid(-1, &rv, 0);

		if (WEXITSTATUS(rv) != 0)
		{
			fprintf(stderr, "\033[31mWEXITSTATUS: %d\n", WEXITSTATUS(rv));
			exit(1);
		}

		FILE *const file_pointer = fopen("/tmp/out_file_last.txt", "r");
		if (file_pointer == nullptr)
		{
			fprintf(stderr, "\033[31mfopen failed, errno: %s\n", strerror(errno));
			exit(1);
		}

		char ch;
		int itr = 0;
		int missing_libs_count = 0;
		while((ch = fgetc(file_pointer)) != EOF)
		{
			if (ch == '>')
			{
				ch = fgetc(file_pointer);
				itr--;
				if (ch == ' ')
				{
					ch = fgetc(file_pointer);
					itr--;
					if (ch == 'n')
					{
						missing_libs_count++;
					}
				}
			}
			itr--;
		}

		fseek(file_pointer, itr, SEEK_CUR);
		char* *const missing_libs = malloc(sizeof(missing_libs) * missing_libs_count);

		int last_newline = 0;
		uint8_t lib_itr = 0;
		itr = 0;
		int lib_name_len = 0;

		shm_simple_open(&shmInfo, "need_lib");

		while((ch = fgetc(file_pointer)) != EOF)
		{
			if (ch == '\n')
			{
				last_newline = itr+1;
			}
			if (ch == '>')
			{
				ch = fgetc(file_pointer);
				itr++;
				if (ch == ' ')
				{
					ch = fgetc(file_pointer);
					itr++;
					if (ch == 'n')
					{
						fseek(file_pointer, (last_newline - itr), SEEK_CUR);
						lib_name_len = (itr - last_newline - 5);

						missing_libs[lib_itr] = malloc(lib_name_len+1);

						for (int i = 0; i < lib_name_len; i++)
						{
							ch = fgetc(file_pointer);
							missing_libs[lib_itr][i] = ch;
						}
						missing_libs[lib_itr][lib_name_len] = '\0';

						shm_write(&shmInfo, missing_libs[lib_itr], lib_name_len+1, 0);

						fseek(file_pointer, (last_newline - itr + lib_name_len+2) * -1, SEEK_CUR);
						lib_itr++;
					}
					else
					{
						fseek(file_pointer, -2, SEEK_CUR);
					}
				}
				else
				{
					fseek(file_pointer, -1, SEEK_CUR);
				}
			}
			itr++;
		}

		fclose(file_pointer);
		if (remove("/tmp/out_file_last.txt") == -1)
		{
			fprintf(stderr, "\033[31mremove failed, errno: %s\n", strerror(errno));
		}

		shm_close(&shmInfo);
		dlclose(pShmLib);

		for (int i = 0; i < missing_libs_count; i++)
		{
			printf("missing_libs = %s\n", missing_libs[i]);
			free(missing_libs[i]);
		}
		free(missing_libs);
	}

	return 0;
}
