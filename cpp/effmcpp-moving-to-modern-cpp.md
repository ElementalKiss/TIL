# 들어가기

C++11과 C++14에 거물급 기능들이 생겼는데 auto, smart-pointer, move semantics, lambda, perfect forwarding, concurrency api가 그것이다.

C++98에서 현대적 C++로 가는데 마주치는 질문.
> 이 장에서 알아볼 내용인듯 하다.

* 객체를 생성할 때 괄호 대신 중괄호를 사용해야 하는 때는 언제인가?
* 별칭 선언이 typedef 보다 나은 이유?
* constexpr은 const와 어떻게 다른가?
* const 멤버 함수와 스레드 안전성의 관계?

# 항목7: 객체 생성 시 괄호(())와 중괄호 ({})을 구분하라.

## 균일 초기화

기존 C++98에서 여러가지 초기화 방법이 있는데 여러 가지 초기화 구문이 주는 혼동을 완화하게 위해 C++11은 균일 초기화(uniform initialization)을 도입했다. 개념상으로 단 한 종류의 초기화 구문이다.

균일 초기화 구문은 중괄호를 사용한다. 중괄호로 감싼 초기화(barced initialization).

```
std::vector<int> v{1, 2, 3}; // v의 초기 내용은 1, 2, 3
```

### 다양한 문맥에서 사용이 가능

객채 내의 int z(0)과 std::atomic<int> = 0와 같이 괄호와 "="" 오퍼레이터로 초기화를 못 하는 곳이 있다. 이런 곳을 전부 중괄호로 가능하다는 것이다.

### 암묵적 좁히기 변환(narrowing conversion) 방지

중괄호 초기치로 초기화를 하려할 때 객체의 형식으로 온전하게 표현할 수 있음이 보장되지 않으면 컴파일러는 해당 사실을 에러로 알려준다.

```
double x, y, z;

int sum1{x + y + z}; // 오류. double 합을 int로 표현하지 못할 수 있다.

int sum2(x + y + z); // 오류 아님. int에 맞게 잘려나간다.

int sum3 = x + y + z; // 마찬가지
```

### 가장 성가신 구문 해석(most vexing parse)에 자유롭다.

```
Widget w1(10); // 인수 10으로 Widget 생성자 호출

Widget w2(); // Widget을 리턴하는 w2라는 이름의 함수?

Widget w3(); // 인수 없이 Widget의 생성자 호출
```

## 왜 제목이 균일 초기화를 선호하라가 아닌가?

중괄호의 단점은 종종 예상치 못한 행동을 보인다는 것. 중괄호 초기치, std::initializer_list, 생성자 중복적재 해소 사이의 관계에서 비롯되는 문제이다.

```
class Widget {
public:
    Widget(int i, bool b);
    Widget(int i, double d);
    Widget(std::initializer_list<long double> il);
    ...
};
```

이렇게 Widget 클래스가 정의되어 있을 때,

```
Widget w1(10, true); // 괄호; 첫 생성자 호출
Widget w2{10, true}; // 중괄호; std::initializer_list 생성자 호출 10과 true가 long double로 변환

Widget w3(std::move(w1)); // 괄호; 이동 생성자 호출
Widget w4(std::move(w2)); // 중괄호; initializer_list 생성자 호출
```

심지어 Widget initializer_list<bool>로 선언해놔도 Widget w{10, 5.0};은 initializer_list 생성자를 호출해 오류를 낸다.

### 컴파일러가 중복적재를 포기하는 경우

```
Widget(std::initializer_list<std::string> li);
```

int와 bool은 string으로 변환하는 방법이 없으므로 괄호, 중괄호 모두 Widget(int i, bool b)를 호출한다.

### 흥미로운 극단적 경우

기본 생성을 지원하며 std::initializer_list 생성도 지원하는 객체를 빈 중괄호 쌍으로 생성하는 경우이다.

#### 컴파일러가 빈 중괄호를 어떻게 받아들이는가?

* 인수 없음: 기본 생성자 호
* 빈 std::initializer_list: 요소가 하나도 없는 std::initializer_list를 초기치로 사용한 객채 생성

