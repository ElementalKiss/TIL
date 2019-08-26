# 23 IOCP(Input Output Completion Port

## 23-1 Overlapped IO를 기반으로 IOCP 이해하기

### 논의가 한참인 epoll과 IOCP의 성능비교

* select와 같은 전통적인 IO 모델의 한계를 극복하는 목적으로 운영체제 레벨에서 성능을 향상시킨 IO 모델들이 나왔다.
* 리눅스의 epoll, BSD의 kqueue 그리고 윈도우의 IOCP.
* 이들의 공통점은 운영체제에 의해 기능 지원 및 완성이 된다는 것.
* 성능 비교는 어렵다. 뭐가 좋다고 판단하기는 쉽지 않다.

### 넌-브로킹 모드의 소켓 구성하기

```cpp
SOCKET hLisnSock, int mode = 1;
hLisnSock = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
ioctlsocket(hLisnSock, FIONBIO, &mode); // for non-blocking socket
```

* ioctlsocket 함수는 소켓의 IO방식을 컨트롤하는 함수이다.
* 핸들 hLisnSock이 참조하는 소켓의 입출력 모드(FIONBIO)를 변수 mode에 저장된 값의 형태로 변경한다.
* FIONBIO는 소켓의 입출력 모드를 변경하는 옵션. 0이면 넌-블로킹 모드이다.
    + 이외에도 다음 특징을 가지는데, 클라이언트의 연결요청이 존재하지 않는 상태에서 accept가 불리면 INVALId_SOCKET을 곧바로 반환한다.
    + 이어서 WSAGetLastError 함수를 호출하면 WSAEWOULDBLOCK가 반환된다.
    + accept 함수호출을 통해 새로 생성되는 소켓 역시 넌-블로킹 속성을 지닌다.
* 따라서 넌-블로킹 입출력 소켓을 대상으로 accept 함수를 호출해서 INVALID_SOCKET가 반환되면, WSAGetLastError 함수의 호출을 통해 INVALID_SOCKET의 이유를 확인하고 적절한 처리를 해야 한다.

### Overlapped IO만 가지고 에코 서버 구현하기

* 클라이언트가 연결되면 WSARecv 함수를 호출하면서 넌-블로킹 모드로 데이터가 수신되게 한다.
* 수신이 완료되면 ReadCompRoutine 함수가 호출되게 한다.
* ReadCompRoutine 함수가 호출되면 WSASend 함수를 호출하면서 넌-블로킬 모드로 데이터가 송신되게 한다.
* 송신이 완료되면 WriteCompRoutine 함수가 호출되게 한다.
* 입출력 완료 시 자동으로 호출되는 Completion Routine 내부로 클라이언트 정보(소켓과 버퍼)를 전달하기 위해 WSAOVERLAPPED 구조체의 멤버 hEvent를 사용한다.

### OVerlapped IO 모델에서 IOCP 모델로

* Overlapped IO 모델의 에코 서버가 지니는 단점
    + 넌-블로킹 모드의 accept 함수와 alertable wait 상태의 진입을 위한 SleepEx 함수가 번갈아 가며 반복 호출되는 것은 성능에 영향을 미칠 수 있다.
    + 즉, 연결요청 처리를 위한 accept 함수만 호출할 수 있는 상황도 아니고, Completion Routine 함수의 호출을 위해 SleepEx 함수만 호출할 수 있는 상황도 아니다.
    + 그래서 while 반복문을 통해 accpet는 넌-블로킹 모드로, SleepEx는 타임아웃을 짧게 지정해 돌아가며 반복 호출한다.

* 그러면 어떻게 하면 좋을까?
    + accpet 함수의 호출은 main 쓰레드가 직접 처리한다.
    + 별도의 쓰레드를 추가로 생성하여 클라이언트와의 입출력을 담당하게 한다.

* IOCP 과찰의 중점
    + 쓰레드에 너무 초점을 두지 말라.
    + 입력과 출력은 넌-블로킹 모드로 동작하는가?
    + 넌-블로킹 모드로 진행된 입력과 출력의 완료는 어떻게 확인하는가?

## 23-2 IOCP의 단계적 구현

### Completion Port 생성

* IOCP에서는 완료된 IO의 정보가 Completion Port 오브젝트라는 커널 오브젝트에 등록된다.
* 그냥 등록하는 것이 아니라 이렇게! '이 소켓을 기반으로 진행되는 IO의 완료 상황은 전부 CP 오브젝트에 등록해 주세요!'
* 이를 가리켜 '소켓과 CP 오브젝트와의 연결 요청'이라 한다.
    + Completion Port 오브젝트의 생성
    + Completion Port 오브젝트와의 소켓의 ㅇ녀결
* 이때 소켓은 반드시 Overlapped 속성이 부여된 소켓이어야 한다.

