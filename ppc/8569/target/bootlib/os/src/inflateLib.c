/* inflateLib.c - inflate code using public domain zlib functions */

/* Copyright notice:
 *
 * (C) 1995-1996, 2013 Jean-loup Gailly and Mark Adler
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * Jean-loup Gailly        Mark Adler
 * gzip@prep.ai.mit.edu    madler@alumni.caltech.edu
 */

/*
 * Copyright (c) 1996, 1998-1999, 2001, 2003-2004, 2006, 2010-2013
 * Wind River Systems, Inc.
 *
 * The right to copy, distribute or otherwise make use of this software
 * may be licensed only pursuant to the terms of an applicable Wind River
 * license agreement. 
 */

/*
modification history
--------------------
01q,22sep13,xms  fix DEAD_CODE error. (WIND00414265)
                 fix prevent warnings
01p,02nov12,swu  WIND00385128 - Fix error in large compress ratio
01o,10may12,jpb  Defect WIND00070686.  Fixed missing hyperlinks.
01n,10jan12,pgh  Fix build errors when DEBUG is defined.
01m,12jan11,pgh  Place functions in a separate section from .text
01l,02sep10,s_s  fixed compiler warnings
01k,06apr06,kk   doc cleanup (SPR# 119026)
01j,07apr06,dgp  doc: correct formatting of leading spaces, lists, headings in
                 library
01i,09sep04,job  Reformatted function definitions to match ANSI C standard
                 and docs requirements
01h,22jan03,nrj  merged changes made by Jifei Song (01/14/03)
                 fix SPR20426(77381,81849)
01g,26oct01,cyr  doc: correct SPR 22609 url link
01f,19oct01,dat  Documentation formatting
01e,23mar99,fle  doc : fixed INTERNAL handling
01d,27aug98,fle  doc : documented inflate_codes_new routine header
01c,01may98,cdp  fix bzero; fix storage allocator overwriting memory.
01b,07nov96,dgp  doc: final formatting
01a,18aug96,ms   written based on public domain zlib code.
*/

/*                                                                            
DESCRIPTION
This library is used to inflate a compressed data stream, primarily
for boot ROM decompression. Compressed boot ROMs contain a compressed 
executable in the data segment between the symbols `binArrayStart' and 
`binArrayEnd'.  The boot ROM startup code calls inflate() to decompress the 
executable and then jump to it.

This library is based on the public domain zlib code, which has been
modified by Wind River Systems.  For more information, see the zlib home page 
at `http://www.zlib.net/'.

INTERNAL

Questions about zlib should be sent to <zlib@quest.jpl.nasa.gov> or,
if this fails, to the addresses given in the Copyright section.

Internal Details on Compression algorithm (deflate)

Note: deflate() is not implemented in this library but the documentation is
kept internal for legacy reasons for now.

The deflation algorithm used by zlib (also zip and gzip) is a variation of
LZ77 (Lempel-Ziv 1977, see reference below). It finds duplicated strings in
the input data.  The second occurrence of a string is replaced by a
pointer to the previous string, in the form of a pair (distance,
length).  Distances are limited to 32K bytes, and lengths are limited
to 258 bytes. When a string does not occur anywhere in the previous
32K bytes, it is emitted as a sequence of literal bytes.  (In this
description, `string' must be taken as an arbitrary sequence of bytes,
and is not restricted to printable characters.)

Literals or match lengths are compressed with one Huffman tree, and
match distances are compressed with another tree. The trees are stored
in a compact form at the start of each block. The blocks can have any
size (except that the compressed data for one block must fit in
available memory). A block is terminated when the deflate() determines that 
it would be useful to start another block with fresh trees. (This is
somewhat similar to the behavior of LZW-based _compress_.)

Duplicated strings are found using a hash table. All input strings of
length 3 are inserted in the hash table. A hash index is computed for
the next 3 bytes. If the hash chain for this index is not empty, all
strings in the chain are compared with the current input string, and
the longest match is selected.

The hash chains are searched starting with the most recent strings, to
favor small distances and thus take advantage of the Huffman encoding.
The hash chains are singly linked. There are no deletions from the
hash chains, the algorithm simply discards matches that are too old.

To avoid a worst-case situation, very long hash chains are arbitrarily
truncated at a certain length, determined by a runtime option (level
parameter of deflateInit). So deflate() does not always find the longest
possible match but generally finds a match which is long enough.

deflate() also defers the selection of matches with a lazy evaluation
mechanism. After a match of length N has been found, deflate() searches for a
longer match at the next input byte. If a longer match is found, the
previous match is truncated to a length of one (thus producing a single
literal byte) and the longer match is emitted afterwards.  Otherwise,
the original match is kept, and the next match search is attempted only
N steps later.

The lazy match evaluation is also subject to a runtime parameter. If
the current match is long enough, deflate() reduces the search for a longer
match, thus speeding up the whole process. If compression ratio is more
important than speed, deflate() attempts a complete second search even if
the first match is already long enough.

The lazy match evaluation is not performed for the fastest compression
modes (level parameter 1 to 3). For these fast modes, new strings
are inserted in the hash table only when no match was found, or
when the match is not too long. This degrades the compression ratio
but saves time since there are both fewer insertions and fewer searches.

`DEFLATE Compressed Data Format Specification' available in
ftp://ds.internic.net/rfc/rfc1951.txt

OVERVIEW OF DECOMPRESSION ALGORITHM

The real question is, given a Huffman tree, how to decode fast.  The most
important realization is that shorter codes are much more common than
longer codes, so pay attention to decoding the short codes fast, and let
the long codes take longer to decode.

inflate() sets up a first level table that covers some number of bits of
input less than the length of longest code.  It gets that many bits from the
stream, and looks it up in the table.  The table will tell if the next
code is that many bits or less and how many, and if it is, it will tell
the value, else it will point to the next level table for which inflate()
grabs more bits and tries to decode a longer code.

How many bits to make the first lookup is a tradeoff between the time it
takes to decode and the time it takes to build the table.  If building the
table took no time (and if you had infinite memory), then there would only
be a first level table to cover all the way to the longest code.  However,
building the table ends up taking a lot longer for more bits since short
codes are replicated many times in such a table.  What inflate() does is
simply to make the number of bits in the first table a variable, and set it
for the maximum speed.

inflate() sends new trees relatively often, so it is possibly set for a
smaller first level table than an application that has only one tree for
all the data.  For inflate(), which has 286 possible codes for the
literal/length tree, the size of the first table is nine bits.  Also the
distance trees have 30 possible values, and the size of the first table is
six bits.  Note that for each of those cases, the table ended up one bit
longer than the `average' code length, i.e. the code length of an
approximately flat code which would be a little more than eight bits for
286 symbols and a little less than five bits for 30 symbols.  It would be
interesting to see if optimizing the first level table for other
applications gave values within a bit or two of the flat code size.

Jean-loup Gailly        Mark Adler
gzip@prep.ai.mit.edu    madler@alumni.caltech.edu

References:

[LZ77] Ziv J., Lempel A., `A Universal Algorithm for Sequential Data
Compression,' IEEE Transactions on Information Theory, Vol. 23, No. 3,
pp. 337-343.

MORE INTERNAL DETAILS

Huffman code decoding is performed using a multi-level table lookup.
The fastest way to decode is to simply build a lookup table whose
size is determined by the longest code.  However, the time it takes
to build this table can also be a factor if the data being decoded
is not very long.  The most common codes are necessarily the
shortest codes, so those codes dominate the decoding time, and hence
the speed.  The idea is you can have a shorter table that decodes the
shorter, more probable codes, and then point to subsidiary tables for
the longer codes.  The time it costs to decode the longer codes is
then traded against the time it takes to make longer tables.

This results of this trade are in the variables lbits and dbits
below.  lbits is the number of bits the first level table for literal/
length codes can decode in one step, and dbits is the same thing for
the distance codes.  Subsequent tables are also less than or equal to
those sizes.  These values may be adjusted either when all of the
codes are shorter than that, in which case the longest code length in
bits is used, or when the shortest code is *longer* than the requested
table size, in which case the length of the shortest code in bits is
used.

There are two different values for the two tables, since they code a
different number of possibilities each.  The literal/length table
codes 286 possible values, or in a flat code, a little over eight
bits.  The distance table codes 30 possible values, or a little less
than five bits, flat.  The optimum values for speed end up being
about one bit more than those, so lbits is 8+1 and dbits is 5+1.
The optimum values may differ though from machine to machine, and
possibly even between compilers.  Your mileage may vary.

Notes beyond the 1.93a appnote.txt:
\ms
\m 1.
Distance pointers never point before the beginning of the output
stream.
\m 2.
Distance pointers can point back across blocks, up to 32k away.
\m 3.
There is an implied maximum of 7 bits for the bit length table and
15 bits for the actual data.
\m 4.
If only one code exists, then it is encoded using one bit.  (Zero
would be more efficient, but perhaps a little confusing.)  If two
codes exist, they are coded using one bit each (0 and 1).
\m 5.
There is no way of sending zero distance codes--a dummy must be
sent if there are none.  (History: a pre 2.0 version of PKZIP would
store blocks with no distance codes, but this was discovered to be
too harsh a criterion.)  Valid only for 1.93a.  2.04c does allow
zero distance codes, which is sent as one code of zero bits in
length.
\m 6.
There are up to 286 literal/length codes.  Code 256 represents the
end-of-block.  Note however that the static length tree defines
288 codes just to fill out the Huffman codes.  Codes 286 and 287
cannot be used though, since there is no length base or extra bits
defined for them.  Similarly, there are up to 30 distance codes.
However, static trees define 32 codes (all 5 bits) to fill out the
Huffman codes, but the last two had better not show up in the data.
\m 7.
Unzip can check dynamic Huffman blocks for complete code sets.
The exception is that a single code would not be complete (see #4).
\m 8.
The five bits following the block type is really the number of
literal codes sent minus 257.
\m 9.
Length codes 8,16,16 are interpreted as 13 length codes of 8 bits
(1+6+6).  Therefore, to output three times the length, you output
three codes (1+1+1), whereas to output four times the same length,
you only need two codes (1+3).
\m 10.
In the tree reconstruction algorithm, Code = Code + Increment
only if BitLength(i) is not zero.  (Pretty obvious.)
\m 11.
Correction: 4 Bits: # of Bit Length codes - 4     (4 - 19)
\m 12.
Note: length code 284 can represent 227-258, but length code 285
really is 258.  The last length deserves its own, short code
since it gets used a lot in very redundant files.  The length
258 is special since 258 - 3 (the min match length) is 255.
\m 13.
The literal/length and distance code bit lengths are read as a
single stream of lengths.  It is possible (and advantageous) for
a repeat code (16, 17, or 18) to go across the boundary between
the two sets of lengths.
\me
*/

