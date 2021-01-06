
#include <string.h>
#include "mymath.h"
#include <ctype.h>

void Ascii_to_hex_char(char *dstbuf,char*rstbuf)
{
	
	//char *p = "jyh:abcdefghijklmnop";
	unsigned char  i,len;
	len = strlen(rstbuf);
	//printf(rstbuf);
	for(i =0;i<len;i++)
	{
		sprintf(dstbuf+i*2,"%2x",rstbuf[i]);
	}
	dstbuf[i*2]=0;
	//printf(dstbuf);
}  

void HexStrToByte(const char* source, unsigned char* dest, int sourceLen)  
{  
    short i;  
    unsigned char highByte, lowByte;  
      
    for (i = 0; i < sourceLen; i += 2)  
    {  
        highByte = toupper(source[i]);  
        lowByte  = toupper(source[i + 1]);  
				highByte = source[i];  
        lowByte  = source[i + 1]; 
        if (highByte > 0x39)  
            highByte -= 0x37;  
        else  
            highByte -= 0x30;  
  
        if (lowByte > 0x39)  
            lowByte -= 0x37;  
        else  
            lowByte -= 0x30;  
  
        dest[i/2] = (highByte << 4) | lowByte;  
    }  
    return ;  
} 

//�ֽ���ת��Ϊʮ�������ַ���  
void ByteToHexStr(const unsigned char* source, char* dest, int sourceLen)  
{  
    short i;  
    unsigned char highByte, lowByte;  
  
    for (i = 0; i < sourceLen; i++)  
    {  
        highByte = source[i] >> 4;  
        lowByte = source[i] & 0x0f ;  
  
        highByte += 0x30;   
        if (highByte > 0x39)  
                dest[i * 2] = highByte + 0x07;  
        else  
                dest[i * 2] = highByte;  
  
        lowByte += 0x30;  
        if (lowByte > 0x39)  
            dest[i * 2 + 1] = lowByte + 0x07;  
        else  
            dest[i * 2 + 1] = lowByte;  
    }  
    return ;  
}  


//�ֽ���ת��Ϊʮ�������ַ�������һ��ʵ�ַ�ʽ  
void Hex2Str( const char *sSrc,  char *sDest, int nSrcLen )  
{  
    int  i;  
    char szTmp[3];  
  
    for( i = 0; i < nSrcLen; i++ )  
    {  
        sprintf( szTmp, "%02X", (unsigned char) sSrc[i] );  
        memcpy( &sDest[i * 2], szTmp, 2 );  
    }  
    return ;  
}  
//���ַ���ת��Ϊʮ�������ַ����Ĵ��룺
int Myatoi(char *buf)
{
	int result = 0;
	char ch;
	while((ch = *(buf++)) != '\0')
	{ 
		result = result*10 + ch-'0';
	}
	return result;
}

//��ʮ����ת���ַ���
char * Myitoa(int val)
{
	static char szTmp[4];
	memset(szTmp,0,4);
	sprintf(szTmp, "%02d", val );
	return szTmp;
}


/**************************************************************************** 
��������: str_to_hex 
��������: �ַ���ת��Ϊʮ������ 
�������: string �ַ��� cbuf ʮ������ len �ַ����ĳ��ȡ� 
�������: �� 
*****************************************************************************/   
int str_to_hex(char *string, unsigned char *cbuf, int len)  
{
	unsigned char high, low;
	int idx, ii=0;	
	for (idx=0; idx<len; idx+=2)   
	{  
		high = string[idx];  
		low = string[idx+1];  
		  
		if(high>='0' && high<='9')	
			high = high-'0';  
		else if(high>='A' && high<='F')  
			high = high - 'A' + 10;  
		else if(high>='a' && high<='f')  
			high = high - 'a' + 10;  
		else  
			return -1;	
		  
		if(low>='0' && low<='9')  
			low = low-'0';	
		else if(low>='A' && low<='F')  
			low = low - 'A' + 10;  
		else if(low>='a' && low<='f')  
			low = low - 'a' + 10;  
		else  
			return -1;	
		  
		cbuf[ii++] = high<<4 | low;  
	}  
	return 0;  
}
  

/**************************************************************************** 
��������: hex_to_str 
��������: ʮ������ת�ַ��� 0->30 1->31
�������: ptr �ַ��� buf ʮ������ len ʮ�������ַ����ĳ��ȡ� 
�������: �� 
*****************************************************************************/   
void hex_to_str(char *ptr,unsigned char *buf,int len)  
{  
	for(int i = 0; i < len; i++)  
	{  
		sprintf(ptr, "%02x",buf[i]);  
		ptr += 2;
	}  
}  


/**************************************************************************** 
��������: hexstr_to_byte 
��������: ʮ�������ַ���ת�ֽ�����
*****************************************************************************/   
int hexstr_to_byte(const unsigned char* source)  
{
	int i=0,ret=0;
	char dest[2];

	for(i=0;i<2;i++)
	{
		dest[i]=source[i]-'0';
		if((dest[i] < 0) || (dest[i] > 54))
			return 0;
		if((dest[i]> 9) && (dest[i]< 23))					//A~F
			dest[i] -= 7;
		else if((dest[i]> 48) && (dest[i]<55))		//a~f
			dest[i] -= 39;
//		printf("dest[%d]:%d\r\n",i,dest[i]);
	}
	ret = dest[0]*16+dest[1];

	return ret;
	
}  

/**************************************************************************** 
��������: decstr_to_int 
��������: ʮ�����ַ���ת����
*****************************************************************************/   
int decstr_to_int(const unsigned char* src)  
{
	int i=0,ret=0;
	int len = strlen(src);
	for(i=0;i<len;i++)
	{
		if((src[i] < '0') || (src[i] > '9'))
			return -1;
		ret =ret*10+(src[i]-'0');
	}

	return ret;
} 



/**************************************************************************** 
src Դ�ַ������׵�ַ(buf�ĵ�ַ) 
separator ָ���ķָ��ַ�
dest �������ַ���������
num �ָ�����ַ����ĸ���
*****************************************************************************/
void split(char *src,const char *separator,char **dest,int *num) {
	
     char *pNext;
     int count = 0;
     if (src == NULL || strlen(src) == 0) //�������ĵ�ַΪ�ջ򳤶�Ϊ0��ֱ����ֹ 
        return;
     if (separator == NULL || strlen(separator) == 0) //��δָ���ָ���ַ�����ֱ����ֹ 
        return;
     pNext = (char *)strtok(src,separator); //����ʹ��(char *)����ǿ������ת��(��Ȼ��д�еı������в������ָ�����)
     while(pNext != NULL) {
		if(dest)
          *dest++ = pNext;
		++count;
		pNext = (char *)strtok(NULL,separator);  //����ʹ��(char *)����ǿ������ת��
    }
	if(num)
    	*num = count;
} 	












