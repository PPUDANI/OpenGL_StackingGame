# OpenGL_StackingGame
Stacking_Game은 OpenGL(GLUT32 사용)과 MFC를 사용하여 만든 게임 프로젝트 입니다!

 - Blog Review : https://hardroot.tistory.com/78
 - YouTube : https://www.youtube.com/watch?v=PiNPRnHT_dE


[컴파일 방법]

1. MFC가 없다면 Visual Studio Installer에서 MFC를 추가해주세요!

2. glut 파일에 있는 .h와 .dll을 아래 디렉터리에 복사해주세요 (운영체제에 따라 디렉터리가 다를 수도 있음)
   - glut32.dll은 C:\Windows\SysWOW64 디렉터리에 복사
   - glut32.lib와 glaux.lib는 C:\Program Files(x86)\Window Kits\10\lib\10.0.18362.0\um\x86 디렉터리에 복사
   - glut.h와 glaux.h는 C:\Program Files(x86)\Window Kits\10\Include\10.0.18362.0\um\gl 디렉터리에 복사

3. Visual Studio 프로젝트 > 속성 > 링커 > 입력 > 추가 종속성에 다운받았던 파일들을 적어주세요!

4. 컴파일 시 32bit(x86)로 컴파일 해주세요!
