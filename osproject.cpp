#include<bits/stdc++.h>
#include<iostream>
#include<string>
#include<stdlib.h>
#include<string.h>
#include "md5.h"

using namespace std;

vector<int>chk1;
string str=""; // this is the recovery string
queue<int>q;  // contains the block references matched
int c=0,jc;
void createnew(string s,FILE *fpnew){

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
        for(j=0;j<bsize&&(jc+j)<len2;j++)
	       m=m+s2[jc+j];
        m[j]='\0';

        mm=mm+md5(m.c_str());
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
            str=str+"$";
	        str=str+ s2[jc];  // we add the modified data with $ as delimiter for the unmatched data.
	        jc++;
	        flag=1;
            break;
	    }
	}
  }
  if(i==n)  // if the checksum does not match to any of the checksum values of OLD string.
    {
        str=str+"$";
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
    for(i=jc;i<jc+bsize&&s2[i]!='\0';i++)   // Calculation of checksum value for every string of block size
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
int main(){
    FILE *fp = fopen("hima.txt","a+");
    FILE *fpn = fopen("ffnew.txt","a+");
    if(fp==NULL||fpn==NULL)
        cout<<"File not found";
    else
    {
        string s1="";
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
            else break;
        }
         string s2="";
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
       cout<<s1.length();
        if(s1.length()==0)
        {
            createnew(s2,fp);
        }
        else{
        int bsize=10;            // block size
        vector<string> md5chksum;
        int n,m,c;
        char ch='0';
        n=s1.length();
        m=s2.length();
        int a=0,b=0;
        int i,j,k,len;
        int M=1<<16;
        len=ceil((float)s1.length()/(float)bsize);
        int chk[len],rchk[n];
        for(j=0;j<len;j++)
        {
            k=bsize;
            a=0;
            for(i=bsize*j;i<bsize*j+bsize&&s1[i]!='\0';i++)
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
            for(i=j;i<j+bsize&&s1[i]!='\0';i++)
            {
                a=(a%M+s1[i]%M)%M;
                ss=ss+s1[i];
            }
            md5chksum.push_back(md5(ss.c_str()));
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
            ss[j]='\0';
            md5chksum.push_back(md5(ss.c_str()));
            rchk[n-k]=a;
            k--;

        }

        rch(s2,rchk,bsize,n,md5chksum);
        FILE *fpe = fopen("ff.txt","w");
      //  cout<<str<<endl;
        for(i=0;str[i]!='\0';i++){
               if(str[i]=='@')
               {
                   int qq=q.front();
                   q.pop();
                   for(j=qq;j<qq+bsize;j++)
                    fwrite(&s1[j],1,1,fpe);
               }
                else if(str[i]=='$')
                    continue;
                else
                    fwrite(&str[i],1,1,fpe);
               }

    }
    }
	return 0;
}
