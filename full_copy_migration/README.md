
![full-copy-migration-final-test](https://user-images.githubusercontent.com/45413267/117600984-086d9500-b188-11eb-81dd-93f43ee9d784.png)

![image](https://user-images.githubusercontent.com/45413267/117600976-0277b400-b188-11eb-9917-ca5f1734f153.png)

------

user -> contoller -> VM1 -> VM2 순서대로 메시지를 보내게 되며 controller로부터 신호를 받아 migration을 실행한다.

실행 결과로 VM1(.142)에 존재하던 nginx 전체를 VM2(.141)로 옮겨 실행시킨 모습이며 제대로 나온것을 확인할수 있다.
