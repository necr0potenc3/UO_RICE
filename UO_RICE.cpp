// UO_RICE.cpp : Definiert den Einsprungpunkt für die Konsolenanwendung.
//

#include "stdafx.h"
#include "stdlib.h"
#include "string.h"
#include "conio.h"
#include <time.h>

#include "mini_flex.h"
#include "UO_RICE.h"

#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;

void strup(char* in)
{
  if (in==NULL) return;
  int s=strlen(in);
  int a;
  for (a=0; a<s; a++) if (in[a]>=97 && in[a]<=122) in[a]-=32;  
}

int find_verinfo(char *clientpath, int &type, int &patch)
{
   ifstream in;
   string my_str(clientpath);
   string line,line2;
   int typ=-1;

   my_str = my_str + "verinfo";

   in.open(my_str.c_str(), ios::in );
   if (!in.is_open()) { cout << "error opening client dir !" << endl; return -1;}

   if ( !(getline(in, line) ) ) { cout << "error opening client dir 2" << endl; return -1; } 
   if ( !(getline(in,line2) ) ) { cout << "error opening client dir 3" << endl; return -1; }

   in.close();

   int g,h,i,j,k,p,m,n;
   g = line2.find("Win32_LBR2D");
   h = line2.find("Win32_LBR3D");
   i = line2.find("Win32_UO_Gold");
   j = line2.find("Win32_UOTD");
   m = line2.find("Win32_AOS2D");
   n = line2.find("Win32_AOS3D");

   
   if      ( g!=-1 && h==-1 && i==-1 && j==-1 && m==-1 && n==-1 )  { k=g+12; typ = 2; }
   else if ( g==-1 && h!=-1 && i==-1 && j==-1 && m==-1 && n==-1 )  { k=h+12; typ = 3; }
   else if ( g==-1 && h==-1 && i!=-1 && j==-1 && m==-1 && n==-1)   { k=i+14; typ = 0; }
   else if ( g==-1 && h==-1 && i==-1 && j!=-1 && m==-1 && n==-1)   { k=j+11; typ = 1; }
   else if ( g==-1 && h==-1 && i==-1 && j==-1 && m!=-1 && n==-1)   { k=m+12; typ = 4; }
   else if ( g==-1 && h==-1 && i==-1 && j==-1 && m==-1 && n!=-1)   { k=n+12; typ = 5; }
   else { type=-1; patch=-1; return -1; }

   string patchnum = line2.substr(k, line2.length() );

   istringstream istr(patchnum);
   if (! (istr >> dec >> p ) ) { cout << " error in finding patchlevel " << endl; return -1; }		
   
   // cout << endl << "CLAN: " << my_str << endl << "Typ: " << typ << endl << "patchnumstr:" << patchnum << endl << "patchnum_num: " << p << endl;

   type  = typ;
   patch = p;

   return 1;
}

int find_verinfoUOSE(char *clientpath, int &type, int &patch)
{
   ifstream in;
   string my_str(clientpath);
   string line;
   int typ=-1;

   my_str = my_str + "verinfo";

   in.open(my_str.c_str(), ios::in );
   if (!in.is_open()) { cout << "error opening client dir !" << endl; return -1;}

   if ( !(getline(in, line) ) ) { cout << "error opening client dir 2" << endl; return -1; } 
  
   in.close();

   int g,p,k;
   g = line.find("Win32_SE"); 

   if   ( g!=-1)  { k=g+9; typ = 1; } 
   else { type=-1; patch=-1; return -1; }

   string patchnum = line.substr(k, line.length() );

   istringstream istr(patchnum);
   if (! (istr >> dec >> p ) ) { cout << " error in finding patchlevel " << endl; return -1; }		
   
   type  = typ;
   patch = p;

   return 1;
}

