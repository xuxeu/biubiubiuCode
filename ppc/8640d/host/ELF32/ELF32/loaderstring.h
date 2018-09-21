/**********************************************************************************
 *                     北京科银京成技术有限公司 版权所有
 *     Copyright C) 2000-2010 CoreTek Systems Inc All Rights Reserved
**********************************************************************************/

/**
 *修改历史:
       2010-9-15 9:55:08        amwyga      北京科银京成技术有限公司
       修改及方案:创建本文件
 */

/**
 *@file   LoaderString.h
 *@brief
       <li>加载器用到的字符串类,
	   支持标准模板库String的大多数功能，并且可以和基本类型自动相互转换
	   </li>
 *@Note
       
 */

#ifndef _INCLUDE_LOADERSTRING_H_2010915_AMWYGA
#define _INCLUDE_LOADERSTRING_H_2010915_AMWYGA
/************************头 文 件******************************/
#include "String.h"
#include "assert.h"

#include "basictypes.h"
#include "basicNumber.h"
#include "utils.h"


/************************引    用******************************/
/************************宏 定 义******************************/
#define ELF32_API __declspec(dllexport)
/************************类型定义******************************/

namespace com_coretek_tools_stl
{
	class ELF32_API String
	{
	public :
  		typedef size_t size_type;

		static const size_type npos ;//= -1;

		/*空构造函数*/
		String () : rep_(&nullrep_)
		{
		}

		/*复制构造函数*/
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
		*字符串转化成小写
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
		*字符串转化成大写
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
			去掉字符串前后的空格
		*@param
			待处理字符串
		*@Return
			处理后的字符串
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
			while (*pSrc != '\0')//去掉前面的空格和制表符
			{
				if (*pSrc == ' ' || *pSrc == '\t')
				{
					pSrc++;
					count++;
				}else{
					break;
				}
			}

			//说明前面有空格或制表符
			if (pSrc != *src)
			{
				//把后面的字面的字符依次前移
				UINT8* temp = *src;
				UINT8* temp2 = pSrc;
				while(*temp2 != '\0')
				{
					*temp = *temp2;
					temp++;
					temp2++;
				}

				//去掉后面多余的字符
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
			判断字符c在字符串buf中的位置(从0开始)
		 *@param buf
			判断的字符串
		 *@param c
			判断的字符
		 *@Return
			找到返回相应的位置，否则返回-1
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
			是否以startStr开头(要区分大小写)
		 *@param startStr
				指定开头的字符串
		 *@Return
				s是以startStr开头返回TRUE,否则返回FALSE
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
			是否以endStr结尾(要区分大小写)
		*@param startStr
			指定结尾的字符串
		*@Return
			s是以endStr开头返回TRUE,否则返回FALSE
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

		//析构函数
		~String ()
		{
			quit();
		}

		//=运算符重载
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

		/*获得字符串起始指针*/
		const char *c_str () const { return rep_->str; }

		const char *data () const { return rep_->str; }

		/*获得字符串长度 */
		size_type length() const { return rep_->size; }

		size_type size() const { return rep_->size; }

		/* 检查字符串是否为空 */
		bool empty () const { return rep_->size == 0; }

		/* 获得字符串容量 */
		size_type capacity () const { return rep_->capacity; }


		/*下标访问,越界访问判断 */
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

		/*查找指定字符 */
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

		/*查找指定字符的第一次出现,这里仅仅是为了匹配c++实现，pos无效*/
		size_type find_first_of(char lookup,size_t pos) const
		{
			return find(lookup, pos);
		}

		//获取子字符串，必须传入2个个参数
		String substr(size_t startIndex = 0,size_t length = npos) const
		{
			String tmp;

			if(npos == length)
			{
				length = rep_->size - startIndex;
			}

			//加1，用于存放结束符
			char *charBuffer = new char[length+1];
			memset(charBuffer,0,(length+1) * sizeof(char));
			memcpy(charBuffer,((char*)start() + startIndex),length);
			//存放结束符
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

		/*重新设置内存大小*/
		void reserve(size_type cap);
		
		String& assign(const char* str, size_type len);

		/*追加字符*/
		//String& append(const char* str, size_type len);
		/*追加字符*/
		String& append(const char* str, size_type len = npos);

		String& append(String str, size_type len = npos);

		/*字符串交换*/
		void swap(String& other)
		{
			Rep *r = rep_;
			rep_ = other.rep_;
			other.rep_ = r;
		}

	  private:

		void init(size_type sz) { init(sz, sz); }
		
		void set_size(size_type sz) { rep_->str[ rep_->size = sz ] = '\0'; }
		
		/*获取字符串的头指针*/
		char *start() const { return rep_->str; }

		/*获取字符串的未指针*/
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
