# Django 개발 환경 설치

## 장고 버전에 따른 파이썬 버전

[공식홈페이지 정보](https://docs.djangoproject.com/en/1.10/faq/install/#faq-python-version-support)


| Django version | Python versions                                 |
| ---            | ---                                             |
| 1.8            | 2.7, 3.2 (until the end of 2016), 3.3, 3.4, 3.5 |
| 1.9, 1.10      | 2.7, 3.4, 3.5                                   |
| 1.11           | 2.7, 3.4, 3.5, 3.6                              |
| 2.0            | 3.5+                                            |

> 일단 python 3.4, django 1.10 버전을 기준으로 하자.

## Centos7 x64

digital ocean 5천원짜리 서버 RAM 512MB, 1CPU, 20GB SSD

### 파이썬 설치

현재 2.7이 깔려 있어 3.4로 업그레이드 하려고 한다. 그냥 상위 버전 설치하면 되는 듯.

#### 필수 라이브러리 설치

```
sudo yum groupinstall "Development tools"
sudo yum install zlib-devel bzip2-devel openssl-devel ncurses-devel sqlite-devel readline-devel tk-devel gdbm-devel db4-devel libpcap-devel xz-devel
```

#### 파이썬 다운로드 및 설치

```
wget http://python.org/ftp/python/3.4.5/Python-3.4.5.tar.xz
tar xf Python-3.4.5.tar.xz
cd Python-3.4.5
./configure --prefix=/usr/local --enable-shared LDFLAGS="-Wl,-rpath /usr/local/lib"
sudo make && sudo make altinstall
```

#### 심볼릭 링크로 버전 변경

```
sudo ln -sf /usr/local/bin/python3.4 /usr/bin/python
```

#### pip 프로그램 설치

> What a pip? pip(Python Install Program) 프로그램은 파이썬의 오픈소스 저장소인 PyPi(Python Package Index)에 있는 SW 패키지를 설치하고 관리해주는 명령.

```
wget https://bootstrap.pypa.io/get-pip.py
sudo python get-pip.py
```

#### 장고 설치

```
sudo pip install Django==1.10.1
```

### Troubleshooting

* sudo pip 안 될 때

여러 방법이 있지만 local/bin에 있는걸 /bin으로 심볼릭 링크

```
$ sudo ln -s /usr/local/bin/pip /usr/bin/pip
```

## Windows7 x64

### Troubleshooting

## Mac OSX 10.11.6

### Troubleshooting
