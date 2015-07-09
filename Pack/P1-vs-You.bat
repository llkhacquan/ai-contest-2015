pushd Server
start node Server.js -h 127.0.0.1 -p 3011
popd
pushd WebClient
start index.html
popd
pushd Arena
IF EXIST AI_Template.exe (
	start "Bot C++" cmd /c call AI_Template.exe -h 127.0.0.1 -p 3011
) ELSE IF EXIST P1.js (
	start node P1.js -h 127.0.0.1 -p 3011
) ELSE IF EXIST P1.jar (
	start "Bot Java" cmd /c call java -jar P1.jar -h 127.0.0.1 -p 3011
)
popd