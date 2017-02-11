/**********************************************************************************
* Program Name: Play Adventure - Text Adventure Game
* Author: Lily Shellhammer
* Date: Feb 10th, 2017
* Description: 
***********************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

/*****************************************************************
* Function name: directions
* Description: 
* Input: 
* Output: 
*****************************************************************/
void directions(char** saved, char* answer, int *counter){
	int i, flag = 0;
	do{
		printf("WHERE TO? > ");
		scanf("%s",answer);
		for(i = 0; i < 7; i++){
			if(saved[i][0] != '\0')
				if(!strcmp(answer,saved[i])){
					//printf("Headed to %s\n", saved[i]);
					flag = 1;
					(*counter)+=1;
				}
		}
		if(flag == 0){
			printf("HUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
		}
	}while(flag == 0);
}

/*****************************************************************
* Function name: directions
* Description: 
* Input: 
* Output: 
*****************************************************************/
int tolkien(char* readBuffer, char** saved, int* counter){
	int i, j;
	char* token = strtok(readBuffer, " "); 
	token = strtok(NULL, " "); 
	token = strtok(NULL, "\n"); 
	printf("CURRENT LOCATION: %s\n",token);
	token = strtok(NULL, " ");
	token = strtok(NULL, " "); 
	token = strtok(NULL, "\n");

	saved[0] = token;
	token = strtok(NULL, " ");
	token = strtok(NULL, " "); 
	token = strtok(NULL, "\n");
	i = 1;
	while(strcmp(token, "MID_ROOM") && strcmp(token, "BEG_ROOM") && strcmp(token, "END_ROOM") ){
		saved[i] = token;
		token = strtok(NULL, " ");
		token = strtok(NULL, " "); 
		token = strtok(NULL, "\n");
		i++;
	}
	if(!strcmp("END_ROOM",token)){
		printf("You made it! It took you %d steps\n", *counter);
		exit(0);
	}
	else{
		printf("POSSIBLE CONNECTIONS: ");
		for(j = 0; j < i; j++){
			if(j == (i-1)){
				printf("%s.\n", saved[j]);
			}
			else{
				printf("%s, ", saved[j]);
			}
		}
	}
	return 0;
}


void file_turn(char* filePath, int* counter, char* next_place, char** saved){
	int file_descriptor;
	int i;
	ssize_t nread, nwritten;

	file_descriptor = open(filePath, O_RDWR , S_IRUSR | S_IWUSR);
	if (file_descriptor == -1)
	{
		printf("Hull breach - open() failed on \"%s\"\n", filePath); perror("In main()");
		exit(1);
	}

	char readBuffer[256];
	memset(readBuffer, '\0', sizeof(readBuffer)); // Clear out the array before using it 
	lseek(file_descriptor, 0, SEEK_SET); // Reset the file pointer to the beginning of the file 
	nread = read(file_descriptor, readBuffer, sizeof(readBuffer));

	printf("File contents:\n%s\n", readBuffer);


	/*END IS A FLAG THAT SAYS IF WE MADE IT TO END OR NOT*/
	int end = tolkien(readBuffer,saved, counter);
	//if(end != 1) //aka the end has happened
		directions(saved, next_place, counter);
	

	for(i = 0; i < 7; i++){
		memset(saved[i], '\0', 6);
	}
}


/*****************************************************************
* Function name: main
* Description: 
* Input: 
* Output: 
*****************************************************************/
int main(){
	//TO DO!!!!!!!!!!!!!!!!!!!!
	//Need to open files one at a time, and find beginning file
	//Display options, ask user for direction 
	
	char filePath[20];
	memset(filePath, '\0', 20);
	int counter = 0;
	char* next_place = 	malloc(10 * sizeof(char));
	strcpy(next_place, "Cobra");

	/*CREATE ARRAY TO SAVE LOCATIONS*/
	char** saved = malloc(7 * sizeof(char*));
	int i, j;
	for(i = 0; i < 7; i++)
	{
		saved[i] = malloc(6 * sizeof(char));
		memset(saved[i], '\0', 6);
	}

	do{
	sprintf(filePath, "%s_room", next_place);
	file_turn(filePath, &counter, next_place, saved);
	}while(1);


	exit(0);
}