/* includes */

#include <limits.h>
#include <types/vxCpu.h>
#define NULL 0
/* definitions */

/*
* When _WRS_INFLATE_IN_RAM is defined, it allows the inflate code to be placed 
* into a section other than .text, so it can be located in RAM for faster 
* execution on some target hardware.
*/
#ifndef _WRS_INFLATE_IN_RAM
#define _WRS_INFLATE_IN_RAM
#endif /* _WRS_INFLATE_IN_RAM */

/* Diagnostic functions */
#ifdef DEBUG
#  include <stdio.h>
#  ifndef verbose
#    define verbose 0
#  endif
#  define Assert(cond,msg) {if(!(cond)) z_error(msg);}
#  define Trace(x) fprintf x
#  define Tracev(x) {if (verbose) fprintf x ;}
#  define Tracevv(x) {if (verbose>1) fprintf x ;}
#  define Tracec(c,x) {if (verbose && (c)) fprintf x ;}
#  define Tracecv(c,x) {if (verbose>1 && (c)) fprintf x ;}
#  define DBG_PUT(a,b)	fprintf (stderr, a, (unsigned int)b);
#else
#  define Assert(cond,msg)
#  define Trace(x)
#  define Tracev(x)
#  define Tracevv(x)
#  define Tracec(c,x)
#  define Tracecv(c,x)
#  define DBG_PUT(a,b)
#endif

/* Maximum value for windowBits in deflateInit2 and inflateInit */
#define DEF_WBITS   15 /* 32K LZ77 window */
#define PRESET_DICT 0x20 /* preset dictionary flag in zlib header */

                        /* Type declarations */

#ifndef OF /* function prototypes */
#  ifdef STDC
#    define OF(args)  args
#  else
#    define OF(args)  ()
#  endif
#endif

#define Z_OK            0
#define Z_STREAM_END    1
#define Z_NEED_DICT     2
#define Z_ERRNO        (-1)
#define Z_STREAM_ERROR (-2)
#define Z_DATA_ERROR   (-3)
#define Z_MEM_ERROR    (-4)
#define Z_BUF_ERROR    (-5)
#define Z_VERSION_ERROR (-6)
/* Return codes for the compression/decompression functions. Negative
 * values are errors, positive values are used for special but normal events.
 */

#define Z_DEFLATED   8
/* The deflate compression method (the only one supported in this version) */

#define Z_NULL  0  /* for initializing zalloc, zfree, opaque */

#define BASE 65521L /* largest prime smaller than 65536 */
#define NMAX 5552
/* NMAX is the largest n such that 255n(n+1)/2 + (n+1)(BASE-1) <= 2^32-1 */

#define ZALLOC(strm, items, size) \
           (*((strm)->zalloc))((strm)->opaque, (items), (size))
#define ZFREE(strm, addr)  (*((strm)->zfree))((strm)->opaque, (voidp)(addr))
#define TRY_FREE(s, p) {if (p) ZFREE(s, p);}

/* ===========================================================================
 * Internal compression state.
 */

#define LENGTH_CODES 29
/* number of length codes, not counting the special END_BLOCK code */

#define LITERALS  256
/* number of literal bytes 0..255 */

#define L_CODES (LITERALS+1+LENGTH_CODES)
/* number of Literal or Length codes, including the END_BLOCK code */

#define D_CODES   30
/* number of distance codes */

#define BL_CODES  19
/* number of codes used to transfer the bit lengths */

#define HEAP_SIZE (2*L_CODES+1)
/* maximum heap size */

#undef  MAX_BITS
#define MAX_BITS 15
/* All codes must not exceed MAX_BITS bits */

#define INIT_STATE    42
#define BUSY_STATE   113
#define FINISH_STATE 666
/* Stream status */


#define Freq fc.freq
#define Code fc.code
#define Dad  dl.dad
#define Len  dl.len

/* defines for inflate input/output */
/*   update pointers and return */
#define UPDBITS {s->bitb=b;s->bitk=k;}
#define UPDIN {z->avail_in=n;z->total_in+=p-z->next_in;z->next_in=p;}
#define UPDOUT {s->write=q;}
#undef  UPDATE
#define UPDATE {UPDBITS UPDIN UPDOUT}
#define LEAVE {UPDATE return inflate_flush(s,z,r);}
/*   get bytes and bits */
#define LOADIN {p=z->next_in;n=z->avail_in;b=s->bitb;k=s->bitk;}
#define NEEDBYTE {if(n)r=Z_OK;else LEAVE}
#define NEXTBYTE (n--,*p++)
#define NEEDBITS(j) {while(k<(j)){NEEDBYTE;b|=((uLong)NEXTBYTE)<<k;k+=8;}}
#define DUMPBITS(j) {b>>=(j);k-=(j);}
/*   output bytes */
#define WAVAIL (uInt)(q<s->read?s->read-q-1:s->end-q)
#define LOADOUT {q=s->write;m=(uInt)WAVAIL;}
#define WRAP {if(q==s->end&&s->read!=s->window){q=s->window;m=(uInt)WAVAIL;}}
#define FLUSH {UPDOUT r=inflate_flush(s,z,r); LOADOUT}
#define NEEDOUT {if(m==0){WRAP if(m==0){FLUSH WRAP if(m==0) LEAVE}}r=Z_OK;}
#define OUTBYTE(a) {*q++=(Byte)(a);m--;}
/*   load local pointers */
#define LOAD {LOADIN LOADOUT}

/* Output a byte on the stream.
 * IN assertion: there is enough room in pending_buf.
 */
#define put_byte(s, c) {s->pending_buf[s->pending++] = (c);}

#define DO1(buf,i)  {s1 += buf[i]; s2 += s1;}
#define DO2(buf,i)  DO1(buf,i); DO1(buf,i+1);
#define DO4(buf,i)  DO2(buf,i); DO2(buf,i+2);
#define DO8(buf,i)  DO4(buf,i); DO4(buf,i+4);
#define DO16(buf)   DO8(buf,0); DO8(buf,8);

/* simplify the use of the inflate_huft type with some defines */
#define base more.Base
#define next more.Next
#define exop word.what.Exop
#define bits word.what.Bits

/* If BMAX needs to be larger than 16, then h and x[] should be uLong. */
#define BMAX 15         /* maximum bit length of any code */
#define N_MAX 288       /* maximum number of codes in any set */

#define C0 *p++ = 0;
#define C2 C0 C0 C0 C0
#define C4 C2 C2 C2 C2

#define FIXEDH 530      /* number of hufts used by fixed tables */

#define	BUF_SIZE	100000
#define	MEM_ALIGN	4

#define	BLK_ALIGN	sizeof(int)
#undef  ROUND_UP
#define	ROUND_UP(n)	((n + BLK_ALIGN - 1) & (~(BLK_ALIGN - 1)))

#define	BLK_HDR_SIZE	(2 * sizeof(int))
#define	BLK_NEXT(b)	(*(((int *)(b)) - 1))
#define	BLK_PREV(b)	(*(((int *)(b)) - 2))
#define	BLK_HDRS_LINK(this,next)		\
		{				\
		BLK_NEXT(this) = (int)(next);	\
		BLK_PREV(next) = (int)(this);	\
		}

#define	BLK_IS_FREE(b)	(BLK_NEXT(b) & 1)
#define	BLK_FREE_SET(b)	(BLK_NEXT(b) |= 1)
#define	BLK_IS_VALID(b)	(((char *)b == buf)	\
		|| (BLK_PREV(BLK_NEXT(b)) == (int)(b)))

/* simplify the use of the inflate_huft type with some defines */
#define base more.Base
#define next more.Next
#define exop word.what.Exop
#define bits word.what.Bits

/* macros for bit input with no checking and for returning unused bytes */
#define GRABBITS(j) {while(k<(j)){b|=((uLong)NEXTBYTE)<<k;k+=8;}}
#define UNGRAB {n+=(c=k>>3);p-=c;k&=7;}

/* internal data types */

typedef unsigned char	Byte;  /* 8 bits */
typedef unsigned int	uInt;  /* 16 bits or more */
typedef unsigned long	uLong; /* 32 bits or more */
typedef void     *	voidp;

typedef unsigned char  uch;
typedef unsigned short ush;
typedef unsigned long  ulg;

#ifdef __cplusplus
extern "C" {
#endif

typedef voidp (*alloc_func) OF((voidp opaque, uInt items, uInt size));
typedef void   (*free_func)  OF((voidp opaque, voidp address));

struct internal_state;

typedef struct z_stream_s {
    Byte    *next_in;  /* next input byte */
    uInt     avail_in;  /* number of bytes available at next_in */
    uLong    total_in;  /* total nb of input bytes read so far */

    Byte    *next_out; /* next output byte should be put there */
    uInt     avail_out; /* remaining free space at next_out */
    uLong    total_out; /* total nb of bytes output so far */

    char     *msg;      /* last error message, NULL if no error */
    struct internal_state  *state; /* not visible by applications */

    alloc_func zalloc;  /* used to allocate the internal state */
    free_func  zfree;   /* used to free the internal state */
    voidp     opaque;  /* private data object passed to zalloc and zfree */

    int     data_type;  /* best guess about the data type: ascii or binary */
    uLong   adler;      /* adler32 value of the uncompressed data */
    uLong   reserved;   /* reserved for future use */
} z_stream;

typedef z_stream  *z_streamp;

#ifdef __cplusplus
}
#endif

typedef uLong (*check_func) OF((uLong check, const Byte *buf, uInt len));

/* Data structure describing a single value and its code string. */
typedef struct ct_data_s {
    union {
        ush  freq;       /* frequency count */
        ush  code;       /* bit string */
    } fc;
    union {
        ush  dad;        /* father node in Huffman tree */
        ush  len;        /* length of bit string */
    } dl;
}  ct_data;

typedef struct static_tree_desc_s  static_tree_desc;

typedef struct tree_desc_s {
    ct_data *dyn_tree;           /* the dynamic tree */
    int     max_code;            /* largest code with non zero frequency */
    static_tree_desc *stat_desc; /* the corresponding static tree */
}  tree_desc;

typedef ush Pos;
typedef Pos  Posf;
typedef unsigned IPos;

