//	Author:		zhangyongding
//	Date:		20080922
//	Version:	1.0

#include <stdio.h>
#include <string.h>

#include "uppleDes.h"
//#define DEBUG_DES

// The pPlainKey must be 64bits, i.e. BCD 16chars?????  should be 16 chars ,not bcd 
// pPlainText must be 64bits,i.e. BCD 16chars ??????  also be 16 chars,not bcd
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

// The pPlainKey must be 128bits, i.e. BCD 32chars
// The plaintext must be 64bits, i.e. BCD 16 chars
int Upple3DesEncrypt64BitsText(char *pPlainKey,char *pPlainText,char *pCryptogram)
{
	unsigned char	aKey1[9],aKey2[9];
	unsigned char	pData[9];
	unsigned char	pResult1[9],pResult2[9],pResult3[9];
	int		ret;
	
	/* compress the 16 bytes left key into 8 bytes */
	if ((ret = UppleCompress128BCDInto64Bits(pPlainKey,aKey1)) < 0)
	{
#ifdef DEBUG_DES
		UppleUserErrLog("in Upple3DesEncrypt64BitsText:: UppleCompress128BCDInto64Bits!\n");
#endif
		return(ret);
	}
	/* compress the 16 bytes right key into 8 bytes */
	if ((ret = UppleCompress128BCDInto64Bits(&pPlainKey[16],aKey2)) < 0)
	{
#ifdef DEBUG_DES
		UppleUserErrLog("in Upple3DesEncrypt64BitsText:: UppleCompress128BCDInto64Bits!\n");
#endif
		return(ret);
	}

	// Compress PlainText
	if ((ret = UppleCompress128BCDInto64Bits(pPlainText,pData)) < 0)
	{
#ifdef DEBUG_DES
		UppleUserErrLog("in Upple3DesEncrypt64BitsText:: UppleCompress128BCDInto64Bits!\n");
#endif
		return(ret);
	}

	UPPLE_DES(pData,pResult1,8,aKey1,1);
	UPPLE_DES(pResult1,pResult2,8,aKey2,0);
	UPPLE_DES(pResult2,pResult3,8,aKey1,1);
	
	// Uncompress Result
	if ((ret = UppleUncompress64BitsInto128BCD(pResult3,pCryptogram)) < 0)
	{
#ifdef DEBUG_DES
		UppleUserErrLog("in Upple3DesEncrypt64BitsText:: UppleUncompress64BitsInto128BCD!\n");
#endif
		return(ret);
	}
	
	return(0);
}

// The pPlainKey must be 128bits, i.e. BCD 32chars
// the cryptogram must be 64bits, i.e. BCD 16chars
int Upple3DesDecrypt64BitsText(char *pPlainKey,char *pCryptogram,char *pPlainText)
{
	unsigned char	aKey1[9],aKey2[9];
	unsigned char	pData[9];
	unsigned char	pResult1[9],pResult2[9],pResult3[9];
	int		ret;
	
	/* compress the 16 bytes left key into 8 bytes */
	if ((ret = UppleCompress128BCDInto64Bits(pPlainKey,aKey1)) < 0)
	{
#ifdef DEBUG_DES
		UppleUserErrLog("in Upple3DesDecrypt64BitsText:: UppleCompress128BCDInto64Bits!\n");
#endif
		return(ret);
	}
	/* compress the 16 bytes right key into 8 bytes */
	if ((ret = UppleCompress128BCDInto64Bits(&pPlainKey[16],aKey2)) < 0)
	{
#ifdef DEBUG_DES
		UppleUserErrLog("in Upple3DesDecrypt64BitsText:: UppleCompress128BCDInto64Bits!\n");
#endif
		return(ret);
	}

	// Compress PlainText
	if ((ret = UppleCompress128BCDInto64Bits(pCryptogram,pData)) < 0)
	{
#ifdef DEBUG_DES
		UppleUserErrLog("in Upple3DesEncrypt64BitsText:: UppleCompress128BCDInto64Bits!\n");
#endif
		return(ret);
	}

	UPPLE_DES(pData,pResult1,8,aKey1,0);
	UPPLE_DES(pResult1,pResult2,8,aKey2,1);
	UPPLE_DES(pResult2,pResult3,8,aKey1,0);
	
	// Uncompress Result
	if ((ret = UppleUncompress64BitsInto128BCD(pResult3,pPlainText)) < 0)
	{
#ifdef DEBUG_DES
		UppleUserErrLog("in Upple3DesEncrypt64BitsText:: UppleUncompress64BitsInto128BCD!\n");
#endif
		return(ret);
	}
	
	return(0);
}


