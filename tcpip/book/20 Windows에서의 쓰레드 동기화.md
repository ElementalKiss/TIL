# 20 Windows에서의 쓰레드 동기화

## 20-1 동기화 기법의 분류와 CRITICAL_SECTION 동기화

### 유저모드(User mode)와 커널 모드(Kernel Mode)

* 유저모드: 응용 프로그램이 실행되는 기본모드, 물리적인 영역으로 접근이 허용되지 않으며 접근할 수 있는 메모리의 영역에도 제한적이다.
* 커널모드: 운영체제가 실행될 때의 모드, 메모리뿐만 아니라, 하드웨어의 접근에도 제한이 따르지 않는다.
* 단순정의
    * 유저모드: 응용프로그램의 실행 모드
    * 커널모드: 운영체제의 실행 모드

* 두 가지 모드를 정의하는 이유는 무엇인가?
    * 안정성이 높다.
    * 유저모드 일때는 잘못된 포인터 연산을 통해 운영체제의 중요 데이터가 망가지는 일이 없게 메모리 영역을 보호한다.
* 쓰레드 같이 커널 오브젝트의 생성을 동반하는 리소스의 생성을 위해서는 다음 변환 과정을 기본적으로 거친다.
    * 유저모드 -> 커널모드 -> 유저모드
    * 첫 번째 변환은 리소스의 생성을 위한 것이고 두 번째 변환은 응용 프로그램의 나머지 부분을 이어서 실행하기 위함이다.
    * 커널 오브젝트와 관련된 모든 일은 커널모드에서 진행된다.
    * 모드 변환은 시스템이 부담이 되는 일이며 잦은 변환은 성능에 영향을 준다.

### 유저모드 동기화

* 정의: 운영체제의 도움 없이 응용 프로그램상에서 진행되는 동기화를 유저모드 동기화.
* 장점은 속도가 빠르다.
* 유저모드 동기화 기법엔 대표적으로 CRITICAL_SECTION 기반 동기화가 있다.

### 커널모드 동기화

* 유저모드 동기화에 비해 제공되는 기능이 더 많다.
* Dead-lock에 걸리지 않도록 타임아웃의 지정이 가능하다.

### CRITICAL_SECTION 기반의 동기화

* CRITICAL_SECTION 오브젝트라는 것을 생성하고 이를 동기화에 활용.
* 이는 커널 오브젝트가 아니고 임계영역 진입에 필요한 Key로 생가하면 된다.

```cpp
#include <windows.h>

void InitializeCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
void DeleteCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
void EnterCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
void LeaveCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
// lpCriticalSection: 함수에서 초기화나 해제, 소유권 획득이나 반납할 CRITICAL_SECTION 오브젝트 주소.
```

## 20-2 커널모드 동기화 기법

* 대표적인 커널모드 동기화 기법은 Event, Semaphore, Mutex가 있다.

### Mutex(Mutual Exclusion) 오브젝트 기반 동기화

* CS와 유사하게 Key에 비유할 수 있다.

```cpp
#include <windows.h>

HANDLE CreateMutex(LPSECURITY_ATTRIBUTES lpMutexAttributes, BOOL bInitialOwner, LPCTSTR lpName);
// 성공 시 Mutex 오브젝트의 핸들, 실해 시 NULL
// lpMutexAttribute: 보안관련 특성 정보 전달, 디폴트 보안설정을 위해 NULL 전달.
// bInitalOwner: TRUE 전달 시, 생성되는 Mutex 오브젝트는 이 함수를 호출한 쓰레드의 소유가 되면서 non-signaled 상태가 됨.
//              반면 FALSE 전달 시, 생성되는 Mutex 오브젝트는 소유자가 존재하지 않으며, signaled 상태로 생성된다.
// lpName: Mutex 오브젝트에 이름을 부여할 때 사용한다.
```

* Mutex 오브젝트는 소유자가 없는 경우에 signaled 상태가 된다.
* 이러한 특성을 이용해 동기화를 진행한다.

