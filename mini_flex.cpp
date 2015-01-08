
////////////////////////////////////////////////////////////////////////////////
//
// Mini_flex.cpp
//
// Copyright (C) 2001 Nikolaus 'Lord Binary' Gerstmayr
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
////////////////////////////////////////////////////////////////////////////////


#include "StdAfx.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <conio.h>

#include "mini_flex.h"


/****************************************************/
/* copying the file "filename" to internal buffer   */
/****************************************************/
void patterns::set_file(const char* filename)
{
   FILE* theFile=NULL;
   unsigned long int length,l,l2;
  
   theFile=fopen(filename,"rb"); // open file

   if (theFile==NULL) // can't find file
   {
	   errorcode=-1;	  
	   return;
   }

   l=fseek(theFile,0,SEEK_END); // seek to end

   if (l!=0) // can't seek error
   {	
	  fclose(theFile);	   
	  errorcode=-2;
	  return;
   }

   length=ftell(theFile); // get position of end == filelength
	
   if (length==-1L) // can't tell position error
   {
	  fclose(theFile);		
	  errorcode=-3;
	  return;
   }

   l=fseek(theFile,0,SEEK_SET); // rewind
   if (l!=0) // can't rewind error
   {
	  fclose(theFile);
	  errorcode=-4;
	  return;
   }

   free_mem();

   filebuffer = new unsigned char[length+1024]; // 1024 -> crash paranoia =;)

   l2 = fread(filebuffer, sizeof(char), length, theFile);

   if (l2!=length)
   {
	  fclose(theFile);
	  errorcode=-5;
	  return;
   }

   file_length = length;
   fclose(theFile);
   errorcode = 0;
  
}

/** helper helper method for parsing **/
bool patterns::isSexyChar(char* candidate)
{
	int len = strlen(candidate);
	if (len !=4) return false;
	if (candidate[0]==48 && candidate[1]==120) // 0x
	{
		if ( (candidate[2]>=48 && candidate[2]<=57) || (candidate[2]>=65 && candidate[2]<=70) || (candidate[2]>=97 && candidate[2]<=102) ) // hexdigit 1
		{
			if ( (candidate[3]>=48 && candidate[3]<=57) || (candidate[3]>=65 && candidate[3]<=70) || (candidate[3]>=97 && candidate[3]<=102) ) // hexdigit 2
			{  } else return false;
		} else return false;          
	}
	else return false;
   return true;
}

/**** helper method for parsing ****/
int patterns::get_single(char* inp, char  &out)
{
	bool is;
	int i;
	char *t;

	is = isSexyChar(inp);				
	if (is==false) return -13; 	
			
	i=strtol(inp, &t, 16);
	if (strlen(t)!=0) i=-1;		
	if (i==-1) return -14;
	
	out = static_cast<char>(i);
	return 0;
}

/**** helper method for parsing ****/
int patterns::get_any_ns(char* inp, int &out1, char &out2, bool &nv)
{
	char *t, *tok2;
	char tok3[33];
	int a,b,c,i,j,p;
	bool is;

    tok2=strstr(inp,"?");

	// get value before ?
	a=strlen(inp);b=strlen(tok2);
	for (c=1; c<(a-b); c++) tok3[c-1]=inp[c];
	tok3[c-1]=0;
				
    if (tok2!=NULL) 
	{							  					  
	   i=strtol(tok3, &t, 10);
	   if (strlen(t)!=0) i=-1;				
	   if (i==-1 || i<0) return -16;
		
	    // get value after ?
		if ( (a-b+1)<0 ) tok3[0]=0; else
		{ 	
			if (inp[(a-b+1)]=='!')
			{
				nv=true;
				p=1;
			} else { nv=false; p=0; }

			for (c=(a-b+1+p); c<a-1; c++) tok3[c-(a-b+1+p)]=inp[c];
			tok3[c-(a-b+1+p)]=0;
		}
					  
		is = isSexyChar(tok3);					  					  					  
		j=strtol(tok3, &t, 16);
	    if (strlen(t)!=0) j=-1;					  
		if (j==-1 || j<0 || is==false) return -18;					  			
					  
	} else return -15;				  			

  out1 = i;
  out2 = static_cast<char>(j);
  return 0;
}

