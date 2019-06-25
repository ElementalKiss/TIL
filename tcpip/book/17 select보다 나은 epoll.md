# 17 select보다 나은 epoll

## 17-1 epoll의 이해와 활용

* select는 오래 전 개발된 멀티플랙싱 기법으로 허용할 수 있는 동접수가 100을 넘기 어렵다.(하드웨어 성능에 따라 편차가 크다.)
* 오늘날 개발환경에 적절치 않고 그 대안으로 epoll을 사용.

### select기반의 IO 멀티플렉싱이 느린 이유

* select 함수호출 이후에 항상 등장하는 모든 파일 디스크립터를 대상으로 하는 반복문
* select 함수를 호출할 때마다 인자로 매번 전달해야하는 관찰대상에 대한 정보들
* select 함수는 소켓(운영체제가 관리)의 변화를 관찰하는 함수이기때문에 매번 운영체제의 도움을 받아야 한다.
* 위의 select의 단점을 해결하는 방법
    * 운영체제에게 관찰대상에 대한 정보를 딱 한번만 알려준다.
    * 관찰대상의 범위, 내용에 변경이 있을 때 변경 사항만 알려주도록 한다.
    * 단, 운영체제가 이 방식에 동의할 경우. 즉 그 기능을 지원하는지가 중요하다. 리눅스는 epoll 윈도우는 IOCP

### select를 사용처와 장점

* 서버의 접속자 수가 적다.
* 다양한 운영체제에서 운영이 가능하다.

### epoll의 구현에 필요한 함수와 구조체

* 상태변화의 확인을 위한 전체 파일 디스크립터를 대상으로 하는 반복문이 필요 없다.
* select 함수에 대응하는 epoll_wait 함수호출 시, 관찰대상의 정보를 매번 전달할 필요가 없다.

```cpp
epoll_create // epoll 파일 디스크립터 저장소 생성
epoll_ctrl   // 저장소에 파일 디스크립터 등록 및 삭제
epoll_wait   // select 함수와 마찬가지로 파일 디스크립터의 변화를 대기
```

### epoll_create

```cpp
#include <sys/epoll.h>
int epoll_create(int size);
// 성공 시 epoll 파일 디스크립터, 실패 시 -1 반환
```

* epoll_create 함수호출 시 생성되는 파일 디스크립터의 저장소를 가리켜 'epoll 인스턴스'라 한다.
* 그러나 변형되어서 다양하게 불리고 있으니 주의.
* size는 epoll 인스턴스가 생성될 크기가 아닌 운영체제가 참고할 힌트 역할만 한다.
* 해당 함수로부터 반환되는 파일 디스크립터는 운영체제에 의해 관리된다.
* 반환하는 파일 디스크립터는 epoll 인스턴스를 구분하는 목적으로 사용되며 소멸 시에 close 함수호출을 통한 종료 과정이 필요하다.

### epoll_ctl

```c
#include <sys/epoll.h>
int epoll_ctl(int epfd, int op, int fd, struct epoll_event* event);
// 성공 시 0, 실패 시 -1
/*
epfd  관찰대상을 등록할  epoll 인스턴스의 파일 디스크립터.
op    관찰대상의 추가, 삭제 또는 변경여부 지정.
fd    등록할 관찰대상의 파일 디스크립터.
event 관찰대상의 관찰 이벤트 유형.
*/
```



### epoll_wait

```c
#include <sys/epoll.h>
int dpoll_wait(int epfd, struct epoll_event* events, int maxevents, int timeout);
// 성공 시 이벤트가 발생한 파일 디스크립터의 수, 실패 시 -1
/*
events      이벤트가 발생한 파일디스크립터가 채워질 버퍼의 주소 값.
maxevents   두 번째 인자로 전달된 주소 값의 버퍼에 등록 가능한 최대 이벤트 수.
timeout     ms 단위 대기시간. -1 전달 시, 이벤트 발생까지 무한 대기.
*/
```