# 항목1. auto 키워드는 템플릿에 대한 형식 연역을 기반으로 작동한다.

템플릿은 대게 이런 형태로 사용되고 호출된다.

```
template<class T>
void f(ParamType param);
...
f(expr);
```

## ParamType은 총 세 가지 경우로 나뉜다.

* 포인터 또는 참조 형식이지만 보편 참조(universal reference)는 아님.
* 보편 참조.(일단 좌측값 참조나 우측값 참조와는 다르다는 것만 알아둔다.)
* 포인터도 아니고 참조도 아님.

### 경우1. 포인터 또는 참조 형식이지만 보편 참조 아닐 때

#### 연역 과정

* expr이 참조 형식이면 참조 부분을 무시한다.
* expr의 형식을 ParamType에 패턴 매칭 시켜 T의 형식을 결정한다.

이건 평소에 자주 사용하는 template 이용방법이다.

T&, const T&, T* 형태로 ParamType을 정의하고 여러 expr로 넘기는 상황이다.
이런 경우 expr의 const 성은 계속 유지되며 ParamType의 선언된 형태로 매칭된다는 것.

### 경우2. 보편 참조일 때

일단 형식은 T&& 형식인데 우측값 참조와 다른 방식으로 행동한다고 한다.

#### 연역 과정

* expr이 좌측값이면, T와 ParamType 둘 다 좌측값 참조로 연역된다. 이는 이중으로 비정상적인 상황이다.
* 첫째로, 템플릿 형식 연역에서 T가 참조 형식으로 연역되는 경우는 이것이 유일하다.
* 둘째로, ParamType의 선언 구문은 우측값 참조와 같은 모습이지만 연역된 형식은 좌측값 참조다.
* 만일 expr이 우측값이면 '정상적으로' 경우1의 규칙들이 적용된다.

```
template<class T>
void f(T&& param);
 
int x = 27;
const int cx = x;
const int& rx = x;
 
f(x);    
// x is left value, T is int&
// param is int&
 
f(cx);
// cx is left value, T is const int&
// param is const int&
 
f(rx);
// rx is left value, T is const int&
// param is const int&
 
f(27);
// 27 is right value, T is int
// param is int&&
```

> 보편 참조 형식 템플릿을 왜 쓰는지부터 좀 알아봐야겠다......

### 경우3. ParamType이 포인터도 아니고 참조도 아님

T인 경우를 의미한다.
안수가 pass by value로 값으로 전달되는 상황이다.

#### 연역 과정

* 만일 expr의 형식이 참조면 참조 부분은 무시된다.
* expr의 참조성을 무시한 후, 만일 expr이 const면 그 const 역시 무시한다.

```
int x = 27;
const int cx = x;
const int& rx = x;
 
f(x);
f(cx);
f(rx);
```

이렇게 넘겨도 모두 int로 연역된다. param은 복사본이기 때문!

## 배열 인수

C++에선 배열과 포인터가 별 차이가 없다.
배열 매개변수와 포인터 매개변수의 동치성은 C의 기초에서 나오는 것이기 때문에..
이런것이 필자는 환상이라고 정의한다. 배열과 포인터를 정확히 구분하는 것이 좋다는 뜻인가?

### 여튼 매개변수로 넘길 때 그럼 배열로 넘기는 방법이 있는가?

```
template<class T>
void f(T param);
 
const char name[] = "J. P. Briggs";
const char* ptrtoName = name;
 
f(name);
```

이런식으로 하게 되면 실제로 T의 연역은 const char*로 연역된다는 것...

### 배열 형식을 그대로 넘기는 꼼수

```
template<class T>
void f(T& param);
 
...
 
f(name);
```

이렇게 호출하면 실제 T는 const char [13]으로 연역되고
f의 매개변수는 const char (&)[13]으로 연역된다. (놀라워!)

### 배열 형식을 넘기고 원소 개수를 연역하는 템플릿

```
template<class T, class N>
constexpr size_t arraySize(T (&)[N]) noexcept
{
    return N;
}
```