#### 표준

표준에 따르면 이런 경우는 기본 생성자가 호출된다.

```
class Widget {
public:
    Widget();
    Widget(std::initializer_list<int> li);

    ...
}

Widget w1{}; // 기본 생성자 호출
Widget w2(); // 함수 선언
```

#### if std::initializer_list를 호출하고 싶다면

중괄호 쌍을 괄호로 감싸거나 빈 중괄호 쌍을 또 다른 중괄호 쌍으로 감싸면 된다.

```
Widget w3({});
Widget w4{{}};
```

### std::initializer_list 중복적재에 대한 논의가 중요한 이유

일상적인 프로그래밍과 얼마나 관련 있나 하겠지만 실제로 std::vector가 컨테이너의 초기 크기와 컨테이너의 모든 초기 요소의 값을 지정할 수 있는 비std::initializer_list 생성자가 있다.

#### 괄호 vs 중괄호

* std::vector<int> v1(10, 20);

비std::initailizer_list 생성자를 사용: 모든 요소의 값이 20인, 요소 10개짜리 std::vector가 생성된다.

* std::vector<int> v2{10, 20};

std::initializer_list 생성자가 사용: 값이 각각 10과 20인 두 요소를 담는 std::vector가 생성된다.

#### 이 논의에서 배워야 할 사항

* 클래스를 작성할 때, 만일 중복적재된 생성자 중에 std::initializer_list를 받는 함수가 하나 이상 존재한다면, 중괄로 초기화 구문을 이용하는 클라이언트 코드에서 std::initializer_list 중복적재들만 적용될 수 있음을 주의해야 한다.

* 클래스 사용자로서 객체를 생성할 때 괄호와 중괄호를 세심하게 선택해야 한다는 것이다.

## 기억해 둘 사항들

* 중괄호 초기화는 가장 광범위하게 적용할 수 있는 초기화 구문이며, 좁히기 변환을 방지하며, C++의 가장 성가신 구문 해석에서 자유롭다.
* 생성자 중복적재 해소 과정에서 중괄호 초기화는 가능한 한 std::initializer_list 매개변수가 있는 생성자와 부합한다.(겉으로 보기에 더 잘 부합하는 인수가 있더라도)
* 괄호와 중괄호의 선택이 의미 있는 차이를 만드는 예는 인수 두 개로 std::vector<수치>을 생성하는 것이다.
* 템플릿 안에서 객체를 생성할 때 괄호를 사용할 것인지 중괄호를 사용할 것인지 선택하기 어려울 수 있다.

# 항목 8: 0과 NULL보다 nullptr을 선호하라.

## 0

* 리터럴 0은 int이지 포인터가 아니다.
* 포인터만 사용할 수 있는 위치에 0이 있으면 C++은 마지못해 그것을 null 포인터로 해석한다.
* C++ 기본적인 방침은 0은 int이고 포인터가 아니다.

## NULL

* 컴파일러가 NULL을 정의한 것에 따라 다르다.
* NULL에 int 이외의 정수 형식 long을 부여할 수 있다.
* NULL도 0과 마찬가지로 포인터 형식이 아니다.

## 예시

```
// f의 세 가지 중복적재
void f(int);
void f(bool);
void f(void*);

f(0); // f(int)를 호출
f(NULL); // 컴파일되지 않을 수 있지만 보통 f(int)를 호출
```

## nullptr 장점

### nullptr은 정수 형식이 아니다.

모든 형식의 포인터라고 생각하면 된다. std::nullptr_t이고 std::nullptr_t 자체는 다시 "nullptr의 형식"으로 정의한 순환 정의이다. std::nullptr_t는 모든 raw 포인터 형식으로 암묵적으로 변환되며, nullptr은 마치 모든 형식의 포인터처럼 동작한다.

```
f(nullptr) // f(void*)를 호출
```

### auto

```
auto result = findRecord();

if (result == 0) { ... }

if (result == nullptr) { ... }
```

