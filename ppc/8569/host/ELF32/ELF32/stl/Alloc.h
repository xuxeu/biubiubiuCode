#ifndef _CCG_ALLOC_H
#define _CCG_ALLOC_H
#include "basictypes.h"

#define _TRY_BEGIN	try {
 #define _CATCH(x)	} catch (x) {
 #define _CATCH_ALL	} catch (...) {
 #define _CATCH_END	}

////////////////////////////////////////////////////
#define _IGNORE_MYCONT ((const _Container_base *)-2)

inline void* operator new(UINT32 first, void *_Where)
{
	return (_Where);
}

// Used to initialize _Myfirstiter and _Mynextiter when _HAS_ITERATOR_DEBUGGING is off
#define _IGNORE_MYITERLIST ((_Iterator_base *)-3)

namespace com_coretek_tools_stl
{
	class ELF32_API _Container_base
		{	// store head of iterator chain
	public:
		friend class _Iterator_base;

		 _Container_base()
			: _Myfirstiter(0)
			{	// construct childless container
			}

		 _Container_base(const _Container_base&)
			: _Myfirstiter(0)
			{	// copy a container
			}

		_Container_base&  operator=(const _Container_base&)
			{	// assign a container
			return (*this);
			}

		 ~_Container_base()
			{	// destroy the container
			_Orphan_all();
			}

		void  _Orphan_all() const;	// orphan all iterators
		void  _Swap_all(_Container_base&) const;	// swaps all iterators

		_Iterator_base *_Myfirstiter;
		};

	class _Iterator_base
		{	// store links to container, next iterator
	public:
		friend class _Container_base;

		 _Iterator_base()
			: _Mycont(0), _Mynextiter(0)
			{	// construct orphaned iterator
			}

		 _Iterator_base(const _Iterator_base& _Right)
			: _Mycont(0), _Mynextiter(0)
			{	// copy an iterator
			*this = _Right;
			}

		_Iterator_base&  operator=(const _Iterator_base& _Right)
			{	// assign an iterator
			if (_Mycont != _Right._Mycont)
				{	// change parentage
				_Orphan_me();
				_Adopt(_Right._Mycont);
				}
			return (*this);
			}

		 ~_Iterator_base()
			{	// destroy the iterator
			_Orphan_me();
			}

		void  _Adopt(const _Container_base *_Parent)
			{	// adopt this iterator by parent
			if (_Mycont != _Parent)
				{	// change parentage
				_Orphan_me();
				if (_Parent != 0 && _Parent->_Myfirstiter != _IGNORE_MYITERLIST)
					{	// switch to new parent
					_Mynextiter = _Parent->_Myfirstiter;
					((_Container_base *)_Parent)->_Myfirstiter = this;
					}
				_Mycont = _Parent;
				}
			}

		void  _Orphan_me()
			{	// cut ties with parent
			if (_Mycont != 0 && _Mycont->_Myfirstiter != _IGNORE_MYITERLIST)
				{	// adopted, remove self from list
				_Iterator_base **_Pnext =
					(_Iterator_base **)&_Mycont->_Myfirstiter;
				while (*_Pnext != 0 && *_Pnext != this)
					_Pnext = &(*_Pnext)->_Mynextiter;

				if (*_Pnext == 0)
					//_DEBUG_ERROR("ITERATOR LIST CORRUPTED!");
					return;
				*_Pnext = _Mynextiter;
				_Mycont = 0;
				}
			}

		const _Container_base *_Mycont;
		_Iterator_base *_Mynextiter;
		};

	typedef _Iterator_base _Iterator_base_secure;


	template<class _Ty,
		class _Diff,
		class _Pointer,
		class _Reference>
		struct _Ranit
			: public _Iterator_base

		{	// base type for container random-access iterator classes
		typedef _Ty value_type;
		typedef _Diff difference_type;
		typedef _Diff distance_type;	// retained
		typedef _Pointer pointer;
		typedef _Reference reference;
		};


	template<class _FwdIt, class _Ty> inline
		void _Fill(_FwdIt _First, _FwdIt _Last, const _Ty& _Val)
		{	// copy _Val through [_First, _Last)
		for (; _First != _Last; ++_First)
			*_First = _Val;
		}

	inline void _Fill(char *_First, char *_Last, int _Val)
		{
		memset(_First, _Val, _Last - _First);
		}

	inline void _Fill(signed char *_First, 
		 signed char *_Last, int _Val)
		{
		memset(_First, _Val, _Last - _First);
		}

	inline void _Fill(
		 unsigned char *_First,
		 unsigned char *_Last, int _Val)
		{
		memset(_First, _Val, _Last - _First);
		}

	template<class _FwdIt, class _Ty> inline
		void fill(_FwdIt _First, _FwdIt _Last, const _Ty& _Val)
		{
		_Fill(_First, _Last, _Val);
		}


	template<class _BidIt1, class _BidIt2>
	inline
		_BidIt2 _Copy_backward_opt(_BidIt1 _First, _BidIt1 _Last, _BidIt2 _Dest)
		{
		while (_First != _Last)
			*--_Dest = *--_Last;
		return (_Dest);
		}

	template<class _BidIt1,	class _BidIt2> 
	inline _BidIt2 copy_backward(_BidIt1 _First, _BidIt1 _Last, _BidIt2 _Dest)
		{	
		return _Copy_backward_opt(_First, _Last, _Dest);
		}

