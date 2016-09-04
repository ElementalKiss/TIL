# 웹프로그래밍 이해

## 웹 프로그래밍이란?

### 정의 
* HTTP 프로그톨로 통신하는 클라이언트와 서버를 개발 하는 것.
* 장고는 이를 돕는 웹 프레임워크
* 웹 클라이언트 리퀘스트 -> http/https 프로토콜 -> 웹 서버 리스폰

### 웹 클라이언트의 종류
* 웹 브라우저
* 리눅스 curl
http/https/ftp 등 여러가지의 프로토콜을 사용하여 데이터를 송수신할 수 있는 명령
* Telnet
* 직접 만든 클라이언트

## HTTP 프로토콜
Hypertext Transfer Protocol로 웹 서버와 웹 클라이언트 사이에서 데이터를 주고받기 위해 사용하는 통신 방식으로 TCP/IP 프로토콜 위에서 동작한다.

### HTTP 메시지 구조
* 스타트라인(Start Line) - 요청라인 또는 상태라인
* 헤더(Header) - 헤더는 생략 가능
* 빈 줄(Blank Line) - 헤더의 끝을 빈 줄로 식별
* 바디(Body) - 바디는 생략 가능

요청 메시지인 경우 요청라인, 응답 메시지인 경우 상태라인이라 한다. 스타트라인에 이어 헤더는 각 행의 끝에 줄 바꿈 문자인 CRLF(Carriage Return Line Feed)가 있으며, 헤더와 바디는 빈 줄로 구분된다. 헤더와 바디는 생략할 수 있고 바디에는 텍스트뿐만 아니라 바이너리 데이터도 들어갈 수 있다.

* 요청 메시지 예시
```
GET /book/shakespeare HTTP/1.1
Host: example.com:8080
```
첫 번째 줄은 요청라인 Method, URI, Protocol Version으로 구성된다.
두 번째 줄은 Name: Value 형식으로 표현하며, 여러 줄도 가능하다. Host 항목은 필수이다. 요청 라인 URI에 Host를 표시하면 헤더는 생략 가능하다.

* 요청 메시지 헤더 생략 가능 예시
```
GET http://exmaple.com:8080/book/sharespeare HTTP/1.1
```

* 응답 메시지 예시
```
HTTP/1.1 200 OK
Context-Type: application/xhtml+xml; charset=utf-8

<html>
...
</html>
```

첫 번째 줄은 상태라인으로 Protocol Version, 상태 코드, 상태 텍스트로 구성된다.
두 번재 줄부터는 헤더이다. 헤더 항목이 하나뿐인 응답 메시지로, 바디와 헤더가 빈 줄로 구분되어있다. 바디에는 HTML을 포함한다.

* URI란?

Uniform Resource Indentifier의 약자로 URL(Uniform Resource Locator)과 URN(Uniform Resource Name)을 포함하는 좀 더 넓은 의미의 표현이다.

### HTTP 처리 방식
* GET: 리소스 취득. Read(조회)
* POST: 리소스 생성. Create(생성)
* PUT: 리소스 변경. Update(변경)
* Delete: 리소스 삭제. Delete(삭제)
* HEAD: 리소스의 헤더(메타데이터) 취득
* OPTIONS: 리소스가 서포트하는 메소드 취득
* TRACE: 루프백 시험에 사용
* CONNECT: 프록시 동작의 터널 접속으로 변경

### GET과 POST 메소드

HTML 폼에서 지정할 수 있는 메소드가 GET과 POST 밖에 없어 가장 많이 쓰는 메소드.

* GET 방식은 URI 부분의 ? 뒤에 Key=Value 쌍으로 이어붙여 보낸다.
* POST 방식은 GET의 URI에 넣던 것을 요청 메시지의 바디에 넣는다.

### 상태 코드

서버에서의 처리 결과는 응답 메시지의 상태라인에 있는 상태코드(Status Code)를 보고 파악할 수 있다.

[Status Code](http://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html)