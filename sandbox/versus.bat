IF EXIST *.hlt del /S *.hlt
IF EXIST *.log del /S *.log

SET root=cmake-build-debug
SET enemy=enemy
SET /p enemy="Enemy bot: "
SET /A width=%RANDOM% * 264 / 32768 + 120
SET /A height=%RANDOM% * 136 / 32768 + 120

.\halite.exe -d "%width% %height%" "..\%root%\halite_orion.exe" ".\version\%enemy%.exe"
PAUSE