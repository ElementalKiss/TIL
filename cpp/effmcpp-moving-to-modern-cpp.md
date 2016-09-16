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

## 왜 제목이 균일 초기화를 선호하라가 아니가?

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