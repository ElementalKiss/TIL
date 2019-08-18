# 21 Asynchronous Notification IO 모델

## 21-1 비동기 Notificatino IO 모델의 이해

* '비동기'의 뜻은 일치하지 않음을 의미한다.

### 동기화된 입출력은 무엇을 말하는가?

* send 함수가 호출되면 데이터의 전송이 이뤄지고 전송이 완료된 후에야 반환이 이뤄진다.
* 정확히는 출력 버퍼로 데이터가 완전히 전송되어야 반환이 이뤄진다는 뜻.

### 비동기 입출력

* 입출력 함수의 반환시점과 데이터 송수신의 완료시점이 일치하지 않는 경우를 말한다.
* 예시: epoll 비동기 입출력

### 동기화 입출력의 단점과 비동기의 해결책

* 입출력이 진행되는 동안 호출된 함수가 반환을 하지 않아 CPU가 다른 일을 할 수 없다.
* 비동기 방식으로 다른 일을 할 수 있다면 CPU를 효율적으로 사용할 수 있고 동기 방식의 단점을 극복할 수 있다.

### 비동기 Notification 입출력 모델에 대한 이해

* 입력버퍼에 데이터가 수신되어서 데이터의 수신이 필요하거나, 출력버퍼가 비어서 데이터의 전송이 가능한 상태를 알림.
* 가장 대표적인 Notification IO 모델은 select.
* select 함수의 비동기 버전이 WSAEventSelect.
* 비동기 Notification IO는 IO 관찰을 명령하기 위한 함수호출, 실제 상태의 변화를 체크하는 함수호출이 분리되어있다.
* IO 관찰을 명령하고 다른 일을 열심히 하다가 이후에 상태의 변화가 실제로 있었는지 확인하는 것이 가능하다.

## 12-2 비동기 Notification IO 모델의 이해와 구현

* 두 가지 구현 방법: WSAEventSelect, WSAAsyncSelect

### WSAEventSelect 함수와 Notification

* IO의 상태변화를 알리는 것이 Notification이라고 했다.
* IO의 상태는 다음과 같이 달리 표현할 수 있음.
    + 소켓의 상태변환 - 소켓에 대한 IO의 상태변화
    + 소켓의 이벤트 발생 - 소켓에 대한 IO관련 이벤트 발생

```cpp
#include <winsock2.h>
int WSAEventSelect(SOCKET s, WSAEVENT hEventObject, long lNetworkEvents);
// 성공 시 0, 실패 시 SOCKET_ERROR 반환
/*
s: 관찰대상인 소켓의 핸들 전달.
hEventObject: 이벤트 발생유무의 확인을 위한 Event 오브젝트의 핸들 전달.
lNetworkEvents: 감시하고자 하는 이벤트의 유형 정보전달.
*/
```

* 즉, WSAEventSelect 함수는 매개변수 s에 전달된 핸들의 소켓에서 lNetworkEvent에 전달된 이벤트 중 하나가 발생하면
* hEventObject에 전달된 핸들의 커널 오브젝트를 signaled 상태로 바꾸는 함수이다.
* Event 오브젝트와 소켓을 연결하는 함수.
* WSAEventSelect 함수는 이벤트의 발생유무에 관계없이 바로 반환하기 때문에 이후 다른 작업을 진행할 수 있다.
* 세 번째 인자로 전달되는 이벤트의 종류
    + FD_READ: 수신할 데이터가 존재하는가?
    + FD_WRITE: 블로킹 없이 데이터 전송이 가능한가?
    + FD_OOB: Out-of-band 데이터가 수신되었는가?
    + FD_ACCEPT: 연결요청이 있었는가?
    + FD_CLOSE: 연결의 종료가 요청되었는가?
* select는 다수 소켓을 대상으로 호출되는데 WSAEventSelect는 단 하나의 소켓밖에 호출이 안된다
    + select 함수는 반환되고 나면 이벤트의 발생확인을 위해 또 다시 모든 핸들을 재호출 해야 한다.
    + WSAEventSelect는 함수호출을 통해 전달된 소켓의 정보가 운영체제에 등록되고 재호출이 불필요한 장점이 있다.
* 추가로 WSAEventSelect에 대해 알아야 되는 것.
    + WSAEventSelect 함수의 두 번째 인자전달을 위한 Event 오브젝트 생성방법
    + WSAEventSelect 함수호출 이후의 이벤트 발생 확인방법
    + 이벤트 발생이 확인된 경우, 발생된 이벤트의 유형을 확인하는 방법

