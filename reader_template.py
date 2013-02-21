# Autograph
"""
This code will automatically write a comma delimited stuff for stuff.   
"""
import sys
import os


def viewFile():
  pArray = [2,4,8,16,32,64]
  nArray = [2,4,6,8,10,20,30,40,50,60,70,80,90,100,200,300,400,500,600,700,800,900,1000,2000,3000,4000,5000,6000,7000,8000,9000,10000]
  for p in pArray:
    for n in nArray:
      newdata=os.system("./wtc 1 inputFiles/inputX"+str(p)+"X"+str(n))
      print newdata

if __name__ == "__main__":
  import sys
  viewFile()
