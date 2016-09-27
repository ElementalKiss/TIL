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

```
python manage.py startapp polls
```

polls라는 어플리케이션을 만든다.

### 데이터베이스 변경사항 반영

```
python manage.py migrate
```

rails의 그것과 매우 유사하다.

### 확인해보기

```
python manage.py runserver 0.0.0.0:8000
```

> 확인해보니 "It worked! Congratulations on your first Django-powered page."라는 메시지의 웹 페이지가 정상 동작 하였다.

#### 관리자 계정 들어가기

```
host:8000/admin
```

#### 관리가 계정 만들기

```
python manage.py createsuperuser
```

아이디와 비밀번호를 입력하여 만들 수 있다.

기본적으로 setting.py에 django.contrib.auth 애플리케이션이 등록되기 때문에 해당 기능을 이용할 수 있다.

## 애플리케이션 개발하기 - 설계

### 예제로 짜 볼 애플리케이션

설문에 해당하는 질문을 보여주고 질문에 대한 답변과 결과를 보여주는 애플리ㅔ이션.

* index.html: 최근 실시한 질문의 리스트를 보여준다.
* detail.html: 하나의 질문에 대해 투표할 수 있도록 답변 항목을 폼으로 보여준다.
* results.html: 질문에 따른 투표 결과를 보여준다.

### Question 테이블 설계

| 컬럼명  | 타입      | 제약 조건                 | 설명   |
| ---- | ----    | ----                  | ---- |
| id   | integer | NotNull, PK, Auto Inc | PK   |
|question_text|varchar(200)|NotNull|질문 문장|
|pub_date|datetime|NotNull|질문 생성 시각|

### Choice 테이블 설계

| 컬럼명         | 타입           | 제약 조건                          | 설명       |
| ----        | ----         | ----                           | ----     |
| id          | integer      | NotNull, PK, Auto Inc          | PK       |
| choice_text | varchar(200) | NotNull                        | 답변 항목 문구 |
| votes       | integer      | NotNull                        | 투표 카운트   |
| question_id | integer      | NotNull,FK(Question.id), index | FK       |

* Question 테이블: 질문을 저장하는 테이블.
* Choice 테이블: 질문별로 선택용 답변 항목을 저장하는 테이블.
* 모든 컬럼은 Not NULL.
* PK는 Auto Inc 속성으로 지정.
* Choice 테이블의 question_id 컬럼은 Question의 FK 관계로 연결. 인덱스로 생성하도록 하였다.

## 애플리케이션 개발하기 - Model 코딩

### DB 지정

settings.py 설정 파일은 프로젝트 전반적인 사항들을 설정해주는 곳이다. 

```
# Database
# https://docs.djangoproject.com/en/1.10/ref/settings/#databases

DATABASES = {
    'default': {
        'ENGINE': 'django.db.backends.sqlite3',
        'NAME': os.path.join(BASE_DIR, 'db.sqlite3'),
    }
}
```

### polls 애플리케이션 등록

```
# Application definition

INSTALLED_APPS = [
    'django.contrib.admin',
    'django.contrib.auth',
    'django.contrib.contenttypes',
    'django.contrib.sessions',
    'django.contrib.messages',
    'django.contrib.staticfiles',
    'polls',
]
```

### 타임 존 지정

```
LANGUAGE_CODE = 'en-us'

#TIME_ZONE = 'UTC'

TIME_ZONE = 'Asia/Seoul'
```

### 테이블 정의

models.py 작성

```
from django.db import models

# Create your models here.

class Question(models.Model):
  question_text = model.CharField(max_length=200)
  pub_date = models.DateTimeField('data published')

  def __unicode__(self): # __str__ on Python 3
    return self.question_text

class Choice(models.Model):
  question = models.ForeignKey(Question)
  choice_text = models.CharField(max_length=200)
  votes = models.IntegerField(default=0)

  def __unicode__(self): # __str__ on Python 3
    return self.choice_text
```

* PK는 클래스에 지정해주지 않아도 이름을 접두어로 자동으로 생성해준다.
* DateTimeField() 필드 클래스에 정의한 문구는 pub_date 컬럼에 대한 레이블 문구이다. Admin 사이트에서 이 문구를 보게 된다.
* FK는 다른 테이블의 PK와 항상 연결되므로 클래스만 연결시켜 주면 된다.
* __unicode()__ 함수는 객체를 스트링으로 표현할 때 사용하는 함수이다. 파이썬 3에서는 __str__() 함수로 변경되었다.

### Admin 사이트에 테이블 반영

Admin 사이트에 장고에서 기본으로 제공하는 Users, Groups 테이블이 보인다. models.py 파일에서 정의한 테이블도 Admin 사이트에서 보게 하려면 admin.py 파일에 등록하면 된다.

```
from django.contrib import admin

# Register your models here.

from django.contrib import admin
from polls.models import Question, Choice

admin.site.register(Question)
admin.site.register(Choice)
```

models.py 모듈에서 정의한 두 클래스를 임포트 하고, admin.site.register 함수를 이용해 Admin 사이트에 등록해주면 된다.

**테이블을 새로 만들때는 models.py와 admin.py 두 개의 파일을 함께 수정해야 한다는 것을 기억해두자.**

### DB 변경사항 반영

```
python manage.py makemigrations
python manage.py migrate
```

#### 실행

```
[elkiss@elkiss-centos ch3]$ python manage.py makemigrations
Migrations for 'polls':
  polls/migrations/0001_initial.py:
    - Create model Choice
    - Create model Question
    - Add field question to choice
[elkiss@elkiss-centos ch3]$ python manage.py migrate
Operations to perform:
  Apply all migrations: admin, auth, contenttypes, polls, sessions
Running migrations:
  Applying polls.0001_initial... OK
```

makemigrations 명령을 하면 polls/migrations 디렉토리 하위에 마이그레이션 파일들이 생긴다.

### 확인해보기

![alt text](https://github.com/elementalkiss/til/django/img/1.png "Admin View")

외부에서 접근하기 위해

```
python manage.py runserver 0.0.0.0:8000
```

## 애플리케이션 개발하기 - View와 Template 코딩

### URLconf 코딩

### 뷰 함수 index() 및 템플릿 작성

### 뷰 함수 detail() 및 폼 템플릿 작성

### 뷰 함수 vote() 및 리다이렉션 작성

### 뷰 함수 results() 및 템플릿 작성

### 확인해보기
