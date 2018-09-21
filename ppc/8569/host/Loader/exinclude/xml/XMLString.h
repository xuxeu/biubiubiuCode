/************************************************************************
 *                北京科银京成技术有限公司 版权所有
 *      Copyright (C) 2000-2009 CoreTek Systems Inc. All Rights Reserved.
 ***********************************************************************/

/*
 * $Log: String.h,v $
 * Revision 1.0  2009/10/20 10:10:00
 * 创建文件。
 */


/*
 * @file   String.h
 * @brief
 *    <li>字符串类提供字符串的相关操作</li>
 *                      
 * @author   彭元志
 * @date     2009-10-20
 * <p>部门： 系统部
 */

#ifndef _XMLSTRING_H
#define _XMLSTRING_H

#include <assert.h>
#include <string.h>


#if defined(_MSC_VER) && (_MSC_VER >= 1200 )
	#define XML_EXPLICIT explicit
#elif defined(__GNUC__) && (__GNUC__ >= 3 )
	#define XML_EXPLICIT explicit
#else
	#define XML_EXPLICIT
#endif

class String
{
  public :

  	typedef size_t size_type;

	static const size_type npos; // = -1;

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

	XML_EXPLICIT String (const char *copy) : rep_(0)
	{
		init(static_cast<size_type>(strlen(copy)));
		memcpy(start(), copy, length());
	}

	XML_EXPLICIT String (const char *str, size_type len) : rep_(0)
	{
		init(len);
		memcpy(start(), str, len);
	}

	//析构函数
	~String ()
	{
		quit();
	}

	//=运算符重载
	String& operator = (const char * copy)
	{
		return assign( copy, (size_type)strlen(copy));
	}

	String& operator = (const String & copy)
	{
		return assign(copy.start(), copy.length());
	}


	String &operator += (const char *suffix)
	{
		return append(suffix, static_cast<size_type>( strlen(suffix) ));
	}

	String &operator += (char single)
	{
		return append(&single, 1);
	}

	String &operator += (const String  &suffix)
	{
		return append(suffix.data(), suffix.length());
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

	size_type find(char tofind, size_type offset) const;

	void clear()
	{
		quit();
		init(0,0);
	}

	/*重新设置内存大小*/
	void reserve(size_type cap);
	
	String& assign(const char* str, size_type len);

	/*追加字符*/
	String& append(const char* str, size_type len);

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
		size_type size, capacity;
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

class XmlOutStream : public String
{
public :

	XmlOutStream & operator << (const String & in)
	{
		*this += in;
		return *this;
	}

	XmlOutStream & operator << (const char * in)
	{
		*this += in;
		return *this;
	}

} ;

#endif
