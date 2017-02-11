#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

char** create_names(){
	int i;
	char** names = malloc(10 * sizeof(char*));
	for(i = 0; i <= 10; i++)
	{
		names[i] = malloc(10 * sizeof(char));
		memset(names[i], '\0', 10);
		switch(i){
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

char** create_room_names(char **names){
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

	for(i = 0; i < 7; i++)
	{
		printf("%s\n", rooms[i]);
	}
	return rooms;
}


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

	/*PRINT DESCRIPTIONS TO SEPARATE FILES*/
	int i;
	char name_des[22],type_des[22];
	char type[15],filename[15];
	for(i = 0; i < 7; i++){
		switch (i){
			case 1: strcpy(type,"BEGIN_ROOM"); break;
			case 7: strcpy(type, "END_ROOM"); break;
			default: strcpy(type, "MID_ROOM"); break;
		}
		sprintf(filename, "%s_room",rooms[i]);
		file_descriptor = open(filename, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
		sprintf(name_des,"ROOM NAME: %s\n",rooms[i]);
		nwritten = write(file_descriptor, name_des, strlen(name_des) * sizeof(char));
		sprintf(type_des, "ROOM TYPE: %s\n", type);
		nwritten = write(file_descriptor, type_des, strlen(type_des) * sizeof(char));
	}

	char readBuffer[38];
	memset(readBuffer, '\0', sizeof(readBuffer)); // Clear out the array before using it 
	lseek(file_descriptor, 0, SEEK_SET); // Reset the file pointer to the beginning of the file 
	nread = read(file_descriptor, readBuffer, sizeof(readBuffer));

	printf("File contents:\n%s\n", readBuffer);
	exit(0);
}