* constexpr - 함수 호출의 결과를 컴파일 도중에 사용할 수 있다.(...이런 신기한...!!!)
* noexcept - 컴파일러가 더 나은 코드를 산출하는데 도움을 주는 것.

constexpr을 사용하면 이런 문장도 가능하다고 한다. (놀라워! 이런게 있다니!!!!!!!!!!!!!!!!!!!!)

```
int keyVals[] = { 1, 3, 5, 7, 9, 11, 22, 35 };
 
int mappedVals[arraySize(keyVals)]; 
 
std::array<int, arraySize(keyVals)> mappedVals;
```

> 배열 선언 시에 저런게 가능하다니...!! 현대적인 C++ 개발자라면 당연히 내장 배열보다는 std::array를 선호할 것이라고 한다. 나는 아주 구시대적인 C++ 개발자인 것으로... array 많이 써봐야겠다.

### 함수 인수

C++에서 포인터로 되는 것은 함수도 있다. 함수 포인터!

```
void some_func(int, double);
 
template<class T>
void f1(T param);
 
template<class T>
void f2(T& param);
 
f1(some_func); // param is void (*)(int, double)
f2(some_func); // param is void (&)(int, double)
```

이렇게 함수 포인터와 함수 참조로 연역된다.

## 기억해 둘 사항
* 템플릿 형식 연역 도중에 참조 형식의 인수들은 비참조로 취급한다. 참조성 무시.
* 보편 참조 매개변수에 대한 형식 연역 과정에서 좌측값 인수들은 특별하게 취급.
* 값 전달 방식의 매개변수에 대한 형식 연역과정에서 const나 volatile 인수는 비 const와 비 volatile 인수로 취급된다.
* 연역 과정에서 배열 이름이나 함수 이름은 포인터로 붕괴된다. 
단, 그런 인수가 참조를 초기화하는 데 쓰이는 경우에는 포인터로 붕괴하지 않는다.

# 항목 2 : auto 연역 규칙

## 핵심

auto 형식 연역은 대체로 템플릿 형식 연역과 같다. 다른 경우는 auto 형신 연역은 중괄호 초기치가
std::initializer_list를 나타낸다고 가정하는 반면 템플릿 형식 연역은 그렇지 않다. 타당한 이유는 없다. 그냥 이렇다.

```
auto x = {1,2,3}; // std::initializer_list<int>
 
template<class T>
void f(T param);
 
f({1,2,3}); // error 
```

두 상황의 연역 규칙이 다르다는 것을 꼭 알아두어야 한다고 한다.

함수의 반환 형식이나 람다 매개변수에 쓰인 auto에 대해서는 auto 형식 영역이 아니라 템플릿 형식 연역이 적용된다.

```
 auto create_init_list()
 {
     return {1,2,3}; // error
 }
  
 std::vector<int> v;
 ...
 auto resetV = [&v](const auto& newValue) { v = newValue; }; // c++14
 ...
 resetV({1,2,3}); // error
```

위 코드의 에러 부분은 {1,2,3}의 형식을 연역할 수 없어 컴파일을 실패한다.

# 항목 3: decltype의 작동 방식 숙지

## decltype란

주어진 이름이나 표현식의 형식을 알려준다. C++11에서 decltype는 함수의 반환 형식이 그 매개변수 형식들에 의존하는 함수 템플릿을 선언할 때 주로 쓰인다.

> 너무 이해 안 되서 한 10번은 넘게 읽은 것 같다.

## 예시로 이해해보기
```
    std::deque<int> intDq;
    intDq.resize(10);
 
    authAndAccess(intDq, 5) = 10;
    std::cout << intDq.at(5) << std::endl;;
```

기본 적으론 해당 문장을 컴파일 하기 위해서이다. authAndAccess 함수로부터 intDq의 원소의 레퍼런스를 받아 값을 바로 넣는 것이다.

template로 선언하면

