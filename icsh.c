#include<ctype.h>
#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<string.h>
#include <unistd.h>

char pre_cmd[256];

void check_command(char* x, int z){ 
	char *token = strtok(x," ");
	if(strcmp(token,"echo\n")==0){
		
		printf("\n");
		free(token);
		if(z ==0){
			starting();
		}
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
		exit(atoi(token));
	}
	else{
		printf("bad command\n");
		if(z ==0){
			starting();
		}
		free(token);
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
