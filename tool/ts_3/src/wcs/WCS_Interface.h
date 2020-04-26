//
//	WCS - Winbond Chip Simulator
//
//	Copyright (C) 2006-2007 Winbond Electronics Corp.
//

//
//		WCS Interface
//

#ifndef __WCS_INTERFACE__
#define __WCS_INTERFACE__


#define	WCS_INTERFACE_VER	1		// version
#define	WCS_INTERFACE_REV	4		// revision



// Type/macro definition
typedef unsigned char		UINT8;
typedef unsigned int		UINT32;

#ifndef	NULL
#define	NULL	((void*)0)
#endif


//////////////////////////////////////////////////////////////////////////////
// External type/macro defined by Debugger

typedef enum
{
	WCS_SOFT_BKPT	= 0x0,
	WCS_HARD_BKPT	= 0x1,
	WCS_WRITE_WKPT  = 0x2,
	WCS_READ_WKPT	= 0x3,
	WCS_ACCESS_WKPT = 0x4
} WCS_BKPT_TYPE_T;

typedef	struct
{
	WCS_BKPT_TYPE_T	uType;			// breakpoint type
	UINT32			uAddr;			// breakpoint addr
	UINT32			uLength;		// breakpoint data width
} WCS_BKPT_T;

typedef enum
{
	WCS_DEBUG_MODE   = 0x00,
	WCS_NORMAL_MODE  = 0x01
} WCS_CPUSTAT_MODE_T;

typedef enum
{
	WCS_SIGNON_SIGNAL  = 0x00,		// nothing
	WCS_SIGHUP_SIGNAL  = 0x01,		// hangup
	WCS_SIGINT_SIGNAL  = 0x02,		// interrupt
	WCS_SIGQUIT_SIGNAL = 0x03,      // quit
	WCS_SIGILL_SIGNAL  = 0x04,		// illegal instruction (not reset when caught)
	WCS_SIGTRAP_SIGNAL = 0x05,		// trace trap (not reset when caught)
	WCS_SIGIOT_SIGNAL  = 0x06,		// IOT instruction
	WCS_SIGABRT_SIGNAL = 0x06,		// used by abort, replace SIGIOT in the future
	WCS_SIGEMT_SIGNAL  = 0x07,		// EMT instruction
	WCS_SIGFPE_SIGNAL  = 0x08,		// floating point exception
	WCS_SIGKILL_SIGNAL = 0x09,		// kill (cannot be caught or ignored)
	WCS_SIGBUS_SIGNAL  = 0x0A,		// bus error
	WCS_SIGSEGV_SIGNAL = 0x0B,		// segmentation violation
	WCS_SIGSYS_SIGNAL  = 0x0C,		// bad argument to system call
	WCS_SIGPIPE_SIGNAL = 0x0D,		// write on a pipe with no one to read it
	WCS_SIGALRM_SIGNAL = 0x0E,		// alarm clock
	WCS_SIGTERM_SIGNAL = 0x0F		// software termination signal from kill
} WCS_CPUSTAT_SIG_T;

typedef	struct
{
	WCS_CPUSTAT_MODE_T	uMode;		// cpu mode, debug mode or normal mode
	WCS_CPUSTAT_SIG_T	uSigNum;    // UNIX signal number
} WCS_CPUSTAT_T;

typedef enum
{
    LDEBUG,							// general debugging message
    LINFO,							// informative message
    LWARNING,						// warning message
    LERROR,							// general error message
    LFATAL,							// fatal error message
} WCS_LOG_LEVEL;

typedef void (*WCS_MODE_SWITCH)(UINT32 uMode);
typedef void (*WCS_LOG_NOTICE)(WCS_LOG_LEVEL Level, char *pFmt,...);

typedef	struct
{
	UINT32			uCpuType;		// cpu mode: debug mode or normal mode
	UINT32			uEndian;        // target endian: 0 little, 1 big
	WCS_MODE_SWITCH pModeSwitch;	// called when WCS switch mode from debug(normal) to normal(debug) 
	WCS_LOG_NOTICE  pLogNotice;		// log information to debugger
} WCS_DBGINFO_T;


