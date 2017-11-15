#include <bits/stdc++.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include<fstream>
#include <errno.h>
#include "md5.h"

using std::cout;
using std::string;
using std::vector;
using std::endl;
using std::ifstream;
using namespace std;

int c =1,filecounter=0;
vector<int>chk1;
string str="";
// this is the recovery string
queue<int>q;  // contains the block references matched
int jc;

void createnew(string s,FILE *fpnew)
{

    for(int i=0;s[i]!='\0';i++)
        fwrite(&s[i],1,1,fpnew);
}

int chksum(int rchk[],int v,string s2,int n,vector<string>md5chksum,int bsize)
{
    int i,flag=0,j;
    int len2=s2.length();

    for(i=0;i<n;i++)
    {
      	if(rchk[i]==v)  // if the checksum of a block in the NEW string matches with any of the checksum of the OLD string
      	{
      	      string m="",mm="";
              for(j=0;j<bsize && (jc+j)<len2;j++)
      	            m = m + s2[jc+j];
              //m[j]='\0';

               mm = mm + md5(m.c_str());
               if(mm.compare(md5chksum[i])==0)
          	    {
                    string ch="@";
                    string buffer;
                    str=str+ch;
                    q.push(i);
                   // str=str+ch; // we push @ into str if we find a matched checksum
                    flag=0;
                    jc=jc+bsize;
                    break;
                }
                else
          	    {
          	        string ch1="$";
                    str=str+ch1;
          	        str=str+ s2[jc];  // we add the modified data with $ as delimiter for the unmatched data.
          	        jc++;
          	        flag=1;
                      break;
          	    }
    	  }
    }
    if(i==n)  // if the checksum does not match to any of the checksum values of OLD string.
    {
        string ch1="$";
        str=str+ch1;
        str=str+ s2[jc];  // we add the modified data with $ as delimiter for the unmatched data.
        jc++;
        flag=1;
    }
    return flag;
}
void rch(string s2,int rchk[],int bsize,int n,vector<string>md5chksum)
{
    int i,k=0,flag=0;
    int M=1<<16;
    int a=0,b=0;
    int cnt=bsize;
    int len=s2.length();
    vector<int>arr,barr;
    jc=0;
	  while(jc<len)
  	{
    	  if(flag==0)
        {
            a=0;
            for(i=jc;i<jc+bsize && s2[i]!='\0';i++)   // Calculation of checksum value for every string of block size
                a=(a%M+s2[i]%M)%M;
        }

        else     // calculation of rolling checksum
        {
            if(jc+bsize>len)
                    a=((a%M)-(s2[jc-1])%M)%M;

            else
                    a=((a%M)-(s2[jc-1])%M+(s2[jc+bsize-1]%M))%M;

        }

        int v;
    	  chk1.push_back(a);
    	  v=a;
    	  flag=chksum(rchk,v,s2,n,md5chksum,bsize);
    }
}

static void mkdirRecursive(const char *path, mode_t mode) 
{
    char opath[PATH_MAX];
    char *p;
    size_t len;

    strncpy(opath, path, sizeof(opath));
    opath[sizeof(opath) - 1] = '\0';
    len = strlen(opath);
    if (len == 0)
        return;
    else if (opath[len - 1] == '/')
        opath[len - 1] = '\0';
    for(p = opath; *p; p++)
        if (*p == '/') 
        {
            *p = '\0';
            if (access(opath, F_OK))
                mkdir(opath, mode);
            *p = '/';
        }
    if (access(opath, F_OK))         /* if path is not terminated with / */
        mkdir(opath, mode);
}

string correct(string st,string des)
{
      int flag=0;
      int len=st.length();
      for(int i=0;i<len;i++)
      {
        if(st[i]=='/' && st[i+1]=='/')
        {
          st.erase(remove(st.begin()+i,st.begin()+i+1,st[i+1]), st.begin()+i+1);
        }
        else
          continue;
      }
      //cout<<c<<" "<<str<<endl;
      c++;
      return des+""+st;
}

//removing filenames from end of path

string trim(string st)
{
      int len=st.length();
      for(int i=len-1;i>=0;i--)
      {
        if(st[i]=='/')
        {
          st.erase(st.begin()+i,st.end());
          break;
        }

      }
      return st;
}