// The pPlainKey must be 64bits, i.e. BCD 16chars
// pPlainText must be 64bits,i.e. BCD 16chars
int Upple64BitsKeyEncrypt(char *pPlainKey,char *pPlainText,char *pCryptogram)
{
	unsigned char	aKey[9];
	unsigned char	pData[9];
	unsigned char	pResult[9];
	int		ret;
	
	/* compress the 16 bytes key into 8 bytes */
	if ((ret = UppleCompress128BCDInto64Bits(pPlainKey,aKey)) < 0)
	{
#ifdef DEBUG_DES
		UppleUserErrLog("in Upple64BitsKeyEncrypt:: UppleCompress128BCDInto64Bits!\n");
#endif
		return(ret);
	}

	// Compress PlainText
	if ((ret = UppleCompress128BCDInto64Bits(pPlainText,pData)) < 0)
	{
#ifdef DEBUG_DES
		UppleUserErrLog("in Upple64BitsKeyEncrypt:: UppleCompress128BCDInto64Bits!\n");
#endif
		return(ret);
	}

	UPPLE_DES(pData,pResult,8,aKey,1);
	
	// Uncompress Result
	if ((ret = UppleUncompress64BitsInto128BCD(pResult,pCryptogram)) < 0)
	{
#ifdef DEBUG_DES
		UppleUserErrLog("in Upple64BitsKeyEncrypt:: UppleUncompress64BitsInto128BCD!\n");
#endif
		return(ret);
	}
	
	return(0);
}

// The pPlainKey must be 64bits, i.e. BCD 16chars
// The pCryptogram must be 64bits, i.e. BCD 16chars.
int Upple64BitsKeyDecrypt(char *pPlainKey,char *pCryptogram,char *pPlainText)
{
	unsigned char	aKey[9];
	unsigned char	pData[9];
	unsigned char	pResult[9];
	int		ret;
	
	/* compress the 16 bytes key into 8 bytes */
	if ((ret = UppleCompress128BCDInto64Bits(pPlainKey,aKey)) < 0)
	{
#ifdef DEBUG_DES
		UppleUserErrLog("in Upple64BitsKeyDecrypt:: UppleCompress128BCDInto64Bits!\n");
#endif
		return(ret);
	}

	// Compress Cryptogram
	if ((ret = UppleCompress128BCDInto64Bits(pCryptogram,pData)) < 0)
	{
#ifdef DEBUG_DES
		UppleUserErrLog("in Upple64BitsKeyDecrypt:: UppleCompress128BCDInto64Bits!\n");
#endif
		return(ret);
	}

	UPPLE_DES(pData,pResult,8,aKey,0);
	
	// Uncompress Result
	if ((ret = UppleUncompress64BitsInto128BCD(pResult,pPlainText)) < 0)
	{
#ifdef DEBUG_DES
		UppleUserErrLog("in Upple64BitsKeyDecrypt:: UppleUncompress64BitsInto128BCD!\n");
#endif
		return(ret);
	}
	
	return(0);
}