//////////////////////////////////////////////////////////////////////////////

// memory address
typedef	struct
{
	UINT32		uAddr;
	UINT32		uType;				// reserved to support virtual memory
} WCS_ADDR_T;

// instruction opcode
typedef UINT32	WCS_OPCODE_T;

// register value
typedef UINT32	WCS_REGVAL_T;

// register/processor mode
typedef enum
{
	WCS_USR_MODE  = 0x10,
	WCS_FIQ_MODE  = 0x11,
	WCS_IRQ_MODE  = 0x12,
	WCS_SVC_MODE  = 0x13,
	WCS_ABT_MODE  = 0x17,
	WCS_UND_MODE  = 0x1B,
	WCS_SYS_MODE  = 0x1F,
	WCS_CURR_MODE = 0xFF
} WCS_REGMODE_T;

// register real Id
typedef enum
{
	// Current/USR (compitable with GDB's definition)
	WCS_R0, WCS_R1, WCS_R2, WCS_R3, WCS_R4, WCS_R5, WCS_R6, WCS_R7, WCS_R8,
	WCS_R9, WCS_R10, WCS_R11, WCS_R12, WCS_R13, WCS_R14, WCS_R15, WCS_CPSR,
	WCS_SPSR,
	// FIQ
	WCS_SPSR_FIQ, WCS_R8_FIQ, WCS_R9_FIQ, WCS_R10_FIQ, WCS_R11_FIQ, WCS_R12_FIQ, WCS_R13_FIQ,
	WCS_R14_FIQ,
	// IRQ
	WCS_SPSR_IRQ, WCS_R13_IRQ, WCS_R14_IRQ,
	// SVC/SYS
	WCS_SPSR_SVC, WCS_R13_SVC, WCS_R14_SVC,
	// ABT
	WCS_SPSR_ABT, WCS_R13_ABT, WCS_R14_ABT,
	// UND
	WCS_SPSR_UND, WCS_R13_UND, WCS_R14_UND,
} WCS_REG_REALID_T;

#define USER_MODE_REG_NUM                              18
#define FIQ_MODE_PRIVATE_REG_NUM                8
#define IRQ_MODE_PRIVATE_REG_NUM                3
#define SVC_MODE_PRIVATE_REG_NUM                3
#define ABT_MODE_PRIVATE_REG_NUM                3
#define UND_MODE_PRIVATE_REG_NUM               3


const unsigned int jtagIDtoRealID[38] = {
        WCS_R0, WCS_R1, WCS_R2, WCS_R3, WCS_R4, WCS_R5, WCS_R6, WCS_R7, WCS_R8,
	WCS_R9, WCS_R10, WCS_R11, WCS_R12, WCS_R13, WCS_R14, WCS_R15, WCS_R8_FIQ,
	WCS_R9_FIQ, WCS_R10_FIQ, WCS_R11_FIQ, WCS_R12_FIQ, WCS_R13_FIQ,
	WCS_R14_FIQ, WCS_R13_IRQ, WCS_R14_IRQ, WCS_R13_ABT, WCS_R14_ABT,
	WCS_R13_SVC, WCS_R14_SVC, WCS_R13_UND, WCS_R14_UND, WCS_CPSR,
        WCS_SPSR_FIQ, WCS_SPSR_IRQ, WCS_SPSR_ABT, WCS_SPSR_SVC, WCS_SPSR_UND};


#define WCS_R0_USR		WCS_R0
#define WCS_R1_USR		WCS_R1
#define WCS_R2_USR		WCS_R2
#define WCS_R3_USR		WCS_R3
#define WCS_R4_USR		WCS_R4
#define WCS_R5_USR		WCS_R5
#define WCS_R6_USR		WCS_R6
#define WCS_R7_USR		WCS_R7
#define WCS_R8_USR		WCS_R8
#define WCS_R9_USR		WCS_R9
#define WCS_R10_USR		WCS_R10
#define WCS_R11_USR		WCS_R11
#define WCS_R12_USR		WCS_R12
#define WCS_R13_USR		WCS_R13
#define WCS_R14_USR		WCS_R14

