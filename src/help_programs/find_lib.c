#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>

#include "../../../shm_lib/include/shm_api.h"


int8_t findFile(const char *const directory, const char *const name, char *const buffer, const int buffer_len)
{
	//fprintf(stderr, "Start with:\033[0m dir: \033[33m%s \033[0mname: \033[35m%s\033[0m\n", directory, name);
	DIR *pOpenDirectory;
	struct dirent *pDirent;

	errno = 0;
	pOpenDirectory = opendir(directory);

	uint32_t item_count = 0;
	char** itemList = nullptr;

	if (pOpenDirectory == nullptr)
	{
	//	fprintf(stderr, "\033[34m%s \033[0m%s\n", directory, strerror(errno));
	}
	else
	{
		while (true)
		{
			pDirent = readdir(pOpenDirectory);
			if (pDirent == nullptr )
			{
				break;
			}
			else
			{
				if (strcmp(pDirent->d_name, name) == 0)
				{
					//char buffer[1024];
					getcwd(buffer, buffer_len);
				//	printf("buffer = %ld\n", sizeof(buffer));
					strncat(buffer, "/", 1);
					strncat(buffer, directory, strlen(directory));
					strncat(buffer, "/", 1);
					strncat(buffer, name, strlen(name));
					printf("\033[34m'%s'\033[32m are find: \033[36m%s\033[0m\n", name, buffer);
					return 2;
				}
				if (strcmp(pDirent->d_name, ".") == 0 || strcmp(pDirent->d_name, "..") == 0)
				{
					continue;
				}
				item_count++;
			}
		}
		seekdir(pOpenDirectory, 0);
		itemList = malloc(sizeof(itemList) * item_count);

		int i = 0;
		while (true)
		{

			pDirent = readdir(pOpenDirectory);
			if (pDirent == nullptr )
			{
				break;
			}
			else
			{
				if (strcmp(pDirent->d_name, ".") == 0 || strcmp(pDirent->d_name, "..") == 0)
				{
					continue;
				}
				itemList[i] = malloc(sizeof(**itemList) * (strlen(pDirent->d_name)+1));
			//	fprintf(stderr, "\033[35md_name\033[0m %ld: %s\n", sizeof(**itemList) * (strlen(pDirent->d_name)), pDirent->d_name);
				memcpy(itemList[i], pDirent->d_name, strlen(pDirent->d_name)+1);
			//	fprintf(stderr, "\033[35mitemList\033[0m %ld: \033[2m\033[35m%s\033[0m\n", strlen(itemList[i]), itemList[i]);
			}
			i++;
		}
		closedir(pOpenDirectory);
	}

	for (uint32_t i = 0; i < item_count; i++)
	{
	//	fprintf(stderr, "\033[33m%s \033[34m%d \033[35m%s\033[0m\n", directory, i, itemList[i]);
		chdir(directory);
		if (findFile(itemList[i], name, buffer, 1024) == 2)
		{
			return 2;
		}
		if (strcmp(directory, ".") != 0)
		{
			chdir("..");
		}

	}

	for (uint32_t i = 0; i < item_count; i++)
	{
		free(itemList[i]);
	}
	free(itemList);
	return 0;
}


int main(const int argc, const char *const *const argv)
{
	printf("find_lib started, arg0: %s\n", argv[0]);
	void *const pShmLib = dlopen("./lib_SharedMemoryLibrary.so", RTLD_LAZY);
	const shmOpen_fp shm_simple_open = dlsym(pShmLib, "shm_simple_open");
	const shmWrite_fp shm_write = dlsym(pShmLib, "shm_write");
	const shmGetData_fp shm_getData = dlsym(pShmLib, "shm_getData");
	const shmTerminate_fp shm_close = dlsym(pShmLib, "shm_close");
	const shmTerminate_fp shm_destroy = dlsym(pShmLib, "shm_destroy");

	SharedMemoryInfo shmInfo = {};
	SharedMemoryInfo shmInfoResult = {};
	shmInfo.shmByteSize = 4;
	shmInfoResult.shmByteSize = 4;

	shm_simple_open(&shmInfo, "need_lib");
	shm_simple_open(&shmInfoResult, "path_lib");

	char buffer[1024];
	char buf[256];
	char ch;
	int8_t return_value = 0;
	size_t x = 0;
	for (size_t i = 0; i < shmInfo.shmByteSize; i++)
	{
		shm_getData(&shmInfo, &ch, sizeof(ch), i);
		if (ch == '\0')
		{
			buf[x] = ch;
			return_value = findFile(argv[0], buf, buffer, 1024);
			shm_write(&shmInfoResult, buffer, strlen(buffer), 0);
			shm_write(&shmInfoResult, "\0", 1, 0);
			if (return_value == 0)
			{
				printf("\033[31mFile are not found\033[0m\n");
			}
			for (uint16_t j = 0; j < i; j++)
			{
				buf[j] = ' ';
			}
			x = 0;
		}
		else
		{
			buf[x] = ch;
			x++;
		}
	}


	shm_destroy(&shmInfo);
	shm_close(&shmInfoResult);
	dlclose(pShmLib);

	return 0;
}
