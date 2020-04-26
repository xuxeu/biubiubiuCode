/**********************************************************************************
 *                     �����������ɼ������޹�˾ ��Ȩ����
 *     Copyright C) 2000-2010 CoreTek Systems Inc All Rights Reserved
**********************************************************************************/

/**
 *�޸���ʷ:
       2010-9-15 9:55:08        amwyga      �����������ɼ������޹�˾
       �޸ļ�����:�������ļ�
 */

/**
 *@file   LoaderString.h
 *@brief
       <li>�������õ����ַ�����,
	   ֧�ֱ�׼ģ���String�Ĵ�������ܣ����ҿ��Ժͻ��������Զ��໥ת��
	   </li>
 *@Note
       
 */

#ifndef _INCLUDE_LOADERSTRING_H_2010915_AMWYGA
#define _INCLUDE_LOADERSTRING_H_2010915_AMWYGA
/************************ͷ �� ��******************************/
#include "String.h"
#include "assert.h"

#include "basictypes.h"
#include "basicNumber.h"
#include "utils.h"


/************************��    ��******************************/
/************************�� �� ��******************************/
#define ELF32_API __declspec(dllexport)
/************************���Ͷ���******************************/

namespace com_coretek_tools_stl
{
	class ELF32_API String
	{
	public :
  		typedef size_t size_type;

		static const size_type npos ;//= -1;

		/*�չ��캯��*/
		String () : rep_(&nullrep_)
		{
		}

		/*���ƹ��캯��*/
		String (const String& copy) : rep_(0)
		{
			init(copy.length());
			memcpy(start(), copy.data(), length());
		}

		 String (const char *copy) : rep_(0)
		{
			init(static_cast<size_type>(strlen(copy)));
			memcpy(start(), copy, length());
		}

		String (const unsigned char *copy) : rep_(0)
		{
			init(static_cast<size_type>(strlen((const char*)copy)));
			memcpy(start(), (const char*)copy, length());
		}

		 String (char *copy) : rep_(0)
		{
			init(static_cast<size_type>(strlen(copy)));
			memcpy(start(), copy, length());
		}

		String (int i) : rep_(0)
		{			
			char tempStr[TEMPSTR_LENGTH];
			char* pTempStr = tempStr;
			memset(tempStr,0,TEMPSTR_LENGTH);
			ITOA(i,tempStr,TEMPSTR_LENGTH,10);

			init(static_cast<size_type>(strlen(pTempStr)));
			memcpy(start(), pTempStr, length());
		}

		String (unsigned int i) : rep_(0)
		{			
			char tempStr[TEMPSTR_LENGTH];
			char* pTempStr = tempStr;
			memset(tempStr,0,TEMPSTR_LENGTH);
			ITOA(i,tempStr,TEMPSTR_LENGTH,10);

			init(static_cast<size_type>(strlen(pTempStr)));
			memcpy(start(), pTempStr, length());
		}

		String (unsigned long long i) : rep_(0)
		{			
			char tempStr[TEMPSTR_LENGTH];
			char* pTempStr = tempStr;
			memset(tempStr,0,TEMPSTR_LENGTH);
			I64TOA(i,tempStr,TEMPSTR_LENGTH,10);

			init(static_cast<size_type>(strlen(pTempStr)));
			memcpy(start(), pTempStr, length());
		}

		String (long long i) : rep_(0)
		{			
			char tempStr[TEMPSTR_LENGTH];
			char* pTempStr = tempStr;
			memset(tempStr,0,TEMPSTR_LENGTH);
			I64TOA(i,tempStr,TEMPSTR_LENGTH,10);

			init(static_cast<size_type>(strlen(pTempStr)));
			memcpy(start(), pTempStr, length());
		}

		 String (const char *str, size_type len) : rep_(0)
		{
			init(len);
			memcpy(start(), str, len);
		}

		/**
		*�ַ���ת����Сд
		*/
		String toLower() const
		{
			UINT8 *str=new UINT8[length()+1];
			UINT8 *cStr=str;
			memset(cStr,0,length()+1);
			memcpy(cStr,c_str(),length());
			STRLWR((char*)cStr,length()+1);
			String result =str;

			delete[] str;
			str = NULL;
			cStr = NULL;

			return result;
		}

		/**
		*�ַ���ת���ɴ�д
		*/
		String toUpper() const
		{
			UINT8 *cStr=new UINT8[length()+1];
			memset(cStr,0,length()+1);
			memcpy(cStr,c_str(),length());

			STRUPR((char*)cStr,length()+1);
			String result = cStr;

			delete[] cStr;
			cStr = NULL;

			return result;
		}

