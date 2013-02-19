import random
import sys

def createFile( p, n, amount ):
  f = open('test.in','wb')
  f.write(p+'\n')
  f.write(n+'\n')
  for i in range(int(amount)):
  	if i != int(amount)/2:
  		string = str(i+1)
  		string += " "
  		string += str(i+2)
  		string += '\n'
  		f.write(string)
  f.close()
  
if __name__ == "__main__":
  import sys
  createFile(sys.argv[1], sys.argv[2], sys.argv[3])
