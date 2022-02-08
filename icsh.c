#include<ctype.h>
#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h> 
#include<sys/stat.h>
#include <fcntl.h> 


char pre_cmd[256];
int save_exit;

void delete_excess_space(char* x){
	int len;
	len = strlen(x)-1;
	for(int i = len; i > 0; i--){
		if(*(x+i) == '\n' || *(x+i)==' '){
			*(x+i) = '\0';
		}
		else{
			*(x+i+1) = '\n';
			break;
		}
	}
}
void redirecto(char** x){
	int i =0;
	while(x[i]!= NULL){
		if(strcmp(x[i],">")==0 && x[i+1] != NULL){
			int file = open(x[i+1],O_WRONLY|O_CREAT,0666);
			if(file == -1){
				perror ("Fail");
        exit(errno);
			}else{
				dup2(file,STDOUT_FILENO);
				x[i] = NULL;
				x[i+1] = NULL;
				close(file);
			}
		}
		else if(strcmp(x[i],"<")==0 && x[i+1] != NULL){
			int file = open(x[i+1],O_RDONLY);
			if(file == -1){
				perror ("Fail");
        exit(errno);
			}else{
				dup2(file,STDIN_FILENO);
				x[i] = NULL;
				close(file);
			}
		}
		else if(x[i] == NULL){
			break;
		}
		else{
			i++;
		}
	}
}
void check_command(char* x, int z){ 
	delete_excess_space(x);
	char *token = strtok(x," ");
	if(strcmp(token,"echo\n")==0){
		printf("\n");
		free(token);
		starting();
		save_exit = 0;
	}
	if(strcmp(token,"echo")==0){
		token = strtok(NULL," ");
		if(strcmp(token,"$?\n")==0){
			printf("%d\n",save_exit);
			save_exit = 0;
		}
		else{
			printf("%s",token);
			token = strtok(NULL," ");
			while(token != NULL){
				printf(" %s",token);
				token = strtok(NULL," ");
			}
		}
		free(token);
		save_exit = 0;
		if(z ==0){
			starting();
		}
	}
	else if(strcmp(token,"exit")==0){
		token = strtok(NULL," ");
		printf("bye");
		free(token);
		exit(atoi(token));
	}
	else if(strcmp(token,"exit\n")==0){
		printf("bye");
		free(token);
		save_exit = atoi(token);
		exit(atoi(token));
	}
	else{
		pid_t pid;
		if ((pid=fork()) < 0)
      {
        perror ("Fork failed");
        exit(errno);
      }
		if (!pid){
			save_exit = 0;
			int i =0;
			int j =0;
			char *y = malloc(sizeof(char)*256);
			while(token != NULL){
				strcat(y,token);
				strcat(y," ");
				token = strtok(NULL," ");
			}
			char *token2 = strtok(y," ");
			char *args[4] = {};
			while(token2 != NULL){
				*(args+j)=token2;
				if(*(*(args+j)+ strlen(args[j])-1)=='\n' || *(*(args+j)+ strlen(args[j])-1)==' '){
					*(*(args+j)+ strlen(args[j])-1) = '\0';
					}
				token2 = strtok(NULL," ");
				j++;
			}
			args[j]=NULL;
			setpgid(0,0);
			signal (SIGTTOU, SIG_IGN);
			tcsetpgrp(STDIN_FILENO, getpid());
			signal (SIGTSTP, SIG_DFL);
			signal (SIGINT, SIG_DFL);
			redirecto(args);
			int z = execvp(args[0],args);
		if (z == -1){
			save_exit = 127;	
			printf("bad command\n");
			kill(getpid(),SIGINT);
		}
		}
		if (pid)
      {
        waitpid (pid,NULL, WUNTRACED);
				signal (SIGTTOU, SIG_IGN);
				tcsetpgrp(STDIN_FILENO, getpid());
				signal(SIGTSTP, SIG_IGN);
				signal (SIGINT, SIG_IGN);
      }
		if(z ==0){
			starting();
			free(token);
		}
		else{
			free(token);
		}
	}
}

void is_space(char* x, int z){
	int i =0;
	while(*(x+i )== ' '){
		i++;
	}
	if(strlen(x)-1 == i || strlen(x)==1){
		if(z==1){
			printf("\n");
			free(x);
		}
		else{
			starting();
			free(x);
		}
	}
	else{
		if(*(x+i)=='!' && *(x+i+1)=='!'){
			int test =0;
			while(1){
				if(*(x+i+2)!='\n' && *(x+i+2)!=' '){
					i++;
					test++;
				}
				else if(*(x+i+2)=='\n'){
					break;
				}
				else{
					i++;
				}
			}
			if(test == 0 && *(pre_cmd) != '\0'){
				if(z ==0){
					printf("%s",pre_cmd);
				}
				strcpy(x,pre_cmd);
				check_command(x, z);
			}
			else{
				if(test >0){
					strcpy(pre_cmd,x);
					check_command(x, z);
				}
				else if(z==1){
					printf("\n");
					free(x);
				}
				else{
					starting();
					free(x);
				}
			}
		}
		else{
			strcpy(pre_cmd,x);
			check_command(x, z);
		}
	}
}
int starting(){
  char *input = malloc(sizeof(char)*256);
  printf("icsh : ");
  fgets(input,256,stdin);
  is_space(input, 0);
  return 0;
}
int main(int argc, char *argv[]){
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	signal (SIGINT, SIG_IGN);
	if(argc == 1){
  	printf("Starting IC shell\n");
  	starting();
  	return 0;
	}
	else{
		char *input = malloc(sizeof(char)*256);
		FILE *fp;
		fp = fopen(argv[1],"r");
		while( fgets (input, 256, fp)!=NULL ) {
			is_space(input, 1);
   }
   fclose(fp);
	 return 0;
	}
}