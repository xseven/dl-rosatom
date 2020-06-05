import os
import io
import shutil
from pathlib import Path
from striprtf.striprtf import rtf_to_text

currentDir = os.getcwd() 
workingDir = Path(currentDir, "source-data", "ROSATOM", "data")
outDir = Path(currentDir, "converted-data")

if outDir.exists():
    shutil.rmtree(outDir)

outDir.mkdir()

for file in os.listdir(workingDir):
    if file.endswith(".rtf"):
        print("Processing {}".format(file))
        with open(Path(workingDir, file), "r") as rtfFile:
            rtfContent = rtfFile.read()

            try:
                txtContent = rtf_to_text(rtfContent)
                fileParts = file.split('.', 1)
                with open(Path(outDir, "{}.txt".format(fileParts[0])), "w+") as txtFile:
                    txtFile.write(txtContent)
            except:
                print("Skipping")
            
