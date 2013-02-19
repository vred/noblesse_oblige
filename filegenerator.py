import random
import sys

def createFile( path, p, n ):
  f = open(path,'wb')
  f.write(p+'\n')
  f.write(n+'\n')
  j = 0
  for i in range(int(n)):
  	if j%int(n) != (int(n))/2:
  		string = str(j%int(n)+1)
  		string += " "
  		if (j+1)%int(n) == 0:
  			string += str(1)
  		else:
  			string += str(j%int(n)+2)
  		string += '\n'
  		f.write(string)
  	j=j+1
  f.close()
  
if __name__ == "__main__":
  import sys
  createFile(sys.argv[1], sys.argv[2], sys.argv[3])
