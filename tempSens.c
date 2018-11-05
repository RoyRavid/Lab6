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
 int highTemp = 0;
 int lowTemp = 100000; 
 char id[100];
 int ctemp = 0;
 char commandstr[1000];
 char htstr[10];
 char ltstr[10];
 char ctstr[10];

 if (argc != 2)
 {
  printf("Wrong arg count\n");
  (void) exit(1);
 }
strcpy(id, argv[1]);
 
for(;;)
{
 int gotTemp = getTemp(id);
 if (gotTemp < lowTemp){lowTemp = gotTemp;}
 if (gotTemp > highTemp){highTemp = gotTemp;}
 if ((gotTemp - ctemp) > 1000 || (ctemp - gotTemp) > 1000)
 { 
  ctemp = gotTemp;
  printf("Temperature changed to %4.1f\n", (gotTemp/1000.0)); 

  sprintf(htstr, "%d", highTemp);
  sprintf(ltstr, "%d", lowTemp);  
  sprintf(ctstr, "%d", gotTemp);

  printf("Low is %d, high is %d\n", lowTemp, highTemp);
  sprintf(commandstr, "curl -X POST -H \"Content-Type: application/json\" -d \'{\" %s \":\"High\",\" %s \":\"Low\",\" %s \":\"Current\"}\' https://maker.ifttt.com/trigger/TempChanged/with/key/dx6eyL5uEw3ydlVK32hHSw" , ltstr, htstr, ctstr);
  system(commandstr);
  printf("\n");
  htstr[0] = '\0';
  ltstr[0] = '\0';
  ctstr[0] = '\0';
}
 sleep(5);
 
}
return 0;
}