		/**
		*@brief
			ȥ���ַ���ǰ��Ŀո�
		*@param
			�������ַ���
		*@Return
			�������ַ���
		*/
		String trim() const
		{
			UINT8 *tempStr=new UINT8[length()+1];
			memset(tempStr,0,length()+1);
			memcpy(tempStr,c_str(),length());

			UINT8 **src=&tempStr;
			UINT8* pSrc = *src;			

			INT32 i=0;
			INT32 count = 0;
			while (*pSrc != '\0')//ȥ��ǰ��Ŀո���Ʊ��
			{
				if (*pSrc == ' ' || *pSrc == '\t')
				{
					pSrc++;
					count++;
				}else{
					break;
				}
			}

			//˵��ǰ���пո���Ʊ��
			if (pSrc != *src)
			{
				//�Ѻ����������ַ�����ǰ��
				UINT8* temp = *src;
				UINT8* temp2 = pSrc;
				while(*temp2 != '\0')
				{
					*temp = *temp2;
					temp++;
					temp2++;
				}

				//ȥ�����������ַ�
				for(INT32 j = 0; j < count; j++)
				{
					*temp = '\0';
					temp++;
				}
			}

			pSrc = *src;
			count=(INT32)strlen((char*)pSrc);
			i=count;
			for (INT32 j=count-1;j>0;j=j-1)
			{
				if (pSrc[j] != ' ' && pSrc[j]!='\t')
				{
					break;
				}

				pSrc[j]='\0';
			}


			String result=pSrc;

			delete[] tempStr;
			tempStr = NULL;

			return result;
		}

		 /**
		 *@brief
			�ж��ַ�c���ַ���buf�е�λ��(��0��ʼ)
		 *@param buf
			�жϵ��ַ���
		 *@param c
			�жϵ��ַ�
		 *@Return
			�ҵ�������Ӧ��λ�ã����򷵻�-1
		 */
		INT32 indexOf(UINT8 c) const
		{
			INT32 len = (INT32)length();
			for (INT32 i = 0; i < len; i=i+1)
			{
				if (at(i) == c)
				{
					return i;
				}
			}

			return -1;
		}

		INT32 indexOf(String substr) const
		{
			return (INT32)find(substr);
		}

		/**
		 *@brief
			�Ƿ���startStr��ͷ(Ҫ���ִ�Сд)
		 *@param startStr
				ָ����ͷ���ַ���
		 *@Return
				s����startStr��ͷ����TRUE,���򷵻�FALSE
		 */
		BOOL startsWith(String &startStr) const
		{
			return ((INT32)find(startStr) == 0);
		}

		BOOL startsWith(const char* startStr) const
		{
			return ((INT32)find(startStr) == 0);
		}

		/**
		*@brief
			�Ƿ���endStr��β(Ҫ���ִ�Сд)
		*@param startStr
			ָ����β���ַ���
		*@Return
			s����endStr��ͷ����TRUE,���򷵻�FALSE
		*/
		BOOL endsWith(String &endStr)
		{
			if ((INT32)find(endStr)+endStr.length() == length())
			{
				return TRUE;
			}else{
				return FALSE;
			}
		}

		//��������
		~String ()
		{
			quit();
		}

		//=���������
		String& operator = (const char* copy)
		{
			return assign( copy, (size_type)strlen(copy));
		}

		String& operator = (char* copy)
		{
			return assign( copy, (size_type)strlen(copy));
		}

		String& operator = (const String  &copy)
		{
			return assign(copy.start(), copy.length());
		}

		String& operator += (const char *suffix)
		{
			return append(suffix, static_cast<size_type>( strlen(suffix) ));
		}

		String& operator += (char single)
		{
			return append(&single, 1);
		}

		String &operator += (const String  &suffix)
		{
			return append(suffix.data(), suffix.length());
		}

		String& operator=(int i)
		{
			char tempStr[TEMPSTR_LENGTH];
			memset(tempStr,0,TEMPSTR_LENGTH);
			ITOA(i,tempStr,TEMPSTR_LENGTH,10);
			*this = tempStr;
			return *this;
		}

		String& operator=(unsigned int i)
		{
			char tempStr[TEMPSTR_LENGTH];
			memset(tempStr,0,TEMPSTR_LENGTH);
			ITOA(i,tempStr,TEMPSTR_LENGTH,10);
			*this = tempStr;
			return *this;
		}

		String& operator=(unsigned long long i)
		{
			char tempStr[TEMPSTR_LENGTH];
			memset(tempStr,0,TEMPSTR_LENGTH);
			I64TOA(i,tempStr,TEMPSTR_LENGTH,10);
			*this = tempStr;
			return *this;
		}

		String& operator=(long long i)
		{
			char tempStr[TEMPSTR_LENGTH];
			memset(tempStr,0,TEMPSTR_LENGTH);
			I64TOA(i,tempStr,TEMPSTR_LENGTH,10);
			*this = tempStr;
			return *this;
		}

		virtual operator int ()
		{
			return atoi(c_str());			
		}

		virtual operator unsigned int ()
		{
			return (unsigned int)atol(c_str());			
		}