/* Huffman code lookup table entry--this entry is four bytes for machines
   that have 16-bit pointers (e.g. PC's in the small or medium model). */

typedef struct inflate_huft_s  inflate_huft;

struct inflate_huft_s {
  union {
    struct {
      Byte Exop;        /* number of extra bits or operation */
      Byte Bits;        /* number of bits in this code or subcode */
    } what;
    Byte *pad;         /* pad structure to a power of 2 (4 bytes for */
  } word;               /*  16-bit, 8 bytes for 32-bit machines) */
  union {
    uInt Base;          /* literal, length base, or distance base */
    inflate_huft *Next; /* pointer to next level of table */
  } more;
};

struct inflate_blocks_state;
typedef struct inflate_blocks_state  inflate_blocks_statef;

struct inflate_codes_state;
typedef struct inflate_codes_state  inflate_codes_statef;

typedef enum {
      TYPE,     /* get type bits (3, including end bit) */
      LENS,     /* get lengths for stored */
      STORED,   /* processing stored block */
      TABLE,    /* get table lengths */
      BTREE,    /* get bit lengths tree for a dynamic block */
      DTREE,    /* get length, distance trees for a dynamic block */
      CODES,    /* processing fixed or dynamic block */
      DRY,      /* output remaining window bytes */
      DONE,     /* finished last block, done */
      BAD}      /* got a data error--stuck here */
inflate_block_mode;

/* inflate blocks semi-private state */
struct inflate_blocks_state {

  /* mode */
  inflate_block_mode  mode;     /* current inflate_block mode */

  /* mode dependent information */
  union {
    uInt left;          /* if STORED, bytes left to copy */
    struct {
      uInt table;               /* table lengths (14 bits) */
      uInt index;               /* index into blens (or border) */
      uInt *blens;             /* bit lengths of codes */
      uInt bb;                  /* bit length tree depth */
      inflate_huft *tb;         /* bit length decoding tree */
    } trees;            /* if DTREE, decoding info for trees */
    struct {
      inflate_huft *tl;
      inflate_huft *td;         /* trees to free */
      inflate_codes_statef 
         *codes;
    } decode;           /* if CODES, current state */
  } sub;                /* submode */
  uInt last;            /* true if this block is the last block */

  /* mode independent information */
  uInt bitk;            /* bits in bit buffer */
  uLong bitb;           /* bit buffer */
  Byte *window;        /* sliding window */
  Byte *end;           /* one byte after sliding window */
  Byte *read;          /* window read pointer */
  Byte *write;         /* window write pointer */
  check_func checkfn;   /* check function */
  uLong check;          /* check on output */

};


/* inflate codes private state */
struct inflate_codes_state {

  /* mode */
  enum {        /* waiting for "i:"=input, "o:"=output, "x:"=nothing */
      START,    /* x: set up for LEN */
      LEN,      /* i: get length/literal/eob next */
      LENEXT,   /* i: getting length extra (have base) */
      DIST,     /* i: get distance next */
      DISTEXT,  /* i: getting distance extra */
      COPY,     /* o: copying bytes in window, waiting for space */
      LIT,      /* o: got literal, waiting for output space */
      WASH,     /* o: got eob, possibly still output waiting */
      END,      /* x: got eob and all data flushed */
      BADCODE}  /* x: got error */
    mode;               /* current inflate_codes mode */

  /* mode dependent information */
  uInt len;
  union {
    struct {
      inflate_huft *tree;       /* pointer into tree */
      uInt need;                /* bits needed */
    } code;             /* if LEN or DIST, where in tree */
    uInt lit;           /* if LIT, literal */
    struct {
      uInt get;                 /* bits to get for extra */
      uInt dist;                /* distance back to copy from */
    } copy;             /* if EXT or COPY, where and how much */
  } sub;                /* submode */

  /* mode independent information */
  Byte lbits;           /* ltree bits decoded per branch */
  Byte dbits;           /* dtree bits decoder per branch */
  inflate_huft *ltree;          /* literal/length/eob tree */
  inflate_huft *dtree;          /* distance tree */
};

/* inflate private state */
struct internal_state {

  /* mode */
  enum {
      METHOD,   /* waiting for method byte */
      FLAG,     /* waiting for flag byte */
      DICT4,    /* four dictionary check bytes to go */
      DICT3,    /* three dictionary check bytes to go */
      DICT2,    /* two dictionary check bytes to go */
      DICT1,    /* one dictionary check byte to go */
      DICT0,    /* waiting for inflateSetDictionary */
      BLOCKS,   /* decompressing blocks */
      CHECK4,   /* four check bytes to go */
      CHECK3,   /* three check bytes to go */
      CHECK2,   /* two check bytes to go */
      CHECK1,   /* one check byte to go */
      INF_DONE, /* finished check, done */
      INF_BAD}  /* got an error--stay here */
    mode;               /* current inflate mode */

  /* mode dependent information */
  union {
    uInt method;        /* if FLAGS, method byte */
    struct {
      uLong was;                /* computed check value */
      uLong need;               /* stream check value */
    } check;            /* if CHECK, check values to compare */
  } sub;        /* submode */

  /* mode independent information */
  int  nowrap;          /* flag for no wrapper */
  uInt wbits;           /* log2(window size)  (8..15, defaults to 15) */
  inflate_blocks_statef 
    *blocks;            /* current inflate_blocks state */

};


/* static variables */

/* Tables for deflate from PKZIP's appnote.txt. */
static uInt cplens[31] = { /* Copy lengths for literal codes 257..285 */
        3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31,
        35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258, 0, 0};
        /* actually lengths - 2; also see note #13 above about 258 */
static uInt cplext[31] = { /* Extra bits for literal codes 257..285 */
        0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
        3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0, 192, 192}; /* 192==invalid */
static uInt cpdist[30] = { /* Copy offsets for distance codes 0..29 */
        1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193,
        257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145,
        8193, 12289, 16385, 24577};
static uInt cpdext[30] = { /* Extra bits for distance codes */
        0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6,
        7, 7, 8, 8, 9, 9, 10, 10, 11, 11,
        12, 12, 13, 13};

static uInt inflate_mask[17] = {
    0x0000,
    0x0001, 0x0003, 0x0007, 0x000f, 0x001f, 0x003f, 0x007f, 0x00ff,
    0x01ff, 0x03ff, 0x07ff, 0x0fff, 0x1fff, 0x3fff, 0x7fff, 0xffff
};

/* Table for deflate from PKZIP's appnote.txt. */
static uInt border[] = { /* Order of the bit length code lengths */
        16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};

/* build fixed tables only once--keep them here */
static int fixed_built = 0;
static inflate_huft fixed_mem[FIXEDH];
static uInt fixed_bl = 0;
static uInt fixed_bd = 0;
static inflate_huft *fixed_tl = 0;
static inflate_huft *fixed_td = 0;

/* allocate two extra words for prev/next pointers for first block */
static int  intBuf [(BLK_HDR_SIZE + BUF_SIZE)/sizeof(int)];
static char * buf	=	BLK_HDR_SIZE + (char *)intBuf;
static char * nextBlock =	BLK_HDR_SIZE + (char *)intBuf;

/* forward static function declarations */

static _WRS_INFLATE_IN_RAM int huft_build OF((
    uInt *,            /* code lengths in bits */
    uInt,              /* number of codes */
    uInt,              /* number of "simple" codes */
    uInt *,            /* list of base values for non-simple codes */
    uInt *,            /* list of extra bits for non-simple codes */
    inflate_huft * *,  /* result: starting table */
    uInt *,            /* maximum lookup bits (returns actual) */
    z_streamp ));       /* for zalloc function */

static _WRS_INFLATE_IN_RAM voidp falloc OF((
    voidp,             /* opaque pointer (not used) */
    uInt,              /* number of items */
    uInt));            /* size of item */

static _WRS_INFLATE_IN_RAM int inflate_trees_free OF((
    inflate_huft *	t,	/* table to free */
    z_streamp		z       /* for zfree function */
    ));

static _WRS_INFLATE_IN_RAM int inflate_flush OF((
    inflate_blocks_statef *,
    z_streamp ,
    int));

static _WRS_INFLATE_IN_RAM int inflate_fast OF((
    uInt,
    uInt,
    inflate_huft *,
    inflate_huft *,
    inflate_blocks_statef *,
    z_streamp ));

/******************************************************************************
*
* memcopy - copy memory
* 
* \NOMANUAL
*/ 

static _WRS_INFLATE_IN_RAM void memcopy
    (
    Byte * dest,
    Byte * src,
    uInt   nBytes
    )
    {
    if ((((uInt)dest & 0x3) == 0) && (((uInt)src & 0x3) == 0))
	{
	while (nBytes >= 4)
	    {
	    *((uInt *)dest) = *((uInt *)src);
	    dest   += 4;
	    src    += 4;
	    nBytes -= 4;
	    }
	}

    while (nBytes > 0)
	{
	*dest++ = *src++;
	nBytes--;
	}
    }

/******************************************************************************
*
* bufZero - zeroes a buffer
* 
* \NOMANUAL
*/ 

static _WRS_INFLATE_IN_RAM void bufZero
    (
    char  *buffer,  /* buffer to be zeroed       */
    int   nbytes    /* number of bytes in buffer */
    )
    {
    if (((int)buffer & 0x3) == 0)
	{
	while (nbytes >= 4)
	    {
	    *(int *)buffer = 0;
	    buffer += 4;
	    nbytes -= 4;
	    }
	}

    while (nbytes >= 1)
	{
	*buffer = 0;
	buffer += 1;
	nbytes -= 1;
	}
    }

/******************************************************************************
*
* adler32 - 32 bit checksum
* 
* \NOMANUAL
*/ 

static _WRS_INFLATE_IN_RAM uLong adler32
    (
    uLong	adler,		/* previous total */
    const Byte *buf,		/* buffer to checksum */
    uInt	len		/* size of buffer */
    )
    {
    unsigned long s1 = adler & 0xffff;
    unsigned long s2 = (adler >> 16) & 0xffff;
    int k;

    if (buf == Z_NULL)
	return 1L;

    while (len > 0)
	{
        k = len < NMAX ? len : NMAX;
        len -= k;
        while (k >= 16)
	    {
            DO16(buf);
	    buf += 16;
            k -= 16;
            }
        if (k != 0)
	    do
		{
        	s1 += *buf++;
		s2 += s1;
        	} while (--k);

        s1 %= BASE;
        s2 %= BASE;
	}

    return (s2 << 16) | s1;
    }

