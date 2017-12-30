#include "FileManager.h"

int main()
{
	//unsigned char * Buffer = new unsigned char[PAGE_SIZE];
	FileManager dingry;
	char cmd[1000];
	//dingry.TestRead(INDEX_PAGE_NUM + SUPER_NUM, Buffer);
	//printf("%s\n", Buffer);
	//delete[] Buffer;
	while (true) {
		dingry.Show();
		putchar(' ');
		putchar('>');
		putchar('>');
		scanf("%s", cmd);
		if (strcmp(cmd, "pwd") == 0) {   // pwd 只需当前输出
			dingry.Show();
			putchar('\n');
		}
		else {
			char * temp = cmd;
			while (*temp != ' ' && *temp != '\0') {
				temp++;
			}

			if (*temp == '\0') {
				printf("Command Wrong\n");
			}
			else {
				printf(cmd);
				*temp = '\0';
				if (strcmp(cmd, "mkdir") == 0) {     // mkdir
					printf("mkdir cmd\n");
				}
				else if (strcmp(cmd, "cd") == 0) {   //  cd
					printf("cd cmd\n");
				}
				else if (strcmp(cmd, "ls") == 0) {   // ls
					printf("ls cmd\n");
				}
				else if (strcmp(cmd, "rmdir") == 0) {   // rmdir
					printf("rmdir cmd\n");
				}
				else if (strcmp(cmd, "echo") == 0) {    // echo
					printf("echo cmd\n");
				}
				else if (strcmp(cmd, "cat") == 0) {    // cat
					printf("cat cmd\n");
				}
				else if (strcmp(cmd, "rm") == 0) {     // rm
					printf("rm cmd\n");
				}
				else {
					printf("Command Wrong\n");
				}
			}
		}
	}
	return 0;
}