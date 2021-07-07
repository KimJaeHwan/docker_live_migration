# docker_live_migration

------

docker를 활용한 migration이다.

migration 방식으로 full-copy, log-replay 를 사용했다.


<table border="1">
  <tr><th colspan="2">위치별 프로세스명</th></tr>
  <tr><td width="100px">controller</td><td width="100px">VM1</td></tr>
  <tr><td>user</td><td>VM2</td></tr>
</table>

1. full-copy 
  - docker conatiner자체를 save하여 이미지 tar파일로 만들어 전송하는 것으로 현재 상태 시스템을 가장 정확하게 전송할 수 있으나 시간이 오래걸린다.
      | ![fullcopy_delay](https://user-images.githubusercontent.com/45413267/124786913-14f14c80-df83-11eb-8798-e2e6a209e1bc.PNG) |
      | ------------------------------------------------------------ |
      |해당 이미지는 ubuntu 이미지를 full-copy 한것으로 migraion이후 종료 까지 15초의 진행시간이 걸렸음을 알 수 있다.|


  
2. log-replay
  - docker는 자체적으로 컨테이너에 대한 log를 남기게되며 이를 이용하여 container를 다시 빌드하는 방식이다.
  - 해당 방식은 full-copy에 비해 전송되는 파일의 크기가 작아 빠르게 진행되나 log 커맨드 정보에 따라 불필요한 작업을 수행할 수도 있으며, 진행하지 못 할 수도 있다.
  
  | 시나리오 1번 |
  | ------------------------------------------------------------ |
  | ![100M_file](https://user-images.githubusercontent.com/45413267/124787788-bf696f80-df83-11eb-8d91-c6479a0e0341.PNG) |
  | full-copy와는 다르게 2초의 시간이 소요된것을 확인할 수 있다. |
  
  ------
  
  | 시나리오 2번 |
  | ------------------------------------------------------------ |
  | ![test_sleep](https://user-images.githubusercontent.com/45413267/124787676-a234a100-df83-11eb-96f8-fc407d522260.PNG) |
  | 불필요한 sleep 명령어로 인해 migration시간이 7초로 나타난것을 확인할 수 있다. |

