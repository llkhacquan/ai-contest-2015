pushd Server
start node Server.js -h 127.0.0.1 -p 3011
popd
pushd WebClient
start index.html
popd
pushd Arena
REM IF EXIST P1.exe (
	REM start "Bot C++" cmd /c call P1.exe -h 127.0.0.1 -p 3011
REM )
REM ELSE IF EXIST P1.js (
	REM start node P1.js -h 127.0.0.1 -p 3011
REM )
popd