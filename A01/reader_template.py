# Autograph
"""
This code will automatically write a comma delimited stuff for stuff.   
"""
import sys
import os
import time


def viewFile():
  pArray = [2,8,32,128,512]
  nArray = [2,4,6,8,10,20,30,40,50,60,70,80,90,100,200,300,400,500,600,700,800,900,1000]
  for p in pArray:
    for n in nArray: 
      firsttime = time.time()
      newdata=os.system("./wtc 1 inputFiles/inputX"+str(p)+"X"+str(n))
      endtime = time.time()-firsttime
      sys.stdout.write(str(endtime)+",")
    print ""
    
if __name__ == "__main__":
  import sys
  viewFile()
