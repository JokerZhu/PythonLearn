#include <string.h>
#include <stdio.h>



/*将json 包加密成密文*/
unsigned char CCC[17][28],D[17][28],K[17][48],c,ch;
int pc_1_c[28]={
        57,49,41,33,25,17,9 ,
        1 ,58,50,42,34,26,18,
        10,2 ,59,51,43,35,27,
        19,11,3 ,60,52,44,36};

int pc_1_d[28]={
        63,55,47,39,31,23,15,
        7 ,62,54,46,38,30,22,
        14,6 ,61,53,45,37,29,
        21,13,5 ,28,20,12,4 };

int pc_2[48]={
        14,17,11,24,1 ,5 ,3 ,28,
        15,6 ,21,10,23,19,12,4 ,
        26,8 ,16,7 ,27,20,13,2 ,
        41,52,31,37,47,55,30,40,
        51,45,33,48,44,49,39,56,
        34,53,46,42,50,36,29,32};
int ls_count[16]={1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1};

int ip_tab[64]={
        58,50,42,34,26,18,10,2,
        60,52,44,36,28,20,12,4,
        62,54,46,38,30,22,14,6,
        64,56,48,40,32,24,16,8,
        57,49,41,33,25,17,9 ,1,
        59,51,43,35,27,19,11,3,
        61,53,45,37,29,21,13,5,
        63,55,47,39,31,23,15,7};

int _ip_tab[64]={
        40,8,48,16,56,24,64,32,
        39,7,47,15,55,23,63,31,
        38,6,46,14,54,22,62,30,
        37,5,45,13,53,21,61,29,
        36,4,44,12,52,20,60,28,
        35,3,43,11,51,19,59,27,
        34,2,42,10,50,18,58,26,
        33,1,41,9,49,17,57,25};
int e_r[48]={
        32,1 ,2 ,3 ,4 ,5 ,4 ,5 ,
        6 ,7 ,8 ,9 ,8 ,9 ,10,11,
        12,13,12,13,14,15,16,17,
        16,17,18,19,20,21,20,21,
        22,23,24,25,24,25,26,27,
        28,29,28,29,30,31,32,1};

int P[32]={
        16,7, 20,21,29,12,28,17,
        1 ,15,23,26,5 ,18,31,10,
        2, 8 ,24,14,32,27,3 ,9 ,
        19,13,30,6 ,22,11,4 ,25};
int SSS[16][4][16]={
        14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7,
        0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8,/* err on */
        4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0,
        15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13,

        15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10,
        3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5,
        0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15,
        13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9,

        10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8,
        13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1,
        13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7,
        1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12,

        7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15,
        13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9,
        10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4,
        3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14, /* err on */

        2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9,
        14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6, /* err on */
        4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14,
        11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3,

        12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11,
        10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8,
        9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6,
        4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13,

        4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1,
        13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6,
        1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2,
        6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12,

        13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7,
        1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2,
        7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8,
        2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11};



int   _Des(unsigned char *text,unsigned char *mtext,unsigned char *key)
//unsigned char *text,*mtext,*key;
{
        unsigned char tmp[64];
        expand0(key,tmp);
        setkeystar(tmp);
        discrypt0(text,mtext);
        return 0;
}

compress0(unsigned char *out,unsigned char *in)
{
        int times;
        int i,j;

        for (i=0;i<8;i++)
        {
                times=0x80;
                in[i]=0;
                for (j=0;j<8;j++)
                {
                        in[i]+=(*out++)*times;
                        times/=2;
                }
        }
        return 0;
}

int s_box(unsigned char *aa,unsigned char *bb)
{
        int i,j,k,m;
        int y,z;
        unsigned char ss[8];

        m=0;
        for (i=0;i<8;i++)
        {
                j=6*i;
                y=aa[j]*2+aa[j+5];
                z=aa[j+1]*8+aa[j+2]*4+aa[j+3]*2+aa[j+4];
                ss[i]=SSS[i][y][z];
                y=0x08;
                for (k=0;k<4;k++)
                {
                        bb[m++]=(ss[i]/y)&1;
                        y/=2;
                }
        }
        return 0;
}
F(int n,unsigned char *ll,unsigned char *rr,unsigned char *LL,unsigned char *RR)
{
        int i;
        unsigned char buffer[64],tmp[64];

        for (i=0;i<48;i++)
                buffer[i]=rr[e_r[i]-1];
        for (i=0;i<48;i++)
                buffer[i]=(buffer[i]+K[n][i])&1;

        s_box(buffer,tmp);

        for (i=0;i<32;i++)
                buffer[i]=tmp[P[i]-1];

        for (i=0;i<32;i++)
                RR[i]=(buffer[i]+ll[i])&1;

        for (i=0;i<32;i++)
                LL[i]=rr[i];

        return 0;
}