int find_clients(char *client_2d, char *client_3d, char* client_UOSE)
{
	HKEY tempKey;

	DWORD pathLength1 = _MAX_PATH+31, pathLength2 = _MAX_PATH+31, pathLength3 = _MAX_PATH+31;
	char strTarget[_MAX_PATH+26];
	char t1,t2,t3;
	bool uo3d, t2a, uose;
    char *UO2d_path = new char[_MAX_PATH+31], *UO3d_path = new char[_MAX_PATH+31], *UOSE_path = new char[_MAX_PATH+31];
    char *t3d_registry = "Software\\Origin Worlds Online\\Ultima Online Third Dawn\\1.0";
    char *t2a_registry = "Software\\Origin Worlds Online\\Ultima Online\\1.0";
	char *uose_registry = "Software\\EA GAMES\\Ultima Online Samurai Empire";
    char *strValue     = "ExePath";
	char *strValue2    = "Install Dir";

	t2a=0;
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, t2a_registry, NULL, KEY_READ, &tempKey) != ERROR_SUCCESS) t1=0; else t1=1;
	if (t1) if (RegQueryValueEx(tempKey, strValue, NULL, NULL, (LPBYTE)strTarget, &pathLength1) != ERROR_SUCCESS) t2a=0; else t2a=1 ;	
	if (tempKey!=NULL) RegCloseKey(tempKey);
	strcpy(UO2d_path, strTarget);

	uo3d=0;
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, t3d_registry, NULL, KEY_READ, &tempKey) != ERROR_SUCCESS) t2=0; else t2=1;
	if (t2) if (RegQueryValueEx(tempKey, strValue, NULL, NULL, (LPBYTE)strTarget, &pathLength2) != ERROR_SUCCESS) uo3d=0; else uo3d=1 ;	
	if (tempKey!=NULL) RegCloseKey(tempKey);
	strcpy(UO3d_path, strTarget);

	uose=0;
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, uose_registry, NULL, KEY_READ, &tempKey) != ERROR_SUCCESS) t3=0; else t3=1;
	if (t3) if (RegQueryValueEx(tempKey, strValue2, NULL, NULL, (LPBYTE)strTarget, &pathLength3) != ERROR_SUCCESS) uose=0; else uose=1 ;	
	if (tempKey!=NULL) RegCloseKey(tempKey);
	strcpy(UOSE_path, strTarget);

	if ( (!uo3d && t2==1) || t2==0)  strcpy(UO3d_path, "NO UO3D");
	if ( (!t2a  && t1==1) || t1==0)  strcpy(UO2d_path, "NO UO2D");
	if ( (!uose && t3==1) || t3==0)  strcpy(UOSE_path, "NO UOSE");

	strcpy(client_2d, UO2d_path);
	strcpy(client_3d, UO3d_path);
	strcpy(client_UOSE, UOSE_path);

	if (UO2d_path!=NULL) delete [] UO2d_path;
	if (UO3d_path!=NULL) delete [] UO3d_path;
	if (UOSE_path!=NULL) delete [] UOSE_path;

    return 1;
}

/// ugly helper function
//  pre: found, counter, type: INITIALIZED !!!

void find_address(patterns* ThePattern, char *pattern, bool &found, int & counter, int & type, const int typenum, unsigned long int & addresse, const int nummer, int & err)
{
   int x;
   unsigned long int addr;

   err=0;
   
   ThePattern->read_pattern( pattern);

   x=ThePattern->get_error(); 
   if (x<0) 
   { 
	   printf("internal error: pattern %i syntax not ok\n", nummer); 	  
	   err=1;
	   return;
   }
   
   addr = ThePattern->find_it();

   x=ThePattern->get_error();

   if ( (x<0) && (addr !=0) ) 
   { 
	   printf("internal error: pattern %i searching error adr: %i ec: %i\n", nummer, addr, x); 	   
	   err=2;
	   return; 
   } else if (addr !=0)
   {
	   found = true;
	   counter++;
	   type=typenum;
	   addresse=addr;
   }

}

