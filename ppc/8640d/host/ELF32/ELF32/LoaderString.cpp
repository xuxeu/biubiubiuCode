/**********************************************************************************
 *                     �����������ɼ������޹�˾ ��Ȩ����
 *     Copyright C) 2000-2010 CoreTek Systems Inc All Rights Reserved
**********************************************************************************/

/**
*�޸���ʷ:
*2010-9-15 9:57:23       ��˼Ω     �����������ɼ������޹�˾
*�޸ļ�����:�������ļ�
*/

/**
 *@file   LoaderString.cpp
 *@brief
 *     <li>��������...</li>
 *@Note     
 */

/************************ͷ �� ��******************************/
#include "LoaderString.h"
/************************�� �� ��******************************/
/************************���Ͷ���******************************/
/************************ȫ�ֱ���******************************/
/************************ģ�����******************************/
/************************�ⲿ����******************************/
/************************ǰ������******************************/
/************************ʵ   ��*******************************/

namespace com_coretek_tools_stl
{
	const String::size_type String::npos = static_cast<String::size_type >(-1);

	String::Rep String::nullrep_ = { 0, 0, { '\0' } };

	 /*
	  * @brief: ���·����ڴ�����
	  * @param[IN] cap: ����
	  * @return: ��
	  *              
	  */
	void String::reserve(size_type cap)
	{
		if (cap > capacity())
		{
			String tmp;

			/*���·����ڴ�����*/
			tmp.init(length(), cap);
			memcpy(tmp.start(), data(), length());
			swap(tmp);
		}
	}

	 /*
	  * @brief: ������ֵ
	  * @param[IN] str: ���ַ���
	  * @param[IN] len: �ַ�������
	  * @return: ���ص�ǰ�ַ���ָ��
	  *              
	  */
	String& String::assign(const char *str, size_type len)
	{
		size_type cap = capacity();

		/*����ַ����ĳ��ȴ����ڴ��������·����ڴ沢���µ��ַ�����ʼ��*/
		if (len > cap || cap > 3 * (len + 8))
		{
			String tmp;
			tmp.init(len);
			memcpy(tmp.start(), str, len);
			swap(tmp);
		}
		else
		{  /*�����ַ�������С��������ֱ�ӿ��������ô�С*/
			memmove(start(), str, len);
			set_size(len);
		}
		return *this;
	}

	 /*
	  * @brief: β������ַ�
	  * @param[IN] str: ���ַ���
	  * @param[IN] len: �ַ�������
	  * @return: ���ص�ǰ�ַ���ָ��
	  *              
	  */
	String& String::append(const char *str, size_type len)
	{
		size_type newsize = length() + len;
		/*���׷�ӵ��ַ�����ԭ�����ַ��������ڴ���������·����ڴ�����*/
		if (newsize > capacity())
		{
			reserve (newsize + capacity());
		}

		memmove(finish(), str, len);
		set_size(newsize);
		return *this;
	}

		 /*
	  * @brief: β������ַ�
	  * @param[IN] str: ���ַ���
	  * @param[IN] len: �ַ�������
	  * @return: ���ص�ǰ�ַ���ָ��
	  *              
	  */
	String& String::append(String str, size_type len)
	{
		return append(str.c_str(),len);
	}

	 /*
	  * @brief: �����ַ�����С���ڴ�����
	  * @param[IN] sz: �ַ�������
	  * @param[IN] cap: �ڴ�����
	  * @return: ��
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
	  * @brief: ���Ҵ���ʼƫ������ʼ��һ�γ��ֵ�ָ���ַ�
	  * @param[IN] sz: ��Ҫ���ҵ��ַ�
	  * @param[IN] cap: ��ʼƫ����
	  * @return: ����ɹ����ظ��ַ����ַ�����ָ�룬���ɹ�����-1
	  *              
	  */
	String::size_type String::find(char tofind, size_type offset) const
	{
		/*ƫ���������ַ�������*/
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
	  * @brief: +��������أ�ʵ�������ַ������           
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
	  * @brief: +��������أ�ʵ�������ַ������           
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
	  * @brief: +��������أ�ʵ�������ַ������           
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