import random
import sys

def createFile( path, p, n ):
  f = open(path,'wb')
  f.write(p+'\n')
  f.write(n+'\n')
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
  createFile(sys.argv[1], sys.argv[2], sys.argv[3])