encrypt0(unsigned char *text,unsigned char *mtext)
{
        unsigned char ll[64],rr[64],LL[64],RR[64];
        unsigned char tmp[64];
        int i,j;
        ip(text,ll,rr);

        for (i=1;i<17;i++)
        {
                F(i,ll,rr,LL,RR);
                for (j=0;j<32;j++)
                {
                        ll[j]=LL[j];
                        rr[j]=RR[j];
                }
        }

        _ip(tmp,rr,ll);

        compress0(tmp,mtext);
        return 0;
}
discrypt0(unsigned char *mtext,unsigned char *text)
{
        unsigned char ll[64],rr[64],LL[64],RR[64];
        unsigned char tmp[64];
        int i,j;

        ip(mtext,ll,rr);

        for (i=16;i>0;i--)
        {
                F(i,ll,rr,LL,RR);
                for (j=0;j<32;j++)
                {
                        ll[j]=LL[j];
                        rr[j]=RR[j];
                }
        }

        _ip(tmp,rr,ll);

        compress0(tmp,text);
        return 0;
}


ip(unsigned char *text,unsigned char *ll,unsigned char *rr)
{
        int i;
        unsigned char buffer[64];

        expand0(text,buffer);

        for (i=0;i<32;i++)
                ll[i]=buffer[ip_tab[i]-1];

        for (i=0;i<32;i++)
                rr[i]=buffer[ip_tab[i+32]-1];
        return 0;
}

_ip(unsigned char *text,unsigned char *ll,unsigned char *rr)
{
        int i;
        char tmp[64];

        for (i=0;i<32;i++)
                tmp[i]=ll[i];
        for (i=32;i<64;i++)
                tmp[i]=rr[i-32];
        for (i=0;i<64;i++)
                text[i]=tmp[_ip_tab[i]-1];
        return 0;
}


LS(unsigned char *bits,unsigned char *buffer,int count)
{
        int i;
        for (i=0;i<28;i++)
        {
                buffer[i]=bits[(i+count)%28];
        }
        return 0;
}

son(unsigned char *cc,unsigned char *dd,unsigned char *kk)
{
        int i;
        char buffer[56];
        for (i=0;i<28;i++)
                buffer[i]=*cc++;

        for (i=28;i<56;i++)
                buffer[i]=*dd++;

        for (i=0;i<48;i++)
                *kk++=buffer[pc_2[i]-1];
        return 0;
}

setkeystar(unsigned char *bits)
{
        int i,j;

        for (i=0;i<28;i++)
                CCC[0][i]=bits[pc_1_c[i]-1];
        for (i=0;i<28;i++)
                D[0][i]=bits[pc_1_d[i]-1];
        for (j=0;j<16;j++)
        {
                LS(CCC[j],CCC[j+1],ls_count[j]);
                LS(D[j],D[j+1],ls_count[j]);
                son(CCC[j+1],D[j+1],K[j+1]);
        }
        return 0;
}

expand0(unsigned char *in,unsigned char *out)
{
        int divide;
        int i,j;

        for (i=0;i<8;i++)
        {
                divide=0x80;
                for (j=0;j<8;j++)
                {
                        *out++=(in[i]/divide)&1;
                        divide/=2;
                }
        }
        return 0;
}


int Des(unsigned char *text,unsigned char *mtext, unsigned char *key)
//unsigned char *text,*mtext,*key;
{
        unsigned char tmp[64];
        expand0(key,tmp);
        setkeystar(tmp);
        encrypt0(text,mtext);
        return 0;
}


int UPPLE_DES(unsigned char *pData,unsigned char *pResult,int len,unsigned char *aKey,int flag)
{
        if (flag == 1)
                Des(pData,pResult,aKey);
        else
                _Des(pData,pResult,aKey);
        return(0);
}   

 /***********************加密解密公用函数******************************/ 
