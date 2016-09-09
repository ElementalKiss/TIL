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


##### GET 방식으로 호출
```
>>> from urllib2 import urlopen
>>> f = urlopen("http://www.example.com")
>>> print f.read(500)
<!doctype html>
<html>
<head>
    <title>Example Domain</title>

    <meta charset="utf-8" />
    <meta http-equiv="Content-type" content="text/html; charset=utf-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1" />
    <style type="text/css">
    body {
        background-color: #f0f0f2;
        margin: 0;
        padding: 0;
        font-family: "Open Sans", "Helvetica Neue", Helvetica, Arial, sans-serif;

    }
    div {
        width: 600px;
        margin:
>>>
```

##### POST 방식으로 호출

```
>>> from urllib2 import urlopen
>>> data = "query=python"
>>> f = urlopen("http://www.example.com", data)
>>> print f.read(300)
<!doctype html>
<html>
<head>
    <title>Example Domain</title>

    <meta charset="utf-8" />
    <meta http-equiv="Content-type" content="text/html; charset=utf-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1" />
    <style type="text/css">
    body {
        background
>>>
```

##### Request 객체를 생성해서 보내는 방법

```
>>> import urllib2
>>> req = urllib2.Request("http://www.example.com")
>>> req.add_header("Content-Type", "text/plain")
>>> req.add_data("query=python")
>>> f = urllib2.urlopen(req)
>>> print f.read(300)
<!doctype html>
<html>
<head>
    <title>Example Domain</title>

    <meta charset="utf-8" />
    <meta http-equiv="Content-type" content="text/html; charset=utf-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1" />
    <style type="text/css">
    body {
        background
>>>
```

##### 좀 더 복잡한 요청

인증 데이터나 쿠키 데이터를 추가하여 요청을 보내거나, 프록시 서버로 요청을 보내는 등 HTTP의 고급 기능을 포함하여 요청을 보낼 수 있다.

* 각 기능에 맞는 핸들러 객체를 정의
* 그 핸들러를 build_opener() 함수를 사용해 오프너로 등록
* urlopen() 함수로 열기 위해 install_opener() 함수를 사용해 디폴트 오프너로 설정

### httplib 모듈

웹 클라이언트는 대부분은 urllib2 모듈로 작성이 가능하지만 저수준의 더 세밀한 기능이 필요할 때 httplib 모듈을 사용한다.


#### httplib 모듈 사용 시 코딩 순서

* 연결 객체 생성: conn=httplib.HTTPConnection("www.python.org")
* 요청을 보냄: conn.request("GET", "/index.html")
* 응답 객체 생성: respone=conn.getresponse()
* 응답 데이터 읽음: data=respone.read()
* 연결 닫음: conn.close()

## 웹 서버 라이브러리

### 간단한 웹 서버

```
#!/usr/bin/env python

from BaseHTTPServer import HTTPServer, BaseHTTPRequestHandler

class MyHandler(BaseHTTPRequestHandler):
  def do_GET(self):
    self.wfile.write("Hello World")

if __name__ == '__main__':
  server = HTTPServer(('', 8888), MyHandler)
  print "Started WebServer on port 8888..."
  server.serve_forever()
```

#### 웹 서버 만드는 기본 룰

* BaseHTTPServer 모듈 임포트
* BaseHTTPRequestHandler를 상속, 원하는 로직으로 핸들러 클래스 정의
* 서버의 IP, PORT 및 핸들러 클래스를 인자로 하여 HTTPServer 객체를 생성
* HTTPServer 객체의 serve_forever() 메소드 호출

Base가 되는 모듈은 BaseHTTPServer, SimpleHTTPServer, CGIHTTPServer 모듈이 있다.

#### SimpleHTTPServer 모듈
간단한 핸들러(SimpleHTTPRequestHandler)가 미리 구현되어 있어 필요할 때 즉시 웹 서버를 실행할 수 있다. SimpleHTTPRequestHandler는 do_GET(), do_HEAD() 메소드가 정의되어 있어서 GET 및 HEAD 방식을 처리할 수 있다.

$ python -m SimpleHTTPServer 8888

#### CGIHTTPServer 모듈
CGIHTTPServer 모듈에는 CGIHTTPRequestHandler가 미리 정의되어 있고 이는 do_POST() 메소드가 정의되어 있어 POST 방식을 처리할 수 있다. 또한 SimpleHTTPRequestHandler를 상속받고 있어서 GET, HEAD 방식 역시 처리 가능하다.

### xxxHTTPServer 모듈간의 관계

#### 상속도
CGIHTTPRequestHandler -> SimpleHTTPRequestHandler -> BaseHTTPRequestHandler -> StreamRequestHandler -> BaseRequestHandler

## CGI/WSGI 라이브러리

WSGI(Web Server Gateway Interface): 웹 서버와 웹 애플리케이션을 연결해주는 규격으로 장고와 같은 웹 프레임워크를 개발하거나 아파치와 같은 웹 서버와 연동할 때 사용한다.

### CGI 관련 모듈

정적요청과 동적요청이 있는데 동적요청은 별도의 애플리케이션에서 처리하는 것이 보통이다. 웹 서버와 애플리케이션 간의 데이터를 주고받기 위한 규격을 CGI라 한다. 파이썬은 CGI을 처리할 수 있도록 CGIHTTPServer 모듈을 제공한다.

### WSGI 개요

CGI의 프로세스 부하의 단점을 해결하기 위해 Fast CGI, 쓰레드 처리 방식, 외부 데몬 프로세스 방식 등이 등장하게 되었다. 웹 서버와 웹 애플리케이션 중간에서 이런 까다로운 연동 처리를 돕는 것이 장고와 같은 웹 프레임워크이다.

### WSGI 서버의 어플리케이션 처리 과정

### wsgiref.simple_server 모듈