		virtual operator unsigned long long ()
		{
			return (unsigned long long)atol(c_str());			
		}

		virtual operator long long ()
		{
			return (long long)atol(c_str());			
		}

		virtual operator double ()
		{
			return atof(c_str());			
		}

		/*����ַ�����ʼָ��*/
		const char *c_str () const { return rep_->str; }

		const char *data () const { return rep_->str; }

		/*����ַ������� */
		size_type length() const { return rep_->size; }

		size_type size() const { return rep_->size; }

		/* ����ַ����Ƿ�Ϊ�� */
		bool empty () const { return rep_->size == 0; }

		/* ����ַ������� */
		size_type capacity () const { return rep_->capacity; }


		/*�±����,Խ������ж� */
		const char& at(size_type index) const
		{
			assert(index < length());
			return rep_->str[index];
		}

		char& operator [] (size_type index) const
		{
			assert( index < length() );
			return rep_->str[ index ];
		}

		/*����ָ���ַ� */
		size_type find(char lookup) const
		{
			return find(lookup, 0);
		}

		size_type find(const char* lookup) const
		{
			char* pStr = strstr(rep_->str,lookup);
			if (pStr == NULL)
			{
				return 0;
			}

			return pStr - c_str();
		}

		size_type find(String lookup) const
		{			
			return find(lookup.c_str());
		}

		void replace(const char ch1,const char ch2)
		{
			for(int i=0; i<(int)size(); i=i+1)
			{
				if (ch1 == *((char*)rep_->str + i))
				{
					*((char*)rep_->str + i) = ch2;
				}
			}
		}

		size_type find(char tofind, size_type offset) const;

		/*����ָ���ַ��ĵ�һ�γ���,���������Ϊ��ƥ��c++ʵ�֣�pos��Ч*/
		size_type find_first_of(char lookup,size_t pos) const
		{
			return find(lookup, pos);
		}

		//��ȡ���ַ��������봫��2��������
		String substr(size_t startIndex = 0,size_t length = npos) const
		{
			String tmp;

			if(npos == length)
			{
				length = rep_->size - startIndex;
			}

			//��1�����ڴ�Ž�����
			char *charBuffer = new char[length+1];
			memset(charBuffer,0,(length+1) * sizeof(char));
			memcpy(charBuffer,((char*)start() + startIndex),length);
			//��Ž�����
			charBuffer[length] = '\0';
			tmp = charBuffer;
			delete[] charBuffer;

			return tmp;
		}
		
		int compare(const char* str) const
		{
			return strcmp(c_str(),str);
		}

		void clear()
		{
			quit();
			init(0,0);
		}

		/*���������ڴ��С*/
		void reserve(size_type cap);
		
		String& assign(const char* str, size_type len);

		/*׷���ַ�*/
		//String& append(const char* str, size_type len);
		/*׷���ַ�*/
		String& append(const char* str, size_type len = npos);

		String& append(String str, size_type len = npos);

		/*�ַ�������*/
		void swap(String& other)
		{
			Rep *r = rep_;
			rep_ = other.rep_;
			other.rep_ = r;
		}

	  private:

		void init(size_type sz) { init(sz, sz); }
		
		void set_size(size_type sz) { rep_->str[ rep_->size = sz ] = '\0'; }
		
		/*��ȡ�ַ�����ͷָ��*/
		char *start() const { return rep_->str; }

		/*��ȡ�ַ�����δָ��*/
		char *finish() const { return rep_->str + rep_->size; }

		struct Rep
		{
			size_type size;
			size_type capacity;
			char str[1];
		};

		void init(size_type sz, size_type cap);

		void quit()
		{
			if (rep_ != &nullrep_)
			{
				delete [] (reinterpret_cast<int*>(rep_ ));
			}
		}

		Rep  *rep_;
		static Rep nullrep_;
	} ;


	inline bool operator == (const String & a, const String & b)
	{
		return    ( a.length() == b.length() )				
			   && ( strcmp(a.c_str(), b.c_str()) == 0 );
	}
	inline bool operator < (const String & a, const String & b)
	{
		return strcmp(a.c_str(), b.c_str()) < 0;
	}

	inline bool operator != (const String & a, const String & b) { return !(a == b); }
	inline bool operator >  (const String & a, const String & b) { return b < a; }
	inline bool operator <= (const String & a, const String & b) { return !(b < a); }
	inline bool operator >= (const String & a, const String & b) { return !(a < b); }

	inline bool operator == (const String &a, const char *b) { return strcmp(a.c_str(), b) == 0; }
	inline bool operator == (const char *a, const String &b) { return b == a; }
	inline bool operator != (const String &a, const char *b) { return !(a == b); }
	inline bool operator != (const char *a, const String &b) { return !(b == a); }

	String operator + (const String &a, const String &b);
	String operator + (const String &a, const char *b);
	String operator + (const char *a, const String &b);
}

#endif
