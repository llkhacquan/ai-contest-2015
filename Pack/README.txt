* Version: 1.5b

* Change log:
- 1.5b
	+ Add version checking
- 1.5:
	+ Allow thinking in opponent's turn
- 1.4b:
	+ Java template: Rebuild class with jdk 1.6.0_18
	+ fix P1-vs-You.bat to support Java bot
- 1.4:
	+ Add Java template
	+ C++ template: fix macro CONVERT_COORD
- 1.3b:
	+ Fix bug build
- 1.3:
	+ C++ template supports VS2012, rename build modes for easier access
	+ Minor fixes
- 1.2:
	+ C++ template supports VS2013. Use Debug_2013 or Release_2013  to build
- 1.1:
	+ Change game map to 11x11
	+ Rename template
- 1.0: First version

* How to use:
- Install NodeJS: https://nodejs.org/download/
- You can code in Javascript (for NodeJS), C++ or Java
- You can do stuffs in opponent's turn. But be careful! If you take too much time, you may miss your turn and lose.
- How to build:
	+ Javascript: No need to build. Bot file: Client.js
	+ C++: Use build mode that match your compiler version. E.g. You use VS2012, then you should build using 2012_Debug or 2012_Release. Bot file: AI_Template.exe
	+ Java: Run #build.bat (Use jdk 1.6.0_18 or later). Bot file: Client.jar
- Copy your bot file to Arena folder, and rename it as P1. (P1.exe, P1.js or P1.jar)
- Run P1-vs-You.bat to fight with your own bot for testing purpose
- If you want to play against someone else, copy their bot to Arena folder and rename it as P2. (P2.exe, P2.js or P2.jar)
- Run P1-vs-P2.bat
- Cross platform is OK (for example, P1.exe vs P2.js)