// The pPlainKey must be 128bits, i.e. BCD 32chars
// The plaintext must be 64bits, i.e. BCD 16 chars
int Upple128BitsKeyEncrypt(char *pPlainKey,char *pPlainText,char *pCryptogram)
{
	unsigned char	aKey1[9],aKey2[9];
	unsigned char	pData[9];
	unsigned char	pResult1[9],pResult2[9],pResult3[9];
	int		ret;
	
	/* compress the 16 bytes left key into 8 bytes */
	if ((ret = UppleCompress128BCDInto64Bits(pPlainKey,aKey1)) < 0)
	{
#ifdef DEBUG_DES
		UppleUserErrLog("in Upple128BitsKeyEncrypt:: UppleCompress128BCDInto64Bits!\n");
#endif
		return(ret);
	}
	/* compress the 16 bytes right key into 8 bytes */
	if ((ret = UppleCompress128BCDInto64Bits(&pPlainKey[16],aKey2)) < 0)
	{
#ifdef DEBUG_DES
		UppleUserErrLog("in Upple128BitsKeyEncrypt:: UppleCompress128BCDInto64Bits!\n");
#endif
		return(ret);
	}

	// Compress PlainText
	if ((ret = UppleCompress128BCDInto64Bits(pPlainText,pData)) < 0)
	{
#ifdef DEBUG_DES
		UppleUserErrLog("in Upple128BitsKeyEncrypt:: UppleCompress128BCDInto64Bits!\n");
#endif
		return(ret);
	}

	UPPLE_DES(pData,pResult1,8,aKey1,1);
	UPPLE_DES(pResult1,pResult2,8,aKey2,0);
	UPPLE_DES(pResult2,pResult3,8,aKey1,1);
	
	// Uncompress Result
	if ((ret = UppleUncompress64BitsInto128BCD(pResult3,pCryptogram)) < 0)
	{
#ifdef DEBUG_DES
		UppleUserErrLog("in Upple128BitsKeyEncrypt:: UppleUncompress64BitsInto128BCD!\n");
#endif
		return(ret);
	}
	
	return(0);
}

// The pPlainKey must be 128bits, i.e. BCD 32chars
// the cryptogram must be 64bits, i.e. BCD 16chars
int Upple128BitsKeyDecrypt(char *pPlainKey,char *pCryptogram,char *pPlainText)
{
	unsigned char	aKey1[9],aKey2[9];
	unsigned char	pData[9];
	unsigned char	pResult1[9],pResult2[9],pResult3[9];
	int		ret;
	
	/* compress the 16 bytes left key into 8 bytes */
	if ((ret = UppleCompress128BCDInto64Bits(pPlainKey,aKey1)) < 0)
	{
#ifdef DEBUG_DES
		UppleUserErrLog("in Upple128BitsKeyDecrypt:: UppleCompress128BCDInto64Bits!\n");
#endif
		return(ret);
	}
	/* compress the 16 bytes right key into 8 bytes */
	if ((ret = UppleCompress128BCDInto64Bits(&pPlainKey[16],aKey2)) < 0)
	{
#ifdef DEBUG_DES
		UppleUserErrLog("in Upple128BitsKeyDecrypt:: UppleCompress128BCDInto64Bits!\n");
#endif
		return(ret);
	}

	// Compress PlainText
	if ((ret = UppleCompress128BCDInto64Bits(pCryptogram,pData)) < 0)
	{
#ifdef DEBUG_DES
		UppleUserErrLog("in Upple128BitsKeyEncrypt:: UppleCompress128BCDInto64Bits!\n");
#endif
		return(ret);
	}

	UPPLE_DES(pData,pResult1,8,aKey1,0);
	UPPLE_DES(pResult1,pResult2,8,aKey2,1);
	UPPLE_DES(pResult2,pResult3,8,aKey1,0);
	
	// Uncompress Result
	if ((ret = UppleUncompress64BitsInto128BCD(pResult3,pPlainText)) < 0)
	{
#ifdef DEBUG_DES
		UppleUserErrLog("in Upple128BitsKeyEncrypt:: UppleUncompress64BitsInto128BCD!\n");
#endif
		return(ret);
	}
	
	return(0);
}

