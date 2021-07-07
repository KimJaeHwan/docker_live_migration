
![full-copy-migration-final-test](https://user-images.githubusercontent.com/45413267/117600984-086d9500-b188-11eb-81dd-93f43ee9d784.png)

![image](https://user-images.githubusercontent.com/45413267/117600976-0277b400-b188-11eb-9917-ca5f1734f153.png)

------

user -> contoller -> VM1 -> VM2 순서대로 메시지를 보내게 되며 controller로부터 신호를 받아 migration을 실행한다.

이때 VM1은 
docker commit c3ed1da9e4b6 nginx_docker_test
docker save -o nginx_docker_test.tar nginx_docker_test
명령어를 사용하여 현재 container를 이미지로 변환하고 tar형식으로 저장하는 과정을 실시하고 

저장된 tar파일을 소켓 프로그래밍을 이용하여 VM2에 전송한다.

VM2에서 VM1에서 보낸 이미지.tar파일을 받은후 
docker load -i nginx_docker_test.tar
docker run -d -p 88:80 nginx_docker_test
명령어를 이용하여 해당 이미지를 이용하여 서비스를 올린다.

실행 결과로 VM1(.142)에 존재하던 nginx 전체를 VM2(.141)로 옮겨 실행시킨 모습이며 제대로 나온것을 확인할수 있다.

-------

##실행순서

1. controller.o &lt;port&gt;
2. user.o &lt;controller IP&gt; &lt;controller port&gt;
3. VM1.o &lt;controller IP&gt; &lt;controller port&gt;
4. VM2.o &lt;controller IP&gt; &lt;controller port&gt;

  모든 가상환경이 빌드되면 user 명령어 라인에서 시작 여부를 묻게되며 y입력으로 migration이 시작된다.
  
##확인
  
  migration되어지는 container는 migraion.rc 코드안에 지정되어있으며 원하는 container ID로 변경해야한다.<br>
  VM1과 VM2사이의 연결은 각 소스코드내에 IP와 port가 하드코딩되어있다.
