
#ifndef _CCG_MEMORY_
#define _CCG_MEMORY_

#include <stdlib.h>
#include "Alloc.h"
#include "basictypes.h"

#define _DESTRUCTOR(ty, ptr)	(ptr)->~ty()

 #define _CPOINTER_X(T, A)		\
	typename A::template rebind<T>::other::const_pointer
 
#define _CREFERENCE_X(T, A)	\
	typename A::template rebind<T>::other::const_reference
 
#define _POINTER_X(T, A)	\
	typename A::template rebind<T>::other::pointer
 
#define _REFERENCE_X(T, A)	\
	typename A::template rebind<T>::other::reference

namespace com_coretek_tools_stl
{
	template<class _Ty> 
	inline	_Ty  *_Allocate(UINT32 _Count, _Ty  *)
	{
		if (_Count <= 0)
		{
			_Count = 0;
		}
		else if (((UINT32)(-1) / _Count) < sizeof (_Ty))
		{			
			return (_Ty  *)0;
		}
		
		 _Ty* alloc =  new _Ty[_Count];
		 return (_Ty*)alloc;
	}

	template<class _T1,	class _T2> 
	inline	void _Construct(_T1  *_Ptr, const _T2& _Val)
	{
		void  *_Vptr = _Ptr;
		//TODO:下面这句调用Alloc.h里面的
		//inline void* operator new(UINT32 first, void *_Where)
		new (_Vptr) _T1(_Val);
	}

	template<class _Ty> inline
	void _Destroy(_Ty  *_Ptr)
	{
		_DESTRUCTOR(_Ty, _Ptr);
	}

	template<> 
	inline	void _Destroy(char  *)
	{
	}

	template<> 
	inline	void _Destroy(wchar_t  *)
	{
	}

	template<class _Ty>
	struct _Allocator_base
	{
		typedef _Ty value_type;
	};

	template<class _Ty>
	struct _Allocator_base<const _Ty>
	{
		typedef _Ty value_type;
	};

	template<class _Ty>
	class allocator
		: public _Allocator_base<_Ty>
	{
	public:
		typedef _Allocator_base<_Ty> _Mybase;
		typedef typename _Mybase::value_type value_type;
		typedef value_type  *pointer;
		typedef value_type & reference;
		typedef const value_type  *const_pointer;
		typedef const value_type & const_reference;

		typedef UINT32 size_type;
		typedef UINT64 difference_type;

		template<class _Other>
		struct rebind
		{
			typedef allocator<_Other> other;
		};

		pointer address(reference _Val) const
		{
			return (&_Val);
		}

		const_pointer address(const_reference _Val) const
		{
			return (&_Val);
		}

		allocator()
		{
		}

		allocator(const allocator<_Ty>&) 
		{
		}

		template<class _Other>
		allocator(const allocator<_Other>&) 
		{
		}

		template<class _Other>
		allocator<_Ty>& operator=(const allocator<_Other>&)
		{
			return (*this);
		}

		void deallocate(pointer _Ptr, size_type)
		{
			//delete[] (_Ptr);
		}

		pointer allocate(size_type _Count)
		{
			return (_Allocate(_Count, (pointer)0));
		}

		pointer allocate(size_type _Count, const void  *)
		{
			return (allocate(_Count));
		}

		void construct(pointer _Ptr, const _Ty& _Val)
		{
			_Construct(_Ptr, _Val);
		}

		void destroy(pointer _Ptr)
		{
			_Destroy(_Ptr);
		}

		UINT32 max_size() const 
		{
			UINT32 _Count = (UINT32)(-1) / sizeof (_Ty);
			return (0 < _Count ? _Count : 1);
		}
	};

	template<class _Ty,	class _Other> 
	inline	bool operator==(const allocator<_Ty>&, const allocator<_Other>&) 
	{
		return (true);
	}

	template<class _Ty,class _Other>
	inline	bool operator != (const allocator<_Ty>&, const allocator<_Other>&) 
	{
		return (false);
	}


	template<> 
	class  allocator<void>
	{
	public:
		typedef void _Ty;
		typedef _Ty  *pointer;
		typedef const _Ty  *const_pointer;
		typedef _Ty value_type;

		template<class _Other>
		struct rebind
		{
			typedef allocator<_Other> other;
		};

		allocator() 
		{
		}

		allocator(const allocator<_Ty>&) 
		{
		}

		template<class _Other>
		allocator(const allocator<_Other>&) 
		{
		}

		template<class _Other>
		allocator<_Ty>& operator=(const allocator<_Other>&)
		{
			return (*this);
		}
	};

	template<class _Ty,	class _Alloc>
	inline	void _Destroy_range(_Ty *_First, _Ty *_Last, _Alloc& _Al)
	{
		for (; _First != _Last; ++_First)
		{
			_Al.destroy(_First);
		}
	}
}

#endif /* RC_INVOKED */