// The pPlainKey must be 128bits, i.e. BCD 32chars
// The plaintext must be 64bits, i.e. BCD 16 chars
int Upple192BitsKeyEncrypt(char *pPlainKey,char *pPlainText,char *pCryptogram)
{
	unsigned char	aKey1[9],aKey2[9],aKey3[9];
	unsigned char	pData[9];
	unsigned char	pResult1[9],pResult2[9],pResult3[9];
	int		ret;
	
	/* compress the 16 bytes left key into 8 bytes */
	if ((ret = UppleCompress128BCDInto64Bits(pPlainKey,aKey1)) < 0)
	{
#ifdef DEBUG_DES
		UppleUserErrLog("in Upple192BitsKeyEncrypt:: UppleCompress128BCDInto64Bits!\n");
#endif
		return(ret);
	}
	/* compress the 16 bytes right key into 8 bytes */
	if ((ret = UppleCompress128BCDInto64Bits(&pPlainKey[16],aKey2)) < 0)
	{
#ifdef DEBUG_DES
		UppleUserErrLog("in Upple192BitsKeyEncrypt:: UppleCompress128BCDInto64Bits!\n");
#endif
		return(ret);
	}
	/* compress the 16 bytes right key into 8 bytes */
	if ((ret = UppleCompress128BCDInto64Bits(&pPlainKey[32],aKey3)) < 0)
	{
#ifdef DEBUG_DES
		UppleUserErrLog("in Upple192BitsKeyEncrypt:: UppleCompress128BCDInto64Bits!\n");
#endif
		return(ret);
	}

	// Compress PlainText
	if ((ret = UppleCompress128BCDInto64Bits(pPlainText,pData)) < 0)
	{
#ifdef DEBUG_DES
		UppleUserErrLog("in Upple192BitsKeyEncrypt:: UppleCompress128BCDInto64Bits!\n");
#endif
		return(ret);
	}

	UPPLE_DES(pData,pResult1,8,aKey1,1);
	UPPLE_DES(pResult1,pResult2,8,aKey2,0);
	UPPLE_DES(pResult2,pResult3,8,aKey3,1);
	
	// Uncompress Result
	if ((ret = UppleUncompress64BitsInto128BCD(pResult3,pCryptogram)) < 0)
	{
#ifdef DEBUG_DES
		UppleUserErrLog("in Upple192BitsKeyEncrypt:: UppleUncompress64BitsInto128BCD!\n");
#endif
		return(ret);
	}
	
	return(0);
}

// The pPlainKey must be 128bits, i.e. BCD 32chars
// the cryptogram must be 64bits, i.e. BCD 16chars
int Upple192BitsKeyDecrypt(char *pPlainKey,char *pCryptogram,char *pPlainText)
{
	unsigned char	aKey1[9],aKey2[9],aKey3[9];
	unsigned char	pData[9];
	unsigned char	pResult1[9],pResult2[9],pResult3[9];
	int		ret;
	
	/* compress the 16 bytes left key into 8 bytes */
	if ((ret = UppleCompress128BCDInto64Bits(pPlainKey,aKey1)) < 0)
	{
#ifdef DEBUG_DES
		UppleUserErrLog("in Upple192BitsKeyDecrypt:: UppleCompress128BCDInto64Bits!\n");
#endif
		return(ret);
	}
	/* compress the 16 bytes right key into 8 bytes */
	if ((ret = UppleCompress128BCDInto64Bits(&pPlainKey[16],aKey2)) < 0)
	{
#ifdef DEBUG_DES
		UppleUserErrLog("in Upple192BitsKeyDecrypt:: UppleCompress128BCDInto64Bits!\n");
#endif
		return(ret);
	}
	/* compress the 16 bytes right key into 8 bytes */
	if ((ret = UppleCompress128BCDInto64Bits(&pPlainKey[32],aKey3)) < 0)
	{
#ifdef DEBUG_DES
		UppleUserErrLog("in Upple192BitsKeyEncrypt:: UppleCompress128BCDInto64Bits!\n");
#endif
		return(ret);
	}

	// Compress PlainText
	if ((ret = UppleCompress128BCDInto64Bits(pCryptogram,pData)) < 0)
	{
#ifdef DEBUG_DES
		UppleUserErrLog("in Upple192BitsKeyEncrypt:: UppleCompress128BCDInto64Bits!\n");
#endif
		return(ret);
	}

	UPPLE_DES(pData,pResult1,8,aKey3,0);
	UPPLE_DES(pResult1,pResult2,8,aKey2,1);
	UPPLE_DES(pResult2,pResult3,8,aKey1,0);
	
	// Uncompress Result
	if ((ret = UppleUncompress64BitsInto128BCD(pResult3,pPlainText)) < 0)
	{
#ifdef DEBUG_DES
		UppleUserErrLog("in Upple192BitsKeyEncrypt:: UppleUncompress64BitsInto128BCD!\n");
#endif
		return(ret);
	}
	
	return(0);
}

