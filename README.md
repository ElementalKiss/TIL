# TIL
Today I Learned

# Conventions
* .md [GFM](https://help.github.com/categories/writing-on-github/)을 사용한다.
* 제목은 모두 소문자로 구성한다.
* 단어 구분은 하이픈(-)으로 한다.
* [학습종류]-[제목] ex) cpp-dynamic-array
* [Markdown Cheatsheet](https://github.com/adam-p/markdown-here/wiki/Markdown-Cheatsheet)

# Gollum Wiki

## gollum 설치 및 실행(centos)
* rvm 설치. 현재 버전은 2.2.2

예전에 rails 설치하면서 다양한 삽질을 한 것 같은데 다시 설치할 일 있으면 trouble shooting을 해야겠다.

* gollum 설치

```
sudo yum group install "Development Tools"
sudo yum install ruby ruby-devel libicu libicu-devel zlib zlib-devel git
sudo gem install gollum
```
* git 리포지토리에서 gollum 명령어로 실행
* address:4567 로 접근

# Troubleshooting