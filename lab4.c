#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int* fileRead(int fileNum);
void fileWrite(char fileName[], double set[], int size);
double* offset(double off, int fileNum, int data[]);
double* scale(double scal, int fileNum, int data[]);
double average(int data[]);
double max(int data[]);
void getStats(int data[], int fileNum);
void center(int data[], int fileNum);
void normalize(int data[], int fileNum);
void help(void);

int main(int argc,char* argv[]){
	int* data;
        double* set;
        double* scaled;
	int size = argc;
        int i,j,fileNum,check = 0;
        double off,scal = 0;
        char fileName[50] = {};
        char fileName1[50] = {};
        char fileName2[50] = {};

        
        //-n handling case
//printf("\n\nargc: %d\n\n",argc);
        for(j = 1; j < size; j++){
	
//printf("\n\nargv[%d]: %s\n\n",j,argv[j]);
            if(!strcmp(argv[j],"-n")){
				//checks to see if there is a valid entry at argv[j+1]
				if(j + 1 > argc - 1){
					printf("\n\nInvalid input detected! Expected argument after -n \n\n");
					help();
					return 1;
				}
				else
					fileNum = atoi(argv[j+1]);
				//checks filenum to make sure its in a reasonable range
				if(fileNum < 1 || fileNum > 99){
					printf("\n\nThe file input number was not valid!\n\n");
					help();
					return -1;
				}
			}
        }
        
		//opens the input file and populates data with its contents
        data = fileRead(fileNum);

		//checks for various user input options
	for(i = 1; i < size; i++){
                //-h handling
		if(!strcmp(argv[i],"-h"))
                    help();   //displays help menu
                //-r handling
		if(!strcmp(argv[i],"-r")){
			//checks to see if there is a valid entry at argv[j+1]
					if(i + 1 > argc - 1){
						printf("\n\nInvalid input detected! Expected argument after -r \n\n");
						help();
						return 1;
					}
					sprintf(fileName1,"Raw_data_%02d.txt",fileNum);   //creates new filename
                    check = rename(fileName1,argv[i+1]);    	//renames file
                }
                //-s handling scale call
                if(!strcmp(argv[i],"-s")){
					//checks to see if there is a valid entry at argv[j+1]
					if(i + 1 > argc - 1){
						printf("\n\nInvalid input detected! Expected argument after -s \n\n");
						help();
						return 1;
					}
                    scal = atoi(argv[i+1]);     //gets user inputted scale value
                    scaled = scale(scal,fileNum,data);		//calls the scale function and sets the result to a new double pointer
                    sprintf(fileName2, "Scaled_data_%02d.txt", fileNum);   //generates the save file name Scaled_data_xx.txt
                    fileWrite(fileName2,scaled,data[0]);			//writes the scaled data to the new filename
                }
                //-o handling offset call
                if(!strcmp(argv[i],"-o")){
					//checks to see if there is a valid entry at argv[j+1]
					if(i + 1 > argc - 1){
						printf("\n\nInvalid input detected! Expected argument after -o \n\n");
						help();
						return 1;
					}
                    off = atoi(argv[i+1]); 			//gets user inputted offset value
                    set = offset(off,fileNum,data); 		//calls the ofset function and sets the result to a new double pointer
                    sprintf(fileName, "Offset_data_%02d.txt", fileNum);			//generates the save file name Offset_data_xx.txt
                    fileWrite(fileName,set,data[0]);  				//writes the set data to the new filename
                }
                // -S handling auto statistics
                if(!strcmp(argv[i],"-S")){
                    getStats(data,fileNum);  //function for finding the average and maximum values in a set, then saving it to a new file
                }
                //-C handling center the signal
                if(!strcmp(argv[i],"-C")){
                    center(data,fileNum);   //centers the data and saves it to a new file
                }
                //-N handling normalizes the signal between 1 and 0
                if(!strcmp(argv[i],"-N")){
                    normalize(data,fileNum); 		//normalizes the data to be between 0 and 1 then saves the new data to a file
                }
        }

	return 0;
}

void help(void){
    printf("\nThis program uses command line arguments for control. Each command is began with a `-` followed"
    " by a letter, then a space and an additional parameter if needed.  Here are the options:"
    "\n\n-n: specifies the file number and MUST be directly followed by a number"
    "\n-o: calls the offset function and must be followed by the desired value of offset"
    "\n-s: calls the scale function and must be followed by the desired value of scaling"
    "\n-r: allows user to rename a file and must be followed by a string"
    "\n-h: displays this help message"
    "\n-S: performs automated statistics with no additional parameter needed"
    "\n-C: centers the data provided with no additional parameter needed"
    "\n-N: normalizes the data to be between 0 and 1 with no additional parameter needed\n\n");
}

