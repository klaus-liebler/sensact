
import sys
import fileinput
import glob

source = glob.glob("*.kicad_pcb")[0]
target = source+"-modified"
sourceHandle=open(source, mode="r")
sourceLines = sourceHandle.read().splitlines()
targetLines=[]

for line in sourceLines:
    if(line.find("fp_text reference")!=-1 and line.find("hide")==-1):
        line+= " hide"
    if(line.find("fp_text value")!=-1 and line.find("hide")!=-1):
        line=line.replace("hide", "")
    if(line.find("fp_text value")!=-1):
        line=line.replace("layer F.Fab", "layer F.SilkS")
        line=line.replace("layer B.Fab", "layer B.SilkS")
    targetLines.append(line)

targetHandle = open(target, "w")
targetHandle.write("\n".join(targetLines))