			// TEMPLATE FUNCTION _Move_backward
	template<class _BidIt1, class _BidIt2>
	inline
		_BidIt2 _Move_backward_opt(_BidIt1 _First, _BidIt1 _Last, _BidIt2 _Dest)
		{	// move defaults to copy if there is not a more effecient way
		return _Copy_backward_opt(_First, _Last, _Dest);
		}

	template<class _InIt, class _OutIt>
	inline
		_OutIt _Copy_opt(_InIt _First, _InIt _Last, _OutIt _Dest)
		{
		for (; _First != _Last; ++_Dest, ++_First)
			*_Dest = *_First;
		return (_Dest);
		}

	template<class _InIt,
		class _OutIt> inline
		_OutIt unchecked_copy(_InIt _First, _InIt _Last, _OutIt _Dest)
		{
			return _Copy_opt(_First, _Last, _Dest);
		}

	template<class _InIt,
		class _OutIt> inline
		_OutIt checked_copy(_InIt _First, _InIt _Last, _OutIt _Dest)
		{
			return _Copy_opt(_First, _Last, _Dest);
		}

	template<class _BidIt1,
		class _BidIt2> inline
		_BidIt2 _Unchecked_move_backward(_BidIt1 _First, _BidIt1 _Last, _BidIt2 _Dest)
		{	// move [_First, _Last) backwards to [..., _Dest)
			return _Move_backward_opt((_First), (_Last), _Dest);
		}

	template<class _BidIt1,
		class _BidIt2> inline
		_BidIt2 _Checked_move_backward(_BidIt1 _First, _BidIt1 _Last, _BidIt2 _Dest)
		{	// move [_First, _Last) backwards to [..., _Dest)
			return (_Move_backward_opt(_CHECKED_BASE(_First), _CHECKED_BASE(_Last), _Dest,
				_Iter_random(_First, _Dest), _Move_cat(_Dest), _Checked_cat(_Dest)));
		}

		template<class _FwdIt,
		class _Diff,
		class _Tval,
		class _Alloc> inline
		void _Uninit_fill_n(_FwdIt _First, _Diff _Count,
			const _Tval& _Val, _Alloc& _Al)
		{

		_FwdIt _Next = _First;

		_TRY_BEGIN
			for (; 0 < _Count; --_Count, ++_First)
				_Al.construct(_First, _Val);
		_CATCH_ALL
			for (; _Next != _First; ++_Next)
				_Al.destroy(_Next);
		_CATCH_END
		}

		template<class _FwdIt,
		class _Diff,
		class _Tval,
		class _Alloc> inline
		void unchecked_uninitialized_fill_n(_FwdIt _First, _Diff _Count,
			const _Tval& _Val, _Alloc& _Al)
		{
			_Uninit_fill_n(_First, _Count, _Val, _Al);
		}

		template<class _InIt,
		class _FwdIt,
		class _Alloc> inline
		_FwdIt _Uninit_copy(_InIt _First, _InIt _Last, _FwdIt _Dest,
			_Alloc& _Al)
		{

		_FwdIt _Next = _Dest;

		_TRY_BEGIN
			for (; _First != _Last; ++_Dest, ++_First)
				_Al.construct(_Dest, *_First);
		_CATCH_ALL
			for (; _Next != _Dest; ++_Next)
				_Al.destroy(_Next);

		_CATCH_END
		return (_Dest);
		}

		template<class _InIt,
		class _FwdIt,
		class _Alloc> inline
		_FwdIt unchecked_uninitialized_copy(_InIt _First, _InIt _Last, _FwdIt _Dest,
			_Alloc& _Al)
		{	// copy [_First, _Last) to raw _Dest, using _Al
			return _Uninit_copy(_First, _Last, _Dest, _Al);
		}

		template<class _InIt, class _FwdIt, class _Alloc>
	inline
		_FwdIt _Uninit_move(_InIt _First, _InIt _Last, _FwdIt _Dest,
			_Alloc& _Al)
		{	// move defaults to copy if there is not a more effecient way
		return unchecked_uninitialized_copy(_First, _Last, _Dest, _Al);
		}

		template<class _InIt,
		class _FwdIt,
		class _Alloc> inline
		_FwdIt _Unchecked_uninitialized_move(_InIt _First, _InIt _Last, _FwdIt _Dest,
			_Alloc& _Al)
		{	// move [_First, _Last) to raw _Dest, using _Al
			return _Uninit_move(_First, _Last, _Dest, _Al);
		}


	inline void _Container_base::_Orphan_all() const
		{	// orphan all iterators
		if (_Myfirstiter != _IGNORE_MYITERLIST)
			{
			for (_Iterator_base **_Pnext = (_Iterator_base **)&_Myfirstiter;
				*_Pnext != 0; *_Pnext = (*_Pnext)->_Mynextiter)
				(*_Pnext)->_Mycont = 0;
			*(_Iterator_base **)&_Myfirstiter = 0;
			}
		}

	template<class _InIt,
		class _OutIt> inline
		_OutIt copy(_InIt _First, _InIt _Last, _OutIt _Dest)
		{	// copy [_First, _Last) to [_Dest, ...)
		return _Copy_opt(_First, _Last, _Dest);
		}
}

#endif