#define WCS_MAKE_REGID(RegMode, RegRealId)	( ((RegMode) << 16) | (RegRealId) )
#define WCS_MASK_REGMODE(RegId)				( (RegId) >> 16 )
#define WCS_MASK_REGREALID(RegId)			( (RegId) & 0xFFFF )


//////////////////////////////////////////////////////////////////////////////
// Return code (could be combined by bitwise operator except WCS_RET_OK)

typedef enum
{
	WCS_RET_OK			 = 0,			// no error
	WCS_RET_FAIL		 = 0x80000000,	// some error(s)
	WCS_RET_FAIL_MEMOUT  = 0x40000000,	// out of memory
	WCS_RET_FAIL_TIMEOUT = 0x20000000,	// timeout to start or stop WCS
	WCS_RET_BAD_ARG		 = 0x10000000,	// invalid argument
	WCS_RET_BAD_ADDR	 = 0x08000000,	// invalid address
	WCS_RET_BAD_BKPT	 = 0x04000000,	// invalid breakpoint
	WCS_RET_BAD_FLAG	 = 0x02000000,	// invalid flag
	WCS_RET_BAD_ID		 = 0x01000000,	// invalid Id, such as register real or
										//		thread Id
	WCS_RET_BAD_MEM		 = 0x00800000,	// image downloading to discontinuous or
										//		control register memory
	WCS_RET_BAD_MODE	 = 0x00400000,	// invalid mode, such as register or
										//		processor mode
	WCS_RET_BAD_NAME	 = 0x00200000,	// invalid name, such as function name
	WCS_RET_BAD_NUM		 = 0x00100000,	// invalid number, such as signal number
	WCS_RET_BAD_PROP	 = 0x00080000,	// invalid property
	WCS_RET_BAD_PTR		 = 0x00040000,	// null pointer assignment or access
	WCS_RET_BAD_TYPE	 = 0x00020000,	// invalid type, such as address or
										//		breakpoint type
	WCS_RET_BAD_VALUE	 = 0x00010000,	// invalid value
} WCS_RETCODE_T;

#define	WCS_FAILED(RetCode)		( (RetCode) != WCS_RET_OK )
#define	WCS_SUCESSED(RetCode)	( (RetCode) == WCS_RET_OK )


#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////////
// Target connect function

// Purpose:
//		Invoked to connect the target according the specified information,
//		when Debugger initialization.
WCS_RETCODE_T	wcsConnect
(
	const WCS_DBGINFO_T	*pDbgInfo
);

// Purpose:
//		Invoked to disconnect the target, when debugger exits.
// Note:
//		Debugger should auto-invoke this function when wcsConnect() failed.
WCS_RETCODE_T	wcsDisConnect();


//////////////////////////////////////////////////////////////////////////////
// Target register access function

// Purpose:
//		Invoked to retrieve the contents of the specified registers.
WCS_RETCODE_T	wcsGetRegs
(
	UINT32				uNum,
	const UINT32		*puRegIdList,	// puRegIdList[uNum]
	WCS_REGVAL_T		*puContent		// puContent[uNum]
);

// Purpose:
//		Invoked to set the contents of the specified registers.
WCS_RETCODE_T	wcsSetRegs
(
	UINT32				uNum,
	const UINT32		*puRegIdList,	// puRegIdList[uNum]
	const WCS_REGVAL_T	*puContent		// puContent[uNum]
);


//////////////////////////////////////////////////////////////////////////////
// Target memory access function

// Purpose:
//		Invoked to download the image to the target memory.
// Note:
//		1. DO NOT download image to control register memory.
//		2. Debugger should make the download data as possible as continuous,
//		   although the whole continuous block has to be split into some frame
//		   for buffer size limitation.
//		3. uFlag is currently unused and reserved to indicate the frame attribute,
//		   such as the first, end, or middle frame in a continuous block.
WCS_RETCODE_T	wcsDownload
(
	WCS_ADDR_T			*pAddr,
	const UINT8			*puImage,
	UINT32				uByteLen,
	UINT32				uTotalLen,
	UINT32				uFlag			// reserved
);

