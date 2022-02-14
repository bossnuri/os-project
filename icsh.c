#include <ctype.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>

char pre_cmd[256];
int save_exit;
int number_node;
typedef struct node
{
	int id;
	int pid;
	char** value;
	int check;
	struct node *next;
}node;

struct node *head = NULL;
struct node *current = NULL;

char** copy_array_2_dimension(char** x) {
    char** a = malloc(256 * sizeof(char*));
    for (int i = 0; i < 256; i++) {
        a[i] = malloc(256 * sizeof(char));

    } 
    int i = 0;
    while (x[i] != NULL) {
        strcpy(a[i],x[i]);
        i++;
    }
    return a;
}
void delete_excess_space(char *x)
{
	int len;
	len = strlen(x) - 1;
	for (int i = len; i > 0; i--)
	{
		if (*(x + i) == '\n' || *(x + i) == ' ')
		{
			*(x + i) = '\0';
		}
		else
		{
			*(x + i + 1) = '\n';
			break;
		}
	}
}
void bg(int id){
	
}
void fg(int id){
	int get_pid = getpid();
	int count_node = 1;
	int i=0;
	int stat;
	struct node *current = head;
	while(current != NULL){
		
		if(current->id == id && current->check != 1){
			while(strcmp(current->value[i],"") != 0){
				printf("%s ",current->value[i]);
				i++;
			}
			printf("\n");
			tcsetpgrp(STDIN_FILENO, current->pid); //turn this pid to forground
			kill(current->pid,SIGCONT);
			waitpid(current->pid,&stat,WUNTRACED);
			if(WIFEXITED(stat)){
				save_exit = WEXITSTATUS(stat);
			}
			if (WIFSIGNALED(stat)) { 
				current->check = 2;
        save_exit = WTERMSIG(stat);           
      }
			if(WIFSTOPPED(stat)){
				int value_index = 0;
				if (count_node == number_node)
				{
					printf("[%d]+  Stopped                ", current->id);
				}
				else if (count_node == number_node - 1)
				{
					printf("[%d]-  Stopped               ", current->id);
				}
				else
				{
					printf("[%d]   Stopped                ", current->id);
				}
				while (strcmp(current->value[value_index],"") != 0) {
            printf("%s ", current->value[value_index]);
            value_index++;
        }
				printf("&\n");
				current->check = 3;
				tcsetpgrp(STDIN_FILENO,get_pid);
				save_exit = WSTOPSIG(stat);
				starting();
			}
			tcsetpgrp(STDIN_FILENO,get_pid);
			starting();		
		}
		current = current->next;
		count_node++;
	}
	printf("bad command");
	save_exit =1;
	starting();
}
void terminate_sleep(){
	struct node *current = head;
	struct node *previous = NULL;
	int count = 1;
	int total_delete = 0;
	while (current != NULL)
	{
		if (current->check != 2)
		{
			previous = current;
			current = current->next;
			count++;
		}
		else
		{
				if (current->pid == head->pid)
				{
					head = head->next;
					current = current->next;
					count++;
					total_delete++;
				}
				else
				{
					previous->next = current->next;
					current = current->next;
					count++;
					total_delete++;
			}
		}
	}
	number_node -= total_delete;
}


