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