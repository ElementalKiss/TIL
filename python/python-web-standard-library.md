# 파이썬 웹 표준 라이브러리

크게 웹 클라이언트 프로그래밍이냐 웹 서버 프로그래밍으로 나눈다. <br>
파이썬 3.x 버전에서 웹 개발 수요가 많아짐에 따라 라이브러리 모듈도 개선하고 재구성되었다.

WSGI: 아파치와 같은 상용 웹 서버와의 연동에 필요

## 웹 라이브러리 구성

& 책에 그림으로 아주 이쁘게 나와있지만 난 텍스트를 사랑하므로...

* 웹 서버 프로그래밍

Web Framework: Django, Flask, Tornado, 등 <br>
웹 서버용 API: http.cookie, http.server <br>


* 웹 클라이언트 프로그래밍

고수준 API: urllib.parse, urllib.request, urllib.error, urllib.respone, urllib.robotparser
웹 클라이언트용 API(저수준 API) : http.client, http.cookiejar