# 파이썬 웹 표준 라이브러리

크게 웹 클라이언트 프로그래밍이냐 웹 서버 프로그래밍으로 나눈다. <br>
파이썬 3.x 버전에서 웹 개발 수요가 많아짐에 따라 라이브러리 모듈도 개선하고 재구성되었다.

WSGI: 아파치와 같은 상용 웹 서버와의 연동에 필요

## 웹 라이브러리 구성

& 책에 그림으로 아주 이쁘게 나와있지만 난 텍스트를 사랑하므로...

* 웹 서버 프로그래밍

Web Framework: Django, Flask, Tornado, 등 <br>
웹 서버용 API: http.cookie, http.server


* 웹 클라이언트 프로그래밍

고수준 API(urllib 패키지): urllib.parse, urllib.request, urllib.error, urllib.respone, urllib.robotparser <br>
웹 클라이언트용 API(저수준 API) : http.client, http.cookiejar

## 웹 클라이언트 라이브러리

### urlparse 모듈

URL의 분해, 조립, 변경 등을 처리하는 함수를 제공한다.

urlparse() 함수: URL을 파싱한 결과로 ParseResult 인스턴스를 반환한다. 

ParseResult 클래스의 속성

* scheme: URL에 사용된 프로토콜을 의미한다.
* netloc: 네트워크 위치. user:password@host:port 형식으로 표현되며, http 프로토콜인 경우 hots:port 형식이다.
* path: 파일이나 애플리케이션 경로를 의미.
* params: 애플리케이션에 전달될 매개변수.
* query: 질의 문자열로, 앰퍼샌드(&)로 구분된 key=value 형식으로 표시.
* fragment: 문서 내의 앵커 등 조각을 지정한다.

그 외에도 urlsplit(), urljoin(), parse_qs() 함수 등이 있다.

& 책에는 2.7버전 링크가 되어있으나 난 3.4 stable 버전
[urlparse module link](https://docs.python.org/3.4/library/urllib.parse.html#module-urllib.parse)

### urllib2 모듈

주어진 URL에서 데이터를 가져오는 기본 기능을 제공. urlopen() 함수

```
urlopen(url, data=None, [timeout])
```

* url 인자로 지정한 URL로 유사 파일 객체를 반환한다. url 인자는 문자열이거나 Request 클래스의 인스턴스가 올 수 있다.
* url에 file 스킴을 지정하면 로컬 파일을 열 수 있다.
* 디폴트 요청 방식은 GET
* POST로 보내고 싶으면 data 인자에 질의 문자열을 지정해주면 된다.

이 외에도 URL을 처리하기 위해서 urlretrieve(), quote(), unquote(), urlencode() 등의 함수가 필요하다. <br>

& 3.x 이상 버전에선 urllib.request 쪽으로 통합된 듯 하다. [url.request link](https://docs.python.org/3.4/library/urllib.request.html)

#### urlopen() 함수 사용 방법

* URL로 GET/POST 방식의 간단한 요청 처리: urlopen() 함수만으로 가능
* 요청 헤더 추가, 변경이 필요한 경우: Request 클래스를 같이 사용
* 인증, 쿠키, 프록시 등 복잡한 요청 처리: 인증/쿠키/프록시 해당 핸들러 클래스를 같이 사용

