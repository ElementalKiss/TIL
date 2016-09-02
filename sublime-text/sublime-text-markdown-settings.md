# Install Package Control

* [Package Control](https://packagecontrol.io/installation) 설치
* control + ` 로 콘솔 창 열기, 아래 코드 삽입 후 엔터

import urllib.request,os,hashlib; h = '2915d1851351e5ee549c20394736b442' + '8bc59f460fa1548d1514676163dafc88'; pf = 'Package Control.sublime-package'; ipp = sublime.installed_packages_path(); urllib.request.install_opener( urllib.request.build_opener( urllib.request.ProxyHandler()) ); by = urllib.request.urlopen( 'http://packagecontrol.io/' + pf.replace(' ', '%20')).read(); dh = hashlib.sha256(by).hexdigest(); print('Error validating download (got %s instead of %s), please try manual install' % (dh, h)) if dh != h else open(os.path.join( ipp, pf), 'wb' ).write(by)

* restart

# 추가 플러그인 설치하기

* cmd + shift + p 로 Package Control: Install Package 선택
* MarkdownEditing: 코드 이쁘게
* SmartMarkdown: 헤더 단위로 접는 폴딩 기
* Markdown Preview: 미리보기