/**********************************************************************************
 *                     北京科银京成技术有限公司 版权所有
 *     Copyright C) 2000-2010 CoreTek Systems Inc All Rights Reserved
**********************************************************************************/

/**
*修改历史:
*2010-9-15 9:57:23       文思惟     北京科银京成技术有限公司
*修改及方案:创建本文件
*/

/**
 *@file   LoaderString.cpp
 *@brief
 *     <li>功能描述...</li>
 *@Note     
 */

/************************头 文 件******************************/
#include "LoaderString.h"
/************************宏 定 义******************************/
/************************类型定义******************************/
/************************全局变量******************************/
/************************模块变量******************************/
/************************外部声明******************************/
/************************前向声明******************************/
/************************实   现*******************************/

namespace com_coretek_tools_stl
{
	const String::size_type String::npos = static_cast<String::size_type >(-1);

	String::Rep String::nullrep_ = { 0, 0, { '\0' } };

	 /*
	  * @brief: 重新分配内存容量
	  * @param[IN] cap: 容量
	  * @return: 无
	  *              
	  */
	void String::reserve(size_type cap)
	{
		if (cap > capacity())
		{
			String tmp;

			/*重新分配内存容量*/
			tmp.init(length(), cap);
			memcpy(tmp.start(), data(), length());
			swap(tmp);
		}
	}

	 /*
	  * @brief: 赋以新值
	  * @param[IN] str: 新字符串
	  * @param[IN] len: 字符串长度
	  * @return: 返回当前字符串指针
	  *              
	  */
	String& String::assign(const char *str, size_type len)
	{
		size_type cap = capacity();

		/*如果字符串的长度大于内存容量从新分配内存并用新的字符串初始化*/
		if (len > cap || cap > 3 * (len + 8))
		{
			String tmp;
			tmp.init(len);
			memcpy(tmp.start(), str, len);
			swap(tmp);
		}
		else
		{  /*若果字符串长度小于容量则直接拷贝并设置大小*/
			memmove(start(), str, len);
			set_size(len);
		}
		return *this;
	}

	 /*
	  * @brief: 尾部添加字符
	  * @param[IN] str: 新字符串
	  * @param[IN] len: 字符串长度
	  * @return: 返回当前字符串指针
	  *              
	  */
	String& String::append(const char *str, size_type len)
	{
		size_type newsize = length() + len;
		/*如果追加的字符加上原来的字符串大于内存容量则从新分配内存容量*/
		if (newsize > capacity())
		{
			reserve (newsize + capacity());
		}

		memmove(finish(), str, len);
		set_size(newsize);
		return *this;
	}

		 /*
	  * @brief: 尾部添加字符
	  * @param[IN] str: 新字符串
	  * @param[IN] len: 字符串长度
	  * @return: 返回当前字符串指针
	  *              
	  */
	String& String::append(String str, size_type len)
	{
		return append(str.c_str(),len);
	}

	 /*
	  * @brief: 设置字符串大小和内存容量
	  * @param[IN] sz: 字符串长度
	  * @param[IN] cap: 内存容量
	  * @return: 无
	  *              
	  */
	void String::init(size_type sz, size_type cap)
	{
		if(cap)
		{
			const size_type bytesNeeded = sizeof(Rep) + cap;
			const size_type intsNeeded = (bytesNeeded + sizeof(int) - 1) / sizeof(int); 
			rep_ = reinterpret_cast<Rep*>(new int[intsNeeded]);
			rep_->str[rep_->size = sz] = '\0';
			rep_->capacity = cap;
		}
		else
		{
			rep_ = &nullrep_;
		}
	}


	 /*
	  * @brief: 查找从起始偏移量开始第一次出现的指定字符
	  * @param[IN] sz: 需要查找的字符
	  * @param[IN] cap: 起始偏移量
	  * @return: 如果成功返回该字符在字符串的指针，不成功返回-1
	  *              
	  */
	String::size_type String::find(char tofind, size_type offset) const
	{
		/*偏移量超过字符串长度*/
		if(offset >= length())
		{
			return npos;
		}

		for (const char *p = c_str() + offset; *p != '\0'; ++p)
		{
			  if(*p == tofind) return static_cast<size_type>(p - c_str());
		}

		return npos;
	}


	 /*
	  * @brief: +运算符重载，实现两个字符串相加           
	  */
	String operator + (const String &a, const String &b)
	{
		String tmp;
		tmp.reserve(a.length() + b.length());
		tmp += a;
		tmp += b;
		return tmp;
	}

	 /*
	  * @brief: +运算符重载，实现两个字符串相加           
	  */
	String operator + (const String &a, const char *b)
	{
		String tmp;
		String::size_type b_len = static_cast<String::size_type>(strlen(b));
		tmp.reserve(a.length() + b_len);
		tmp += a;
		tmp.append(b, b_len);
		return tmp;
	}

	 /*
	  * @brief: +运算符重载，实现两个字符串相加           
	  */
	String operator + (const char *a, const String &b)
	{
		String tmp;
		String::size_type a_len = static_cast<String::size_type>( strlen(a) );
		tmp.reserve(a_len + b.length());
		tmp.append(a, a_len);
		tmp += b;
		return tmp;
	}
}