int* fileRead(int fileNum){
    int i;
    int max = 0;
    int size = 0;
    char fileName[50] = {};

    sprintf(fileName, "Raw_data_%02d.txt", fileNum);
	 
    FILE *fp;
    fp = fopen(fileName,"r");
    if(fp == NULL){
        return 0;
        printf("The file could not be found!");
    }
    fscanf(fp,"%d %d",&size,&max);

	//dynamic allocation of the input data
    int *data = (int*)malloc(sizeof(int)*(size+1));
    
	//populates the data set from the file
    data[0] = size;        
    for(i = 1; i < size+1; i++){
		fscanf(fp,"%d",&data[i]);
    }

    rewind(fp);
    fclose(fp);

    return data;	
}

void fileWrite(char fileName[], double set[],int size){
    int i;
	
	//opens or creates the appropriate file
    FILE *fp = fopen(fileName, "w+");
	//checks for file creation failure
    if(fp == NULL){
        return;
        printf("The file could not be found!");
    }

	//populates the new file
    fprintf(fp,"%lf %lf\n",set[0],set[1]);

    for(i = 2; i < size+2; i++){
	fprintf(fp, "%lf\n",set[i]);
    }

    rewind(fp);
    fclose(fp);
}

double* offset(double off, int fileNum, int data[]){
    int i,j;
    double size = (double)data[0];
    char fileName[30] = {};
     
	//dynamic allocation of the offset data space
    double *set = (double*)malloc(sizeof(double)*(size+2));

	//populates set
    set[0] = size;
    set[1] = off;
    j = 1;
    for(i = 2; i < size+2; i++){
        set[i] = (double)data[j] + off;  //offsets the input data 
        j++;
    }

    return set;
}

double* scale(double scal, int fileNum, int data[]){
    int i,j;
    double size = (double)data[0];
    char fileName[30] = {};
        
	//dynamic allocation of the scaled data space
    double *scaled = (double*)malloc(sizeof(double)*(size+2));     

	//populates scaled
    scaled[0] = size;
    scaled[1] = scal;
    j = 1;
    for(i = 2; i < size+2; i++){
	scaled[i] = (double)data[j] * scal;		//scales the input data
        j++;
    }
    
    return scaled;
}

//modular function for calculating the average of a data set
double average(int data[]){
    double size = (double)data[0];
    int i;
    double sum = 0;
    double avg = 0;
    
    for(i=1; i < size+1; i++){
        sum = sum + data[i];
    }
    avg = sum / size;
    
    return avg;
}

//modular function for finding the largest value present in a data set
double max(int data[]){
    double size = (double)data[0];
    int i;
    double max = 0;
    
    for(i=1; i < size+1; i++){
        if(max < data[i])
            max = data[i];
    }
    
    return max;
}

void getStats(int data[], int fileNum){
    double avg = 0;
    double maximum = 0;
    char fileName[30] = {0};
    
	//calls the average and maximum functions
    avg = average(data);
    maximum = max(data);
    sprintf(fileName,"Statistics_data_%02d.txt", fileNum);
    
	//opens or creates a save file
    FILE *fp = fopen(fileName, "w+");
    if(fp == NULL)
        return;
    
	//prints the average then the maximum to a file
    fprintf(fp,"%lf %lf\n",avg, maximum);
    
    rewind(fp);
    fclose(fp);
    
}

void center(int data[], int fileNum){
    double avg = average(data);
	//uses offset to center the data based on the sets average
    double* set = offset(avg,fileNum,data);
    char fileName[30] = {};
    double size = (double)data[0];
    
	//generates file name and calls the function to save data to the file
    sprintf(fileName, "Centered_data_%02d.txt", fileNum);
    fileWrite(fileName,set,size);
}

void normalize(int data[], int fileNum){
    int max = 0;
    int size = 0;
    char fileName[50] = {};
    char fileName1[50] = {};
    double* set;

	//gets the largets possible value of a data set
    sprintf(fileName, "Raw_data_%02d.txt", fileNum);
    FILE *fp;
    fp = fopen(fileName,"r");
    if(fp == NULL)
        return;
    fscanf(fp,"%d %d",&size,&max);
    
	//uses scale to normalize the data between 0 and 1 by scaling every value by 1/max where max is the largest possible value, not the largest value present
    double param = (1.0/max);
    set = scale(param,fileNum,data);
    
	//generates file name and calls the function to save data to the file
    sprintf(fileName1, "Normalized_data_%02d.txt", fileNum);
    fileWrite(fileName1,set,size);
}
