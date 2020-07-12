#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cstdio>
#include <stdlib.h>

int parse() {

  std::ifstream file("ppDerivation_pnfsLocation.txt");
  std::string line;
  // keep track of how much Gb has been allocated to each person
  ofstream condorFile; // 4.9 Tb to use

  condorFile.open("HION7.pp.condor.job");

  float totalSize = 0.;
  while (std::getline(file, line))
    {
      std::stringstream linestream(line);
      std::string item;
      int linePos = 0;
      std::string fileName;
      while (std::getline(linestream, item, '|'))
        {
	  std::string::iterator end_pos = std::remove(item.begin(), item.end(), ' ');
	  item.erase(end_pos, item.end());

	  //	  std::cout <<  item << " linePos " << linePos << endl;
	  if (linePos == 5) {
	    fileName = item;
	    condorFile << "Executable   = condor.sh" << endl << "Arguments    = " << fileName << endl << "Queue" << endl;	    

	  }
	  ++linePos;
        }
    }    

  file.close(); 
  condorFile.close();

  return 0;

}