```cpp
#include <windows.h>
HANDLE CreateIoCompletionPort(
    HANDLE FileHandle, HANDLE ExistingCompletionPort, ULONG_PTR CompletionKey, DWORD NumberOfConcurrentThreads
); // 성공 시 CP 오브젝트의 핸들, 실패 시 NULL
/*
FileHAndle: CP 오브젝트 생성시에는 INVALID_HANDLE_VALUE를 전달
ExistingCompletionPort: CP 오브젝트 생성시에는 NULL 전달
CompletionKey: CP 오브젝트 생성시에는 0 전달.
NumberOfConcurrentThreads: CP 오브젝트에 할당되어 완료된 IO를 처리할 쓰레드의 수 전달. 0으로 전달하면 시스템의 CPU 개수가 동시 실행 가능한 쓰레드의 최대수로 지정.
*/

HANDLE hCpObject;
hCpObject = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 2);
```

### Completion Port 오브젝트와 소켓의 연결

```cpp
#include <windows.h>
HANDLE CreateIoCompletionPort(
    HANDLE FileHandle, HANDLE ExistingCompletionPort, ULONG_PTR CompletionKey, DWORD NumberOfConcurrentThreads
); // 성공 시 CP 오브젝트의 핸들, 실패 시 NULL
/*
FileHAndle: CP 오브젝트에 연결할 소켓의 핸들 전달.
ExistingCompletionPort: 소켓과 연결할 CP 오브젝트의 핸들 전달.
CompletionKey: 완료된 IO 관련 정보의 전달을 위한 매개변수
NumberOfConcurrentThreads: 어떠한 값을 전달하건, 이 함수의 두 번째 매개변수가 NULL이 아니면 그냥 무시된다.
*/

HaNDLE hCpObject;
SOCKET hSock;
CreateIoCompletionPort((HANDLE)hSock, hCpObject, (DWORD)ioInfo, 0);
```

* 이렇게 해당 함수가 호출된 이후부터는 hSock을 대상으로 진행된 IO가 완료되면 이에 대한 정보가 핸들 hCpObject에 해당하는 CP 오브젝트에 등록된다.

### Completion Port의 완료된 IO 확인과 쓰레드의 IO처리

```cpp
#include <windows.h>
BOOL GetQueuedCompletionStatus(
    HANDLE CompletionPort, LPDWORD lpNumberOfBytes, PULONG_PTR lpCompletionKey,
    LPOVERLAPPED* lpOverlapped, DWORD dwMillisecondes
); // 성공 시 TRUE, 실패 시 FALSE
/*
CompletionPort: 완료된 IO 정보가 등록되어 있는 CP 오브젝트의 핸들 전달.
lpNumberOfBytes: 입출력 과정에서 송수신 된 데이터의 크기정보를 저장할 변수의 주소 값 전달.
lpCompletionKey: CreateIoCompletionPort 함수의 세 번째 인자로 전달된 값의 저장을 위한 변수의 주소 값 전달.
lpOverlapped: WSASend, WSARecv 함수호출 시 전달하는 OVERLAPPED 구조체 변수의 주소 값이 저장될 변수의 주소 값 전달.
dwMilliseconds: 타임아웃 정보전달. 지정한 시간이 완료되면 FALSE 반환하면서 함수 빠져나감. INFINITE 전달하면 IO가 CP 오브젝트에 등록될 때까지 블로킹 상태에 있게 된다.
*/
```

* 세 번째와 네 번째 인자는 값을 얻기 위해서 추가된 매개변수이다.
    + 세 번째 인자를 통해 얻게 되는 것은 소켓과 CP 오브젝트의 연결을 목적으로 CreateIoCompletionPort 함수가 호출될 때 전달되는 세 번째 인자 값이다.
    + 네 번째 인자를 통해 얻게 되는 것은 WSASend, WSARecv 함수호출 시 전달되는 WSAOVERLAPPED 구조체 변수의 주소 값이다.
* IO에 어떻게 쓰레드를 할당해야 하나?
    + WSASend, WSARecv 함수를 호출하는 쓰레드는 우리가 직접 생성해야 한다.
    + 다만 이 쓰레드가 입출력의 완료를 위해 GetQueuedCompletionStatus 함수를 호출할 뿐이다.
    + GetQueuedCompletionStatus 함수는 어떠한 쓰레드라도 호출이 가능하지만 실제 IO의 완료에 대한 응답을 받는 쓰레드의 수는 CreateIoCompletionPort 함수호출 시 지정한 최대 쓰레드의 수를 넘지 않는다.

### IOCP가 성능이 좀더 나오는 이유!

* 넌-블로킹 방식으로 IO가 징행되기 때문에 IO 작업으로 인한 시간의 지연이 발생하지 않는다.
* IO가 완료된 핸들을 찾기 위해서 반복문을 구성할 필요가 없다.
* IO의 진행대상인 소켓의 핸들을 배열에 저장해놓고, 관리할 필요가 없다.
* IO의 처리를 위한 쓰레드의 수를 조절할 수 있다.