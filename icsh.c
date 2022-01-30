#include<ctype.h>
#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<string.h>
#include <unistd.h>

char pre_cmd[256];

void check_command(char* x){ 
	char *token = strtok(x," ");
	if(strcmp(token,"echo\n")==0){
		
		printf("\n");
		free(token);
		starting();
	}
	if(strcmp(token,"echo")==0){
		token = strtok(NULL," ");
		printf("%s",token);
		token = strtok(NULL," ");
		while(token != NULL){
			printf(" %s",token);
			token = strtok(NULL," ");
			
		}
		free(token);
		starting();
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
		exit(atoi(token));
	}
	else{
		printf("bad command\n");
		starting();
		free(token);
	}

}

void is_space(char* x){
	int i =0;
	while(*(x+i )== ' '){
		i++;
	}
	if(strlen(x)-1 == i || strlen(x)==1){
		starting();
		free(x);
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
				printf("%s",pre_cmd);
				strcpy(x,pre_cmd);
				check_command(x);
			}
			else{
				if(test >0){
					strcpy(pre_cmd,x);
					check_command(x);
				}
				starting();
				free(x);
			}
		}
		else{
			strcpy(pre_cmd,x);
			check_command(x);
		}
	}
}
int starting(){
  char *input = malloc(sizeof(char)*256);
  printf("icsh : ");
  fgets(input,256,stdin);
  is_space(input);
  return 0;
}
int main(){
  printf("Starting IC shell\n");
  starting();
  return 0;
}
