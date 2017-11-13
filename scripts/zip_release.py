import os
import zipfile

files = ['MyBot.cpp', 'CMakeLists.txt']
folders = ['hlt', 'bot']


def zipdir(path, ziph):
    for root, dirs, files in os.walk(path):
        for file in files:
            ziph.write(os.path.join(root, file),
                       os.path.relpath(os.path.join(root, file),
                                       os.path.join(path, '..')))


with zipfile.ZipFile("release.zip", "w") as zipf:
    for file in files:
        zipf.write('../' + file, file, zipfile.ZIP_DEFLATED)
    for folder in folders:
        zipdir('../' + folder, zipf)
