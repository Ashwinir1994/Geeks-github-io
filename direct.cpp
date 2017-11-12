#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

using std::cout;
using std::string;
using std::vector;
using std::endl;
using std::ifstream;
using namespace std;
int c =1;
void correct(string str)
{
      int flag=0;
      int len=str.length();
      for(int i=0;i<len;i++)
      {
        if(str[i]=='/' && str[i+1]=='/')
        {
          str.erase(remove(str.begin()+i,str.begin()+i+1,str[i+1]), str.begin()+i+1);
        }
        else
          continue;
      }
      cout<<c<<" "<<str<<endl;
      c++;
}

void printDirectoryStructure(string dir, string prefix, vector<string>& ignore) 
{
      string filepath;
      DIR *dp;
      struct dirent *dirp;
      struct stat filestat;

      /**
       * Open current directory
       */
      dp = opendir(dir.c_str());
      
      /**
       * Error check & base case
       */
      if (dp == NULL) 
      {
        cout << "Error " << errno << " while trying opening" << dir << endl;
        return;
      }

      //Recurse through all sub-directories
      string fl;
      while (dirp = readdir(dp)) 
      {
        
        filepath = dir + "/" + dirp->d_name;
        
        correct(filepath);   
        string nameString(dirp->d_name);
        
        if(std::find(ignore.begin(), ignore.end(), nameString) != ignore.end() || std::find(ignore.begin(), ignore.end(), filepath) != ignore.end()) 
        {
          continue;
        } 
        else 
        {
            string tmpprefix = prefix;      
            fl = tmpprefix+dirp->d_name;
        }

        if (stat(filepath.c_str(), &filestat)) 
        {
            continue;
        }
        
        if (S_ISDIR(filestat.st_mode)) 
        {
            printDirectoryStructure(filepath,fl , ignore);
        }
      
      }
      closedir(dp);
}
//A vector to store all files to be ignored
  vector<string> buildIgnoreVector() 
  {
      vector<string> ignore;
      ignore.push_back(".");
      ignore.push_back("..");
      ignore.push_back("node_modules");
      ignore.push_back("bower_components");
      ignore.push_back(".git");
      return ignore;
  }

  int main(int argc, char *argv[]) 
  {
      if (!argv[1]) 
      {
        cout << "Give a directory name" << endl;
        return -1;
      }

      string dir(argv[1]);
      vector<string> ignore = buildIgnoreVector();
     
      for (int i = 2; i < argc; ++i) 
      {
          ignore.push_back(string(argv[i]));
      }
      
      printDirectoryStructure(dir, "", ignore);
      
  return 0;
}