static int doSwapNext = 0;
/******************************************************************************
*
* cksum - compute checksum
* 
* \NOMANUAL
*/ 

static _WRS_INFLATE_IN_RAM ush cksum
    (
    ush		prevSum,	/* previous total */
    const uch *	buf,		/* buffer to checksum */
    ulg		len		/* size of buffer */
    )
    {
    int doSwap = 0;
    ulg sum = prevSum;
    union
	{
	uch	c[2];
	ush	s;
	} shorty;
    union
	{
	ush	s[2];
	ulg	l;
	} longy;

#define ADDCARRY(x) (x > 65535 ? x -= 65535 : x)
#define	REDUCE(x) {longy.l = x; x = longy.s[0] + longy.s[1]; ADDCARRY(x);}


    REDUCE(sum);

    /*
     * doSwapNext is 1 if 0 was effectively added to end of last chunk of data
     * 'effectively' means swap is taken into account
     */

    if (doSwapNext)
	{
        /* do the first byte now for misaligned buffers */
        if ((int)buf & 1)
	    {
	    shorty.c[0] = 0;
	    shorty.c[1] = *buf;
	    buf++;
	    len--;
	    sum += shorty.s;
	    }
	else
	    {
	    uch tmp;
	    doSwap = 1;
	    shorty.s = sum;
	    tmp = shorty.c[0];
	    shorty.c[0] = shorty.c[1];
	    shorty.c[1] = tmp;
	    sum = shorty.s;
	    }
	}
    else
	if ((int)buf & 1)
	    {
	    uch tmp;
	    doSwap = 1;
	    shorty.s = sum;
	    tmp = shorty.c[0];
	    shorty.c[0] = shorty.c[1];
	    shorty.c[1] = tmp;
	    sum = shorty.s;

	    shorty.c[0] = 0;
	    shorty.c[1] = *buf;
	    buf++;
	    len--;
	    sum += shorty.s;
	    }

    /* do the rest two bytes at a time */

    while (len > 1)
	{
	sum += *((ush *)buf);
	buf += 2;
	len -= 2;
	if ((len & 0x7ffe) == 0)
	    REDUCE(sum);
	}

    /* add in the last byte if needed */

    if (len == 1)
	{
	/*
	 * if swap will happen, then this 0 is effectively inserted before
	 * last byte, not added to the end of the data
	 */
	doSwapNext = 1 & ~doSwap;
	shorty.c[0] = *buf;
	shorty.c[1] = 0;
	sum += shorty.s;
	}

    REDUCE(sum);
    REDUCE(sum);
    shorty.s = sum;

    /* swap bytes if needed */

    if (doSwap)
	{
	uch tmp;
	tmp = shorty.c[0];
	shorty.c[0] = shorty.c[1];
	shorty.c[1] = tmp;
	}

    return (shorty.s);
    }

/******************************************************************************
*
* zcalloc - allocate memory
* 
* \NOMANUAL
*/ 

static _WRS_INFLATE_IN_RAM voidp zcalloc
    (
    voidp opaque,
    unsigned items,
    unsigned size
    )
    {
    voidp thisBlock = (voidp)nextBlock;
    int nBytes = ROUND_UP (items * size);

    if ((char *)thisBlock + nBytes + BLK_HDR_SIZE >= &buf[BUF_SIZE])
	{
	DBG_PUT ("zcalloc %d bytes: buffer overflow!\n", nBytes);
	return (0);
	}

    nextBlock = (char *)thisBlock + nBytes + BLK_HDR_SIZE;
    BLK_HDRS_LINK (thisBlock, nextBlock);

    return (thisBlock);
    }

/******************************************************************************
*
* zcfree - free memory
* 
* \NOMANUAL
*/ 

static _WRS_INFLATE_IN_RAM void  zcfree
    (
    voidp opaque,
    voidp ptr
    )
    {
    voidp thisBlock;

    /* make sure block is valid */

    if (!BLK_IS_VALID(ptr))
	{
	DBG_PUT ("free at invalid address 0x%x\n", ptr);
	return;
	}

    /* mark block as free */

    BLK_FREE_SET (ptr);

    /* pop free blocks from the top of the stack */

    for (thisBlock = (voidp)BLK_PREV(nextBlock);
	 thisBlock != 0 && BLK_IS_FREE(thisBlock);
	 thisBlock = (voidp)BLK_PREV(thisBlock))
	{
	nextBlock = thisBlock;
	BLK_NEXT(nextBlock) = 0;
	}

    return;
    }

/* normally stack variable for huft_build - but stack was too large */

static uInt bitCnt[BMAX+1];        /* bit length count table */
static inflate_huft *u[BMAX];      /* table stack */
static uInt v[N_MAX];              /* values in order of bit length */
static uInt x[BMAX+1];             /* bit offsets, then code stack */

/******************************************************************************
*
* huft_build - build a huffman tree
*
* Given a list of code lengths and a maximum table size, make a set of
* tables to decode that set of codes.  Return Z_OK on success, Z_BUF_ERROR
* if the given code set is incomplete (the tables are still built in this
* case), Z_DATA_ERROR if the input is invalid (all zero length codes or an
* over-subscribed set of lengths), or Z_MEM_ERROR if not enough memory.
* 
* \NOMANUAL
*/ 

static _WRS_INFLATE_IN_RAM int huft_build
    (
    uInt *	b,	/* code lengths in bits (all assumed <= BMAX) */
    uInt	n,	/* number of codes (assumed <= N_MAX) */
    uInt	s,	/* number of simple-valued codes (0..s-1) */
    uInt *	d,	/* list of base values for non-simple codes */
    uInt *	e,	/* list of extra bits for non-simple codes */  
    inflate_huft ** t,	/* result: starting table */
    uInt *	m,	/* maximum lookup bits, returns actual */
    z_streamp	zs	/* for zalloc function */
    )
    {
    uInt a;                     /* counter for codes of length k */
    uInt f;                     /* i repeats in table every f entries */
    int g;                      /* maximum code length */
    int h;                      /* table level */
    register uInt i;            /* counter, current code */
    register uInt j;            /* counter */
    register int k;             /* number of bits in current code */
    int l;                      /* bits per table (returned in m) */
    register uInt *p;           /* pointer into c[], b[], or v[] */
    inflate_huft *q;            /* points to current table */
    struct inflate_huft_s r;    /* table entry for structure assignment */
    register int w;             /* bits before this table == (l * h) */
    uInt *xp;			/* pointer into x */
    int y;                      /* number of dummy codes added */
    uInt z;                     /* number of entries in current table */


    /* Generate counts for each bit length */
    p = bitCnt;
    C4                            /* clear c[]--assume BMAX+1 is 16 */
    p = b;  i = n;
    do
	{
	bitCnt[*p++]++;                  /* assume all entries <= BMAX */
	} while (--i);
    if (bitCnt[0] == n)                /* null input--all zero length codes */
	{
	*t = (inflate_huft *)Z_NULL;
	*m = 0;
	return Z_OK;
	}

    /* Find minimum and maximum length, bound *m by those */
    l = *m;
    for (j = 1; j <= BMAX; j++)
	if (bitCnt[j])
	    break;
    k = j;                        /* minimum code length */
    if ((uInt)l < j)
	l = j;
    for (i = BMAX; i; i--)
	if (bitCnt[i])
	    break;
    g = i;                        /* maximum code length */
    if ((uInt)l > i)
	l = i;
    *m = l;

    /* Adjust last length count to fill out codes, if needed */
    for (y = 1 << j; j < i; j++, y <<= 1)
	if ((y -= bitCnt[j]) < 0)
	    return Z_DATA_ERROR;
    if ((y -= bitCnt[i]) < 0)
	return Z_DATA_ERROR;
    bitCnt[i] += y;

    /* Generate starting offsets into the value table for each length */
    x[1] = j = 0;
    p = bitCnt + 1;  xp = x + 2;
    while (--i)
	{                 /* note that i == g from above */
	*xp++ = (j += *p++);
	}

    /* Make a table of values in order of bit lengths */
    p = b;  i = 0;
    do
	{
	if ((j = *p++) != 0)
	    v[x[j]++] = i;
	} while (++i < n);

    /* Generate the Huffman codes and for each, make the table entries */
    x[0] = i = 0;                 /* first Huffman code is zero */
    p = v;                        /* grab values in bit order */
    h = -1;                       /* no tables yet--level -1 */
    w = -l;                       /* bits decoded == (l * h) */
    u[0] = (inflate_huft *)Z_NULL;        /* just to keep compilers happy */
    q = (inflate_huft *)Z_NULL;   /* ditto */
    z = 0;                        /* ditto */

    /* go through the bit lengths (k already is bits in shortest code) */
    for (; k <= g; k++)
	{
	a = bitCnt[k];
	while (a--)
	    {
	    /* here i is the Huffman code of length k bits for value *p */
	    /* make tables up to required level */
	    while (k > w + l)
		{
		h++;
	        w += l;                 /* previous table always l bits */

	        /* compute minimum size table less than or equal to l bits */
	        z = g - w;
	        z = z > (uInt)l ? l : z;        /* table size upper limit */
	        if ((f = 1 << (j = k - w)) > a + 1) /* try a k-w bit table */
	            {                 /* too few codes for k-w bit table */
	            f -= a + 1;       /* deduct codes from patterns left */
	            xp = bitCnt + k;
	            if (j < z)
	   	        while (++j < z) /* try smaller tables up to z bits */
		            {
	                    if ((f <<= 1) <= *++xp)
	                        break; /* enough codes to use up j bits */
		            f -= *xp;  /* else deduct codes from patterns */
			    }
        	    }
	        z = 1 << j;             /* table entries for j-bit table */

	        /* allocate and link in new table */
	        q = (inflate_huft *)ZALLOC(zs,z + 1,sizeof(inflate_huft));
	        if (q == Z_NULL)
        	    {
	            if (h && (zs->zfree != Z_NULL))
	                 inflate_trees_free(u[0], zs);
	            return Z_MEM_ERROR; /* not enough memory */
        	    }

	        *t = q + 1;             /* link to list for huft_free() */
	        *(t = &(q->next)) = Z_NULL;
	        u[h] = ++q;             /* table starts after link */

	        /* connect to last table, if there is one */
	        if (h)
	            {
	            x[h] = i;           /* save pattern for backing up */
	            r.bits = (Byte)l;	/* bits to dump before this table */
	            r.exop = (Byte)j;   /* bits in this table */
	            r.next = q;         /* pointer to this table */
	            j = i >> (w - l);   /* (get around Turbo C bug) */
	            u[h-1][j] = r;      /* connect to last table */
	            }
		}

	    /* set up table entry in r */
	    r.bits = (Byte)(k - w);
	    if (p >= v + n)
	        r.exop = 128 + 64;      /* out of values--invalid code */
	    else if (*p < s)
	        {
		/* 256 is end-of-block */
	        r.exop = (Byte)(*p < 256 ? 0 : 32 + 64);
	        r.base = *p++;          /* simple code is just the value */
	        }
	    else
	        {
		/* non-simple--look up in lists */
	        r.exop = (Byte)(e[*p - s] + 16 + 64);
	        r.base = d[*p++ - s];
	        }

	    /* fill code-like entries with r */
	    f = 1 << (k - w);
	    for (j = i >> w; j < z; j += f)
	        q[j] = r;

	    /* backwards increment the k-bit code i */
	    for (j = 1 << (k - 1); i & j; j >>= 1)
	        i ^= j;
	    i ^= j;

	    /* backup over finished tables */
	    while ((i & ((1 << w) - 1)) != x[h])
	        {
	        h--;                    /* don't need to update q */
	        w -= l;
	        }
	    }
	}

    /* Return Z_BUF_ERROR if we were given an incomplete table */
    return y != 0 && g != 1 ? Z_BUF_ERROR : Z_OK;
    }