int UppleCompress128BCDInto64Bits(char *p128BCDKey,                  unsigned char *p64BitsKey)
{    
 	int i;     
 	for (i = 0; i < 8; i++) 
 	{    
 		if (p128BCDKey[2 * i] >= 'A')        
 			p64BitsKey[i] = (char) ((p128BCDKey[2 * i] - 0x37) << 4);    
 		else        
 			p64BitsKey[i] = (char) ((p128BCDKey[2 * i] - '0') << 4);    
 			if (p128BCDKey[2 * i + 1] >= 'A')        
 				p64BitsKey[i] |= p128BCDKey[2 * i + 1] - 0x37;    
 			else        
 				p64BitsKey[i] |= p128BCDKey[2 * i + 1] - '0';    
 	}    
 	return (0);
} 
int UppleUncompress64BitsInto128BCD(unsigned char *p64BitsKey,                    char *p128BCDKey)
{    
	int i;    
	unsigned char xxc;     
	for (i = 0; i < 8; i++) 
	{    
		xxc = p64BitsKey[i] / 16;    
		if (xxc < 0x0a)        
			p128BCDKey[2 * i] = xxc + '0';    
		else        
			p128BCDKey[2 * i] = xxc + 0x37;     
			xxc = p64BitsKey[i] % 16;    
		if (xxc < 0x0a)        
			p128BCDKey[2 * i + 1] = xxc + '0';    
		else        
			p128BCDKey[2 * i + 1] = xxc + 0x37;    
	}    
	return (0);
}



int Upple1DesEncrypt64BitsText(char *pPlainKey,char *pPlainText,char *pCryptogram)
{
	unsigned char	aKey[9];
	unsigned char	pData[9];
	unsigned char	pResult[9];
	int		ret;
	
#ifdef DEBUG_DES
		UppleUserErrLog("########in Upple1DesEncrypt64BitsText:: process des begin!\n");
		UppleUserErrLog("in Upple1DesEncrypt64BitsText:: tak[%s]!\n",pPlainKey);
		UppleUserErrLog("in Upple1DesEncrypt64BitsText:: text[%s]!\n",pPlainText);
#endif
	/* compress the 16 bytes key into 8 bytes */
	if ((ret = UppleCompress128BCDInto64Bits(pPlainKey,aKey)) < 0)
	{
#ifdef DEBUG_DES
		UppleUserErrLog("in Upple1DesEncrypt64BitsText:: UppleCompress128BCDInto64Bits!\n");
#endif
		return(ret);
	}

	// Compress PlainText    
	if ((ret = UppleCompress128BCDInto64Bits(pPlainText,pData)) < 0)
	{
#ifdef DEBUG_DES
		UppleUserErrLog("in Upple1DesEncrypt64BitsText:: UppleCompress128BCDInto64Bits!\n");
#endif
		return(ret);
	}
#ifdef DEBUG_DES
	UppleMemLog("tak:",aKey,8);
	UppleMemLog("text:",pData,8);
#endif
	UPPLE_DES(pData,pResult,8,aKey,1);
#ifdef DEBUG_DES
	UppleMemLog("result:",pResult,8);
#endif
	
#ifdef DEBUG_DES
		UppleUserErrLog("#######in Upple1DesEncrypt64BitsText:: process des end!\n");
#endif
	// Uncompress Result
	if ((ret = UppleUncompress64BitsInto128BCD(pResult,pCryptogram)) < 0)
	{
#ifdef DEBUG_DES
		UppleUserErrLog("in Upple1DesEncrypt64BitsText:: UppleUncompress64BitsInto128BCD!\n");
#endif
		return(ret);
	}
	
	return(0);
}

// The pPlainKey must be 64bits, i.e. BCD 16chars
// The pCryptogram must be 64bits, i.e. BCD 16chars.
int Upple1DesDecrypt64BitsText(char *pPlainKey,char *pCryptogram,char *pPlainText)
{
	unsigned char	aKey[9];
	unsigned char	pData[9];
	unsigned char	pResult[9];
	int		ret;

	/* compress the 16 bytes key into 8 bytes */
	if ((ret = UppleCompress128BCDInto64Bits(pPlainKey,aKey)) < 0)
	{
#ifdef DEBUG_DES
		UppleUserErrLog("in Upple1DesDecrypt64BitsText:: UppleCompress128BCDInto64Bits!\n");
#endif
		return(ret);
	}

	// Compress Cryptogram
	if ((ret = UppleCompress128BCDInto64Bits(pCryptogram,pData)) < 0)
	{
#ifdef DEBUG_DES
		UppleUserErrLog("in Upple1DesDecrypt64BitsText:: UppleCompress128BCDInto64Bits!\n");
#endif
		return(ret);
	}

	UPPLE_DES(pData,pResult,8,aKey,0);
	
	// Uncompress Result
	if ((ret = UppleUncompress64BitsInto128BCD(pResult,pPlainText)) < 0)
	{
#ifdef DEBUG_DES
		UppleUserErrLog("in Upple1DesDecrypt64BitsText:: UppleUncompress64BitsInto128BCD!\n");
#endif
		return(ret);
	}
	
	return(0);
}

