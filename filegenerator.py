import random
import sys

def createFile( p, n, amount ):
  f = open('test.in','wb')
  f.write(p+'\n')
  f.write(n+'\n')
  for i in range(int(amount)):
  	string = str(random.randint(1,int(n)))
  	string += " "
  	string += str(random.randint(1,int(n)))
  	string += '\n'
  	f.write(string)
  f.close()
  
if __name__ == "__main__":
  import sys
  createFile(sys.argv[1], sys.argv[2], sys.argv[3])