### 'manual-reset 모드 Event 오브젝트'의 또 다른 생성방법

* CreateEvent 함수를 이용하면 auto-reset, manual-reset 두 모드 중 하나를 만들 수 있다.
* 하지만 해당 WSAEventSelect 시에는 manual-reset 모드면서 non-signaled 상태인 Event 오브젝트가 필요하므로 WSACreateEvent 함수를 사용하는 것이 여러모로 편리하다.

```cpp
#include <winsock2.h>
WSAEVENT WSACreateEvent(void);
```

* 반환형은 define된 WSAEVENT HANDLE. 커널 오브젝트의 핸들이 반환되는 것이고 이를 다른 유형의 핸들로 구분지으면 안된다.

```cpp
#include <winsock2.h>
BOOL WSACloseEvent(WSAEVENT hEvent);
```

* WSACreateEvnet로 생성된 오브젝트는 다음 함수를 통해 종료를 해야 한다.

### 이벤트 발생유무의 확인

* WSAEventSelect 함수호출 이후를 고민할 차례.
* 이벤트 발생 유무를 확인하는 함수가 있는데 WaitForMultipleObject와 매개변수 하나 많다는걸 제외하고는 같다.

```cpp
#include <winsock2.h>
DWORD WSAWaitForMultipleEvents(DWORD cEvents, const WSAEVENT* lphEvents, BOOL fWaitAll, DWORD dwTimeout, BOOL fAlertable);
```

* 단 한번의 호출로 signaled 상태로 전이된 Event 오브젝트의 핸들 정보를 모두 알 수 없다.
* 이 함수에서는 signaled 상태로 전이된 Event 오브젝트의 첫 번째 인덱스 값만 알 수 있다.
* 해당 Evnet 오브젝트가 manual-reset 모드기 떄문에 다음과 같은 방식으로 코드 작성이 가능하다.

~~~cpp
pos = WSAWaitForMultipleEvents(numOfSock, hEventArray, FALSE, WSA_INFINITE, FALSE);
startIndex = pos - WSA_WAIT_EVENT_0;

for (i = startIndex; i < numOfSock; ++i) {
    int sigEventIndex = WSAWaitForMultipleEvents(1, &hEventArray[i], TRUE, 0, FALSE);
}
~~~

* siganeld 상태로 놓인 첫 번쩌 Event 오브젝트로부터 for loop을 통해 마지막 오브젝트까지 signaled 상태로 전이 여부를 확인.
* Timeout을 0을 보냈기 때문에 호출과 동시에 반환한다. 이는 Event 오브젝트가 manual-reset 모드기떄문에 가능하다.

### 이벤트 종류의 구분

* 마지막으로 signaled 상태로 된 원인을 확인해야 한다.

```cpp
#include <winsock2.h>
int WSAEnumNetworkEvents(SOCKET s, WSAEVENT hEventObject, LPWSANETWORKEVENTS lpNetworkEvents);
```

* 해당 함수는 manual-reset 모드의 Event 오브젝트를 non-signaled 상태로 되돌리기 까지 한다. 별도의 ResetEvent를 호출할 필요가 없다.
* 구조체 WSANETWORKEVENTS

```cpp
typedef struct _WSANETWORKEVENTS
{
    long lNetworkEvents;
    int iErrorCode[FD_MAX_EVENTS];
} WSANETWORKEVENTS, *LPWSANETWORKEVENTS;
```

* WSAEventSelect 함수의 세 번째 인자로 전달되는 상수와 마찬가지로 수신할 데이터가 존재하면 FD_READ, 연결요청이 있는 경우 FD_ACCEPT.

~~~cpp
WSAEnumNetworkEvents(hSock, hEvent, &netEvents);
if (netEvents.lNetworkEvents & FD_ACCECPT) {
    // FD_ACCECPT 이벤트 핸들링
}

if (netEvets.lNetworkEvets & FD_READ) {
    // FD_READ 이벤트 핸들링
}

// ... 등등 이벤트 핸들링

if (netEvents.iErrorCode[FD_READ_BIT] != 0) {
    // FD_READ 이벤트 관련 오류 발생
}

// ... 등등 이벤트 오류 핸들링
~~~