// Purpose:
//		Invoked to read the contents from the specified memory location.
WCS_RETCODE_T	wcsGetMem
(
	WCS_ADDR_T			*pAddr,
	UINT8				*puBuffer,
	UINT32				uByteLen
);

// Purpose:
//		Invoked to set the contents to the specified memory location, control
//		register memory especially.
WCS_RETCODE_T	wcsSetMem
(
	WCS_ADDR_T			*pAddr,
	const UINT8			*puBuffer,
	UINT32				uByteLen
);


//////////////////////////////////////////////////////////////////////////////
// Simulation function

// Purpose:
//		Invoked to make the target enter into debug mode, in other words, halt
//		the target.
// Note:
//		uThreadId is currently unused and reserved to specify thread Id required.
WCS_RETCODE_T	wcsAbort
(
	UINT32				uThreadId		// reserved
);

// Purpose:
//		Invoked to retrieve the current status of the CPU.
WCS_RETCODE_T	wcsGetCpuStatus
(
	WCS_CPUSTAT_T		*pCpuStatus
);

// Purpose:
//		Invoked to make the target enter normal mode, in other word, leave
//		debug mode, or run the target until meet a breakpoint or till address.
// Note:
//		1. Debugger can call wcsGetCpuStatus() to polling the CPU status:
//		   running or halted.
//		2. pTillAddr is unused if it equals to NULL.
//		3. uThreadId is currently unused and reserved to specify thread Id.
WCS_RETCODE_T	wcsGo
(
	UINT32				uThreadId,		// reserved
	WCS_ADDR_T			*pTillAddr
);

// Purpose:
//	 	Invoked to reset the target.
// Note:
//	 	1. pHaltAddr is used to specify an address required to be halted after
//		   reset.
//		2. pHaltAddr is unused if it equals to NULL.
WCS_RETCODE_T	wcsReset
(
	WCS_ADDR_T			*pHaltAddr		// reserved
);

// Purpose:
//		Invoked to execute the specified number assembly instructions step by
//		step.
// Note:
//	 	uStepCount is currently unused and reserved to specify the step count
//		required.
WCS_RETCODE_T	wcsStepOne
(
	UINT32				uStepCount		// reserved
);


//////////////////////////////////////////////////////////////////////////////
// Breakpoint handle function
//
// Note:
//		Debugger should implement breakpoint with memory-replacement mechanism.

// Purpose:
//		Invoked to clear the breakpoint at the specified address.
// Note:
//		uSavedOpCode is unused for watchpoint.
WCS_RETCODE_T	wcsClearBp
(
	const WCS_BKPT_T	*pBkpt,
	WCS_OPCODE_T		uSavedOpCode
);

// Purpose:
//		Invoked to set the breakpoint at the specified address.
// Note:
//		puSavedOpCode is unused for watchpoint.
WCS_RETCODE_T	wcsSetBp
(
	const WCS_BKPT_T	*pBkpt,
	WCS_OPCODE_T		*puSavedOpCode
);


//////////////////////////////////////////////////////////////////////////////
// Property function (reserved)

// Purpose:
//	 	Gets some property of simulator.
WCS_RETCODE_T	wcsGetProperty
(
	UINT32				uProp,
	UINT32				*puValue
);

// Purpose:
//	 	Sets some property of simulator.
WCS_RETCODE_T	wcsSetProperty
(
	UINT32				uProp,
	UINT32				uValue
);


//////////////////////////////////////////////////////////////////////////////
// Misc function (reserved for extension)

// Purpose:
//	 	Invoked to call predefined misc function.
// Note:
//		psFunName is a null-terminated string.
WCS_RETCODE_T	wcsMiscFun
(
	char				*psFunName,
	void				*pFunArg,
	void				*pFunResult
);


#ifdef __cplusplus
} // extern "C"
#endif

#endif // __WCS_INTERFACE__
