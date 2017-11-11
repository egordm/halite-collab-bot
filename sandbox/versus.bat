IF EXIST *.hlt del /S *.hlt
IF EXIST *.log del /S *.log

SET root=cmake-build-debug
SET enemy=enemy
SET /p enemy="Enemy bot: "
.\halite.exe -d "240 160" "..\%root%\MyBot.exe" ".\version\%enemy%.exe"
PAUSE