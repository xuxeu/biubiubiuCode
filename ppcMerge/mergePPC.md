# mergePPC

[TOC]

## 工作目的

完成PPC目标机端代码的合并，使ppc版本：8569,8280,8640d，p2020，使这四个版本的ppc合并成一个目标机端源码，我把这个工作取名mergePPC。

## 版本依据

根据笑哥的给予svn路径，从以下取得：

**8280:**

http://192.168.16.4/svn/PPC/branches/task/022ppc8280针对(0.0.0.9)自测试修改后打包/output/code(没有工具库源码)

**8569:**

http://192.168.16.4/svn/722道系统PPC采购/branches/task/09ppc8569阶段交付打包/output/code

**P2020:**

http://192.168.16.4/svn/PPC/branches/task/023ppcP2020试用包打包_723所

**8640D:**

http://192.168.16.4/svn/PPC/branches/task/017ppc8640d自测试前打包（0.0.0.8）/output/code

另外：

ppc8280到现在还没有找到目标机端源码，但获取以下信息：

1. 小倩告诉我目标机端源码就是和8569的一样，但是ts有改过,小倩给的svn路径http://192.168.16.4/svn/PPC/branches/task/016ppc8569自测试前打包(0.0.0.7)/output/src/tool
2. 江哥告诉我8280没有目标机端代码，当时只是做了主机端硬件调试
3. 李兵告诉我他当时只是接着小倩做了BDI的调试
4. 和江哥，笑哥沟通，确定8280就没有目标机端的代码，只需合并**8569,8640d，p2020**

## 对比依据

1. 对比.h文件和.c文件(include/src)
2. 对比主makefile
3. 对比ppcGnu4.12
4. 先对比简单的5个库，最后对比sysDebug库
5. 需要注意确定宏，然后这个宏在哪定义一定要确定好
6. 以8569为模板，把2020和8640d里面的差异加进去
7. 8569和2020用的是同一个型号的CPU，只不过8569是单核，2020是多核
8. 2020只需进行合并工作，但不需要验证，因为没有板子。
9. 8569，2020是软件单步，8640d是硬件单步。

## 对比工作

### 5库源码差异

2017-3-27完成了hostShell，mdl，rse，tautils，wdb库的h和c文件的比较。

#### hostshell

shellAcceptor(port, 1024*40, 8000, 0);  //8569

//shellAcceptor(port, 1460, 8000, 0);    //p2020,8640d

#### mdl

mdl库的LoadModule接口中：

moduleInfo.segment[1].address = dataAdd;

moduleInfo.segment[1].size = dataSize;

moduleInfo.segment[1].type = SEGMENT_DATA;

moduleInfo.segment[1].flags = 0;

**修改原因：**

dataSize，不用说了吧？我认为就是手误吧。。。

flags参数需要改为0，如果参数为SEG_FREE_MEMORY，那么系统在释放内存时会再释放一次，然而系统在分配内存时是统一给seg、data、bss段分配内存的，所以只需要释放一次就行了。

#### rse

rse的源码并没有改动，一毛一样

####tautils

\#include "string.h"

\#include <stdarg.h>

\#include <stdio.h>

//ppc8569和ppc2020

//ppc8640d没有引用stdarg/stdio,只有#include<string.h>

####wdb

wdb的源码并没有改动，一毛一样

### 5库makefile