/******************************************************************************
*
* inflate_trees_bits - inflate bits from huffman tree
* 
* \NOMANUAL
*/ 

static _WRS_INFLATE_IN_RAM int inflate_trees_bits
    (
    uInt *c,		/* 19 code lengths */
    uInt *bb,           /* bits tree desired/actual depth */
    inflate_huft ** tb, /* bits tree result */
    z_streamp z         /* for zfree function */
    )
    {
    int r;

    r = huft_build(c, 19, 19, (uInt*)Z_NULL, (uInt*)Z_NULL, tb, bb, z);
    if (r == Z_DATA_ERROR)
	z->msg = (char*)"oversubscribed dynamic bit lengths tree";
    else if (r == Z_BUF_ERROR)
	{
	inflate_trees_free(*tb, z);
	z->msg = (char*)"incomplete dynamic bit lengths tree";
	r = Z_DATA_ERROR;
	}
    return r;
    }

/******************************************************************************
*
* inflate_trees_dynamic - inflate from dynamic huffman tree
* 
* \NOMANUAL
*/ 

static _WRS_INFLATE_IN_RAM int inflate_trees_dynamic
    (
    uInt	nl,	/* number of literal/length codes */
    uInt	nd,     /* number of distance codes */
    uInt *	c,      /* that many (total) code lengths */
    uInt *	bl,     /* literal desired/actual bit depth */
    uInt *	bd,     /* distance desired/actual bit depth */
    inflate_huft ** tl, /* literal/length tree result */
    inflate_huft ** td, /* distance tree result */
    z_streamp	z       /* for zfree function */
    )
    {
    int r;

    /* build literal/length tree */
    if ((r = huft_build(c, nl, 257, cplens, cplext, tl, bl, z)) != Z_OK)
	{
	if (r == Z_DATA_ERROR)
	    z->msg = (char*)"oversubscribed literal/length tree";
	else if (r == Z_BUF_ERROR)
	    {
	    inflate_trees_free(*tl, z);
	    z->msg = (char*)"incomplete literal/length tree";
	    r = Z_DATA_ERROR;
	    }
	return r;
	}

    /* build distance tree */
    if ((r = huft_build(c + nl, nd, 0, cpdist, cpdext, td, bd, z)) != Z_OK)
	{
	if (r == Z_DATA_ERROR)
	     z->msg = (char*)"oversubscribed literal/length tree";
	else if (r == Z_BUF_ERROR) {
	    inflate_trees_free(*td, z);
	    z->msg = (char*)"incomplete literal/length tree";
	    r = Z_DATA_ERROR;
	    }
	inflate_trees_free(*tl, z);
	return r;
	}

    /* done */
    return Z_OK;
    }

/******************************************************************************
*
* falloc  - fake memory allocator for huffman trees
* 
* \NOMANUAL
*/ 

static _WRS_INFLATE_IN_RAM voidp falloc
    (
    voidp	q,	/* opaque pointer */
    uInt	n,      /* number of items */
    uInt	s       /* size of item */
    )
    {
    *(int *)q -= n;
    return (voidp)(fixed_mem + *(int *)q);
    }

/******************************************************************************
*
* inflate_trees_fixed - do something
* 
* \NOMANUAL
*/ 

static _WRS_INFLATE_IN_RAM int inflate_trees_fixed
    (
    uInt *	bl,	/* literal desired/actual bit depth */
    uInt *	bd,     /* distance desired/actual bit depth */
    inflate_huft ** tl, /* literal/length tree result */
    inflate_huft ** td  /* distance tree result */
    )
    {
    /* build fixed tables if not already (multiple overlapped executions ok) */
    if (!fixed_built)
	{
	int k;              /* temporary variable */
	unsigned *c = (unsigned *)zcalloc (0, 288, sizeof (unsigned));
			    /* length list for huft_build */
	z_stream z;         /* for falloc function */
	int f = FIXEDH;     /* number of hufts left in fixed_mem */

    if (c == NULL)
        return Z_MEM_ERROR;

	/* set up fake z_stream for memory routines */
	z.zalloc = falloc;
	z.zfree = Z_NULL;
	z.opaque = (voidp)&f;

	/* literal table */
	for (k = 0; k < 144; k++)
	    c[k] = 8;
	for (; k < 256; k++)
	    c[k] = 9;
	for (; k < 280; k++)
	    c[k] = 7;
        for (; k < 288; k++)
	    c[k] = 8;
	fixed_bl = 7;
	(void)huft_build(c, 288, 257, cplens, cplext, &fixed_tl, &fixed_bl, &z);
            
	/* distance table */
	for (k = 0; k < 30; k++)
#if	CPU_FAMILY==MC680X0
	    ((volatile unsigned *)c)[k] = 5;
#else
	    c[k] = 5;
#endif
	fixed_bd = 5;
	(void)huft_build (c, 30, 0, cpdist, cpdext, &fixed_td, &fixed_bd, &z);
            
	/* done */
        
	fixed_built = 1;
	zcfree (0, c);
	}

    *bl = fixed_bl;
    *bd = fixed_bd;
    *tl = fixed_tl;
    *td = fixed_td;
    return Z_OK;
    }

/******************************************************************************
*
* inflate_trees_free - frees inflate trees
*
* Free the malloc'ed tables built by huft_build(), which makes a linked
* list of the tables it made, with the links in a dummy first entry of
* each table.
* 
* \NOMANUAL
*/ 

static _WRS_INFLATE_IN_RAM int inflate_trees_free
    (
    inflate_huft *	t,	/* table to free */
    z_streamp		z       /* for zfree function */
    )
    {
    register inflate_huft *p, *q, *r;

    /* Reverse linked list */
    p = Z_NULL;
    q = t;
    while (q != Z_NULL)
	{
	r = (q - 1)->next;
	(q - 1)->next = p;
	p = q;
	q = r;
	}

    /* Go through linked list, freeing from the malloced (t[-1]) address. */
    while (p != Z_NULL)
	{
	q = (--p)->next;
	ZFREE(z,p);
	p = q;
	} 

    return Z_OK;
    }

/******************************************************************************
*
* inflate_flush - flushes inflate
*
* copy as much as possible from the sliding window to the output area
* 
* \NOMANUAL
*/ 

static _WRS_INFLATE_IN_RAM int inflate_flush
    (
    inflate_blocks_statef * s,
    z_streamp		z,
    int 		r
    )
    {
    uInt n;
    Byte *p;
    Byte *q;

    /* local copies of source and destination pointers */
    p = z->next_out;
    q = s->read;

    /* compute number of bytes to copy as far as end of window */
    n = (uInt)((q <= s->write ? s->write : s->end) - q);
    if (n > z->avail_out)
	n = z->avail_out;
    if (n && r == Z_BUF_ERROR)
	r = Z_OK;

    /* update counters */
    z->avail_out -= n;
    z->total_out += n;

    /* update check information */
    if (s->checkfn != Z_NULL)
	z->adler = s->check = (*s->checkfn)(s->check, q, n);

    /* copy as far as end of window */
    memcopy(p, q, n);
    p += n;
    q += n;

    /* see if more to copy at beginning of window */
    if (q == s->end)
	{
	/* wrap pointers */
	q = s->window;
	if (s->write == s->end)
	    s->write = s->window;

	/* compute bytes to copy */
	n = (uInt)(s->write - q);
	if (n > z->avail_out)
	    n = z->avail_out;
	if (n && r == Z_BUF_ERROR)
	    r = Z_OK;

	/* update counters */
	z->avail_out -= n;
	z->total_out += n;

	/* update check information */
	if (s->checkfn != Z_NULL)
	    z->adler = s->check = (*s->checkfn)(s->check, q, n);

	/* copy */
	memcopy(p, q, n);
	p += n;
	q += n;
	}

    /* update pointers */
    z->next_out = p;
    s->read = q;

    /* done */
    return r;
    }

/******************************************************************************
*
* inflate_fast - inflate fast
*
* Called with number of bytes left to write in window at least 258
* (the maximum string length) and number of input bytes available
* at least ten.  The ten bytes are six bytes for the longest length/
* distance pair plus four bytes for overloading the bit buffer.
* 
* \NOMANUAL
*/ 

