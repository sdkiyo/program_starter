#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>


typedef enum VariableType{
	varType_INT,
	varType_STR
} VariableType;

int foo(const char *const file_name, const char *const variable_name, const uint8_t variable_len, const VariableType type)
{
	printf("foo() start\n");

	FILE* fp = fopen(file_name, "r");
	if (fp == NULL)
	{
		fprintf(stderr, "-ERROR cover by 石狩あかり【歌ってみた】\n");
	}

	char ch;
	bool isCorrect = false;
	bool isVariableEnd = false;
	bool areFind = false;
	bool isComment = false;
	char buf[256];
	uint16_t x = 0;
	uint16_t i = 0;
	printf("\033[35m");
	while((ch = fgetc(fp)) != EOF)
	{
		printf("%c", ch);

		if (ch == '\n' && areFind == true)
		{
			break;
		}
		if (ch == '\n')
		{
			i = 0;
			isCorrect = false;
			isVariableEnd = false;
			isComment = false;
			continue;
		}
		if (ch == ' ' || ch == '\t' || isComment == true)
		{
			continue;
		}
		if (ch == '#')
		{
			isComment = true;
			continue;
		}

		if (ch == '='|| i > 0 && isCorrect == false)
		{
			isVariableEnd = true;
			continue;
		}
		if (isVariableEnd == false)
		{
			if (ch == variable_name[i])
			{
				isCorrect = true;
			}
			else
			{
				isCorrect = false;
			}
			if (ch == variable_name[variable_len-1] && isCorrect == true)
			{
				areFind = true;
			}
			else
			{
				areFind = false;
			}
		}
		if (areFind == true && isVariableEnd == true)
		{
			buf[x] = ch;
			x++;
		}

		i++;
	}
	buf[x] = '\0';
	printf("\033[0m\n");
	printf("buf = %s\n", buf);
	if (type == varType_INT)
	{
		int f = strtol(buf, nullptr, 10);
		printf("type is INT_t, f: %d\n", f);
	}
	if (type == varType_STR)
	{
		printf("type is STR_t, buf: %s\n", buf);
	}

	fclose(fp);
	return 0;
}


int main(const int argc, const char *const *const argv)
{
	foo("start_config.txt", "var", 3, varType_INT);
	foo("start_config.txt", "lib_search_path", 15, varType_STR);

	return 0;
}