/*******************************************************************************************/
/* parses a pattern-definition and copies it to internal data structures if syntacicaly ok */
/* char* must be a null termianted c-style string.                                         */
/* if not it crashes.                                                                      */
/* if ok -> errcode == 0, pattern is copied to internal data structures                    */
/* if not ok -> errorcode <0                                                               */
/*******************************************************************************************/

void patterns::read_pattern(char* patternstring)
{
	int length,a=0,b=0,c=0,j=0,i=0;

	char* psc=NULL;
    char seps[]   = " \t\n";	
	char *token, *token2,*t;
	char ch,ch2,ch3;
	char *tok_c;
	bool is=false, neg=false, neg2;	
	single_char a_char;
	single_pos  a_pos;
	vector<char*> ol;

	if (pat!=NULL)
	{
	   vector<single_pos>::iterator vi;         
       vi = pat->and_list.begin();

       while (! pat->and_list.empty() ) 
	   {	   

	   while(! (vi->or_list.empty()) ) vi->or_list.pop_back();
	   pat->and_list.pop_back();
	   vi++;	   	   
	   }

	   delete pat;
	}

	pat = new single_pattern;

	psc = _strdup(patternstring);
    
	// tokenizing;	

   #ifdef debug
     cout << "Tokens: \n" << psc << "\n";  
   #endif

   token = strtok( psc, seps ); // get first token = start position in file

   i=strtol(token, &t, 16);
   if (strlen(t)!=0 || strlen(token)!=10) i=-1;		
   if (token[0]==48 && token[1]==120) {} else i=-1;
   if (i<=-1) 
   {
	   errorcode=-25;
	   free(psc);
	   return;
   }

   start = i;
   
   token = strtok(NULL, seps); // get second token = end position in file

   i=strtol(token, &t, 16);
   if (strlen(t)!=0 || strlen(token)!=10) i=-1;	
   if (token[0]==48 && token[1]==120) {} else i=-1;
   if (i<=-1) 
   {
	   errorcode=-26;
	   free(psc);
	   return;
   }

   end = i;
   
   if ( ((start>end) && end!=0))
   {

      errorcode=-27;
	  free(psc);
	  return;
   }

   token = strtok(NULL,seps); // get relative offset

   i=strtol(token, &t, 16);
   if (strlen(t)!=0 || strlen(token)!=6) i=-1;	
   if (token[0]==48 && token[1]==120) {} else i=-1;
   if (i<=-1) 
   {
	   errorcode=-90;
	   free(psc);
	   return;
   }

   relative_offset = i;

   token = strtok(NULL, seps);
   
   while( token != NULL )
   {  
      #ifdef debug
        cout << "\n" << token; 
      #endif

	  if (token !=NULL) 
	  {		  
		  length=strlen(token); // each token has to be <32 bytes
		  if (length>32) 
		  {
			  errorcode=-12;
			  free(psc); return;
		  }

	  ch=token[0];
	  tok_c=_strdup(token);
	  if (strstr(tok_c,"|")!=NULL) ch='r';

	  neg=false;
	  if (token[0]=='!')
	  {
		  neg=true;
		  if (ch!='r')
		  {
	        token++;	  
		    ch=token[0];
		  }
	  }
     
	  switch (ch)
	  {
          case '0': // specific, no or list			  

				j = get_single(token, ch2);

				if (j<0)
				{
					errorcode=-13;
					free (psc);
					return;
				}

				a_char.type=specific_1;
				a_char.value=ch2;
				a_char.num_anys=0;
				a_char.neg=neg;			

				a_pos.or_list.push_back(a_char);
				pat->and_list.push_back(a_pos);              
				a_pos.or_list.pop_back(); // leave it empty for next call
			    break;	  			  

		  case '[': // any_n
			  
				  j = get_any_ns(token,i, ch2, neg2);

				  if (j<0 || neg==true)
				  {
					  errorcode=-14;
					  free( psc);
					  return;
				  }

				  a_char.neg=neg2;
				  a_char.num_anys=i;
				  a_char.type=any_n;					 
				  a_char.value=ch2;

				  a_pos.or_list.push_back(a_char); // minmal or-list
				  pat->and_list.push_back(a_pos);
				  a_pos.or_list.pop_back(); // leave the orlist empty for enxt call
				  
				  break;						  

		  case '?': // any_1
			  
				if ( strlen(token)!=1 || neg==true)
				{
				    errorcode=-15;
					free( psc);
					return;
				}
			
				a_char.type=any_1;
				a_char.value=0;
				a_char.num_anys=0;
				a_char.neg=false;
			
				a_pos.or_list.push_back(a_char);
				pat->and_list.push_back(a_pos);
				a_pos.or_list.pop_back();
			
				break;			  

		  case 'r': // real or list			  
			    
				  ol.push_back(tok_c); // save or-string for later parsing

				  // add a dummy entry, so that the order can be preserved later
				  a_char.type=any_1;
				  a_char.value=-1;
				  a_char.num_anys=-1;
				  a_char.neg=neg;
			
				  a_pos.or_list.push_back(a_char);
				  pat->and_list.push_back(a_pos);
				  a_pos.or_list.pop_back();
                 		  
				  break;

         default:			 
				 errorcode=-2;
				 free(psc);
				 return;			 
	   } // end case
	        
	 } // end if token != null
	 token = strtok( NULL, seps ); // get next token	   
   }  // end tokenizer-loop

    /* darn strtoks can't be run in nested loops, hence the linearisation :-( */
	/* that's a bit strange/tricky. re-inserting the saved or-lists at the correct locations */
   
   vector<char*>::iterator oi;      
   vector<single_pos>::iterator v;
   vector<single_char>::iterator vv;
   int z=0;
 
   for (oi=ol.begin(); oi != ol.end(); ++oi)
   {	  
     for (v=pat->and_list.begin(); v!= pat->and_list.end(); ++v) // find the or'ed element
	 {		
	    vv=v->or_list.begin();
	   	   		
        if (vv->value==0xff && vv->num_anys==0xffffffff)
		{							    
		   z=0;         	   
           token2 = strtok( *oi, "|" );
           while( token2 != NULL ) // tokenizing or-lists
		   {                                              				
		      if (token2 !=NULL)
			  {	
				  
		         ch2 = token2[0];			 
				 z++;

				 neg=false;
	             if (token2[0]=='!')
				 {
		            neg=true;
	                token2++;	  
		            ch2=token2[0];
				 }

			     switch (ch2)
				 {

				    case '?': 

						 if ( strlen(token2)!=1 || neg==true)
						 {
				            errorcode=-18;
					        free( psc);
					        return;
						 }
			
				         a_char.type=any_1;
				         a_char.value=0;
				         a_char.num_anys=0;
				         a_char.neg=false;
			
				         if (z!=1) v->or_list.push_back(a_char);
					     else
						 {
						   vv->neg=false;
						   vv->num_anys=0;
						   vv->value=0;
						   vv->type=any_1;                                              
						 }
				         
				      
			
				         break;

				    case '0': 
								  
				          j = get_single(token2, ch3);

				          if (j<0)
						  {
				             errorcode=-17;
					         free(psc);
					         return;
						  }					

				          a_char.type=specific_1;
				          a_char.value=ch3;
				          a_char.num_anys=0;
				          a_char.neg=neg;
			
					      if (z!=1) v->or_list.push_back(a_char);
					      else
						  {
						     vv->neg=neg;
						     vv->num_anys=0;
						     vv->value=ch3;
						     vv->type=specific_1;                                              
						  }
				   
				          break;
						    
				    case '[': 

						  j = get_any_ns(token2,i, ch3, neg2);

				          if (j<0 || neg==true)
						  {
					          errorcode=-19;
					          free( psc);
					          return;
						  }

				          a_char.neg=neg2;
				          a_char.num_anys=i;
				          a_char.type=any_n;					 
				          a_char.value=ch3;

						  if (z!=1) v->or_list.push_back(a_char);
					      else
						  {
						     vv->neg=neg2;
						     vv->num_anys=i;
						     vv->value=ch3;
						     vv->type=any_n;                                              
						  }

				         
					      break;

				    default: 
					   errorcode=-16;
					   free(psc);							 
					   return;							       
				} // end switch
			}  // end if token2!=null
		    token2 = strtok( NULL, "|" );
		} // end tokenizer loop		
		   break; // end the and-list loop if word found
		} // end if
	 } // end and-list iterator
   } // end or-string interator loop

   errorcode=0;
   free (psc);
   
}

