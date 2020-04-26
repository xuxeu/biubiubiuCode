#pragma once

class RegX
{
public:
	RegX(void);
public:
	~RegX(void);

	bool CheckRead();
	bool CheckWrite();
	int WriteLambadaSerial(const char * theValue,int len);
	int ReadLambadaSerial(char * theValue,int & len);
private:
	int CreateSubKey();
	int ReadReg(HKEY rootKey,const char * subKey,const char * valueName,char * theValue);
	int WriteReg(HKEY rootKey,const char * subKey,const char * valueName,const char * theValue);
	int DeleteValue(HKEY rootKey,const char * subKey,const char * valueName);
};
