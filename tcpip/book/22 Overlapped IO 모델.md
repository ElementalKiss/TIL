# 22 Overlapped IO 모델

## 22-1 Overlapped IO 모델의 이해

* 21장의 비동기로 처리되던 것은 IO가 아닌 Notification이다.
* IO로 비동기 처리하는 방법을 알아본다.
* 이 둘의 차이를 명확하게 구분할 수 있어야 IOCP를 이해할 수 있다.

### IO(입출력)의 중첩이란?

* 비동기 IO 모델이 사실상 Overlapped IO나 다름없다.
* 하나의 쓰레드 내에서 동시에 둘 이상의 영역으로 데이터를 전송하는 상황을 가리켜 'IO 중첩'이라 한다.
    + 이런 일이 가능하려면 호출된 입출력 함수가 바로 반환해야 한다.
    + 두 번째, 세 번째 데이터 전송을 시도해야 하기 떄문에 결과적으로 위의 모델의 핵심이 되는 사항은 '비동기 IO'이다.
    + 비동기 IO가 가능하려면 호출되는 입출력 함수는 넌-블로킹 모드로 동작해야 한다.

### 이번 Chapter에서 말하는 Overlapped IO의 포커스는 IO에 있지 않습니다.

* 윈도우에서 말하는 Overlapped IO의 포커서는 IO가 아닌, IO가 완료된 상황의 확인 방법에 있다.
* 입출력 간에 넌-블로킹 모드로 진행된다면, 이후에 완료 결과를 확인해야 한다.
* 즉 Overlapped IO에 대한 입출력만을 알아야되는 것이 아니라 완료를 확인하는 방법까지 알아야된다.

### Overlapped IO 소켓의 생성

```cpp
#include <winsock2.h>

SOCKET WSASocket(int af, int type, int protocol, LPWSAPROTOCOL_INFO lpProtocolInfo, GROUP g, DWORD dwFlags);
// 성공 시 소켓의 핸들, 실패 시 INVALID_SOCKET 반환
/*
af: 프로토콜의 체계 전달
type: 소켓의 데이터 전송방식에 대한 정보 전달
protocol: 두 소켓 사이에 사용되는 프로토콜 정보 전달
lpProtocolInfo: 생성되는 소켓의 특성 정보를 담고 있는 WSAPROTOCOL_INFO 구조체 변수의 주소 값 전달, 필요 없는 경우 NULL 전달.
g: 함수의 확장을 위해서 예약되는 매개변수, 따라서 0 전달
dwFlags: 소켓의 속성정보 전달
*/
```

* 마지막 매개변수는 WSA_FLAG_OVERLAPPED를 전달해서 생성되는 소켓에 Overlapped IO가 가능한 속성을 부여한다.

```cpp
WSASocket(PT_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
```

### Overlapped IO를 진행하는 WSASend 함수

* 두 소켓간의(서버, 클라이언트) 연결 과정은 일반 소켓의 연결과정과 차이가 없다.
* 하지만 데이터의 입출력에 사용되는 함수는 달리해야 한다.

```cpp
#include <winsock2.h>

int WSASend(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount,
            LPDWORD lpNumberOfBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped,
            LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
// 성공 시 0, 실패 시 SOCKET_ERROR 반환
```

* 위 함수의 두 번째 인자로 들어가는 구조체 __WSABUF

```cpp
typedef struct __WSABUF
{
    u_long len;     // 전송할 데이터의 크기
    char FAR* buf;  // 버퍼의 주소 값
} WSABUF, *LPWSABUF;
```

* 함수의 호출 형태 보기

```cpp
WSAEVENT event;
WSAOVERLAPPED overlapped;
WSABUF dataBuf;
char buf[BUF_SIZE] = {"data"};
int recvBytes = 0;
// ...
event = WSACreateEvent();
overlapped.hEvent = event;
dataBuf.len = sizeof(buf);
dataBuf.buf = buf;
WSASend(hSocket, &dataBuf, 1, &recvBytes, 0, &overlapped, NULL);
// ...
```

* WSASend 함수의 세 번째 인자가 1인 이유는 두 번째로 전달된, 전송할 데이터가 하나이기 떄문이다.
* 여섯 번째 인자로 전달된 구조체 WSAOVERLAPPED의 정의

```cpp
typedef struct _WSAOVERLAPPED
{
    DWORD Internal;
    DWORD InternalHigh;
    DWORD Offset;
    DWORD OffsetHigh;
    WSAEVENT hEvent;
} WSAOVERLAPPED, *LPWSAOVERLAPPED;
```

* Internal, InternalHigh는 운영체제 내부적으로 사용되는 멤버
* Offset, OffsetHigh 역시 사용이 예약되는 멤버
* 그렇기 때문에 실제로 관심을 둘 멤버는 hEvent가 전부다.
* Overlapped IO를 진행하려면 lpOverlapped에는 항상 NULL이 아닌 유효한 구조체 변수의 주소 값을 전달해야 한다. NULL이면 블로킹 모드로 동작하는 일반 소켓으로 간주된다.
* WSASend 함수호출을 통해 동시에 둘 이상의 영역으로 데이터 전송하는 경우 WSAOVERLAPPED 구조체 변수를 각각 별도로 구성해야 한다. 이는 구조체 변수가 진행 과정에서 운영체제에 의해 참조되기 떄문이다.

### WSASend, WSARecv 함수에 관련해서 한가지 더!

* WSASend 함수가 호출되자마자 반환하는데, 어떻게 전송된 데이터의 크기가 저장되나요?
    + WSASend 함수라고 무조건 반환과 전송완료 시간이 불일치 하는 것은 아니다.
    + 이런 경우 WSASend함수가 0을 반환하고, 매개변수 lpNumberOfBytesSent로 전달된 주소의 변수에는 실제 전송된 데이터의 크기정보가 저장된다.
* WSA가 반환을 한 다음에도 계속해서 데이터의 전송이 이뤄지는 상황이라면?
    + 확인 가능한 오류코드로 WSA_IO_PENDING이 등록되고 WSAGetOverlappedResult 함수로 실제 전송된 데이터의 크기를 확인 해야 한다.
* WSARecv는 기능적으로 데이터를 수신하는냐에 대한 차이만 있고 나머지는 동일하다.

## 22-2 Overlapped IO에서의 입출력 완료의 확인
### Event 오브젝트 사용하기

* IO가 완료되면 WSAOVERLAPPED 구조체 변수가 참조하는 Event 오브젝트가 signaled 상태가 된다.
* IO의 완료 및 결과를 확인하려면 WSAGetOverlappedResult 함수를 사용한다.
* WSAGetLastError를 통해 오류 값을 가져오고 WSA_IO_PENDING이라면 WSASend 함수의 호출결과가 오류 상황이 아니라 완료되지 않는 상태임을 의미한다.

### Completion Routine 사용하기

* WSASend, WSARecv 마지막 전달인자를 통해 등록되는, Completion Routine가 있다.
* Pending된 IO가 완료되면, 이 함수를 호출해 줘!
    + 자동으로 호출될 함수를 등록하는 형태로 IO 완료 이후의 작업을 처리한다.
    + 운영체제: IO 요청한 쓰레드가 alertable wait 상태에 놓였을 때만 Completion Routine를 호출할게!
    + alertable wait란 운영체제가 전달하는 메시지의 수신을 대기하는 쓰레드의 상태를 뜻이다.
        + WaitForSingleObjectEx
        + WaitForMultipleObjectEx
        + WSAWaitForMultipleEvents
        + SleepEx