```cpp
#include <windows.h>

BOOL CloseHandle(HANDLE hObject);
// 성공 시 TRUE, 실패 시 FASLE 반환
// hObject: 소멸하고자 하는 커널 오브젝트의 핸들 전달
```

* 이 함수는 커널 오브젝트의 소멸 함수라서 Semaphore와 Event의 소멸에도 사용된다.

```cpp
#include <windows.h>

BOOL ReleaseMutex(HANDLE hMutex);
// 성공 시 TRUE, 실패 시 FALSE 반환
// hMutex: 반납할 Mutex 오브젝트의 핸들 전달
```

* Mutex는 소요되었을 때 non-signaled 상태가 되고, 반납되었을 때 signaled 상태가 되는 커널 오브젝트.
* 소유 여부 확인을 WaitForSingleObject 함수를 이용할 수 있음.
* WaitForSingleObject는 호출 후 블로킹 상태거나 반환된 상태가 되며 반활될 때는 자동으로 non-signaled 상태가 된다.

```cpp
WaitForSingleObject(hMutex, INFINITE);
// 임계영역
ReleaseMutex(hMutex);
```

### Semaphore 오브젝트 기반 동기화

* 세마포어 값(Semaphore Value)라 불리는 정수를 기반으로 동기화가 이뤄진다.
* 이 값이 0보다 작아질 수 없다는 특징을 가지고 있다.

```cpp
#include <windows.h>

HANDLE CreateSemaphore(LPSECURITY_ATTRIBUTES lpSemaphoreAttributes, LONG lInitaliCount, LONG lMaximumCount, LPCTSTR lpName);
// 성공 시 생성된 Semaphore 오브젝트의 핸들, 실패 시 NULL 반환
// lpSemaphoreAttributes: 보안관련 정보의 전달, 디폴트 보안설정으로 NULL
// lInitaliCount: 세마포어의 초기 값 지정. lMaximumCount보다 커선 안되고 0 이상이어야 한다.
// lMaximumCount: 최대 세마포어 값 지정. 1을 전달하면 세마포어 값이 0 또는 1이 되는 바이너리 세마포어가 구성.
// lpName: Semaphore 오브젝트에 이름을 부여할 때 사용.
```

* 세마포어 값이 0인 경우 non-signaled 상태가 되고, 0보다 큰 경우에 signaled 상태가 되는 특성을 이용해 동기화 진행.
* lMaximumCount에 3을 전달하면 세마포어의 최대 값이 3이고 세 개의 쓰레드가 동시에 임계영역에 진입하는 유형의 동기화도 가능하다.

```cpp
#include <windows.h>

BOOL ReleaseSemaphore(HANDLE hSemaphore, LONG lReleaseCount, LPLONG lpPreviousCount);
// 성공 시 TRUE, 실패 시 FALSE 반환
// hSemaphore: 반납할 Semaphore 오브젝트의 핸들 전달
// lReleaseCount: 반납은 세마포어 값의 증가를 의미, 이 매개변수를 통해 증가되는 값의 크기를 지정할 수 있음. 최대값을 넘어간다면 FALSE 반환.
// lpPreviousCount: 변경 이전의 세마포어 값 저장을 위한 변수의 주소 값.
```

```cpp
WaitForSingleObject(hSemaphore, INFINITE);
// 임계영역
ReleaseSemaphore(hSemaphore, 1, NULL);
```

### Event 오브젝트 기반 동기화

* 앞선 동기화 오브젝트와 두드러진 차이점이 있다.
* 생성과정에서 auto-reset 모드와 그렇지 않은 manual-reset 모드 중 하나를 선택할 수 있다는 점이다.

```cpp
#include <windows.h>

HANDLE CreateEvent(LPSECURITY_ATTRIBUTES lpEventAttributes, BOOL bManualReset, BOOL bInitialState, LPCTSTR lpName);
```