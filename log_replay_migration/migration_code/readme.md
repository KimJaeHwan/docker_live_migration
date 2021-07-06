# Log_replay_migration

docker는 컨테이너 별로 로그를 남기게 되며 해당로그는 기본적으로 json파일의 형태로 저장되어진다.
Log_replay방식에서는 위와 같이 남는 json파일 형식의 log들을 이용하여 migration을 하는 것을 목표로 한다.

![logreplay](https://user-images.githubusercontent.com/45413267/124140810-c7c53480-dac3-11eb-8b76-4ec0de53809a.png)

------

## docker log 의 특성

1. docker의 로그는 각 docker 이미지의 서비스에 따라 다른 로그가 남게된다. web서버의 경우 web서버의 접근로그, 일반 터미널의 경우 터미널 명령어들이 화면에 나오는 표쥰출력 로그가 남는다.
2. log의 디렉토리위치는 /var/lib/docker/containers 아래에 위치한다.
3. log는 json.log의 형태로 저장되어 있으며 1열당 하나의 json객체이다.
   1. json파일의 로그 내용들을 시간 순서대로 오름차순으로 정렬되어 있다.
   2. json파일의 로그 내용들은 사용자가 입력한 명령어만을 담고있는것이 아닌 shell환경의 표준출력되는 출력 정보를 가지고 있으므로 clear와 같이 shell화면에 영향을 주는경우 해당영향또한 log에 남게된다.
4. docker shell에 접근하는 방법은 docker attach &lt;container&gt; 또는 docker exec -it &lt;container&gt;  /bin/bash 두 가지가 존재하지만 exec 키워드를 이용한 docker shell접속은 별도의 터미널을 열어 접근하기에 명령어 로그가 남지 않는다. 따라서 log_replay_migration을 하기 위해서는 docker attach를 이용한 접근이 필수적이다.

------

## 동작과정

1. 사용자가 중앙 컨트롤러에 migration명령을 전송한다.

2. 중앙 컨트롤러는 source container(VM1)에서 destination container(VM2)로 migration하기위해 VM1에 신호를 전송한다.

3. 신호를 전송받은 VM1은 migration하고자하는 container의 log파일을 dockerfile로 파싱하여 VM2로 전송해준다.

   1. json형식으로 이루어진 log파일은 ANSI형식의 escape code를 전부 포함하고 있으므로 이를 감안하여 파싱을 진행한다.

   2. 테스트용 dockerfile 내용은 다음과 같다

      | FROM ubuntu:latest<br/>RUN apt-get update<br/>RUN apt-get install vim -y<br/>RUN exit |
      | ------------------------------------------------------------ |
      |                                                              |

4. 전송이 완료되면 VM2는 수신한 dockerfile을 이용하여 build를 한다.

5. 이미지 빌드가 완료되면 중앙 컨트롤러를 거쳐 사용자에게 migration의 완료를 알린다.
------

![image](https://user-images.githubusercontent.com/45413267/124644330-182a0100-decd-11eb-8f18-61f7767ee5bd.png)
위는 해당 동작 을 실행한 모습이다.
## 프로그램 실행
프로그램 실행은 각 실행파일 별로 controller, VM1, VM2, user 각각의 가상환경을 가진다고 가정한다.
1. ./controller.o &lt;controller port&gt;
2. ./user.o &lt;controller IP&gt; &lt;controller port&gt;
3. ./VM1.o &lt;controller IP&gt; &lt;controller port&gt;
   1. VM1과 VM2는 별도의 INET소켓으로 연결되어있으며 VM1 포트번호는 #define VM1_port 8888 로 코드상에 지정되어있다.
   2. VM1에는 dockfile을 만들기 위한 -json.log파일의 경로가 DOCKER_LOG_JSON에 코드상에 지정되어있으며 migration하고자 하는 컨테이너의 로그파일경로를 넣어주어야한다.
4. ./VM2.o &lt;controller IP&gt; &lt;controller port&gt;
   1. VM2 또한 VM1에 연결되어있으며 VM1에대한 IP와 port번호는 #define VM1_port 8888 , VM1_IP "192.168.139.142"로 코드 상에 지정되어있다.
5. 모든 프로그램을 실행시킨후 user.o 실행파일에 [Y/n] 으로 실행여부를 응답한다.
7. 이후 마이그레이션실행


## 주의사항

1. 동작과정 3-1을 고려하지 않고 파싱하는 경우 아래와 같이 제대로 dockerfile이 동작하지 못하는것을 확인할 수 있다.
   ![image](https://user-images.githubusercontent.com/45413267/124145126-7e76e400-dac7-11eb-87f2-49ac1237d7b9.png)
   ![image](https://user-images.githubusercontent.com/45413267/124145183-8a62a600-dac7-11eb-9fc8-700712554d1e.png)
   
2. dockerfile로 디렉토리 이동이불가능하다.
cd 명령어를 이용해서 이동하여도 이후 모든 명령어는 / 디렉토리에서 실행된다.
각 명령어라인들은 별개로 동작하는것을 알 수 있다.

3. 몇가지 명령어는 dockerfile의 진행을 막는다
   1.vim vi와 같은 텍스트 에디터로 터미널을 막는경우
   2.test와 같이 종료 상태를 나타내주는 명령어
   3.exit와 같은 쉘 탈출 명령어

