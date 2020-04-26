
/*****************************************************************
*    This source code has been made available to you by CoreTek on an AS-IS
*    basis. Anyone receiving this source is licensed under CoreTek
*    copyrights to use it in any way he or she deems fit, including
*    copying it, modifying it, compiling it, and redistributing it either
*    with or without modifications.
*
*    Any person who transfers this source code or any derivative work
*    must include the CoreTek copyright notice, this paragraph, and the
*    preceding two paragraphs in the transferred software.
*
*    COPYRIGHT   CoreTek  CORPORATION 2004
*    LICENSED MATERIAL  -  PROGRAM PROPERTY OF CoreTek
***********************************************************************/

/**
* TS数据包的管理和包的结构
*
* @file 	ownerarchive.cpp
* @brief 	用于序列话操作
* @author 	彭宏
* @date 	2004-12-6
* @version 3.0
* @warning 严禁将本文件到处分发
*
* 部门：系统部 
*
*/

#ifndef OWNERARCHIVE__INCLUDED_
#define OWNERARCHIVE__INCLUDED_

#include <QDataStream>  //QT库对应的序列化对象 

class OwnerArchive {
private:
public: 
	QDataStream *pDataStream;		//对应的数据
	//有2种状态,load从缓冲区读取数据.storing是往缓冲区写数据
	enum ARCHIVESTATUS{load = 0,storing} status;	

	OwnerArchive(ARCHIVESTATUS c) {
		status = c;
	}
	~OwnerArchive() {
	}
	/**
	* 设置数据对象
	* @param socketDescriptor socket号
	*/
	void setDataStream(QDataStream*  pDataStream) {
		this->pDataStream = pDataStream;                        
	}
	void changeMode(ARCHIVESTATUS c) {
		status = c;
	}
	bool IsStoring(){ return status == storing;};
};

#endif // !defined(OWNERARCHIVE__INCLUDED_)
