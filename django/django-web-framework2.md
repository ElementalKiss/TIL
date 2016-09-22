# Django 웹 프레임워크

## 프로젝트 뼈대 만들기

* 프로젝트: 전체 프로그램.
* 애플리케이션: 프로젝트 하위의 서브 프로그램

이런 개념으로 프로젝트 디렉토리와 애플리케이션 디렉토리를 구분하고, 코딩하는 파일도 프로젝트 파일인지 애플리케이션 파일인지 구분해서 적절한 위치에 저장해야 한다.

하나의 애플리케이션이 여러 프로젝트에 사용될 수 있다. 생산성을 높여준다.

### 디렉토리 구조

```
ch3
  db.sqlite3 
  manage.py
  mysite
    __init__.py
    settings.py
    urls.py
    wsgi.py
  polls
    __init__.py
    admin.py
    migrations
      __init__.py
    models.py
    test.py
    views.py
```

* **ch3 dir: 프로젝트 관련 디렉토리 및 파일을 모아주는 최상위 디렉토리. 보통 settings.py 파일의 BASE_DIR 항목으로 지정된다.**
* db.sqlite3: SQLite3 DB 파일.
* manage.py: 장고의 명령어를 처리하는 파일.
* **mysite dir: 프로젝트명으로 만들어진 디렉토리. 프로젝트 관련 파일이 들어 있다.**
* __init__.py: 디렉토리에 이 파일이 있으면 파이썬 패키지로 인식한다.
* settings.py 프로젝트 설정 파일.
* urls.py: 프로젝트 설정 파일.
* wsgi.py: Apache 같은 상용 웹 서버와 WSGI 규격으로 연동하기 위한 파일이다.
* polls dir: 애플리케이션명으로 만들어진 디렉토리. 해당 애플리케이션 관련 파일이 있다.
* admin.py: Admin 사이트에 모델 클래스를 등록해주는 파일이다.
* **migrations dir: 데이터베이스 변경사항을 관리하기 위한 디렉토리다.**
* models.py: 데이터베이스 모델 클래스를 정의하는 파일.
* tests.py: 단위 테스트용 파일.
* views.py: 뷰 함수를 정의하는 파일. 함수형 뷰 및 클래스형 뷰 모두 이 파일에 정의한다.
* **templates dir: 프로젝트를 진행하면서 추가. 템플릿 파일들이 들어있다. 보통 프로젝트 레벨과 애플리케이션 레벨의 템플릿으로 구분하여 ch3/templates 및 ch3/polls/templates 위치에 생성된다.**
* **static dir: 프로제그를 진행하면서 추가. CSS, Image, Javascript 파일들이 들어 있다. 위와 마찬가지로 ch3/static 및 ch3/polls/static 위치에 생성된다.**
* **logs dir: 프로젝트를 진행하면서 추가. 로그 파일이 있다. 로그 위치는 settings.py 파일의 LOGGING 항목으로 지정한다.**

### 뼈대 만들기

```
$ django-admin.py startproject mysite
$ python manage.py startapp polls
$ python manage.py migrate
$ python manage.py runserver
```

### 프로젝트 생성

```
django-amin.py startproject mysite
mv mysite ch3
```

mysite로 만들면 상위 mysite는 프로젝트 고나련 디렉토리와 파일을 모으는 역할만 하므로 실제 하위 mysite 프로젝트 이름과 혼동될 수 있으므로 mv로 ch3로 바꿔준다.

### 애플리케이션 생성

### 데이터베이스 변경사항 반영

### 확인해보기

## 애플리케이션 개발하기 - 설계

## 애플리케이션 개발하기 - Model 코딩

### DB 지정

### 테이블 정의

### Admin 사이트에 테이블 반영

### DB 변경사항 반영

### 확인해보기

## 애플리케이션 개발하기 - View와 Template 코딩

### URLconf 코딩

### 뷰 함수 index() 및 템플릿 작성

### 뷰 함수 detail() 및 폼 템플릿 작성

### 뷰 함수 vote() 및 리다이렉션 작성

### 뷰 함수 results() 및 템플릿 작성

### 확인해보기