static _WRS_INFLATE_IN_RAM int inflate_fast
    (
    uInt	bl,
    uInt	bd,
    inflate_huft * tl,
    inflate_huft * td,
    inflate_blocks_statef * s,
    z_streamp	z
    )
    {
    inflate_huft *t;	/* temporary pointer */
    uInt e;             /* extra bits or operation */
    uLong b;            /* bit buffer */
    uInt k;             /* bits in bit buffer */
    Byte *p;            /* input data pointer */
    uInt n;             /* bytes available there */
    Byte *q;            /* output window write pointer */
    uInt m;             /* bytes to end of window or read pointer */
    uInt ml;            /* mask for literal/length tree */
    uInt md;            /* mask for distance tree */
    uInt c;             /* bytes to copy */
    uInt d;             /* distance back to copy from */
    Byte *r;            /* copy source pointer */

  /* load input, output, bit values */
  LOAD

  /* initialize masks */
  ml = inflate_mask[bl];
  md = inflate_mask[bd];

  /* do until not enough input or output space for fast loop */
  do {                          /* assume called with m >= 258 && n >= 10 */
    /* get literal/length code */
    GRABBITS(20)                /* max bits for literal/length code */
    if ((e = (t = tl + ((uInt)b & ml))->exop) == 0)
    {
      DUMPBITS(t->bits)
      Tracevv((stderr, t->base >= 0x20 && t->base < 0x7f ?
                "inflate:         * literal '%c'\n" :
                "inflate:         * literal 0x%02x\n", t->base));
      *q++ = (Byte)t->base;
      m--;
      continue;
    }
    do {
      DUMPBITS(t->bits)
      if (e & 16)
      {
        /* get extra bits for length */
        e &= 15;
        c = t->base + ((uInt)b & inflate_mask[e]);
        DUMPBITS(e)
        Tracevv((stderr, "inflate:         * length %u\n", c));

        /* decode distance base of block to copy */
        GRABBITS(15);           /* max bits for distance code */
        e = (t = td + ((uInt)b & md))->exop;
        do {
          DUMPBITS(t->bits)
          if (e & 16)
          {
            /* get extra bits to add to distance base */
            e &= 15;
            GRABBITS(e)         /* get extra bits (up to 13) */
            d = t->base + ((uInt)b & inflate_mask[e]);
            DUMPBITS(e)
            Tracevv((stderr, "inflate:         * distance %u\n", d));

            /* do the copy */
            m -= c;
            if ((uInt)(q - s->window) >= d)     /* offset before dest */
            {                                   /*  just copy */
              r = q - d;
              *q++ = *r++;  c--;        /* minimum count is three, */
              *q++ = *r++;  c--;        /*  so unroll loop a little */
            }
            else                        /* else offset after destination */
            {
              e = d - (uInt)(q - s->window); /* bytes from offset to end */
              r = s->end - e;           /* pointer to offset */
              if (c > e)                /* if source crosses, */
              {
                c -= e;                 /* copy to end of window */
                do {
                  *q++ = *r++;
                } while (--e);
                r = s->window;          /* copy rest from start of window */
              }
            }
            do {                        /* copy all or what's left */
              *q++ = *r++;
            } while (--c);
            break;
          }
          else if ((e & 64) == 0)
            e = (t = t->next + ((uInt)b & inflate_mask[e]))->exop;
          else
          {
            z->msg = (char*)"invalid distance code";
            UNGRAB
            UPDATE
            return Z_DATA_ERROR;
          }
        } while (1);
        break;
      }
      if ((e & 64) == 0)
      {
        if ((e = (t = t->next + ((uInt)b & inflate_mask[e]))->exop) == 0)
        {
          DUMPBITS(t->bits)
          Tracevv((stderr, t->base >= 0x20 && t->base < 0x7f ?
                    "inflate:         * literal '%c'\n" :
                    "inflate:         * literal 0x%02x\n", t->base));
          *q++ = (Byte)t->base;
          m--;
          break;
        }
      }
      else if (e & 32)
      {
        Tracevv((stderr, "inflate:         * end of block\n"));
        UNGRAB
        UPDATE
        return Z_STREAM_END;
      }
      else
      {
        z->msg = (char*)"invalid literal/length code";
        UNGRAB
        UPDATE
        return Z_DATA_ERROR;
      }
    } while (1);
  } while (m >= 258 && n >= 10);

  /* not enough input or output--restore pointers and return */
  UNGRAB
  UPDATE
  return Z_OK;
}

/******************************************************************************
*
* inflate_codes_new - inflate codes new
*
* \NOMANUAL
*/ 

static _WRS_INFLATE_IN_RAM inflate_codes_statef *inflate_codes_new
    (
    uInt bl,
    uInt bd,
    inflate_huft *tl,
    inflate_huft *td, /* need separate declaration for Borland C++ */
    z_streamp z
    )
    {
    inflate_codes_statef *c;

    if ((c = (inflate_codes_statef *)
       ZALLOC(z,1,sizeof(struct inflate_codes_state))) != Z_NULL)
        {
        c->mode = START;
        c->lbits = (Byte)bl;
        c->dbits = (Byte)bd;
        c->ltree = tl;
        c->dtree = td;
        Tracev((stderr, "inflate:       codes new\n"));
        }

    return c;
    }

/******************************************************************************
*
* inflate_codes - inflate codes
*
* \NOMANUAL
*/ 

static _WRS_INFLATE_IN_RAM int inflate_codes
    ( 
    inflate_blocks_statef *s,
    z_streamp z,
    int r
    )
{
  uInt j;               /* temporary storage */
  inflate_huft *t;      /* temporary pointer */
  uInt e;               /* extra bits or operation */
  uLong b;              /* bit buffer */
  uInt k;               /* bits in bit buffer */
  Byte *p;             /* input data pointer */
  uInt n;               /* bytes available there */
  Byte *q;             /* output window write pointer */
  uInt m;               /* bytes to end of window or read pointer */
  Byte *f;             /* pointer to copy strings from */
  inflate_codes_statef *c = s->sub.decode.codes;  /* codes state */

  /* copy input/output information to locals (UPDATE macro restores) */
  LOAD

  /* process input and output based on current state */
  while (1) switch (c->mode)
  {             /* waiting for "i:"=input, "o:"=output, "x:"=nothing */
    case START:         /* x: set up for LEN */
#ifndef SLOW
      if (m >= 258 && n >= 10)
      {
        UPDATE
        r = inflate_fast(c->lbits, c->dbits, c->ltree, c->dtree, s, z);
        LOAD
        if (r != Z_OK)
        {
          c->mode = r == Z_STREAM_END ? WASH : BADCODE;
          break;
        }
      }
#endif /* !SLOW */
      c->sub.code.need = c->lbits;
      c->sub.code.tree = c->ltree;
      c->mode = LEN;
    case LEN:           /* i: get length/literal/eob next */
      j = c->sub.code.need;
      NEEDBITS(j)
      t = c->sub.code.tree + ((uInt)b & inflate_mask[j]);
      DUMPBITS(t->bits)
      e = (uInt)(t->exop);
      if (e == 0)               /* literal */
      {
        c->sub.lit = t->base;
        Tracevv((stderr, t->base >= 0x20 && t->base < 0x7f ?
                 "inflate:         literal '%c'\n" :
                 "inflate:         literal 0x%02x\n", t->base));
        c->mode = LIT;
        break;
      }
      if (e & 16)               /* length */
      {
        c->sub.copy.get = e & 15;
        c->len = t->base;
        c->mode = LENEXT;
        break;
      }
      if ((e & 64) == 0)        /* next table */
      {
        c->sub.code.need = e;
        c->sub.code.tree = t->next;
        break;
      }
      if (e & 32)               /* end of block */
      {
        Tracevv((stderr, "inflate:         end of block\n"));
        c->mode = WASH;
        break;
      }
      c->mode = BADCODE;        /* invalid code */
      z->msg = (char*)"invalid literal/length code";
      r = Z_DATA_ERROR;
      LEAVE
    case LENEXT:        /* i: getting length extra (have base) */
      j = c->sub.copy.get;
      NEEDBITS(j)
      c->len += (uInt)b & inflate_mask[j];
      DUMPBITS(j)
      c->sub.code.need = c->dbits;
      c->sub.code.tree = c->dtree;
      Tracevv((stderr, "inflate:         length %u\n", c->len));
      c->mode = DIST;
      
      /* this is an intentional fall through to the next case */
      
    case DIST:          /* i: get distance next */
      j = c->sub.code.need;
      NEEDBITS(j)
      t = c->sub.code.tree + ((uInt)b & inflate_mask[j]);
      DUMPBITS(t->bits)
      e = (uInt)(t->exop);
      if (e & 16)               /* distance */
      {
        c->sub.copy.get = e & 15;
        c->sub.copy.dist = t->base;
        c->mode = DISTEXT;
        break;
      }
      if ((e & 64) == 0)        /* next table */
      {
        c->sub.code.need = e;
        c->sub.code.tree = t->next;
        break;
      }
      c->mode = BADCODE;        /* invalid code */
      z->msg = (char*)"invalid distance code";
      r = Z_DATA_ERROR;
      LEAVE
    case DISTEXT:       /* i: getting distance extra */
      j = c->sub.copy.get;
      NEEDBITS(j)
      c->sub.copy.dist += (uInt)b & inflate_mask[j];
      DUMPBITS(j)
      Tracevv((stderr, "inflate:         distance %u\n", c->sub.copy.dist));
      c->mode = COPY;
      
      /* this is an intentional fall through to the next case */
      
    case COPY:          /* o: copying bytes in window, waiting for space */
#ifndef __TURBOC__ /* Turbo C bug for following expression */
      f = (uInt)(q - s->window) < c->sub.copy.dist ?
          s->end - (c->sub.copy.dist - (q - s->window)) :
          q - c->sub.copy.dist;
#else
      f = q - c->sub.copy.dist;
      if ((uInt)(q - s->window) < c->sub.copy.dist)
        f = s->end - (c->sub.copy.dist - (uInt)(q - s->window));
#endif
      while (c->len)
      {
        NEEDOUT
        OUTBYTE(*f++)
        if (f == s->end)
          f = s->window;
        c->len--;
      }
      c->mode = START;
      break;
    case LIT:           /* o: got literal, waiting for output space */
      NEEDOUT
      OUTBYTE(c->sub.lit)
      c->mode = START;
      break;
    case WASH:          /* o: got eob, possibly more output */
      FLUSH
      if (s->read != s->write)
        LEAVE
      c->mode = END;
      
      /* this is an intentional fall through to the next case */
      
    case END:
      r = Z_STREAM_END;
      LEAVE
    case BADCODE:       /* x: got error */
      r = Z_DATA_ERROR;
      LEAVE
    default:
      r = Z_STREAM_ERROR;
      LEAVE
  }
}

/******************************************************************************
*
* inflate_codes_free - frees inflate codes
*
* \NOMANUAL
*/ 

static _WRS_INFLATE_IN_RAM void inflate_codes_free
    (
    inflate_codes_statef *c,
    z_streamp z
    )
    {
    ZFREE(z, c);
    }

/******************************************************************************
*
* inflate_blocks_reset - resets inflate blocks
*
* \NOMANUAL
*/ 

