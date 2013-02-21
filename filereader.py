import random
import sys
import os

def createFile():

  pArray = [2,4,8,16,32,64]
  nArray = [2,4,6,8,10,20,30,40,50,60,70,80,90,100,200,300,400,500,600,700,800,900,1000,2000,3000,4000,5000,6000,7000,8000,9000,10000]
  if not os.path.exists("inputFiles"):
    os.makedirs("inputFiles")
  for p in pArray:
    for n in nArray:
      path = "inputX"+str(p)+"X"+str(n)
      f = open("inputFiles/"+path,'wb+')
      f.write(str(p)+'\n')
      f.write(str(n)+'\n')
      for i in range(1,int(n)+1):
        if i >= (int(n))/2:
          j=i+1
          k=i+2
        else:
          j=i
          if(i+1 >= (int(n))/2):
            k=i+2
          else:
            k=i+1
        string = str(j)
        string += " "
        if (j)%int(n) == 0:
          string += str(1)
        else:
          string += str(k)
        string += '\n'
        f.write(string)
        if (j)%int(n) == 0:
          break
      f.close()
  
if __name__ == "__main__":
  import sys
  createFile()
