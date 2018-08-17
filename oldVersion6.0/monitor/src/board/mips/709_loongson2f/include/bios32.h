/*
 * BIOS32, PCI BIOS functions and defines
 */

#ifndef BIOS32_H
#define BIOS32_H

/*
 * Error values that may be returned by the PCI bios.  Use
 * pcibios_strerror() to convert to a printable string.
 */
#define PCIBIOS_SUCCESSFUL		0x00
#define PCIBIOS_FUNC_NOT_SUPPORTED	0x81
#define PCIBIOS_BAD_VENDOR_ID		0x83
#define PCIBIOS_DEVICE_NOT_FOUND	0x86
#define PCIBIOS_BAD_REGISTER_NUMBER	0x87
#define PCIBIOS_SET_FAILED		0x88
#define PCIBIOS_BUFFER_TOO_SMALL	0x89

extern char inb(int);
extern short inw(int);
extern long inl(int);
extern void outb(char,int);
extern void outw(short,int);
extern void outl(long,int);

extern int pcibios_present (void);
extern void pcibios_init ();
#ifdef PCI_FIXUP
extern unsigned long pcibios_fixup (unsigned long memory_start,
				    unsigned long memory_end);
#endif
extern int pcibios_find_class (unsigned int class_code, unsigned short index,int *pBusNo, int *pDeviceNo,int *pFuncNo);

extern int pcibios_find_device (unsigned short vendor, unsigned short dev_id,unsigned short index, unsigned char *bus,unsigned char *dev_fn);

extern int pcibios_read_config_byte(int busNo,int deviceNo, int funcNo,int where, unsigned char *value);

extern  int pcibios_read_config_word (int busNo,int deviceNo, int funcNo,int where, unsigned short *value);
				     
extern int pcibios_read_config_dword (int busNo,int deviceNo, int funcNo,int where, unsigned int *value);
				      
extern int pcibios_write_config_byte (unsigned char bus, unsigned char dev_fn,
				      unsigned char where, unsigned char val);
				      
extern int pcibios_write_config_word (unsigned char bus, unsigned char dev_fn,
				      unsigned char where, unsigned short val);
				      
extern int pcibios_write_config_dword (unsigned char bus, unsigned char dev_fn,
				       unsigned char where, unsigned int val);
				       
extern const char *pcibios_strerror (int error);

#endif /* BIOS32_H */