static _WRS_INFLATE_IN_RAM void inflate_blocks_reset
    (
    inflate_blocks_statef *s,
    z_streamp z,
    uLong *c
    )
{
  if (s->checkfn != Z_NULL)
    *c = s->check;
  if (s->mode == BTREE || s->mode == DTREE)
    ZFREE(z, s->sub.trees.blens);
  if (s->mode == CODES)
  {
    inflate_codes_free(s->sub.decode.codes, z);
    inflate_trees_free(s->sub.decode.td, z);
    inflate_trees_free(s->sub.decode.tl, z);
  }
  s->mode = TYPE;
  s->bitk = 0;
  s->bitb = 0;
  s->read = s->write = s->window;
  if (s->checkfn != Z_NULL)
    z->adler = s->check = (*s->checkfn)(0L, Z_NULL, 0);
  Trace((stderr, "inflate:   blocks reset\n"));
}

/******************************************************************************
*
* inflate_blocks_new - allocates new inflate blocks
*
* \NOMANUAL
*/ 

static _WRS_INFLATE_IN_RAM inflate_blocks_statef *inflate_blocks_new
    (
    z_streamp z,
    check_func c,
    uInt w
    )
{
  inflate_blocks_statef *s;

  if ((s = (inflate_blocks_statef *)ZALLOC
       (z,1,sizeof(struct inflate_blocks_state))) == Z_NULL)
    return s;
  if ((s->window = (Byte *)ZALLOC(z, 1, w)) == Z_NULL)
  {
    ZFREE(z, s);
    return Z_NULL;
  }
  s->end = s->window + w;
  s->checkfn = c;
  s->mode = TYPE;
  Trace((stderr, "inflate:   blocks allocated\n"));
  inflate_blocks_reset(s, z, &s->check);
  return s;
}


/******************************************************************************
*
* inflate_block - inflates a block
* 
* \NOMANUAL
*/ 

static _WRS_INFLATE_IN_RAM int inflate_blocks
    (
    inflate_blocks_statef *s,
    z_streamp z,
    int r
    )
{
  uInt t;               /* temporary storage */
  uLong b;              /* bit buffer */
  uInt k;               /* bits in bit buffer */
  Byte *p;             /* input data pointer */
  uInt n;               /* bytes available there */
  Byte *q;             /* output window write pointer */
  uInt m;               /* bytes to end of window or read pointer */

  /* copy input/output information to locals (UPDATE macro restores) */
  LOAD

  /* process input based on current state */
  while (1) switch (s->mode)
  {
    case TYPE:
      NEEDBITS(3)
      t = (uInt)b & 7;
      s->last = t & 1;
      switch (t >> 1)
      {
        case 0:                         /* stored */
          Trace((stderr, "inflate:     stored block%s\n",
                 s->last ? " (last)" : ""));
          DUMPBITS(3)
          t = k & 7;                    /* go to byte boundary */
          DUMPBITS(t)
          s->mode = LENS;               /* get length of stored block */
          break;
        case 1:                         /* fixed */
          Trace((stderr, "inflate:     fixed codes block%s\n",
                 s->last ? " (last)" : ""));
          {
            uInt bl = 0;
            uInt bd = 0;
            inflate_huft *tl = NULL;
            inflate_huft *td = NULL;

            inflate_trees_fixed(&bl, &bd, &tl, &td);
            s->sub.decode.codes = inflate_codes_new(bl, bd, tl, td, z);
            if (s->sub.decode.codes == Z_NULL)
            {
              r = Z_MEM_ERROR;
              LEAVE
            }
            s->sub.decode.tl = Z_NULL;  /* don't try to free these */
            s->sub.decode.td = Z_NULL;
          }
          DUMPBITS(3)
          s->mode = CODES;
          break;
        case 2:                         /* dynamic */
          Trace((stderr, "inflate:     dynamic codes block%s\n",
                 s->last ? " (last)" : ""));
          DUMPBITS(3)
          s->mode = TABLE;
          break;
        case 3:                         /* illegal */
          DUMPBITS(3)
          s->mode = BAD;
          z->msg = (char*)"invalid block type";
          r = Z_DATA_ERROR;
          LEAVE
      }
      break;
    case LENS:
      NEEDBITS(32)
      if ((((~b) >> 16) & 0xffff) != (b & 0xffff))
      {
        s->mode = BAD;
        z->msg = (char*)"invalid stored block lengths";
        r = Z_DATA_ERROR;
        LEAVE
      }
      s->sub.left = (uInt)b & 0xffff;
      b = k = 0;                      /* dump bits */
      Tracev((stderr, "inflate:       stored length %u\n", s->sub.left));
      s->mode = s->sub.left ? STORED : (s->last ? DRY : TYPE);
      break;
    case STORED:
      if (n == 0)
        LEAVE
      NEEDOUT
      t = s->sub.left;
      if (t > n) t = n;
      if (t > m) t = m;
      memcopy(q, p, t);
      p += t;  n -= t;
      q += t;  m -= t;
      if ((s->sub.left -= t) != 0)
        break;
      Tracev((stderr, "inflate:       stored end, %lu total out\n",
              z->total_out + (q >= s->read ? q - s->read :
              (s->end - s->read) + (q - s->window))));
      s->mode = s->last ? DRY : TYPE;
      break;
    case TABLE:
      NEEDBITS(14)
      s->sub.trees.table = t = (uInt)b & 0x3fff;
      if ((t & 0x1f) > 29 || ((t >> 5) & 0x1f) > 29)
      {
        s->mode = BAD;
        z->msg = (char*)"too many length or distance symbols";
        r = Z_DATA_ERROR;
        LEAVE
      }
      t = 258 + (t & 0x1f) + ((t >> 5) & 0x1f);
      
      if ((s->sub.trees.blens = (uInt*)ZALLOC(z, t, sizeof(uInt))) == Z_NULL)
      {
        r = Z_MEM_ERROR;
        LEAVE
      }
      DUMPBITS(14)
      s->sub.trees.index = 0;
      Tracev((stderr, "inflate:       table sizes ok\n"));
      s->mode = BTREE;
      
      /* this is an intentional fall through to the next case */
      
    case BTREE:
      while (s->sub.trees.index < 4 + (s->sub.trees.table >> 10))
      {
        NEEDBITS(3)
        s->sub.trees.blens[border[s->sub.trees.index++]] = (uInt)b & 7;
        DUMPBITS(3)
      }
      while (s->sub.trees.index < 19)
        s->sub.trees.blens[border[s->sub.trees.index++]] = 0;
      s->sub.trees.bb = 7;
      r = inflate_trees_bits(s->sub.trees.blens, &s->sub.trees.bb,
                             &s->sub.trees.tb, z);
      if (r != Z_OK)
      {
        if (r == Z_DATA_ERROR)
          s->mode = BAD;
        LEAVE
      }
      s->sub.trees.index = 0;
      Tracev((stderr, "inflate:       bits tree ok\n"));
      s->mode = DTREE;
      
      /* this is an intentional fall through to the next case */

    case DTREE:
      while (t = s->sub.trees.table,
             s->sub.trees.index < 258 + (t & 0x1f) + ((t >> 5) & 0x1f))
      {
        inflate_huft *h;
        uInt i, j, c;

        t = s->sub.trees.bb;
        NEEDBITS(t)
        h = s->sub.trees.tb + ((uInt)b & inflate_mask[t]);
        if (h == NULL)
        {

          /* should never get here */

          r = Z_DATA_ERROR;
          LEAVE
        }
        t = h->word.what.Bits;
        c = h->more.Base;
        if (c < 16)
        {
          DUMPBITS(t)
          s->sub.trees.blens[s->sub.trees.index++] = c;
        }
        else /* c == 16..18 */
        {
          i = c == 18 ? 7 : c - 14;
          j = c == 18 ? 11 : 3;
          NEEDBITS(t + i)
          DUMPBITS(t)
          j += (uInt)b & inflate_mask[i];
          DUMPBITS(i)
          i = s->sub.trees.index;
          t = s->sub.trees.table;
          if (i + j > 258 + (t & 0x1f) + ((t >> 5) & 0x1f) ||
              (c == 16 && i < 1))
          {
            s->mode = BAD;
            z->msg = (char*)"invalid bit length repeat";
            r = Z_DATA_ERROR;
            LEAVE
          }
          c = c == 16 ? s->sub.trees.blens[i - 1] : 0;
          do {
            s->sub.trees.blens[i++] = c;
          } while (--j);
          s->sub.trees.index = i;
        }
      }
      inflate_trees_free(s->sub.trees.tb, z);
      s->sub.trees.tb = Z_NULL;
      {
        uInt bl, bd;
        inflate_huft *tl, *td;
        inflate_codes_statef *c;

        bl = 9;         /* must be <= 9 for lookahead assumptions */
        bd = 6;         /* must be <= 9 for lookahead assumptions */
        t = s->sub.trees.table;
        t = inflate_trees_dynamic(257 + (t & 0x1f), 1 + ((t >> 5) & 0x1f),
                                  s->sub.trees.blens, &bl, &bd, &tl, &td, z);
        if (t != Z_OK)
        {
          if (t == (uInt)Z_DATA_ERROR)
            s->mode = BAD;
          r = t;
          LEAVE
        }
        if ((c = inflate_codes_new(bl, bd, tl, td, z)) == Z_NULL)
        {
          inflate_trees_free(td, z);
          inflate_trees_free(tl, z);
          r = Z_MEM_ERROR;
          LEAVE
        }
        ZFREE(z, s->sub.trees.blens);
        s->sub.decode.codes = c;
        s->sub.decode.tl = tl;
        s->sub.decode.td = td;
      }
      s->mode = CODES;
      
      /* this is an intentional fall through to the next case */
      
    case CODES:
      UPDATE
      if ((r = inflate_codes(s, z, r)) != Z_STREAM_END)
        return inflate_flush(s, z, r);
      r = Z_OK;
      inflate_codes_free(s->sub.decode.codes, z);
      inflate_trees_free(s->sub.decode.td, z);
      inflate_trees_free(s->sub.decode.tl, z);
      LOAD
      Tracev((stderr, "inflate:       codes end, %lu total out\n",
              z->total_out + (q >= s->read ? q - s->read :
              (s->end - s->read) + (q - s->window))));
      if (!s->last)
      {
        s->mode = TYPE;
        break;
      }
      if (k > 7)              /* return unused byte, if any */
      {
        Assert(k < 16, "inflate_codes grabbed too many bytes")
        k -= 8;
        n++;
        p--;                    /* can always return one */
      }
      s->mode = DRY;
    case DRY:
      FLUSH
      if (s->read != s->write)
        LEAVE
      s->mode = DONE;
      
      /* this is an intentional fall through to the next case */
      
    case DONE:
      r = Z_STREAM_END;
      LEAVE
    case BAD:
      r = Z_DATA_ERROR;
      LEAVE
    default:
      r = Z_STREAM_ERROR;
      LEAVE
  }
}

