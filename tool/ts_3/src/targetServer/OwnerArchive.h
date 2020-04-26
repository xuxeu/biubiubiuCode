
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
* TS���ݰ��Ĺ���Ͱ��Ľṹ
*
* @file 	ownerarchive.cpp
* @brief 	�������л�����
* @author 	���
* @date 	2004-12-6
* @version 3.0
* @warning �Ͻ������ļ������ַ�
*
* ���ţ�ϵͳ�� 
*
*/

#ifndef OWNERARCHIVE__INCLUDED_
#define OWNERARCHIVE__INCLUDED_

#include <QDataStream>  //QT���Ӧ�����л����� 

class OwnerArchive {
private:
public: 
	QDataStream *pDataStream;		//��Ӧ������
	//��2��״̬,load�ӻ�������ȡ����.storing����������д����
	enum ARCHIVESTATUS{load = 0,storing} status;	

	OwnerArchive(ARCHIVESTATUS c) {
		status = c;
	}
	~OwnerArchive() {
	}
	/**
	* �������ݶ���
	* @param socketDescriptor socket��
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
