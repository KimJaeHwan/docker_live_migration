#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parson.h"

#define BUFFSIZE 80000

int search_str(const char * str,const char * fin);
void chang_exe(char * str,const char * ex, char *ret);		// BUFF, con_ex

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
	const char con_id[13] = "6a7d77816094";
	const char con_ex[] = "6a7d77816094:/# ";
	if(argc != 2){
		printf("Usage : %s <json.log>\n",argv[0]);
		exit(1);
	}
	rfp = fopen(argv[1],"rt");
        wfp = fopen("dockerfile_test","wt");
		strcpy(tempc,"FROM ubuntu:latest\n");
		fputs(tempc,wfp);

	while(fgets(file_read,BUFFSIZE,rfp) != NULL){	// log파일 한줄만 read후json파일로 변경
	
		//printf("test1\n");
		rootValue = json_parse_string(file_read);
		rootObject = json_value_get_object(rootValue);
		
		//printf("test2\n");
		strcpy(BUFF,json_object_get_string(rootObject,"log"));
		
		//printf("test3 %d\n",strlen(json_object_get_string(rootObject,"log")));
		index =search_str(BUFF ,con_id);
		//printf("index : %d\n",index);


		if(index){
			//printf("%s\n",BUFF + index);
			while(temp = search_str(BUFF + index,con_ex)){
				index += temp;
			};
			//printf("log : %s\n",BUFF + index);
			strcpy(buff,BUFF + index + 1);
			//printf("log : %s\n",buff);
			
			for(j = 0,i = 0; buff[i]; i++){
				printf("[%c - %d]",buff[i],buff[i]);
				
				if(buff[i] == 7)	// 경고음 발생 시퀀스
					continue;
				else if(buff[i] == 8){	// 한칸 뒤로 이동 시퀀스
					j--;
				}else if(buff[i] == 13){// 캐리지 리턴
					tempc[j] = 0;
				}else if(buff[i] == 10){// 줄바꿈
					tempc[j] = 0;
				}
				else{
					tempc[j] = buff[i];
					j++;
				}
				
				tempc[j] = 0;
			}
			printf("\n");

			printf("control : [%s]\n",tempc);
			sprintf(buff,"RUN %s\n",tempc);
			fputs(buff,wfp);
			memset(&tempc,0,BUFSIZ);

		}
		
		
		json_value_free(rootValue);
	}
	fclose(wfp);
	fclose(rfp);
	return 0;
}

void chang_exe(char * str,const char * ex, char *ret){		// BUFF, con_ex
	int index = 0;
	int temp;
	strcpy(ret,str);
	while(temp = search_str(ret + index,ex))
	{
		index += temp;
	};
	ret += index;
}

int search_str(const char * str,const char * fin)
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
