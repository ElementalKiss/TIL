# 항목 5 : 명시적 형식 선언보다는 auto를 선호하라

## 요약

auto를 잘 활용하면 타이핑 양이 줄어든다. 형식을 직접 지정했을 때 겪게 되는 정확성 문제와 성능 문제도 방지할 수 있다. 

> 흠.. 하지만 명시적으로 정의 했을 때 보다 코드 리더블은 떨어지겠지..

## auto를 선호하라

반복자가 가르키는 것을 표현하기에 명시적으로 선언하기 매우 어렵다. 간단한 선언의 즐거움으로 auto를 사용한다.

C++14부터는 람다 표현식의 매개변수에도 auto를 사용할 수 있다.

```
    auto derefUPLess = [](const auto& p1, const auto& p2) {
        return p1 < p2; 
    };
```

이런 꼴이 가능하다는 것.

## std::function

C++11 표준 라이브러리의 한 템플릿. 함수 포인터 개념을 일반화 한 것이다. 함수 포인터는 함수만 가르키지만 std::function은 호출 가능한 객체이면 그 어떤 것도 가리킬 수 있다.

앞서 derfUpLess 함수를 Widget 객체에 대해 선언 한다면 람다 함수로 만들어지는 클로저는 호출 가능한 객체이기때문에 std::function으로 만들어야 한다.

```
    std::function<bool(const std::unique_ptr<Widget>&, const std::unique_ptr<Widget>&)> 
        derefUpLessWidget = 
        [](const std::unique_ptr<Widget>& p1, const std::unique_ptr<Widget>& p2) {
        return *p1 < *p2;
    };
```

그럼 대충 이런 꼬라지가 나오는데................. 거의 헬이다.

## 중요

std::function으로 선언된 변수의 형식은 std::function 템플릿의 한 인스턴스이다. 그 크기는 주어진 서명에 대해 고정으로 되어 있다. 그러므로 클로저를 저장하기에 공간이 부족할 수 있는데 이럴 때 std::function은 힙 메모리에 클로저를 저장한다. 

인라이닝을 제한하고 간접 함수 호출을 산출하는 구현 세부사항 때문에 std::function이 클로저를 호출하는 것이auto로 선언된 객체를 통해 호출하는 것보다 대체적으로 느리다.

## v.size()

std::vector<int> v 의 size를 리턴하는 함수이다. 이 함수의 반환 형식은 std::vector<int>::size_type이다.(헐..) unsigned 로 받아 일반적으로 사용하지만 미묘한 문제가 발생할 수 있다.

32Bit Windows에서 unsigend는 32Bit이며 std::vector<int>::size_type도 32Bit로 같다. 64Bit Windows에선 unsigend는 32Bit이며 std::vector<int>::size_type는 64Bit로 서로 다르다. 드물지만 오작동 할 여지가 아주 미세하게 있다.

이럴 때 auto를 사용하면 알아서 맞춰 준다는 것.

## std::pair<const std::string, int>

```
    std::unordered_map<std::string, int> m;
    ...
    for (const std::pair<std::string, int>& p : m ) {
        ...
    }
```

이런 코드가 있을 때 실제 for문 안쪽에 std::pair<std::string>는 std::pair<const std::string, int>로 변환된다.(실제 타입이 그것이기 떄문에.) 이런 쓸데 없는 일이 생길 수 있다.

## 걱정거리(내가 앞에서 언급한 코드 리더블)

auto는 선택이기 때문에 필수가 아니다. 명시적 형식 선언이 유지보수가 쉽다면 그것은 그렇게 사용하면 된다. But, 프로그래밍 세계에서 흔히 형식 추론(type inference)라고 하는데 이것을 C++이 혁신적인 성과를 내진 못했다. 경험에 따르면 다른 언어들이 이런 형식 추론 기술에 성공했고 코드 기반의 작성 유지보수에도 문제가 없었다.

auto로 객체 형식을 파악할 수 없다는 것에 걱정하는 개발자가 많지만 IDE의 기능으로 이를 완화할 수 있다.

auto를 사용하면 리펙토링이 수월해진다. int를 돌려주는 함수가 있었는데 long으로 바꿨다면 이를 받는 부분을 모두 long으로 바꿔야 한다. 리턴 받아 쓰는 부분들을 auto로 했다면 선언만 long으로 바꾸면 된다.

## 기억해 둘 사항들
* auto 변수는 반드시 초기화 해야 한다. 이식성 또는 효율성 문제를 유발할 수 있는 형식 불일치가 발생하는 경우가 거의 없다.
* 대체로 명시적으로 지정할 때보다 타자량이 적다.

# 항목 6 : auto가 원치 않은 형식으로 연역될 때에는 명시적 형식으로 초기화하자.

## 개요

auto를 사용해서 변수를 선언하면 형식을 명시적으로 지정했을 때보다 기술적으로 여러 강점이 있다. But, 가끔 auto가 이상한 짓을 한다.

std::vector<bool> features(const Widget& w); 라고 쳤을 때

## 코드

```
bool highPriority = featrues(w)[5];

processWidget(w, highPriority);
```

## 문제점

여기서 highPriority의 bool을 auto로 변경하면?

processWidget은 미정의 행동이 된다.

## Why?

std::vector<bool>에서 std::vector::operator[]가 돌려주는 것은 bool 형이 아니라 std::vector<bool>::reference 형식의 객체를 리턴한다. 이는 대리자 클래스(proxy class)로 다른 어떤 형식의 행동을 흉내 내고 보강하는 클래스다.

> 프록시 패턴 좀 봐야겠다.

이 객채는 bool당 1비트의 압축된 형태로 표현하도록 명시되어 있기 때문이다.

```
Matrix sum = m1 + m2 + m3 + m4;
```

이런 형태일 때 oprator+가 Sum<Matrix, Matrix> 같은 대리자 클래스의 객체를 돌려준다면? 

```
Sum<Sum<...<Matrix, Matrix>, Matrix...>
```

이런 느낌으로... 여튼 대리자 클래스는 auto와 잘 맞지 않는다.

## 정리

auto someVar = "보이지 않는" 대리자 클래스 형식의 표현식;

## 기억해 둘 사항들
* 보이지 않는 대리자 형식 때문에 auto가 초기화 표현식의 형식을 잘못 연역할 수 있다.
* 형식 명시 초기치 관용구는 auto가 원하는 형식을 연역하도록 강제한다.