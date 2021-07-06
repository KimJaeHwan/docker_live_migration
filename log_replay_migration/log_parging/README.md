#docker-json.logs 파일을 이용한 dockerfile 생성기

------

$./docker_json.o <json.log path>

![docker_json](https://user-images.githubusercontent.com/45413267/124608993-316d8600-deaa-11eb-8335-1403736ac7b6.PNG)
![image](https://user-images.githubusercontent.com/45413267/124609094-477b4680-deaa-11eb-9083-91d15568a190.png)

------
경로만 넣어주면 정상적으로 dockerfile로 만들어준다. 
현재 시스템은 dockerfile명을 dockefile_test로 하여 만들게되며 기본적으로 FROM ubuntu:latest이미지 dockerfile로 되어있다.
해당 이미지를 변경하기 위해서는 직접 코드를 수정해야한다.
