
import sys
import fileinput

source = "sensactOutdoor.kicad_pcb";
target = source+"-modified"
sourceHandle=open(source, mode="r")
sourceLines = sourceHandle.read().splitlines()
targetLines=[]

for line in sourceLines:
    if(line.find("fp_text reference")!=-1 and line.find("hide")==-1):
        line+= " hide"
    if(line.find("fp_text value")!=-1 and line.find("hide")!=-1):
        line=line.replace("hide", "")
    targetLines.append(line)

targetHandle = open(target, "w")
targetHandle.write("\n".join(targetLines))