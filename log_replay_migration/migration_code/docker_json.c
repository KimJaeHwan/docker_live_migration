#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include "parson.h"

#define BUFFSIZE 80000

int search_str(char * str,char * fin);
int container_name(char * str, const char * docker_container);
//void chang_exe(char * str,char * ex, char *ret);		// BUFF, con_ex

int main( int argc, char * argv[])
{
	JSON_Value *rootValue;
	JSON_Object *rootObject;
	int i,index,temp,j;
	char file_read[BUFFSIZE];
	char BUFF[BUFFSIZE];
	char buff[BUFFSIZE];
	char tempc[BUFFSIZE];
	FILE * rfp;
    FILE * wfp; 
	
	char con_ex[17];
	if(argc != 2){
		printf("Usage : %s <json.log>\n",argv[0]);
		exit(1);
	}
	rfp = fopen(argv[1],"rt");
    wfp = fopen("dockerfile","wt");
	strcpy(tempc,"FROM ubuntu:latest\n");
	fputs(tempc,wfp);

	
	container_name(con_ex,basename(argv[1]));
	printf("%s]\n",con_ex);


	while(fgets(file_read,BUFFSIZE,rfp) != NULL){	// log파일 한줄만 read후json파일로 변경
	
		//printf("test1\n");
		rootValue = json_parse_string(file_read);
		rootObject = json_value_get_object(rootValue);
		
		//printf("test2\n");
		strcpy(BUFF,json_object_get_string(rootObject,"log"));
		
		//printf("test3 %d\n",strlen(json_object_get_string(rootObject,"log")));
		index =search_str(BUFF ,con_ex);
		//printf("index : %d\n",index);


		if(index){
			//printf("%s\n",BUFF + index);
			while(temp = search_str(BUFF + index,con_ex)){
				index += temp;
			};
			
			while(temp = search_str(BUFF + index,"# ")){
				index += temp;	
			};
			//printf("log : %s\n",BUFF + index);
			strcpy(buff,BUFF + index + 1);
			printf("log : %s\n",buff);
			
			for(j = 0,i = 0; buff[i]; i++){
			//	printf("[%c - %d]",buff[i],buff[i]);
				
				if(buff[i] == 7)	// 경고음 발생 시퀀스
					continue;
				else if(buff[i] == 8){	// 한칸 뒤로 이동 시퀀스
					j--;
				}else if(buff[i] == 13){// 캐리지 리턴
					BUFF[j] = 0;
				}else if(buff[i] == 10){// 줄바꿈
					BUFF[j] = 0;
				}else if(buff[i] == 27){
					i += 2;
				}
				else{
					BUFF[j] = buff[i];
					j++;
				}
				
				BUFF[j] = 0;
			}
			printf("\n");

			printf("command : [%s]\n",BUFF);
			if(strcmp(BUFF,"exit") && strcmp(BUFF,"test"))	// don't  push message that if "exit" command 
			{
				sprintf(buff,"RUN %s\n",BUFF);
				fputs(buff,wfp);
			}

			memset(&BUFF,0,BUFSIZ);

		}
		
		
		json_value_free(rootValue);
	}
	fclose(wfp);
	fclose(rfp);
	return 0;
}
int search_str(char * str,char * fin)
{
	int i,j;
	for(j = 0,i = 0; str[i]; i++)
	{
		if(str[i] == fin[j])
		{
			j++;
			if(fin[j] == 0)
				return i;
		}
		else
			j = 0;
	}
	return 0;
}
int container_name(char * str, const char * docker_container)
{
	int i;
	for(i = 0; i < 12; i++)
	{
		str[i] = docker_container[i];
	}
	str[i++] = ':';
	str[i++] = '/';
	str[i] = 0;
	/*
	str[i++] = '#';
	str[i++] = ' ';
	str[i] = 0;
	*/
}	
