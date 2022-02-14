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

char pre_cmd[256];
int save_exit;
int number_node;
typedef struct node
{
	int id;
	int pid;
	char **value;
	int check;
	struct node *next;
} node;

struct node *head = NULL;
struct node *current = NULL;

char **copy_array_2_dimension(char **x)
{
	char **a = malloc(256 * sizeof(char *));
	for (int i = 0; i < 256; i++)
	{
		a[i] = malloc(256 * sizeof(char));
	}
	int i = 0;
	while (x[i] != NULL)
	{
		strcpy(a[i], x[i]);
		i++;
	}
	return a;
}
void bg(int id){
	int count_node = 1;
	struct node *current = head;
	
	while (current != NULL){
		int value_index = 0;
		if (current->id == id && current->check == 3){
			current->check = 0;
			if(count_node == number_node){
				printf("[%d]+ ",current->id);
			}
			else if(count_node == number_node-1){
				printf("[%d]- ",current->id);
			}
			else{
				printf("[%d]  ",current->id);
			}
			while (strcmp(current->value[value_index], "") != 0)
				{
					printf("%s ", current->value[value_index]);
					value_index++;
				}
				printf("&\n");
				current->check = 0;
				kill(current->pid,SIGCONT);
				save_exit = 0;
				return;
		}
		else{
			current = current->next;
		}
	}
	printf("no such job\n");
	save_exit = 1;
}
void fg(int id)
{
	int get_pid = getpid();
	int count_node = 1;
	int i = 0;
	int stat;
	struct node *current = head;
	while (current != NULL)
	{
		if (current->id == id && current->check != 1)
		{
			while (strcmp(current->value[i], "") != 0)
			{
				printf("%s ", current->value[i]);
				i++;
			}
			printf("\n");
			tcsetpgrp(STDIN_FILENO, current->pid); // turn this pid to forground
			kill(current->pid, SIGCONT);
			waitpid(current->pid, &stat, WUNTRACED);
			if (WIFEXITED(stat))
			{
				save_exit = WEXITSTATUS(stat);
			}
			if (WIFSIGNALED(stat))
			{
				current->check = 2;
				save_exit = WTERMSIG(stat);
			}
			if (WIFSTOPPED(stat))
			{
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
				while (strcmp(current->value[value_index], "") != 0)
				{
					printf("%s ", current->value[value_index]);
					value_index++;
				}
				printf("&\n");
				current->check = 3;
				tcsetpgrp(STDIN_FILENO, get_pid);
				save_exit = WSTOPSIG(stat);
				return;
			}
			tcsetpgrp(STDIN_FILENO, get_pid);
			return;
		}
		current = current->next;
		count_node++;
	}
	printf("no such job\n");
	save_exit = 1;
}
void terminate_sleep()
{
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
				while (strcmp(current->value[value_index], "") != 0)
				{
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
				while (strcmp(current->value[value_index], "") != 0)
				{
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
				while (strcmp(current->value[value_index], "") != 0)
				{
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
				break;
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
				break;
			}
		}
		else
		{
			i++;
		}
	}
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

int check_empty(char *x)
{
	if (strlen(x) == 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void check_command(char *x)
{
	delete_excess_space(x);
	char *token = strtok(x, " ");
	if (strcmp(token, "echo\n") == 0)
	{

		printf("\n");
		free(token);
		save_exit = 0;
		return;
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
		return;
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
		exit(atoi(token));
	}
	else if (strcmp(token, "fg") == 0)
	{
		int ans;
		token = strtok(NULL, " ");
		if (token[0] == '%')
		{
			token[0] = ' ';
			ans = atoi(token);
			fg(ans);
		}
		else
		{
			printf("bad command\n");
			free(token);
			save_exit = 127;
		}
	}
		else if (strcmp(token, "bg") == 0)
	{
		int ans;
		token = strtok(NULL, " ");
		if (token[0] == '%')
		{
			token[0] = ' ';
			ans = atoi(token);
			bg(ans);
		}
		else
		{
			printf("bad command\n");
			free(token);
			save_exit = 127;
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
			else
			{
				int value_index = 0;
				if (count_node == number_node && ptr->check == 0)
				{
					printf("[%d]+  Running                ", ptr->id);
				}
				else if (count_node == number_node - 1 && ptr->check == 0)
				{
					printf("[%d]-  Running                ", ptr->id);
				}
				else if (ptr->check == 0)
				{
					printf("[%d]   Running                ", ptr->id);
				}
				else if (count_node == number_node && ptr->check == 3)
				{
					printf("[%d]+  Stopped                ", ptr->id);
				}
				else if (count_node == number_node - 1 && ptr->check == 3)
				{
					printf("[%d]-  Stopped                ", ptr->id);
				}
				else
				{
					printf("[%d]   Stopped                ", ptr->id);
				}
				while (strcmp(ptr->value[value_index], "") != 0)
				{
					printf("%s ", ptr->value[value_index]);
					value_index++;
				}
				printf("&\n");
				ptr = ptr->next;
			}
		}
		free(token);
	}
	// command line command
	else
	{
		int bg_check = 0;
		int j = 0;
		int stat;
		pid_t pid;
		char *y = malloc(sizeof(char) * 256);
		while (token != NULL)
		{
			strcat(y, token);
			strcat(y, " ");
			token = strtok(NULL, " ");
		}
		char *token2 = strtok(y, " ");
		char *args[10] = {};
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
		else if (!pid)
		{
			int z = 0;
			save_exit =0;
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
				kill(getpid(), SIGINT);
			}
		}
		else
		{
			setpgid(pid, pid);
			if (bg_check == 0)
			{
				waitpid(pid, &stat, WUNTRACED);
				signal(SIGTTOU, SIG_IGN);
				tcsetpgrp(0, getpid());
				signal(SIGTSTP, SIG_IGN);
				signal(SIGINT, SIG_IGN);
				if (WIFSIGNALED(stat)) {
        	save_exit= WTERMSIG(stat);
        }      
        if (WIFEXITED(stat)) {
          save_exit = WEXITSTATUS(stat);
        }
			}
			if (bg_check == 1 || WIFSTOPPED(stat))
			{
				int i = 1;
				int w_index = 0;
				struct node *newNode = malloc(sizeof(struct node));
				struct node *lastNode = head;
				newNode->check =0;
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
					while (lastNode->next != NULL)
					{
						lastNode = lastNode->next;
					}
					i = lastNode->id + 1;
					newNode->id = i;
					lastNode->next = newNode;
				}
				if(WIFSTOPPED(stat)){
					int new_i = 0;
					save_exit = WSTOPSIG(stat);
					newNode->check =3;
					printf("[%d]+  Stopped                ",newNode->id);
					while(strcmp(newNode->value[new_i],"")!=0){
						printf("%s ",newNode->value[new_i]);
						new_i++;
					}
					printf("\n");
				}
				else{
					newNode->check =0;
					printf("[%d] %d\n", newNode->id, newNode->pid);
				}
			}
			free(token);
			free(token2);
		}
	}
}

void is_space(char *x, int check)
{
	int i = 0;
	while (*(x + i) == ' ')
	{
		i++;
	}
	if (strlen(x) - 1 == i || strlen(x) == 1)
	{
		free(x);
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
				if (check != 1)
				{
					printf("%s", pre_cmd);
				}
				strcpy(x, pre_cmd);
				check_command(x);
			}
			else
			{
				if (test > 0)
				{
					strcpy(pre_cmd, x);
					check_command(x);
				}
				free(x);
			}
		}
		else
		{
			strcpy(pre_cmd, x);
			check_command(x);
		}
	}
}
int starting()
{
	signal(SIGCHLD, child_handler);
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	signal(SIGINT, SIG_IGN);
	char *input = malloc(sizeof(char) * 256);
	printf("icsh : ");
	fgets(input, 256, stdin);
	is_space(input, 0);
	return 0;
}
int main(int argc, char *argv[])
{
	printf("Starting IC shell\n");
	if (argc == 1)
	{
		while (1)
		{
			done_sleep();
			terminate_sleep();
			starting();
		}
	}
	else
	{
		char *input = malloc(sizeof(char) * 256);
		FILE *fp;
		fp = fopen(argv[1], "r");
		if (fp)
		{
			while (fgets(input, 256, fp) != NULL)
			{
				if (check_empty(input))
				{
					continue;
				}
				else
				{
					is_space(input, 1);
				}
			}
		}
		fclose(fp);
	}
	return 0;
}