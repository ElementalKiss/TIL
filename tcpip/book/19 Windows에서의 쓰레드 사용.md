# 19 Windows에서의 쓰레드 사용

## 19-1 커널 오브젝트

### 커널 오브젝트란?

* 운영체제에 의해 생성되는 리소스들은 관리 목적으로 정보를 기록하는 내부 데이터 블록을 생성.
* 이 데이터 블록을 가리켜 '커널 오브젝트'라 한다.
* 프로세스, 쓰레드, 세마포어 등 각각의 관리를 위한 커널 오브젝트를 생성한다.

### 커널 오브젝트의 소유자는 운영체제이다.

* 커널 오브젝트의 생성, 관리 그리고 소멸시점을 결정하는 것까지 모두 운영체제의 몫

## 19-2 윈도우 기반의 쓰레드 생성

### 프로세스와 쓰레드의 관계

* 과거 main 함수 호출 주체가 프로세스라고 했던 적도 있었으나 이제는 쓰레드의 개념이 확립되면서 최소 단위는 쓰레드가 되었다.
* 쓰레드를 별도로 생성하지 않는 프로그램은 '단일 쓰레드 모델의 프로그램'으로 정의한다.
* 쓰레드를 별도로 생성한다면 '멀티 쓰레드 모델의 프로그램'

### 윈도우에서의 쓰레드 생성방법

```cpp
#include <windows.h>

HANDLE CreateThread(
    LPSECURITY_ATTRIBUTS lpThreadAttributes,
    SIZE_T dwStackSize,
    LPTHREAD_START_ROUTINE lpStartAddress,
    LPVOID lpParameter,
    DWORD dwCreationFlags,
    LPDWORD lpThreadId
);
/*
lpThreadAttributes  : 쓰레드의 보안관련 정보전달, 디폴트 보안설정을 위해서 NULL 전달
dwStackSize         : 쓰레드에게 할당할 스택의 크기 전달, 0전달하면 디폴트 크기의 스택 생성
lpStartAddress      : 쓰레드의 main 함수정보 전달
lpParameter         : 쓰레드의 main 함수호출 시 전달할 인자정보 전달.
dwCreationFlags     : 쓰레드 생성 이후의 행동을 결정, 0을 전달하면 생성과 동시에 실행 가능한 상태가 된다.
lpThreadId          : 쓰레드 ID 저장을 위한 변수의 주소 값 전달
*/
```

* 복잡하지만 lpStartAddress와 lpParameger 두 가지 정도만 관심을 가지고 보면 된다.
* 나머진 대부분 0이나 NULL을 전달함녀 됨.

### 멀티 쓰레드 기반의 프로그램 작성을 위한 환경설정

* 프로젝트 세팅에서 '런타임 라이브러리' 부분에 멀티 쓰레드 관련 설정이 있음.
* 해당 부분을 멀티 쓰레드 DLL을 사용하게끔 변경.

### 쓰레드에 안전한 C 표준함수의 호출을 위한 쓰레드 생성

```cpp
uintptr_t _beginthreadex(
    void* security,
    unsigned int stackSize,
    unsigned int(*startAdrdress)(void* ),
    void* argList,
    unsigned int initFlag,
    unsigned int* thrdAddr
);
```
## 19-3 커널 오브젝트의 두 가지 상태

* 종료된 상태를 가리켜 signaled 상태
* 종료되지 않은 상태를 가리켜 non-signaled 상태

### 커널 오브젝트의 상태와 상태의 확인

* 두 가지에 크게 관심을 가진다.
    * 프로세스가 언제 종료 되는가?
    * 쓰레드가 언제 종료되는가?
* '이 커널 오브젝트는 현재 signaled 상태인가?' 해당 질문을 위해 정의된 함수가 WaitForSingleObject, WaitForMultipleObject.

### WaitForSingleObject & WaitForMultipleObjects

```cpp
#include <windows.h>

DWORD WaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds);
/*
    hHandle: 상태 확인의 대상이 되는 커널 오브젝트의 핸들을 전달.
    dwMillisconds: 1/1000초 단위로 타임아웃을 지정, 인자로 INFINITE 전달 시, 커널 오브젝트가 signaled 상태가 되기 전에는 반환하지 않음.
    recturn value: signaled 상태로 인한 반환시, WAIT_OBJECT_0 반환, 타임아웃으로 반환 시 WAIT_TIME_OUT 반환
*/
```

* 이 함수는 이벤트 발생에 의해 반환되면 다시 non-signaled 상태로 되돌리기도 한다.
* 다시 되돌리는 커널 오브젝트를 auto-reset 모드 커널 오브젝트라 부른다.
* 자동으로 되지 않은 오브젝트를 가리켜 manual-reset 모드 커널 오브젝트라 한다.

```cpp
#include <windows.h>

DWORD WaitForMultipleObject(DWORD nCount, const HANDLE* lpHandles, BOOL bWaitAll, DWORD dwMilliseconds);

/*
nCount: 검사할 커널 오브젝트의 수 전달.
lpHAndles: 핸들정보를 담고 있는 배열의 주소 값 전달.
bWaitAll: true 전달 시, 모든 검사대상이 signaled 상태가 되어야 반환. false 전달 시, 검사 대상 중 하나라도 signaled 상태가 되면 반환.
*/
```

* 둘 이상의 커널 오브젝트를 대상으로 상태를 확인하는 경우 호출