int patch_client(char *name, bool almost_nc, unsigned long int key1, unsigned long int key2, int mode)
{

   unsigned long int address1=-1, address2=-1,address3=-1, address1b=-1, address2b=-1, address3b=-1, address7a=-1,address7b=-1,address7c=-1;
   bool found1,found2,found3,found1b,found2b,found3b, found7a,found7b,found7c;
   int c1,c2,c3,r1=0,r2=0,r3=0,r1b=0,r2b=0,r3b=0, c4,c5,c6,c7,c1b,c2b,c3b,c7a,c7b,c7c;
   int r7a=0,r7b=0,r7c=0;
   int err[40];

   patterns *ThePattern = new patterns();

   ThePattern->set_file(name);

   int x=ThePattern->get_error(); 
   if (x<0) { printf("error opening file: %s\n",name);  return -1; }

   found1=found2=found3=found1b=found2b=found3b=false;
   c1=c2=c3=c4=c5=c6=c7=c1b=c2b=c3b=0;

   for (int a=0; a<20;a++) err[a]=false;

   int numrules = 20;

   int debug = 0;

   /////////////////////////////////////////
   /// find first adress1 (login encrytion)
   /////////////////////////////////////////

   find_address(ThePattern, "0x00000000 0x00000000 0x0001 0x0f 0x84 ? 0x00 0x00 0x00 0x8b 0x48 ? 0x85 0xc9 0x8b 0xca",  //  FIRE 0x85
			    found1, c1, r1, 1, address1, 1, err[0]); 
  

   find_address(ThePattern, "0x00000000 0x00000000 0x0001 0x0f 0x84 ? 0x00 0x00 0x00 0x8b 0x88 ? ? ? ? 0x85 0xc9 0x8b 0xca", // FIRE 0x85
                found1, c1, r1, 2, address1, 2, err[1]);

   // 3.0.4++ t2a
   find_address(ThePattern, "0x00000000 0x00000000 0x000e 0x0f 0x8f ? ? 0x00 0x00 0x8b ? ? ? ? ? 0x85 0xc9 0x75 ? 0x85 0xed ? ? 0x0f 0x84",  // FIRE 0x74
	            found1, c1, r1, 3, address1, 3, err[2]);
  
   find_address(ThePattern, "0x00000000 0x00000000 0x000c 0x0f 0x8f ? ? 0x00 0x00 0x8b ? ? ? ? ? 0x85 0xc9 0x75 ? 0x85 0xed ? ? 0x0f 0x84", // FIRE 0x3b 0xc9
                found1b, c1b, r1b, 3, address1b, 4, err[3]); 

   // 3.0.5d ++
   find_address(ThePattern, "0x00000000 0x00000000 0x001c 0x0f 0x8f ? ? 0x00 0x00 0x8b ? ? ? ? ? 0x85 0xc9 ? ? ? ? ? ? ? ? ? ? ? ? ? ? 0x75 ? 0x85 0xed ? ? 0x0f 0x84",  // FIRE 0x74
	            found1, c1, r1, 3, address1, 3, err[2]);
  
   find_address(ThePattern, "0x00000000 0x00000000 0x001a 0x0f 0x8f ? ? 0x00 0x00 0x8b ? ? ? ? ? 0x85 0xc9 ? ? ? ? ? ? ? ? ? ? ? ? ? ? 0x75 ? 0x85 0xed ? ? 0x0f 0x84", // FIRE 0x3b 0xc9
                found1b, c1b, r1b, 3, address1b, 4, err[3]); 

   // 5.0.7 ++
   find_address(ThePattern, "0x00000000 0x00000000 0x001d 0x0f 0x8f ? ? 0x00 0x00 0x8b ? ? ? ? ? 0x85 0xc9 ? ? ? ? ? ? ? ? ? ? ? ? ? ? 0x0f 0x85 ? ? ? ? 0x85 0xed ? ? 0x0f 0x84",  // FIRE 0x84
	            found1, c1, r1, 5, address1, 3, err[2]);
  
   find_address(ThePattern, "0x00000000 0x00000000 0x001a 0x0f 0x8f ? ? 0x00 0x00 0x8b ? ? ? ? ? 0x85 0xc9 ? ? ? ? ? ? ? ? ? ? ? ? ? ? 0x0f 0x85 ? ? ? ? 0x85 0xed ? ? 0x0f 0x84", // FIRE 0x3b 0xc9
                found1b, c1b, r1b, 5, address1b, 4, err[3]); 
   

   // 3.0.4++ uo3d
   find_address(ThePattern, "0x00000000 0x00000000 0x000f 0x0f 0x8f ? ? 0x00 0x00 0x8b ? ? ? ? ? 0x85 0xc9 0x0f 0x85 ? ? ? ? 0x85 ? 0x0f 0x84",  // FIRE 0x84
	            found1, c1, r1, 4, address1, 5, err[4]);

   find_address(ThePattern, "0x00000000 0x00000000 0x000c 0x0f 0x8f ? ? 0x00 0x00 0x8b ? ? ? ? ? 0x85 0xc9 0x0f 0x85 ? ? ? ? 0x85 ? 0x0f 0x84", // FIRE 0x3b 0xc9
                found1b, c1b, r1b, 4, address1b, 6, err[5]);

   // 3.0.5++ uo3d
   find_address(ThePattern, "0x00000000 0x00000000 0x001d 0x0f 0x8f ? ? 0x00 0x00 0x8b ? ? ? ? ? 0x85 0xc9 ? ? ? ? ? ? ? ? ? ? ? ? ? ? 0x0f 0x85 ? ? ? ? 0x85 ? 0x0f 0x84",  // FIRE 0x84
	            found1, c1, r1, 4, address1, 5, err[4]);

   find_address(ThePattern, "0x00000000 0x00000000 0x001a 0x0f 0x8f ? ? 0x00 0x00 0x8b ? ? ? ? ? 0x85 0xc9 ? ? ? ? ? ? ? ? ? ? ? ? ? ? 0x0f 0x85 ? ? ? ? 0x85 ? 0x0f 0x84", // FIRE 0x3b 0xc9
                found1b, c1b, r1b, 4, address1b, 6, err[5]);

   ///////////////////////////////////////////////////
   // find second adress (/game server encryption 1
   ///////////////////////////////////////////////////

   find_address(ThePattern, "0x00000000 0x00000000 0x0005 0x8b 0x46 ? 0x33 0xdb 0x3b 0xc3 0x0f 0x84 ? 0x00 0x00 0x00", //  FIRE 0x90 0x90
                found2, c2, r2, 5, address2, 7, err[6]);
  
   find_address(ThePattern, "0x00000000 0x00000000 0x0003 0x8b 0x46 ? 0x85 0xc0 0x74 ? 0x8b 0x46 ? 0x8b 0x8e ? ? ? ? 0x2b 0xc8", //  FIRE 0x3b 0xc0
                found2, c2, r2, 6, address2, 8, err[7]);
	    
   find_address(ThePattern, "0x00000000 0x00000000 0x0006 0x8b 0x86 ? 0x00 0x00 0x00 0x85 0xc0 0x74 ? 0x8b 0x86 ? 0x00 0x00 0x00 0x8b 0x8e ? ? ? ? 0x2b 0xc8", // FIRE 0x3b 0xc0
                found2, c2, r2, 7, address2, 9, err[8]);

   /// 3.0.4 ++

   find_address(ThePattern, "0x00000000 0x00000000 0x0000 0x85 0xc0 0x74 ? 0x8b 0x86 ? 0x00 0x00 0x00 0x85 0xc0 0x74 ? 0x8b 0x86 ? 0x00 0x00 0x00 0x8b 0x8e ? ? ? ? 0x2b 0xc8", // FIRE 0x3b 0xc0
	            found2b, c2b, r2b, 8, address2b, 10, err[9]);


   ///////////////////////////////////////////////////
   // find third adress (game server encryption 2)
   ///////////////////////////////////////////////////
   
   find_address(ThePattern, "0x00000000 0x00000000 0x0008 0x83 0xf8 0xff 0x74 ? 0x8b 0x4e ? 0x85 0xc9 0x74 ? 0x33 0xc9 0x85 0xc0", //  FIRE 0x3b 0xc0
                found3, c3, r3, 9, address3, 11, err[10]);
  	  
   find_address(ThePattern, "0x00000000 0x00000000 0x000b 0x83 0xf8 0xff 0x74 ? 0x8b 0x8e ? 0x00 0x00 0x00 0x85 0xc9 0x74 ? 0x33 0xc9", //  FIRE 0x3b 0xc0
                found3, c3, r3, 10, address3, 12, err[11]);

   find_address(ThePattern, "0x00000000 0x00000000 0x000d 0x8b 0xf8 0x83 0xff 0xff 0x74 ? 0x8b ? ? ? ? ? 0x85 0xc0 0x74 ? 0x33 0xc0 0x85 0xff", //  FIRE 0x3b 0xff
                found3, c3, r3, 11, address3, 13, err[12]);

   // 3.0.4++

   find_address(ThePattern, "0x00000000 0x00000000 0x0006 0x8b 0x86 ? ? ? ? 0x85 0xc0|0xc9 0x74 ? 0x8b 0x86 ? ? ? ? 0x85 0xc0|0xc9 0x74 ? 0x33 0xc0|0xc9 0x85 ? 0x7e", //  FIRE 0x3b 0xc0
                found3, c3, r3, 12, address3, 14, err[13]);


   ///////////////////////////////////// login encryption keys ///////////////////////

   address7a=-1;
   find_address(ThePattern, "0x00000000 0x00000000 0x0035 0x32 0xd0|0xd1|0xd3|0xd4 0x8b ? ? ? ? ? 0x88 ? ? ? ? ? ? 0x8b ? ? ? ? ? 0x8b ? ? ? ? ? 0x8b ? ? ? ? ? 0x47|0x46 0x8b ? 0x89 ? ? ? ? ? 0x8b ? 0xc1 ? ? ? ? ? ? ? 0x35", 
                found7a, c7a, r7a, 20, address7a, 20, err[20]);

   if (address7a==-1) 
   {
   find_address(ThePattern, "0x00000000 0x00000000 0x0036 0x32 0xd0|0xd1|0xd3|0xd4 0x8b ? ? ? ? ? 0x88 ? ? ? ? ? ? 0x8b ? ? ? ? ? 0x8b ? ? ? ? ? 0x8b ? ? ? ? ? 0x47|0x46 0x8b ? 0x89 ? ? ? ? ? 0x8b ? 0xc1 ? ? ? ? ? ? ? 0x81", 
                found7a, c7a, r7a, 20, address7a, 20, err[20]);
   if (debug) printf("#2\n");
   }

   char temp[256];

   address7b=-1;
   sprintf(temp,"0x%08x 0x%08x 0x0001 0x35", address7a+4, address7a+144);
   if (debug) printf("%s\n",temp);
   find_address(ThePattern, temp, found7b, c7b, r7b, 21, address7b, 21, err[21]);

   if (address7b==-1)
   {
	  sprintf(temp,"0x%08x 0x%08x 0x0002 0x81 0xf1", address7a+4, address7a+144);
      if (debug) printf("%s\n",temp);
      find_address(ThePattern, temp, found7b, c7b, r7b, 21, address7b, 21, err[21]);

	   sprintf(temp,"0x%08x 0x%08x 0x0002 0x81 0xf2", address7a+4, address7a+144);
       if (debug) printf("%s\n",temp);
	   find_address(ThePattern, temp, found7c, c7c, r7c, 22, address7c, 22, err[22]);

   } else
   {
	   sprintf(temp,"0x%08x 0x%08x 0x0002 0x81", address7a+4, address7a+144);
       if (debug) printf("%s\n",temp);
	   find_address(ThePattern, temp, found7c, c7c, r7c, 22, address7c, 22, err[22]);

   }
  
   /////////////////////////////////////////////////////////
   /// now we got the adresses, let's do the actual patching
   /////////////////////////////////////////////////////////

   if (debug) printf("\na1: %x a1b: %x a2: %x a2b: %x a3: %x c1:%i c1b: %i c2:%i c2b: %i c3:%i r1:%i r1b: %i r2:%i r2b: %i r3:%i \n",address1, address1b, address2, address2b, address3, c1,c1b, c2,c2b, c3,r1,r1b, r2, r2b, r3);
   
   // lets check validy

   bool old;     // 2.0.4 and below
   bool verynew; // 3.0.4m and above

   if (c1!=1) { delete ThePattern; printf("encryption changes too huge, plz email the author about it\n"); return -1; }
   if (c2!=1) { delete ThePattern; printf("encryption changes too huge, plz email the author about it\n"); return -1; }

   if (r1==0 || r2==0)           { delete ThePattern; return -1; }
   if (address1<0 || address2<0) { delete ThePattern; return -1; }
  
   if      (c3==0)  old = true; 
   else if (c3==1)  old=false; // new ones
   else { delete ThePattern; printf("encryption changes too huge, plz email the author about it\n"); return -1; }

   if (found1b==true && found2b==true) verynew=true; else verynew=false;

   if ( (found1b==false && found2b==true) || (found1b==true && found2b==false) || (verynew==true && old==true) )
   {
	   delete ThePattern; 
	   printf("encryption changes too huge, plz email the author about it\n"); 
	   return -1;
   }

   if (verynew)
   {
	   if (c1b!=1)  { delete ThePattern; printf("encryption changes too huge, plz email the author about it\n"); return -1; }
	   if (c2b!=1)  { delete ThePattern; printf("encryption changes too huge, plz email the author about it\n"); return -1; }
   }

   // let's do it ...

   unsigned char *buff = new unsigned char[3*1024*1024];
   unsigned long int file_len;
  
   delete ThePattern;

 
   FILE *newfile;
   FILE *oldfile;
   char *na = new char[_MAX_PATH+_MAX_DIR+16];
   unsigned long int l;

   strcpy(na, name);
   l = strlen(na);

   int z=0;
   while ( l>0 )
   {
	   if ( na[l]=='\\' ) { z=l; l=1; }
	   l--;
   }

   na[z]=0;

   if (!almost_nc) strcat(na, "\\No_Crypt_Client");
   else strcat(na, "\\Almost_No_Crypt_Client");

   switch(mode)
   {
     case 0: 
	 case 2: strcat(na,"_2d.exe"); break;
	 case 1:
	 case 3: strcat(na,"_3d.exe"); break;
   }

   printf("New client-file: %s\n", na);

   ////////////////////////////
   // copy old client to mem
   /////////////////////////////
   oldfile = fopen(name, "rb");

   if (oldfile==NULL)
   {
	   printf("error opening old client\n");
	   if (buff!=NULL) delete[] buff;
       if (na!=NULL)   delete[] na;	 
	   return -1;
   }

   l = fseek(oldfile, 0, SEEK_END);
   if (l!=0)
   {
	   printf("error seeking into old client\n");
	   if (buff!=NULL) delete[] buff;
       if (na!=NULL)   delete[] na;	 
	   return -1;

   }

   file_len = ftell(oldfile);
  
   l = fseek(oldfile, 0, SEEK_SET);
   if (l!=0)
   {
	   printf("error seeking into old client2\n");
	   if (buff!=NULL) delete[] buff;
       if (na!=NULL)   delete[] na;	 
	   return -1;

   }

   if (address1>=file_len || address2>=file_len) { printf("encryption error, plz email the author about it\n"); if (buff!=NULL) delete[] buff; fclose(oldfile); return -1; }
   if ( (!old) && address3 <0)                   { printf("encryption error, plz email the author about it\n"); if (buff!=NULL) delete[] buff; fclose(oldfile); return -1; }
   if ( (!old) && address3 >=file_len)           { printf("encryption error, plz email the author about it\n"); if (buff!=NULL) delete[] buff; fclose(oldfile); return -1; }

   if (verynew)
   {
	   if ( (address1b>=file_len) || (address2b>=file_len) )  { printf("encryption error, plz email the author about it\n"); if (buff!=NULL) delete[] buff; fclose(oldfile); return -1; }	   
       if ( (address1b<0) || (address2b<0) )                  { printf("encryption error, plz email the author about it\n"); if (buff!=NULL) delete[] buff; fclose(oldfile); return -1; }	   
   }

   l = fread(buff, sizeof(char), file_len, oldfile);

   if (l!=file_len)
   {
	  printf("error copying old client to memory read:%x len:%x\n", l, file_len);
	  if (buff!=NULL) delete[] buff;
      if (na!=NULL)   delete[] na;	 
	  fclose(oldfile);
	  return -1;
   }

   fclose(oldfile);

   // change client in memory
   
   if (!almost_nc)
   {

     if (verynew==false) 
	 {
	   if (debug) printf ("a1: 0x85\n");
	   buff[address1]=0x85;
	 }
     else
	 {
	   if (debug) printf("a1b: 0x3b 0xc9\n");
	   buff[address1b+0]=0x3b;
	   buff[address1b+1]=0xc9;

	   switch(r1)
	   {
	       case 3:
			   if (debug) printf("a1: 0x74\n");
			   buff[address1+0]=0x74;
			   break;



		   case 4:
			   if (debug) printf("a1: 0x85\n");
			   buff[address1+0]=0x84;
			   break;

			case 5:
			   if (debug) printf("a1: 0x84\n");
			   buff[address1+0]=0x84;
			   break;

		   default: 
			   return -1;
		       break;
	   }

	 }

   } else
   {
	   // patch in custom encryption keys

	   if (debug) printf("LC1: %08x LC2: %08x LC3: %08x %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x \n", address7a+0x00400000, address7b+0x00400000, address7c+0x00400000, buff[address7a+3], buff[address7a+2], buff[address7a+1], buff[address7a+0],buff[address7b+3], buff[address7b+2], buff[address7b+1], buff[address7b+0],buff[address7c+3], buff[address7c+2], buff[address7c+1], buff[address7c+0]);

	   if (address7a==-1 || address7b==-1 ||address7c==-1)
	   {
		   printf("LeightWeight encrpytion error/changed.\nSorry, can't add custom keys\n");
		   return -1;
	   }

	   unsigned char b1,b2,b3, b4;

	   b4 = ((key1-1)>>24);
	   b3 = ((key1-1)>>16);
	   b2 = ((key1-1)>>8);
	   b1 = ((key1-1)%256);

	   if (debug) printf ("%02x %02x %02x %02x\n",b1,b2,b3,b4);

	   buff[address7a+0]=b1;
	   buff[address7a+1]=b2;
	   buff[address7a+2]=b3;
	   buff[address7a+3]=b4;

	   b4 = ((key1)>>24);
	   b3 = ((key1)>>16);
	   b2 = ((key1)>>8);
	   b1 = ((key1)%256);

	   if (debug) printf ("%02x %02x %02x %02x\n",b1,b2,b3,b4);

	   buff[address7b+0]=b1;
	   buff[address7b+1]=b2;
	   buff[address7b+2]=b3;
	   buff[address7b+3]=b4;

	   b4 = ((key2)>>24);
	   b3 = ((key2)>>16);
	   b2 = ((key2)>>8);
	   b1 = ((key2)%256);

	   if (debug) printf ("%02x %02x %02x %02x\n",b1,b2,b3,b4);

	   buff[address7c+0]=b1;
	   buff[address7c+1]=b2;
	   buff[address7c+2]=b3;
	   buff[address7c+3]=b4;
   }

   switch(r2)
   {
       case 5: 
		   if (debug) printf("a2: 0x90 0x90\n");
		   buff[address2+0]=0x90;
		   buff[address2+1]=0x90;
	      break;
	   case 6:
		   if (debug) printf("a2: 0x3b 0xc0\n");
		   buff[address2+0]=0x3b;
		   buff[address2+1]=0xc0;
		   break;
	   case 7:
		   if (debug) printf("a2: 0x3b 0xc0\n");
		   buff[address2+0]=0x3b;
		   buff[address2+1]=0xc0;
		   break;
	   default:
		   return -1;
   }

   if (verynew)
   {
	   if (debug) printf("a2b: 0x3b 0xc0\n");

	   if (r2b==8) 
	   {
		   buff[address2b+0]=0x3b;
		   buff[address2b+1]=0xc0;
	   } else return -1;
   }
     
   if (!old && !verynew)
   {
	  switch(r3)
	  {
       case 9:
           if (debug) printf("a3: 0x3b 0xc0\n");		   
		   buff[address3+0]=0x3b;
		   buff[address3+1]=0xc0;
	      break;
	   case 10:
		   if (debug) printf("a3: 0x3b 0xc0\n");
		   buff[address3+0]=0x3b;
		   buff[address3+1]=0xc0;
		   break;
	   case 11:
		   if (debug) printf("a3: 0x3b 0xff\n");
		   buff[address3+0]=0x3b;
		   buff[address3+1]=0xff;
		   break;
	   default:
		   return -1;
	  }
   }

   if (verynew)
   {
	   switch (r3)
	   {
	       case 12:
			   if (debug) printf("a3: 0x3b 0xc0\n");
			   buff[address3+0]=0x3b;
			   buff[address3+1]=0xc0;
			   break;

			   /*
		   case 13:
			   if (debug) printf("a3: 0x3b 0xc0\n");
			   buff[address3+0]=0x3b;
			   buff[address3+1]=0xc0;
			   break;*/

		   default:
			   return -1;
	   }
   }

   /// write changed client to new file
   newfile = fopen(na, "w+b");

   if (newfile==NULL)
   {
	   printf("error creating new patched client\n");
	   if (buff!=NULL) delete[] buff;
       if (na!=NULL)   delete[] na;	 
	   return -1;
   }

   l = fwrite(buff, sizeof(char), file_len, newfile);

   if (l!=file_len)
   {
	  printf("error writing new patched client\n");
	  if (buff!=NULL) delete[] buff;
      if (na!=NULL)   delete[] na;
	  fclose(newfile);
	  return -1;
   }

   fflush(newfile);


   if (buff!=NULL) delete[] buff;
   if (na!=NULL)   delete[] na;
   fclose(newfile);

   return 1;
}