void done_sleep()
{
	struct node *current = head;
	struct node *previous = NULL;
	int count = 1;
	int total_delete = 0;
	while (current != NULL)
	{
		if (current->check != 1)
		{
			previous = current;
			current = current->next;
			count++;
		}
		else
		{
			int value_index = 0;
			if (count == number_node)
			{
				printf("[%d]+  Done                   ", current->id);
				while (strcmp(current->value[value_index],"") != 0){
            printf("%s ", current->value[value_index]);
            value_index++;
        }
				printf("\n");
				if (current->pid == head->pid)
				{
					head = head->next;
					current = current->next;
					count++;
					total_delete++;
				}
				else
				{
					previous->next = current->next;
					current = current->next;
					count++;
					total_delete++;
				}
			}
			else if (count == number_node - 1)
			{
				printf("[%d]-  Done                   ", current->id);
				while (strcmp(current->value[value_index],"") != 0) {
            printf("%s ", current->value[value_index]);
            value_index++;
        }
				printf("\n");
				if (current->pid == head->pid)
				{
					head = head->next;
					current = current->next;
					count++;
					total_delete++;
				}
				else
				{
					previous->next = current->next;
					current = current->next;
					count++;
					total_delete++;
				}
			}
			else
			{
				printf("[%d]   Done                   ", current->id);
				while (strcmp(current->value[value_index],"") != 0) {
            printf("%s ", current->value[value_index]);
            value_index++;
        }
				printf("\n");
				if (current->pid == head->pid)
				{
					head = head->next;
					current = current->next;
					count++;
					total_delete++;
				}
				else
				{
					previous->next = current->next;
					current = current->next;
					count++;
					total_delete++;
				}
			}
		}
	}
	number_node -= total_delete;
}
void child_handler()
{
	int wstat;
	pid_t pid;
	pid = waitpid(-1, &wstat, WNOHANG);
	if (pid > 0)
	{
		struct node *current = head;
		struct node *previous = NULL;
		while (current->pid != pid)
		{
			// previous = current;
			current = current->next;
		}
		current->check = 1;
	}
}
void redirecto(char **x)
{
	int i = 0;
	while (x[i] != NULL)
	{
		if (strcmp(x[i], ">") == 0 && x[i + 1] != NULL)
		{
			int file = open(x[i + 1], O_WRONLY | O_CREAT, 0666);
			if (file == -1)
			{
				perror("Fail");
				exit(errno);
			}
			else
			{
				dup2(file, STDOUT_FILENO);
				x[i] = NULL;
				x[i + 1] = NULL;
				close(file);
			}
		}
		else if (strcmp(x[i], "<") == 0 && x[i + 1] != NULL)
		{
			int file = open(x[i + 1], O_RDONLY);
			if (file == -1)
			{
				perror("Fail");
				exit(errno);
			}
			else
			{
				dup2(file, STDIN_FILENO);
				x[i] = NULL;
				close(file);
			}
		}
		else if (x[i] == NULL)
		{
			break;
		}
		else
		{
			i++;
		}
	}
}
void check_command(char *x, int z)
{
	delete_excess_space(x);
	char *token = strtok(x, " ");
	if (strcmp(token, "echo\n") == 0)
	{
		printf("\n");
		free(token);
		starting();
		save_exit = 0;
	}
	if (strcmp(token, "echo") == 0)
	{
		token = strtok(NULL, " ");
		if (strcmp(token, "$?\n") == 0)
		{
			printf("%d\n", save_exit);
			save_exit = 0;
		}
		else
		{
			printf("%s", token);
			token = strtok(NULL, " ");
			while (token != NULL)
			{
				printf(" %s", token);
				token = strtok(NULL, " ");
			}
		}
		free(token);
		save_exit = 0;
		if (z == 0)
		{
			starting();
		}
	}
	else if (strcmp(token, "exit") == 0)
	{
		token = strtok(NULL, " ");
		printf("bye");
		free(token);
		exit(atoi(token));
	}
	else if (strcmp(token, "exit\n") == 0)
	{
		printf("bye");
		free(token);
		save_exit = atoi(token);
		exit(atoi(token));
	}
	else if(strcmp(token,"fg")== 0){
		int ans;
		token = strtok(NULL, " ");
		if(token[0]=='%'){
			token[0]=' ';
			ans = atoi(token);
			fg(ans);
		}
		else{
			printf("bad command");
			free(token);
			save_exit=127;
			starting();
		}
	}
	else if (strcmp(token, "jobs\n") == 0)
	{
		int count_node = 0;
		struct node *ptr = head;
		while (ptr != NULL)
		{
			count_node++;
			if (ptr->check == 1)
			{
				ptr = ptr->next;
			}
			else{
				int value_index = 0;
				if (count_node == number_node && ptr->check == 0)
				{
					printf("[%d]+  Running                ", ptr->id);
				}
				else if (count_node == number_node - 1 && ptr->check == 0)
				{
					printf("[%d]-  Running                ", ptr->id);
				}
				else if(ptr->check == 0)
				{
					printf("[%d]   Running                ", ptr->id);
				}
				else if(count_node == number_node && ptr->check == 3)
				{
					printf("[%d]+  Stopped                ", ptr->id);
				}
				else if (count_node == number_node - 1 && ptr->check == 3)
				{
					printf("[%d]-  Stopped                ", ptr->id);
				}
				else{
					printf("[%d]   Stopped                ", ptr->id);
				}
				while (strcmp(ptr->value[value_index],"") != 0) {
            printf("%s ", ptr->value[value_index]);
            value_index++;
        }
				printf("&\n");
				ptr = ptr->next;
			}
		}
		if (z == 0)
		{
			starting();
		}
	}
	else
	{
		pid_t pid;
		int j = 0;
		int bg_check = 0;
		char *y = malloc(sizeof(char) * 256);
		while (token != NULL)
		{
			strcat(y, token);
			strcat(y, " ");
			token = strtok(NULL, " ");
		}
		char *token2 = strtok(y, " ");
		char *args[20] = {};
		while (token2 != NULL)
		{
			*(args + j) = token2;
			if (*(*(args + j) + strlen(args[j]) - 1) == '\n' || *(*(args + j) + strlen(args[j]) - 1) == ' ')
			{
				*(*(args + j) + strlen(args[j]) - 1) = '\0';
			}
			token2 = strtok(NULL, " ");
			j++;
		}
		args[j] = NULL;
		if (j > 1)
		{
			if (strcmp(args[j - 1], "&") == 0 && strcmp(args[0], "sleep") == 0)
			{
				args[j - 1] = NULL;
				bg_check = 1;
			}
		}
		if ((pid = fork()) < 0)
		{
			perror("Fork failed");
			exit(errno);
		}
		if (!pid)
		{
			int z = 0;
			save_exit = 0;
			setpgid(0, 0);
			signal(SIGTTOU, SIG_IGN);
			if (bg_check == 0)
			{
				tcsetpgrp(STDIN_FILENO, getpid());
			}
			signal(SIGTSTP, SIG_DFL);
			signal(SIGINT, SIG_DFL);
			redirecto(args);
			z = execvp(args[0], args);
			if (z == -1)
			{
				save_exit = 127;
				printf("bad command\n");
				exit(save_exit);
			}
		}
		else
		{
			setpgid(pid, pid);
			if (bg_check == 0)
			{
				waitpid(pid, NULL, WUNTRACED);
				signal(SIGTTOU, SIG_IGN);
				tcsetpgrp(STDIN_FILENO, getpid());
				signal(SIGTSTP, SIG_IGN);
				signal(SIGINT, SIG_IGN);
			}
			if (bg_check == 1)
			{
				int i = 1;
				int w_index = 0;
				struct node *newNode = malloc(sizeof(struct node));
				number_node++;
				newNode->pid = pid;
				newNode->value = copy_array_2_dimension(args);
				newNode->next = NULL;
				if (head == NULL)
				{
					newNode->id = 1;
					head = newNode;
				}
				else
				{
					struct node *lastNode = head;
					while (lastNode->next != NULL)
					{
						lastNode = lastNode->next;
					}
					i = lastNode->id + 1;
					newNode->id = i;
					lastNode->next = newNode;
				}
				printf("[%d] %d\n", newNode->id, newNode->pid);
			}
			if (z == 0)
			{
				free(token);
				starting();
				
			}
			else
			{
				free(token);
			}
		}
	}
}

