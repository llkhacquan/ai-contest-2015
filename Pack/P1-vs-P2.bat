pushd Server
start node Server.js -h 127.0.0.1 -p 3011 -k 30 11
popd
pushd WebClient
start index.html
popd
pushd Arena

IF EXIST AI_Template.exe (
	ping 1.1.1.1 -n 1 -w 1000 > nul
	start "Bot C++" cmd /c call AI_Template.exe -h 127.0.0.1 -p 3011 -k 30
) ELSE IF EXIST P1.js (
	start node P1.js -h 127.0.0.1 -p 3011 -k 30
) ELSE IF EXIST P1.jar (
	IF EXIST tyrus-standalone-client-1.10.jar (
		start "Bot Java" cmd /c call java -jar P1.jar -h 127.0.0.1 -p 3011 -k 30
	) ELSE (
		echo tyrus-standalone-client-1.10.jar missing. Please copy it to Arena folder.
	)
)

IF EXIST P2.exe (
	ping 1.1.1.1 -n 1 -w 1000 > nul
	start "Bot C++" cmd /c call P2.exe -h 127.0.0.1 -p 3011 -k 11
) ELSE IF EXIST P2.js (
	start node P2.js -h 127.0.0.1 -p 3011 -k 11
) ELSE IF EXIST P2.jar (
	IF EXIST tyrus-standalone-client-1.10.jar (
		start "Bot Java" cmd /c call java -jar P2.jar -h 127.0.0.1 -p 3011 -k 11
	) ELSE (
		echo tyrus-standalone-client-1.10.jar missing. Please copy it to Arena folder.
	)
)
popd