int Upple3DesDecrypt64BitsText(char *pPlainKey, char *pCryptogram,
                               char *pPlainText)
{
    unsigned char aKey1[9], aKey2[9];
    unsigned char pData[9];
    unsigned char pResult1[9], pResult2[9], pResult3[9];
    int ret;

    /* compress the 16 bytes left key into 8 bytes */
    if ((ret = UppleCompress128BCDInto64Bits(pPlainKey, aKey1)) < 0) {
//        UppleUserErrLog2(__FILE__, __LINE__,
  //                       "in Upple3DesDecrypt64BitsText:: UppleCompress128BCDInto64Bits!\n");
        return (ret);
    }
    /* compress the 16 bytes right key into 8 bytes */
    if ((ret = UppleCompress128BCDInto64Bits(&pPlainKey[16], aKey2)) < 0) {
        //UppleUserErrLog2(__FILE__, __LINE__,
          //               "in Upple3DesDecrypt64BitsText:: UppleCompress128BCDInto64Bits!\n");
        return (ret);
    }
    // Compress PlainText
    if ((ret = UppleCompress128BCDInto64Bits(pCryptogram, pData)) < 0) {
        //UppleUserErrLog2(__FILE__, __LINE__,
          //               "in Upple3DesEncrypt64BitsText:: UppleCompress128BCDInto64Bits!\n");
        return (ret);
    }

    UPPLE_DES(pData, pResult1, 8, aKey1, 0);
    UPPLE_DES(pResult1, pResult2, 8, aKey2, 1);
    UPPLE_DES(pResult2, pResult3, 8, aKey1, 0);

    // Uncompress Result
    if ((ret = UppleUncompress64BitsInto128BCD(pResult3, pPlainText)) < 0) {
        //UppleUserErrLog2(__FILE__, __LINE__,
          //               "in Upple3DesEncrypt64BitsText:: UppleUncompress64BitsInto128BCD!\n");
        return (ret);
    }

    return (0);

}

int Upple3DesEncrypt64BitsText(char *pPlainKey, char *pPlainText,char *pCryptogram)
{    
	unsigned char aKey1[9], aKey2[9];    
	unsigned char pData[9];    
	unsigned char pResult1[9], pResult2[9], pResult3[9];    
	int ret;     
	/* compress the 16 bytes left key into 8 bytes */    
	if ((ret = UppleCompress128BCDInto64Bits(pPlainKey, aKey1)) < 0) 
		{    
			//UppleUserErrLog2(__FILE__, __LINE__,             "in Upple3DesEncrypt64BitsText:: UppleCompress128BCDInto64Bits!\n");    
			return (ret);    
		}    
	/* compress the 16 bytes right key into 8 bytes */    
	if ((ret = UppleCompress128BCDInto64Bits(&pPlainKey[16], aKey2)) < 0) 
	{    
		//UppleUserErrLog2(__FILE__, __LINE__,             "in Upple3DesEncrypt64BitsText:: UppleCompress128BCDInto64Bits!\n");    
		return (ret);    
	}    
	// Compress PlainText    
	if ((ret = UppleCompress128BCDInto64Bits(pPlainText, pData)) < 0) 
	{    
		//UppleUserErrLog2(__FILE__, __LINE__,             "in Upple3DesEncrypt64BitsText:: UppleCompress128BCDInto64Bits!\n");    
		return (ret);    
	}     
	UPPLE_DES(pData, pResult1, 8, aKey1, 1);    
	UPPLE_DES(pResult1, pResult2, 8, aKey2, 0);    
	UPPLE_DES(pResult2, pResult3, 8, aKey1, 1);     
	// Uncompress Result    
	if ((ret = UppleUncompress64BitsInto128BCD(pResult3, pCryptogram)) < 0) 
	{    
		//UppleUserErrLog2(__FILE__, __LINE__,             "in Upple3DesEncrypt64BitsText:: UppleUncompress64BitsInto128BCD!\n");    
		return (ret);    
	}     
	return (0);
}
       
int twobyte_to_onebyte(char *hex,int L,char *s)
{             
        int i;
        int j;
        int v;
        L=strlen(hex);
        for (i=0;i<L/2;i++) {
                sscanf(hex+i*2,"%2X",&v);
                s[i]=(char)(v&0xFFu);
        } 
        s[i]=0; 
        L=strlen(s);
        for (i=0;i<L;i++) sprintf(hex+2*i,"%02X",(unsigned char)s[i]);
        hex[2*i+2]=0; 
        return 0;
} 


