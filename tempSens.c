#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<fcntl.h>
#include<stdlib.h>
#include<string.h>

int getTemp(char* id)
{
 char filePath[100];
 int n;
 char buf[100];
 int temp;

 buf[0] = '\0';

 filePath[0] = '\0';
 strcat(filePath, "/sys/bus/w1/devices/");
 strcat(filePath, id);
 strcat(filePath, "/w1_slave");
 FILE* readingFile = fopen(filePath,"r");

 if (readingFile == (FILE*)NULL)
 {
  perror("File open error\n");
  (void) exit(1);
 }

 n = fread(buf, 1, 74, readingFile);
 buf[n] = '\0';

 char* tempPtr = strstr(buf, "t="); 
 temp = atoi(tempPtr + 2); 
 
 fclose(readingFile);
 return temp;

}

int main(int argc, char *argv[])
{
 
 char id[100];
 int ctemp = 0;
 
 if (argc != 2)
 {
  printf("Wrong arg count\n");
  (void) exit(1);
 }
strcpy(id, argv[1]);
 
for(;;)
{
 int gotTemp = getTemp(id);
 if ((gotTemp - ctemp) > 1000 || (ctemp - gotTemp) > 1000)
 { 
  ctemp = gotTemp;
  printf("Temperature changed to %d\n", gotTemp); 
  system("curl -X POST https://maker.ifttt.com/trigger/{TempChanged}/with/key/dx6eyL5uEw3ydlVK32hHSw ");
  printf("\n");
}
 sleep(5);
 
}
return 0;
}
