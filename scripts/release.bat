IF EXIST *.zip del /S *.zip
python zip_release.py
hlt bot -b release.zip
PAUSE