int main(int argc, char* argv[])

{
	char *name_2d, *name_3d, *name_UOSE, *name_UOSE_2d, *name_UOSE_3d;
	unsigned long int key1=0, key2=0;
	bool almost_nocrypt=false;
	char LBR2d[128]; char LBR3d[128]; char UOSE[128];

	name_2d   = new char[_MAX_PATH+_MAX_DIR+16];
	name_3d   = new char[_MAX_PATH+_MAX_DIR+16];
	name_UOSE = new char[_MAX_PATH+_MAX_DIR+16];
	name_UOSE_2d = new char[_MAX_PATH+_MAX_DIR+16];
	name_UOSE_3d = new char[_MAX_PATH+_MAX_DIR+16];

	bool keyless=true;

	constart();
	printf("\nWelcome to Lord Binary's UO-RICE version minus 3.5\n");

	////// check mode ////////

	if (argc==3)
	{			
		key1 = atol(argv[1]);
		key2 = atol(argv[2]);
		if (key1>1 && key2>1) 
		{
			almost_nocrypt=true;
			printf("\nMode: ALMOST_NO_CRYPT, key1:%i key2:%i\n\n",key1, key2);
		}
		else
		{
		   printf("\nMode: NO_CRYPT\n\n");
		}
		
	}
	else
	{
	   printf("\nMode: NO_CRYPT\n\n");
	}

	//////////////////////////
	///// find clients
	//////////////////////////

	int ver2d=-1, ver3d=-1, patch2d=-1, patch3d=-1, patchUOSE=-1;

	find_clients(name_2d, name_3d, name_UOSE);

	strup(name_2d);
	strup(name_3d);
	strup(name_UOSE);

	char *path2d   = new char[_MAX_PATH+_MAX_DIR+16];
    char *path3d   = new char[_MAX_PATH+_MAX_DIR+16];
	char *pathUOSE = new char[_MAX_PATH+_MAX_DIR+16];

	strcpy(LBR3d,"NONE"); 
	strcpy(LBR2d,"NONE");	

	BOOL two_d,three_d,uose, uose_only;
	if ( strcmp ( name_2d, "NO UO2D" ) != 0) two_d=true; else two_d=false;
	if ( strcmp ( name_3d, "NO UO3D" ) != 0) three_d=true; else three_d=false;
	if ( strcmp ( name_UOSE, "NO UOSE") !=0) uose=true; else uose=false;	

	if ( (!strstr(name_2d, name_UOSE)) && !(strstr(name_3d, name_UOSE)) ) uose_only=false; else uose_only=true; 

	//printf("\nDEBUG:\n2d %i %s \n3d %i %s\nuose %i %s\nuose_only: %i\n\n", two_d,name_2d, three_d,name_3d, uose, name_UOSE, uose_only);

	// no 2d+3d client, leave
	if ( ( !(two_d) ) && ( !( three_d ) ) )
	{
		printf("\nUO not installed, quitting\n");
		if (!keyless) { while (!kbhit())  { Sleep(58); } }
		return -1;
	}
	
	if (!uose_only && uose) 
	{
		printf("Detected UOSE and non UOSE dual installation.\nPatching non UOSE only.\nIf you want to patch your UOSE installation.\nRun again with modified registry.\n\n");
	}

	if (!uose_only)
	{
	  if (two_d)
	  {		
	    strcpy(path2d, name_2d);
        int l = strlen(path2d);
        int z=0;
        while ( l>0 )
		{
	      if ( path2d[l]=='\\' ) { z=l; l=1; }
	      l--;
		}
        path2d[z]=0;
        strcat(path2d, "\\");      

	    find_verinfo(path2d, ver2d, patch2d);

	    if      (ver2d==2) strcpy(LBR2d, "UO:LBR");
	    else if (ver2d==0) strcpy(LBR2d, "NON LBR");
	    else if (ver2d==4) strcpy(LBR2d, "AOS");
	    else 
		{ 
		 printf("WARNING: LBR/NON LBR installation mismatch\n. Might cause troubles, consider reinstalling UO.\n");
		 strcpy(LBR2d,"LBR/NON LBR mismatch");
		}

	    printf("\n2D client (%s, patchlevel:%i) : %s \n", LBR2d, patch2d, name_2d); 
	  }

	  if (three_d)
	  {
	    strcpy(path3d, name_3d);
        int l = strlen(path3d);
        int z=0;
        while ( l>0 )
		{
	     if ( path3d[l]=='\\' ) { z=l; l=1; }
	     l--;
		}
        path3d[z]=0;
        strcat(path3d, "\\");      

	    find_verinfo(path3d, ver3d, patch3d);

	    if      (ver3d==3) strcpy(LBR3d, "UO:LBR");
	    else if (ver3d==1) strcpy(LBR3d, "NON LBR");
	    else if (ver3d==5) strcpy(LBR3d, "AOS");

	    else 
		{ 
		  printf("WARNING: LBR/NON LBR installation mismatch\n. Might cause troubles, consider reinstalling UO\n");
		  strcpy(LBR3d,"LBR/NON LBR mismatch");
		}

	     printf("3D client (%s, patchlevel:%i) : %s \n",   LBR3d, patch3d, name_3d);
	  }
	}

	else
	{

	   strcpy(pathUOSE, name_UOSE);

	   int ver;
	   find_verinfoUOSE(pathUOSE, ver, patchUOSE);

	   if      (ver==1)   strcpy(UOSE, "UO:SE");	
	   else 
	   { 
		 printf("WARNING: Unknown UOSE installation problem\n. Might cause troubles, consider reinstalling UO.\n");
		 strcpy(UOSE,"installation oddity");
	   }

		char tmp[512];
		strcpy(tmp, name_UOSE);
		strcat(tmp, "client.exe");
		strcpy(name_UOSE_2d, tmp);
		strcpy(tmp, name_UOSE);
		strcat(tmp, "uotd.exe");
		strcpy(name_UOSE_3d, tmp);	

		printf("\n");
		printf("UOSE 2d client (%s, patchlevel:%i): %s \n", UOSE, patchUOSE, name_UOSE_2d);
		printf("UOSE 3d client (%s, patchlevel:%i): %s \n", UOSE, patchUOSE, name_UOSE_3d);
	}

	delete [] path2d;
	delete [] path3d;
	delete [] pathUOSE;
	delete [] name_UOSE;

	//////////////////////////
	//// try to patch them
	//////////////////////////

	int r;
	bool nonfatalerror=false;

	if (uose_only)
	{
	   printf("\nTrying to remove UOSE 2D-client encryption.\n");
	   r = patch_client(name_UOSE_2d, almost_nocrypt, key1, key2, 2);
	   if (r==-1)
	   {
		   printf("Error, not patched.\n");
		   nonfatalerror=true;
	   }
	   else printf("Successfully patched.\n");

	   printf("\nTrying to remove UOSE 3D-client encryption\n");
	   r = patch_client(name_UOSE_3d, almost_nocrypt, key1, key2, 3);
	   if (r==-1)
	   {
		   printf("Error, not patched.\n");
		   nonfatalerror=true;
	   }
	   else printf("Successfully patched.\n");
	}
	else
	{
			// 2d client patching
	  if (two_d)
	  {
	    printf("\nTrying to remove 2D-client encryption\n");
	    r = patch_client(name_2d, almost_nocrypt, key1, key2, 0);
	    if (r==-1)
		{
		   printf("Error, not patched.\n");
		   nonfatalerror=true;
		}
	    else printf("Successfully patched.\n");
	  }

	  // 3d client patching
	  if (three_d)
	  {
	    printf("\nTrying to remove 3D-client encryption.\n");
	    r = patch_client(name_3d, almost_nocrypt, key1, key2,1 );
	    if (r==-1)
		{
		   printf("Error, not patched.\n");
		   nonfatalerror=true;
		}
	    else printf("Successfully patched.\n");
	  }
	}

	if (name_2d!=NULL) delete [] name_2d;
	if (name_3d!=NULL) delete [] name_3d;
	if (name_UOSE_2d!=NULL) delete [] name_UOSE_2d;
	if (name_UOSE_3d!=NULL) delete [] name_UOSE_3d;
   
    printf("\nThx for using UO-Rice !\n");

	//printf("errors: %i\n", nonfatalerror);

	if (!keyless) { while (!kbhit()) { Sleep(76); } }

	if (nonfatalerror==true) return -2; else return 1;

	//return (EXIT_SUCCESS);
}