```
/* 
    template<class Container, class Index>
    auto authAndAccess(Container& c, Index i)
    {
        // authenticateUser();
        return c[i];
    }
    // authAndAccess(intDq, 5) = 10; // error
    // 심각도    코드    설명    프로젝트    파일    줄    비표시 오류(Suppression) 상태
    // 오류    C2106    '=': 왼쪽 피연산자는 l-value이어야 합니다.    Test    c:\users\jo minhyuk\documents\visual studio 2015\projects\test\test\test.cpp    39    
*/
```

이런 꼬라지가 될 텐데. 실제로 컴파일을 해보면 return c[i] 에서 auto 연역을 통해  우측값이 변환되어 나오므로 정상적으로 컴파일이 되지 않는다. 실제로 l-value 관련 에러가 난다. intDq[5]를 통해 int&를 돌려주나, authAndAccess에 대한 auto 반환 형식 연역 과정에서 참조가 제거되기 때문에 결국 반환 형태는 int가 된다.

그래서 decltype(auto) 라는 이상한 형태의 문법이 생겨나는데.. auto는 해당 형식이 연역되어야 하며, decltype는 그 연역 과정에서 decltype 형식 연역 규칙을 적용하겠다는 의미

위의 해결 코드의 문제점은 우측값을 전달할 수 없다는 것이다.

> 라고는 하는데 실제로 해보니 정상동작 하였다. 예시에 나온 팩토리 함수에서 리턴 받은 객체를 통해 접근하여 값을 복사했지만 정상동작....

```
std::deque<int> makeIntDque()
{
    std::deque<int> dq;
    dq.resize(10);
 
    static int i = 1000;
    for (auto& value : dq) {
        value = i++;
    }
 
    return dq;
}
 
-- main
 
    auto copyFuncRtValue = authAndAccess(makeIntDque(), 5);
    std::cout << copyFuncRtValue << std::endl;
```

이상이 없었지만 책에서 제안하는 방법은

```
template<class Container, class Index>
decltype(auto) authAndAccess(Container&& c, Index i)
{
    // authenticateUser();
    return std::forward<Container>(c)[i];
}
```

좌측값 뿐만 아니라 우측값도 받아들일 수 있도록 선언을 고치는 것이다. 이를 보편 참조를 통해 좌측값과 우측값을 함께 관리하는 함수를 하나로 통합할 수 있다. 보편 참조에 std::forward를 붙인다.(오른 값에는 move, 보편 참조는 forward)

위는 C++14의 최종 버전이다. 아래는 C++11의 최종 버전.

```
template<class Container, class Index>
auto authAndAccess(Container&& c, Index i) -> decltype(std::forward<Container>(c)[i])
{
    // authenticateUser();
    return std::forward<Container>(c)[i];
}
```

### 주의

```
decltype(auto) f1()
{
    int x = 0;
    ...
    return x;
}
 
decltype(auto) f2()
{
    int x = 0;
    ...
    return (x);
}
```

* x 와 (x)는 decltype에서 전자는 int를 반환 후자는 int&를 반환한다.
* 지역 변수의 레퍼런스를 리턴하기때문에 망할 수 있다. 주의해야 한다.

## 기억해 둘 사항들
* decltype는 항상 변수나 표현식의 형식을 아무 수정 없이 보고한다.
* decltype는 형식이 T이고 이름이 아닌 좌측값 표현식에 대해서는 항상 T& 형식을 보고한다.(중요)
* c++14는 decltype(auto)를 지원한다. decltype(auto)는 auto처럼 초기치로부터 형식을 연역하지만, 그 형식 연역 과정에서 decltype의 규칙을 적용한다.

# 항목 4: 연역된 형식을 파악하는 방법을 알아두자

## 기억해 둘 사항들

* 컴파일러가 연역하는 형식을 IDE 편집기나 컴파일러 오류 메시지, 부스트 TypeIndex 라이브러리를 이용해서 파악할 수 있다.
* 일부 도구의 결과는 유용하지도 않고 정확하지도 않을 수 있다. C++의 형식 연역 규칙들을 제대로 이해하자.