/* A function to traverse through the directory structure recursively */
void printDirectoryStructure(string dir, string prefix, vector<string>& ignore,string des)
{
      string filepath;
      DIR *dp;
      struct dirent *dirp;       //a structure for traversing in directory
      struct stat filestat;

      /*
       * Open current directory
       */

      dp = opendir(dir.c_str());

      /*
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
        string new_path,des_path;
        new_path = correct(filepath,des);
         string news=new_path;
        /*****************trimming last /*******************/

        new_path=trim(new_path);

        const char *np = new_path.c_str();
        mkdirRecursive(np, S_IRWXU);
        struct stat s;
        if( stat(filepath.c_str(),&s) == 0 )
        {

         if( s.st_mode & S_IFREG )   // checking whether the path represents a file or not
        {
            if(s.st_mode & S_IWUSR)   //checking whether the file is writable or not
            {
                str.clear();

                FILE *fp = fopen(news.c_str(),"ab+");          //OLD
                FILE *fpn = fopen(filepath.c_str(),"ab+");     //NEW
                if(fp==NULL||fpn==NULL)
                    cout<<"File not found";
                else
                {
                    filecounter++;
                    string s1="";
                    s1.clear();
                    char buffer[50]={0};
                    while(1)
                    {
                        if(!feof(fp))
                        {
                            int v = fread(buffer,1,50,fp);
                            if(v>0)
                            {
                                buffer[v]='\0';
                                s1=s1+buffer;
                                memset(buffer,0,sizeof(buffer));
                                if (v <50)
                                {

                                    if (feof(fp))
                                        break;
                                }

                            }
                        }
                        else
                            break;
                    }
         string s2="";
         s2.clear();
         memset(buffer,0,sizeof(buffer));
         while(1)
        {

            int v = fread(buffer,1,50,fpn);
            if(v>0)
            {
                buffer[v]='\0';
                s2=s2+buffer;
                memset(buffer,0,sizeof(buffer));
                if (v <50)
                {
                    if (feof(fpn))
                        break;
                }
            }
        }
        if(s1.length()==0)
        {
            createnew(s2,fp);
        }
        else
        {
            int bsize=10;            // block size in which we are dividing our file
            vector<string> md5chksum;
            int n,m,c;
            char ch='0';
            n=s1.length();
            m=s2.length();
            int a=0,b=0;
            int i,j,k,len;
            int M=1<<16;
            len=ceil((float)s1.length()/(float)bsize);
            int chk[len],rchk[n];  // array for storing checksums
            for(j=0;j<len;j++)
            {
                k=bsize;
                a=0;
                for(i=bsize*j;i<bsize*j+bsize && s1[i]!='\0';i++)
                {
                    a=(a%M+s1[i]%M)%M;

                }
                chk[j]=a; // checksum for every block in the OLD string
            }

            a=0;
            for(j=0;j<=n-bsize;j++)
            {
                a=0;
                string ss="";
                for(i=j;i<j+bsize && s1[i]!='\0';i++)
                {
                    a=(a%M+s1[i]%M)%M;                      //rolling checksum for each  block
                    ss=ss+s1[i];
                }

                md5chksum.push_back(md5(ss.c_str()));    //calculating md5 checksum for each block
                rchk[j]=a;
            }

            k=bsize-1;

            while(k)
            {
                a=0;
                string ss="";
                for(j=n-k;j<n;j++)
                {
                    a=(a%M+s1[j]%M);
                    ss=ss+s1[j];
                }
                md5chksum.push_back(md5(ss.c_str()));
                rchk[n-k]=a;
                k--;
            }

            fclose(fp);
            fclose(fpn);
            str.clear();

            rch(s2,rchk,bsize,n,md5chksum);

            FILE *fpe = fopen(news.c_str(),"w");

            cout<<endl<<endl<<"*******Contents of snapshot of file "<<filecounter<<":********"<<endl<<endl;
            cout<<endl<<"********Recovery String********"<<endl<<endl<<str<<endl;

            queue<int>qnew;

            cout<<endl<<"*********Block references of matched data**********"<<endl<<endl;
            while(!q.empty())
            {
                    qnew.push(q.front());
                    cout<<q.front()<<" ";
                    q.pop();
            }
            cout<<endl;
            s1[s1.length()]='\0';
            str[str.length()]='\0';
            for(i=0;str[i]!='\0';i++)
            {
                if(str[i]=='@')                //using the recovery string to reflect the changes to the actual data
                {
                    int qq=qnew.front();
                    qnew.pop();
                    for(j=qq;j<qq+bsize && j<s1.length();j++)
                    {
                        fwrite(&s1[j],1,1,fpe);
                        fflush(fpe);

                    }
                }
                else
                    if(str[i]=='$')
                        continue;
                    else
                    {
                        fwrite(&str[i],1,1,fpe);
                        fflush(fpe);
                    }
            }
            fclose(fpe);
            str.clear();
        }
    }
    }

        }
        }
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
             // checking if it's a directory ; if it is then make a recursive call
        if (S_ISDIR(filestat.st_mode))
        {
            printDirectoryStructure(filepath,fl, ignore,des);
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
      if (!argv[2])
      {
        cout << "Give a Destination directory name" << endl;
        return -1;
      }

      string dir(argv[1]);
      string des(argv[2]);
      vector<string> ignore = buildIgnoreVector();

      for (int i = 3; i < argc; ++i)
      {
          ignore.push_back(string(argv[i]));
      }

      printDirectoryStructure(dir, "", ignore,des);    //passing source and destination as parameters

  return 0;
}
