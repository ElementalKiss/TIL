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

(작성중)

### WSASend 함수에 관련해서 한가지 더!
### Overlapped IO 진행하는 WSARecv 함수

## 22-2 Overlapped IO에서의 입출력 완료의 확인
### Event 오브젝트 사용하기
### Completion Routine 사용하기