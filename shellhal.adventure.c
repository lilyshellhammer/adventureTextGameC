/**********************************************************************************
* Program Name: Play Adventure - Text Adventure Game
* Author: Lily Shellhammer
* Date: Feb 19th, 2017
* Description: 
***********************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pthread.h>

//I know these are global, I don't have to time to undo my sins, it's too late
pthread_mutex_t lock;
pthread_t Time;
void *printTime(void *arg);


//this keeps track of room names you've visited
typedef struct path { 
	char** visited;
	int num;
};

/*****************************************************************
* Function name: directions
* Description: shows options for which room to move into, displays time if you write "time"
* Input: array of possible rooms (saved), char array for answer, counter for times traveled
* Output: prints options of places, reprompts if they enter messed up answers
*****************************************************************/
void directions(char** saved, char* answer, int *counter){
	int i, flag = 0, flag2 =0;
	do{
		printf("WHERE TO? > \n");
		scanf("%s",answer);
		printf("\n");
		for(i = 0; i < 7; i++){
			if(saved[i][0] != '\0')
				if(!strcmp(answer,saved[i])){
					flag = 1;
					(*counter)+=1;
					break;
				}
				else if(!strcmp(answer, "time")){	//this prints out time using multithreading
					pthread_mutex_unlock(&lock);
					pthread_join(Time, NULL);
					pthread_mutex_destroy(&lock);
					pthread_mutex_init(&lock, NULL);
					pthread_mutex_lock(&lock);
					pthread_create(&Time, NULL, printTime, NULL);
					flag2 = 1;
					break;
				}
		}
		if(flag == 0 && flag2 == 0){
			printf("HUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
		}
	}while(flag == 0);
}


/*****************************************************************
* Function name: printTime
* Description: printsTime using mutex thread
* Input: 
* Output: 
*****************************************************************/
void *printTime(void *arg){

	pthread_mutex_lock(&lock);
	time_t curtime;
	time(&curtime);
	printf("\n%s\n", ctime(&curtime));

}
/*****************************************************************
* Function name: tolkien
* Description: Parces files and prints saves information, also checks if you made it to the end
* Input: readBuffer(file contents), saved positions, path object to track progress
* Output: return status (2 == done, 0 == not)
*****************************************************************/
int tolkien(char* readBuffer, char** saved, int* counter, struct path p){
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
	while(strcmp(token, "MID_ROOM") && strcmp(token, "BEG_ROOM") && strcmp(token, "END_ROOM")){
		saved[i] = token;
		token = strtok(NULL, " ");
		token = strtok(NULL, " "); 
		token = strtok(NULL, "\n");
		i++;
	}
	if(!strcmp("END_ROOM",token)){
		printf("You made it! It took you %d steps\n", *counter);
		for(i = 1; i < p.num; i++){
			if( i == (p.num)){
				printf("Final Step: %s\n", i, p.visited[i]);
				return 2;
			}
			else
				printf("Step %d: %s\n", i, p.visited[i]);
		}
		return 2;
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
		printf("\n");
	}
	return 0;
}


/*****************************************************************
* Function name: file_turn
* Description: this opens each file and then calls a fucntion to parse info. check if we are at
* end yet. Saves traveled locations for each file
* Input: filePath name, path object, saved places array
* Output: return value (2 == done, 0 == keep going)
*****************************************************************/
int file_turn(char* filePath, int* counter, char* next_place, char** saved, struct path p){
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


	/*END IS A FLAG THAT SAYS IF WE MADE IT TO END OR NOT*/
	int end = tolkien(readBuffer,saved, counter, p);
	if(end != 2) //aka the end has happened
		directions(saved, next_place, counter);
	else
		return 2;

	for(i = 0; i < 7; i++){
		memset(saved[i], '\0', 6);
	}
	return 0;
}




/*****************************************************************
* Function name: main
* Description: Creates directory with shellhal.room.<PID>, fills with 7 names randomly picked and randomly
* connected, calls functions to traverse files
* Input: none
* Output: none
*****************************************************************/
int main(){
	//seed time
	srand(time(NULL));
	//mutex thread locks
	pthread_mutex_init(&lock, NULL);
	pthread_mutex_lock(&lock);
	pthread_create(&Time, NULL, printTime, NULL);

	//variabels needed for paths to open files
	int max_so_far = -1;
	char* next_place;
	char* latestFile;
	//Need to open files one at a time, and find beginning file
	//Display options, ask user for direction 
	struct dirent *dir;

	const char direct[15] = "shellhal.room."; // string to look for in strstr function
	struct stat statBuffer; //struct for stat()
	char datestring[256];
	time_t lastModified;

	DIR *d = opendir(".");
	DIR *shellhal;
	while ((dir = readdir(d)) != NULL)
	 {	
		 if (strstr(dir->d_name, direct) != NULL)
		 {
		 	stat(dir->d_name, &statBuffer);	 //call stat on directiory
            if (lastModified > max_so_far) // MAX_SO_FAR is a global initialized to -1
            {
            	max_so_far = lastModified;
            	latestFile = dir->d_name;
            }
        }	 	
	}

	max_so_far = -1;

	char shellhaldir[30];
	const char end[6] = "_room";
	struct dirent *s;
	time_t lastModifiedShell;

	memset(shellhaldir, '\0', 30);
	sprintf(shellhaldir, "/%s", latestFile);
	shellhal = opendir(latestFile);

	while ((s = readdir(shellhal)) != NULL){
		if (strstr(s->d_name, end) != NULL) {
	 		stat(s->d_name, &statBuffer);	 //call stat on directiory
	    	lastModifiedShell = statBuffer.st_mtime;
	        if (lastModifiedShell > max_so_far) // MAX_SO_FAR is a global initialized to -1
	        {
	        	max_so_far = lastModifiedShell;
	        	next_place = s->d_name;
			 }
		}
	}
	char* latest = malloc(sizeof(latestFile+2)* sizeof(char));
	strcpy(latest,latestFile);
	printf("%s\n", latest);

	/*DONE WITH OPENING FILES AND FINDIN NEWEST, close files now*/ 
 	closedir(d);
 	closedir(shellhal);
 

	char filePath[40];
	memset(filePath, '\0', 40);
	int counter = 0;

	/*CREATE ARRAY TO SAVE LOCATIONS*/ 
	char** saved = malloc(7 * sizeof(char*));
	int i, j;
	for(i = 0; i < 7; i++)
	{
		saved[i] = malloc(6 * sizeof(char));
		memset(saved[i], '\0', sizeof(saved[i]));
	}
	/*CONSTRUCT PATH TO FOLLOW STEPS TAKEN IN GAME*/
	struct path p;
	p.visited = malloc(50*sizeof(char*));
	for(i = 0; i < 50; i++){
		p.visited[i] = malloc(10*sizeof(char));
		memset(p.visited[i], '\0', sizeof(p.visited[i]));
	}
	p.num = 0;
	int a = 0;
	/*TRAVERSE ROOMS UNTIL GAME IS WON*/
	do{
		strcpy(p.visited[p.num],next_place);
		if (p.num != 0)
			sprintf(filePath, "%s/%s_room", latest, next_place);
		else
			sprintf(filePath, "%s/%s", latest, next_place);
		p.num++;
		a = file_turn(filePath, &counter, next_place, saved, p);
		memset(filePath, '\0', sizeof(filePath));
	}while(a != 2);


	/*ATTEMPT TO FREE MEMORY LMAO*/
	free(latest);
	for(i = 0; i < 50; i++) {
	 free(p.visited[i]);
	}
	free(p.visited);

	for(i = 0; i < 7; i++) { 
	//	free(saved[i]);
	}
	//free(saved);

	exit(0);
}