findRecord의 반환 형식을 모른다면 result의 형식을 명확하게 말할 수 없다. 문맥에서는 포인터 형식일수도 잇고 정수 형식일 수도 있다. 반명 nullptr을 사용하면 이 중의성(ambiguity)가 해소된다.

### 템플릿

3개의 뮤텍스를 잠근 상태로 호출 가능한 함수들이 있다. f1, f2, f3라고 가정하자.

```
int f1(...);
int f2(...);
int f3(...);

std::mutex f1m, f2m, f3m; // 뮤택스들

using MuxGuard = stD::lock_guard<std::mutex>;
...
{
  MuxGuard g(f1m);
  auto result = f1(0);
}
...
{
  MuxGuard g(f2m);
  auto result = f2(NULL);
}
...
{
  MuxGuard g(f1m);
  auto result = f1(0);
}
```

아래 코드를 템플릿화 하여 소스 코드 중복을 피하기 위해 이 패턴을 템플릿화 하면 다음과 같다.

```
template<typename FuncType, typename MuxType, typename PtrType>
decltype(auto) lockAndCall(FuncType func, MuxType& mutex, PtrType ptr)
{
  using MuxFuard = std::lock_guard<MuxType>;
  
  MuxFuard g(mutex);
  return func(ptr);
}
```

이렇게 코드를 짜면,

```
auto result1 = lockAndCall(f1, f1m, 0); // error
auto result2 = lockAndCall(f2, f2m, NULL); // error
auto result3 = lockAndCall(f3, f3m, nullptr); // error
```

## 기억해 둘 사항들

* 0과 NULL보다 nullptr을 선호하라.
* 정수 형식과 포인터 형식에 대한 중복적재를 피하라.


# 항목 9: typedef보다 별칭 선언을 선호하라.

## 별칭 선언(alias declaration)의 장점

### typedef보다 직관적이다.

```
// typedef
typedef std::unique_ptr<std::unordered_map<std::string, std::string>> UPtrMapSS;
typedef void (*FP)(int, const std::string&); // void pointer

// alias declaration
using UPtrMapSS = std::unique_ptr<std::unordered_map<std::string, std::string>>
using FP = void (*)(int, const std::string&);
```

### 템플릿 사용에 강력하다.

```
// typedef
template <typename T>
struct MyAllocList {
  typedef std::list<T, MyAlloc<T>> type;
};

MyAllocList<Widget>::type lw;

// alias declaration
template <typename T>
using MyAllocList = std::list<T, MyAlloc<T>>;

MyAllocList<Widget> lw;
```

중요한 것은 typename과 ::type이 계속 따라다닌 다는 것. MyAlloacList<T>::type은 소위 의존적 형식(dependnt type)이라 하며 C++에선 의존적 형식의 이름 앞에 반드시 typename을 붙여야 한다.

## std의 특빌 변환 예시

```
std::remove_const<T>::type // C++11의 const T -> T
std::remove_const_t<T> // C++14
```

std::remove_const_t<T>의 정의

```
template <class T>
using remove_const_t = typename remove_const<T>::type;
```

## 기억해 둘 사항들

* typedef는 템플릿화를 지원하지 않지만, 별칭 선언은 지원한다.
* 별칭 템플릿에서는 ::type 접미어를 붙일 필요가 없다. 템플릿 안에서 typedef를 지칭할 때에는 typename 접두사를 붙여야 하는 경우가 많다.
* C++14는 C++11의 모든 형식 특질 변환에 대한 별칭 템플릿들을 제공한다.

# 항목 10: 범위 없는 enum보다 범위 있는 enum을 선호하라.

C++98의 enum은 unscoped enum이라 하고, 이에 대응되는 C++11의 새로운 enum은 scoped enum리고 한다. 기존 C++98은 이름 누수가 발생하는 문제가 있다. enum으로 설정한 변수 이름을 못 쓴다는 이야기다.

```
enum class Color { black, white, red};

auto white = false; // 가능.

Color c = white; // 불가능. white라는 열거자 없음

Color c = Color::white; // 가능.
```