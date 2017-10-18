#include<bits/stdc++.h>
#include<iostream>
#include<string>
#include<stdlib.h>
#include<string.h>
using namespace std;

vector<int>chk1;
string str; // this is the recovery string
queue<int>q;  // contains the block references matched
int c=0,jc;
int chksum(int chk[],int v,string s2,int len)
{
    int i,flag=0;
    int len2=s2.length();
    for(i=0;i<len;i++)
  {
	if(chk[i]==v)  // if the checksum of a block in the NEW string matches with any of the checksum of the OLD string
	{
	    string ch="@";
	    string buffer;
	    str=str+ch;
	    q.push(i);
        str=str+ch; // we push @ into str if we find a matched checksum
        flag=0;
        jc=jc+4;
        break;

	}
  }
  if(i==len)  // if the checksum does not match to any of the checksum values of OLD string.
    {
        str=str+"$";
        str=str+ s2[jc];  // we add the modified data with $ as delimiter for the unmatched data.
        jc++;
        flag=1;


    }
    return flag;
}
void rch(string s2,int chk[],int bsize,int len2)
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
	    //k=bsize;
    if(flag==0)
    {
    for(i=jc;i<jc+bsize&&s2[i]!='\0';i++)   // Calculation of checksum value for every string of block size
        a=(a%M+s2[i]%M)%M;

	cout<<a<<endl;
    }
     else     // calculation of rolling checksum
    {
        if(jc+bsize>=len)
                a=((a%M)-(s2[jc-1])%M)%M;

        else
                a=((a%M)-(s2[jc-1])%M+(s2[jc+bsize-1]%M))%M;

    }
     int v;
	chk1.push_back(a);
	v=a;
	cout<<v<<endl;
	flag=chksum(chk,v,s2,len2);
	cout<<"flag="<<flag<<endl;
	}
}
int main(){
	string s1,s2;
	int bsize=4;

	cout<<"enter string1";
	cin>>s1; // OLD
	cout<<"enter string2";
    cin>>s2;  //NEW
	int n,m,c;
	char ch='0';
	n=s1.length();
	m=s2.length();
	int a=0,b=0;
	int i,j,k,len;
	int M=1<<16;
	len=ceil((float)s1.length()/(float)bsize);
	int chk[len];
	for(j=0;j<len;j++)
	{
	  k=bsize;
	for(i=bsize*j;i<bsize*j+bsize&&s1[i]!='\0';i++)
	{
		a=(a%M+s1[i]%M)%M;
	}
    chk[j]=a; // checksum for every block in the OLD string
	}
	for(j=0;j<len;j++)
	  cout<<chk[j]<<endl;
    rch(s2,chk,bsize,len);

	cout<<str;
	return 0;
}
