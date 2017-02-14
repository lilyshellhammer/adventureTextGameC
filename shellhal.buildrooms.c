/**********************************************************************************
* Program Name: Build Rooms - Text Adventure Game
* Author: Lily Shellhammer
* Date: Feb 10th, 2017
* Description: Creates rooms with randomly generated connections, beg/middle/end rooms
* 			   all in separate files within a subdirectory called shellhal.rooms.<PID>
***********************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

/*****************************************************************
* Function name: create_names
* Description: hardcodes a 2d dynamic array with possible room names
* Input: none
* Output: 2d dynamic char array of 10 possible room names
*****************************************************************/
char** create_names(){
	/*DYN CREATE 10 x 10 char array */
	int i;
	char** names = malloc(10 * sizeof(char*));
	for(i = 0; i < 10; i++)
	{
		names[i] = malloc(10 * sizeof(char));
		memset(names[i], '\0', 10);
		switch(i){
			/*hardcoded 10 names*/
			case 0: strcpy(names[i], "NYTimes"); break;
			case 1: strcpy(names[i], "Cobra"); break;
			case 2: strcpy(names[i], "Lambda"); break;
			case 3: strcpy(names[i], "HogParade"); break;
			case 4: strcpy(names[i], "Bumbles"); break;
			case 5: strcpy(names[i], "Snoot"); break;
			case 6: strcpy(names[i], "YYZFace"); break;
			case 7: strcpy(names[i], "Eleven"); break;
			case 8: strcpy(names[i], "GayTony"); break;
			case 9: strcpy(names[i], "DikNixon"); break;
		}
	}

	return names;
}

/*****************************************************************
* Function name: create_room_names
* Description: Randomly selects room names from the list and creates a 2d array of names
* Input: selection 2d array (to choose 7 names from)
* Output: 2d dynamic char array of 7 randomly picked room names
*****************************************************************/
char** create_room_names(char** names){
	/*Randomly chooses names and sets up list of 7*/
	char** rooms = malloc(7 * sizeof(char*));
	int i, j, count=0;
	int used[7];
	for(i = 0; i < 7; i++)
	{
		int r = rand() % 10;
		for(j = 0; j < 7; j++){
			if (r == used[j]){
				r = rand() % 10;
				j = -1;
			}
		}
		rooms[i] = malloc(10 * sizeof(char));
		memset(rooms[i], '\0', 10);
		strcpy(rooms[i], names[r]);
		used[count] = r;
		count++;
	}
	/******************************************/
	/*JUST FOR PRINTING ROOM NAMES*/
	for(i = 0; i < 7; i++)
	{
		printf("%s\n", rooms[i]);
	}
	return rooms;
}

/*****************************************************************
* Function name: connecting
* Description: Randomly generates between 3 and 6 room connections. No repeats, end and beginning rooms don't connect.
* Input: none
* Output: 2d dynamic integer array with connections represented by their int position in "rooms" array
*****************************************************************/
int** connecting(){
	int i ,j, k, g, r, flag = 0;
	int** connected = malloc(7 * sizeof(char*));
	for(i = 0; i < 7; i++)
	{
		connected[i] = malloc(6 * sizeof(char));
		for(j =0; j < 6; j++){
			connected[i][j] = 0;
		}
	}
	int fill = 0;
	for(i = 0; i < 7; i++){ //FOR EACH ROOM
		for(j =0; j < 3; j++){ //MAKE THREE RAND CONNECTIONS
			do{
				flag = 0;
				r = rand()%7;	//rand r
				if (connected[i][r]==1)
					flag = 1;
				else{
					connected[i][r] = 1;
					connected[r][i] = 1;
					flag = 0;
				}
				
			}while(flag == 1);
		}
		fill = 0;
	}
	/******************************************/
	/* THIS IS JUST TO PRINT OUT CONNECTIONS TO SEE IF THEY ARE WORING*/
	printf("\n\n");
	for(i = 0; i < 7; i++){
		for(j =0; j < 6; j++){
			printf("%d ", connected[i][j]);
		}
		printf("\n");
	}
	return connected;
}