/** lets find it :-) **/
unsigned long int patterns::find_it()
{
	 
   vector<single_pos>::iterator vi;   
   vector<single_char>::iterator vii;
   int zz=0,z=0;
   unsigned long int ende,f,pl,a,pos_glue,b;
   bool or_bool, and_bool,jig;
   unsigned char TC;

   #ifdef debug
     cout << hex << endl << endl;   
     for (vi=pat->and_list.begin(); vi != pat->and_list.end(); ++vi)
	 {
	   zz=0;
	   for (vii=vi->or_list.begin(); vii != vi->or_list.end(); ++vii)
	   {
		    zz++;          
			if (zz>1) cout << "| ";
			if ((*vii).neg) cout << "!";
			cout << static_cast<int>((*vii).value) << " ";			             			
	   }
	   z++;	   
	 }  
     cout << endl << endl << "z: " << z << " start: " << start <<" end: " << end << endl;
  #endif

  if (end==0) ende = file_length; else ende = end;
  f=start;

  if ((start>file_length) || end>file_length)
  {
	  errorcode=-35;
	  return 0;
  }

  while (f<ende)
  {    
    and_bool = true; pl = 0; pos_glue = 0;
    for (vi=pat->and_list.begin(); vi != pat->and_list.end(); ++vi) // interate the search positions
    {
	   or_bool = false;
	   for (vii=vi->or_list.begin(); vii != vi->or_list.end(); ++vii) // iterate the or's
	   {		    			
			switch ((*vii).type)
			{
			   case any_1:      
				         
				         or_bool=true;
				         break;

			   case any_n:      
					     
				         TC  = (*vii).value;
						 a   = (*vii).num_anys;
						 jig = (*vii).neg;

                         for (b=0; b<a; b++)
						 {
							 if ( (f+pl+b+pos_glue) < file_length )
							 {
						        if ( jig==false )  
								{
									if ( TC     == filebuffer[f+pl+b+pos_glue] ) { or_bool=true; pos_glue += (b); /*cout << "glue: " << pos_glue << " b: " << b << endl;*/ break; }
								} else if ( TC  != filebuffer[f+pl+b+pos_glue] ) { or_bool=true; pos_glue += (b); /*cout << "glue: " << pos_glue << " b: " << b << endl;*/ break; }						 								           
							 } else
							 {							 							
							 }                            
						 }                         
				         
				         break;

			   case specific_1: 

				         TC = (*vii).value;
						 if ( (f+pl+pos_glue) < file_length )
						 {
						    if ( (*vii).neg==false)  
							{
							   if ( TC     == filebuffer[f+pl+pos_glue] ) or_bool=true;
							} else if ( TC != filebuffer[f+pl+pos_glue] ) or_bool=true;						 								           
						 } else
						 {							 
							
						 }

						 break;

               default: errorcode = -30; return 0;
			}			
					
			if (or_bool==true) break; // one or'd element true -> entire orlist is true, no need to check the rest

	   } // end or-iterator

	   pl++;
	   if (or_bool==false) { and_bool=false; break; } // one and'ed element false -> entire and_list false, no need to check the rest
	      
	} // end and-iterator

	if (and_bool) // match !
	{
	   errorcode=0;	   
	   return (f+relative_offset);
	}

	f++;
  } // end file-"iterator"

  errorcode=-66;
  return 0;
}


void patterns::free_mem()
{
  if (filebuffer != NULL) delete [] filebuffer;  
}

patterns::~patterns()
{
  free_mem();

  vector<single_pos>::iterator vi;   
 
  vi = pat->and_list.begin();

  while (! pat->and_list.empty() ) 
    {	   

	   while(! (vi->or_list.empty()) ) vi->or_list.pop_back();
	   pat->and_list.pop_back();
	   vi++;	   	   
  }

  if (pat !=NULL) delete pat;
}