void is_space(char *x, int z)
{
	int i = 0;
	while (*(x + i) == ' ')
	{
		i++;
	}
	if (strlen(x) - 1 == i || strlen(x) == 1)
	{
		if (z == 1)
		{
			printf("\n");
			free(x);
		}
		else
		{
			starting();
			free(x);
		}
	}
	else
	{
		if (*(x + i) == '!' && *(x + i + 1) == '!')
		{
			int test = 0;
			while (1)
			{
				if (*(x + i + 2) != '\n' && *(x + i + 2) != ' ')
				{
					i++;
					test++;
				}
				else if (*(x + i + 2) == '\n')
				{
					break;
				}
				else
				{
					i++;
				}
			}
			if (test == 0 && *(pre_cmd) != '\0')
			{
				if (z == 0)
				{
					printf("%s", pre_cmd);
				}
				strcpy(x, pre_cmd);
				check_command(x, z);
			}
			else
			{
				if (test > 0)
				{
					strcpy(pre_cmd, x);
					check_command(x, z);
				}
				else if (z == 1)
				{
					printf("\n");
					free(x);
				}
				else
				{
					starting();
					free(x);
				}
			}
		}
		else
		{
			strcpy(pre_cmd, x);
			check_command(x, z);
		}
	}
}
int starting()
{
	done_sleep();
	terminate_sleep();
	char *input = malloc(sizeof(char) * 256);
	printf("icsh : ");
	fgets(input, 256, stdin);
	is_space(input, 0);
	return 0;
}
int main(int argc, char *argv[])
{
	signal(SIGCHLD, child_handler);
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	signal(SIGINT, SIG_IGN);
	if (argc == 1)
	{
		printf("Starting IC shell\n");
		starting();
		return 0;
	}
	else
	{
		char *input = malloc(sizeof(char) * 256);
		FILE *fp;
		fp = fopen(argv[1], "r");
		while (fgets(input, 256, fp) != NULL)
		{
			is_space(input, 1);
		}
		fclose(fp);
		return 0;
	}
}