/*****************************************************************
* Function name: 
* Description:
* Input: 
* Output:
*****************************************************************/
void free_int_time(int** array, int rows){
	int i;
	for(i = 0; i < rows; i++){
		free(array[i]);
	}
	free(array);
}
void free_char_time(char** array, int rows){
	int i;
	for(i = 0; i < rows; i++){
		free(array[i]);
	}
	free(array);
}

/*****************************************************************
* Function name: main
* Description:
* Input: 
* Output:
*****************************************************************/
int main(void){
	/*CREATE RANDOMIZATION*/
	time_t t;
	srand((unsigned) time(&t));

	/*PREP CREATING MAIN FOLDER*/
	int file_descriptor;
	ssize_t nread, nwritten;

	char newFilePath[20];
	memset(newFilePath,'\0',20);
	sprintf(newFilePath,"./shellhal.room.%d",getpid());

	/*OPEN MAIN FOLDER*/
	file_descriptor = open(newFilePath, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if (file_descriptor == -1)
	{
		printf("Hull breach - open() failed on \"%s\"\n", newFilePath); perror("In main()");
		exit(1);
	}

	/*HARDCODE NAMES*/
	char **names = create_names();
	/*CREATE RANDOM NAMES*/
	char** rooms = create_room_names(names);
	/*MAKE CONNECTIONS BASED ON INTS*/
	int** connected = connecting();


	/*PRINT DESCRIPTIONS TO SEPARATE FILES*/
	int i, j;
	/*char *name_des = malloc(10 * sizeof(char)); 
	memset(name_des, '\0', 22);
	char *type_des = malloc(10 * sizeof(char));
	memset(type_des, '\0', 22);
	char *type = malloc(10 * sizeof(char));
	memset(type, '\0', 22);
	char *filename = malloc(10 * sizeof(char)); 
	memset(filename, '\0', 22);
	*/
	char name_des[30], type_des[30], type[30], filename[30], conn_des[30];

	for(i = 0; i < 7; i++){
		memset(name_des, '\0', 30);
		memset(type_des, '\0', 30);
		memset(type, '\0', 30);
		memset(filename, '\0', 30);
		memset(conn_des, '\0', 30);
		switch (i){
			case 1: strcpy(type,"END_ROOM"); break;
			case 7: strcpy(type, "BEG_ROOM"); break;
			default: strcpy(type, "MID_ROOM"); 
		}
		
		sprintf(filename, "%s_room",rooms[i]);
		file_descriptor = open(filename, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
		if (file_descriptor == -1)
		{
			printf("Hull breach - open() failed on \"%s\"\n", newFilePath); perror("In main()");
			exit(1);
		}
		sprintf(name_des,"ROOM NAME: %s\n",rooms[i]);
		nwritten = write(file_descriptor, name_des, strlen(name_des) * sizeof(char));
		j=0;
		for(j=0; j< 6; j++){
			if(connected[i][j] == 1)
				sprintf(conn_des,"ROOM CONNECTION: %s\n",names[j]);
			nwritten = write(file_descriptor, conn_des, strlen(conn_des) * sizeof(char));
		}

		sprintf(type_des, "ROOM TYPE: %s\n", type);
		nwritten = write(file_descriptor, type_des, strlen(type_des) * sizeof(char));
	
	}

	char readBuffer[256];
	memset(readBuffer, '\0', sizeof(readBuffer)); // Clear out the array before using it 
	lseek(file_descriptor, 0, SEEK_SET); // Reset the file pointer to the beginning of the file 
	nread = read(file_descriptor, readBuffer, sizeof(readBuffer));

	printf("File contents:\n%s\n", readBuffer);

//	free_char_time(names, 10);
//	free_char_time(rooms, 7);
//	free_int_time(connected, 7);

	exit(0);
}