/******************************************************************************
*
* inflate_blocks_free - frees inflate blocks
* 
* \NOMANUAL
*/ 

static _WRS_INFLATE_IN_RAM int inflate_blocks_free
    (
    inflate_blocks_statef *s,
    z_streamp z,
    uLong *c
    )
{
  inflate_blocks_reset(s, z, c);
  ZFREE(z, s->window);
  ZFREE(z, s);
  return Z_OK;
}

/******************************************************************************
*
* inflateReset - reset inflate
* 
* \NOMANUAL
*/ 

static _WRS_INFLATE_IN_RAM int inflateReset
    (
    z_streamp z
    )
{
  uLong c;

  if (z == Z_NULL || z->state == Z_NULL)
    return Z_STREAM_ERROR;
  z->total_in = z->total_out = 0;
  z->msg = Z_NULL;
  z->state->mode = z->state->nowrap ? BLOCKS : METHOD;
  inflate_blocks_reset(z->state->blocks, z, &c);
  Trace((stderr, "inflate: reset\n"));
  return Z_OK;
}

/******************************************************************************
*
* inflateEnd - end inflate
* 
* \NOMANUAL
*/ 

static _WRS_INFLATE_IN_RAM int inflateEnd
    (
    z_streamp z
    )
{
  uLong c;

  if (z == Z_NULL || z->state == Z_NULL || z->zfree == Z_NULL)
    return Z_STREAM_ERROR;
  if (z->state->blocks != Z_NULL)
    inflate_blocks_free(z->state->blocks, z, &c);
  ZFREE(z, z->state);
  z->state = Z_NULL;
  Trace((stderr, "inflate: end\n"));
  return Z_OK;
}

/******************************************************************************
*
* inflateInit - initializes inflate
* 
* \NOMANUAL
*/ 

static _WRS_INFLATE_IN_RAM int inflateInit
    (
    z_streamp z
    )
{
  int w = DEF_WBITS;

  /* initialize state */
  if (z == Z_NULL)
    return Z_STREAM_ERROR;
  z->msg = Z_NULL;
  if (z->zalloc == Z_NULL)
  {
    z->zalloc = (alloc_func)zcalloc;
    z->opaque = (voidp)0;
  }
  if (z->zfree == Z_NULL) z->zfree = zcfree;
  if ((z->state = (struct internal_state  *)
       ZALLOC(z,1,sizeof(struct internal_state))) == Z_NULL)
    return Z_MEM_ERROR;
  z->state->blocks = Z_NULL;

  /* handle undocumented nowrap option (no zlib header or check) */
  z->state->nowrap = 0;

#if (DEF_WBITS < 0)
  if (w < 0)    /* DEADCODE - left in for completeness */
  {
    w = - w;
    z->state->nowrap = 1;
  }
#endif

#if (DEF_WBITS < 8) || (DEF_WBITS > 15)
  /* set window size */
  if (w < 8 || w > 15)
  {
    inflateEnd(z);
    return Z_STREAM_ERROR;
  }
#endif

  z->state->wbits = (uInt)w;

  /* create inflate_blocks state */
  if ((z->state->blocks =
      inflate_blocks_new(z, z->state->nowrap ? Z_NULL : adler32, (uInt)1 << w))
      == Z_NULL)
  {
    (void)inflateEnd(z);
    
    return Z_MEM_ERROR;
  }
  Trace((stderr, "inflate: allocated\n"));

  /* reset state */
  inflateReset(z);
  return Z_OK;
}

/* XXX */
#undef	NEEDBYTE
#undef	NEXTBYTE
#define NEEDBYTE {if(z->avail_in==0)return r;r=Z_OK;}
#define NEXTBYTE (z->avail_in--,z->total_in++,*z->next_in++)

/******************************************************************************
*
* zinflate - internal of inflate
* 
* \NOMANUAL
*/ 

static _WRS_INFLATE_IN_RAM int zinflate
    (
    z_streamp z,
    int f
    )
{
  int r;
  uInt b;

  if (z == Z_NULL || z->state == Z_NULL || z->next_in == Z_NULL || f < 0)
    return Z_STREAM_ERROR;
  r = Z_BUF_ERROR;
  while (1) switch (z->state->mode)
  {
    case METHOD:
      NEEDBYTE
      if (((z->state->sub.method = NEXTBYTE) & 0xf) != Z_DEFLATED)
      {
        z->state->mode = INF_BAD;
        z->msg = (char*)"unknown compression method";
        break;
      }
      if ((z->state->sub.method >> 4) + 8 > z->state->wbits)
      {
        z->state->mode = INF_BAD;
        z->msg = (char*)"invalid window size";
        break;
      }
      z->state->mode = FLAG;
    case FLAG:
      NEEDBYTE
      b = NEXTBYTE;
      if (((z->state->sub.method << 8) + b) % 31)
      {
        z->state->mode = INF_BAD;
        z->msg = (char*)"incorrect header check";
        break;
      }
      Trace((stderr, "inflate: zlib header ok\n"));
      if (!(b & PRESET_DICT))
      {
        z->state->mode = BLOCKS;
	break;
      }
      z->state->mode = DICT4;
    case DICT4:
      NEEDBYTE
      z->state->sub.check.need = (uLong)NEXTBYTE << 24;
      z->state->mode = DICT3;
      
      /* this is an intentional fall through to the next case */
      
    case DICT3:
      NEEDBYTE
      z->state->sub.check.need += (uLong)NEXTBYTE << 16;
      z->state->mode = DICT2;
      
      /* this is an intentional fall through to the next case */
      
    case DICT2:
      NEEDBYTE
      z->state->sub.check.need += (uLong)NEXTBYTE << 8;
      z->state->mode = DICT1;
      
      /* this is an intentional fall through to the next case */
      
    case DICT1:
      NEEDBYTE
      z->state->sub.check.need += (uLong)NEXTBYTE;
      z->adler = z->state->sub.check.need;
      z->state->mode = DICT0;
      return Z_NEED_DICT;
    case DICT0:
      z->state->mode = INF_BAD;
      z->msg = (char*)"need dictionary";
      return Z_STREAM_ERROR;
    case BLOCKS:
      r = inflate_blocks(z->state->blocks, z, r);
      if (r == Z_DATA_ERROR)
      {
        z->state->mode = INF_BAD;
        break;
      }
      if (r != Z_STREAM_END)
        return r;
      r = Z_OK;
      inflate_blocks_reset(z->state->blocks, z, &z->state->sub.check.was);
      if (z->state->nowrap)
      {
        z->state->mode = INF_DONE;
        break;
      }
      z->state->mode = CHECK4;
    case CHECK4:
      NEEDBYTE
      z->state->sub.check.need = (uLong)NEXTBYTE << 24;
      z->state->mode = CHECK3;
      
      /* this is an intentional fall through to the next case */
      
    case CHECK3:
      NEEDBYTE
      z->state->sub.check.need += (uLong)NEXTBYTE << 16;
      z->state->mode = CHECK2;
    case CHECK2:
      NEEDBYTE
      z->state->sub.check.need += (uLong)NEXTBYTE << 8;
      z->state->mode = CHECK1;
    case CHECK1:
      NEEDBYTE
      z->state->sub.check.need += (uLong)NEXTBYTE;

      if (z->state->sub.check.was != z->state->sub.check.need)
      {
        z->state->mode = INF_BAD;
        z->msg = (char*)"incorrect data check";
        break;
      }
      Trace((stderr, "inflate: zlib check ok\n"));
      z->state->mode = INF_DONE;
    case INF_DONE:
      return Z_STREAM_END;
    case INF_BAD:
      return Z_DATA_ERROR;
    default:
      return Z_STREAM_ERROR;
  }
}

/* global variables */

int inflateCksum = 0;	/* set to TRUE to validate compressed checksum */

/******************************************************************************
*
* inflate - inflate compressed code
*
* This routine inflates <nBytes> of data starting at address <src>.
* The inflated code is copied starting at address <dest>.
* Two sanity checks are performed on the data being decompressed.
* First, we look for a magic number at the start of the data to
* verify that it is really a compressed stream.
* Second, the entire data is optionally checksummed to verify its
* integrity. By default, the checksum is not verified in order to
* speed up the booting process. To turn on checksum verification,
* set the global variable `inflateCksum' to TRUE in the BSP.
*
* RETURNS: OK or ERROR.
*/ 

_WRS_INFLATE_IN_RAM int inflate
    (
    Byte *	src,
    Byte *	dest,
    int		nBytes
    )
    {
    z_stream d_stream;  	/* decompression stream */
    int err;

    /* we must initialize BSS variables since it's not done in bootInit */

    fixed_built = 0;
    fixed_bl = 0;
    fixed_bd = 0;
    fixed_tl = 0;
    fixed_td = 0;
    bufZero ((char *)fixed_mem, sizeof (fixed_mem));

    BLK_PREV(nextBlock) = 0;	/* set first block's prev pointer */

    /*
     * Validate the compression stream.
     * The first byte should be Z_DEFLATED, the last two a valid checksum.
     */

    if (*src != Z_DEFLATED)
	{
	DBG_PUT ("inflate error: *src = %d. not Z_DEFLATED data\n", *src);
	return (-1);
	}

    if (inflateCksum)
	{
	if (cksum (0, src, nBytes) != 0xffff)
	    {
	    DBG_PUT ("checksum error: 0x%x != 0xffff\n\n", \
		cksum (0, src, nBytes));
	    return (-1);
	    }
	}

    src++;
    nBytes -= 3;

    /* do the decompression */

    d_stream.zalloc = (alloc_func)0;
    d_stream.zfree = (free_func)0;
    d_stream.opaque = (voidp)0;

    d_stream.next_in  = src;
    d_stream.avail_in = nBytes;
    d_stream.next_out = dest;

    /* set UINT_MAX to avoid data truncation under high compression ratio */

    d_stream.avail_out = UINT_MAX;

    if (inflateInit(&d_stream) != Z_OK)
	return (-1);

    err = zinflate(&d_stream, 0);

    if (err == Z_STREAM_END)
	{
	err = inflateEnd(&d_stream);
	}

    if (err != Z_OK)
	return (-1);

    return (0);
    }

