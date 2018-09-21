/**********************************************************************************
 *                     北京科银京成技术有限公司 版权所有
 *     Copyright C) 2000-2010 CoreTek Systems Inc All Rights Reserved
**********************************************************************************/

/**
 *修改历史:
       2010-9-16 13:26:58        amwyga      北京科银京成技术有限公司
       修改及方案:创建本文件
 */

/**
 *@file   Vector.h
 *@brief
       <li>实现动态数组</li>
 *@Note
       
 */

#ifndef _INCLUDE_VECTOR_H_2010916_AMWYGA
#define _INCLUDE_VECTOR_H_2010916_AMWYGA
/************************头 文 件******************************/
#include "Alloc.h"
#include "Memory.h"

/************************引    用******************************/
/************************宏 定 义******************************/
/************************类型定义******************************/
 
namespace com_coretek_tools_stl
{
	template<class _Ty,	class _Ax = allocator<_Ty> >
		class Vector;

			// TEMPLATE CLASS _Vector_const_iterator
	template<class _Ty,
		class _Alloc>
		class _Vector_const_iterator
			: public _Ranit<_Ty, typename _Alloc::difference_type,
				typename _Alloc::const_pointer, typename _Alloc::const_reference>
		{	// iterator for nonmutable Vector
	public:
		typedef _Vector_const_iterator<_Ty, _Alloc> _Myt;
		typedef Vector<_Ty, _Alloc> _Myvec;
		typedef typename _Alloc::pointer _Tptr;

		typedef _Ty value_type;
		typedef typename _Alloc::difference_type difference_type;
		typedef typename _Alloc::const_pointer pointer;
		typedef typename _Alloc::const_reference reference;

		typedef _Tptr _Inner_type;

		_Vector_const_iterator()
			{	// construct with null pointer
			_Myptr = 0;
			}

		_Vector_const_iterator(_Tptr _Ptr)
			{	// construct with pointer _Ptr
			_Myptr = _Ptr;
			}

		reference operator*() const
			{	// return designated object

			return (*_Myptr);
			}

		pointer operator->() const
			{	// return pointer to class object
			return (&**this);
			}

		_Myt& operator++()
			{	// preincrement
			++_Myptr;
			return (*this);
			}

		_Myt operator++(int)
			{	// postincrement
			_Myt _Tmp = *this;
			++*this;
			return (_Tmp);
			}

		_Myt& operator--()
			{	// predecrement
			--_Myptr;
			return (*this);
			}

		_Myt operator--(int)
			{	// postdecrement
			_Myt _Tmp = *this;
			--*this;
			return (_Tmp);
			}

		_Myt& operator+=(difference_type _Off)
			{	// increment by integer
			_Myptr += _Off;
			return (*this);
			}

		_Myt operator+(difference_type _Off) const
			{	// return this + integer
			_Myt _Tmp = *this;
			return (_Tmp += _Off);
			}

		_Myt& operator-=(difference_type _Off)
			{	// decrement by integer
			return (*this += -_Off);
			}

		_Myt operator-(difference_type _Off) const
			{	// return this - integer
			_Myt _Tmp = *this;
			return (_Tmp -= _Off);
			}

		difference_type operator-(const _Myt& _Right) const
			{	// return difference of iterators

				return (_Myptr - _Right._Myptr);
			}

		reference operator[](difference_type _Off) const
			{	// subscript
			return (*(*this + _Off));
			}

		bool operator==(const _Myt& _Right) const
			{	// test for iterator equality

			return (_Myptr == _Right._Myptr);
			}

		bool operator!=(const _Myt& _Right) const
			{	// test for iterator inequality
				return (!(*this == _Right));
			}

		bool operator<(const _Myt& _Right) const
			{	// test if this < _Right
				return (_Myptr < _Right._Myptr);
			}

		bool operator>(const _Myt& _Right) const
			{	// test if this > _Right
			return (_Right < *this);
			}

		bool operator<=(const _Myt& _Right) const
			{	// test if this <= _Right
			return (!(_Right < *this));
			}

		bool operator>=(const _Myt& _Right) const
			{	// test if this >= _Right
			return (!(*this < _Right));
			}

		static void _Xlen()
			{	// report a length_error
			
			}

		static void _Xran()
			{	// report an out_of_range error

			}

		static void _Xinvarg()
			{	// report an invalid_argument error

			}

		_Tptr _Myptr;	// offset of element in Vector
		};

	template<class _Ty,
		class _Alloc> inline
		_Vector_const_iterator<_Ty, _Alloc> operator+(
			typename _Vector_const_iterator<_Ty, _Alloc>::difference_type _Off,
			_Vector_const_iterator<_Ty, _Alloc> _Next)
		{	// add offset to iterator
		return (_Next += _Off);
		}

			// TEMPLATE CLASS _Vector_iterator
	template<class _Ty,
		class _Alloc>
		class _Vector_iterator
			: public _Vector_const_iterator<_Ty, _Alloc>
			{	// iterator for mutable Vector
	public:
		typedef _Vector_iterator<_Ty, _Alloc> _Myt;
		typedef _Vector_const_iterator<_Ty, _Alloc> _Mybase;

		typedef _Ty value_type;
		typedef typename _Alloc::difference_type difference_type;
		typedef typename _Alloc::pointer pointer;
		typedef typename _Alloc::reference reference;


		_Vector_iterator()
			{	// construct with null Vector pointer
			}


		_Vector_iterator(pointer _Ptr)
			: _Mybase(_Ptr)
			{	// construct with pointer _Ptr
			}

		reference operator*() const
			{	// return designated object
			return ((reference)**(_Mybase *)this);
			}

		pointer operator->() const
			{	// return pointer to class object
			return (&**this);
			}

		_Myt& operator++()
			{	// preincrement
			++(*(_Mybase *)this);
			return (*this);
			}

		_Myt operator++(int)
			{	// postincrement
			_Myt _Tmp = *this;
			++*this;
			return (_Tmp);
			}

		_Myt& operator--()
			{	// predecrement
			--(*(_Mybase *)this);
			return (*this);
			}

		_Myt operator--(int)
			{	// postdecrement
			_Myt _Tmp = *this;
			--*this;
			return (_Tmp);
			}

		_Myt& operator+=(difference_type _Off)
			{	// increment by integer
			(*(_Mybase *)this) += _Off;
			return (*this);
			}

		_Myt operator+(difference_type _Off) const
			{	// return this + integer
			_Myt _Tmp = *this;
			return (_Tmp += _Off);
			}

		_Myt& operator-=(difference_type _Off)
			{	// decrement by integer
			return (*this += -_Off);
			}

		_Myt operator-(difference_type _Off) const
			{	// return this - integer
			_Myt _Tmp = *this;
			return (_Tmp -= _Off);
			}

		difference_type operator-(const _Mybase& _Right) const
			{	// return difference of iterators
			return (*(_Mybase *)this - _Right);
			}

		reference operator[](difference_type _Off) const
			{	// subscript
			return (*(*this + _Off));
			}
		};

	template<class _Ty,
		class _Alloc> inline
		_Vector_iterator<_Ty, _Alloc> operator+(
			typename _Vector_iterator<_Ty, _Alloc>::difference_type _Off,
			_Vector_iterator<_Ty, _Alloc> _Next)
		{	// add offset to iterator
		return (_Next += _Off);
		}

			// TEMPLATE CLASS _Vector_val
	template<class _Ty,
		class _Alloc>
		class ELF32_API _Vector_val
			: public _Container_base
		{	// base class for Vector to hold allocator _Alval
	protected:
		_Vector_val(_Alloc _Al = _Alloc())
			: _Alval(_Al)
			{	// construct allocator from _Al
			}

		typedef typename _Alloc::template
			rebind<_Ty>::other _Alty;

		_Alty _Alval;	// allocator object for values
		};

			// TEMPLATE CLASS Vector
	template<class _Ty,
		class _Ax>
		class ELF32_API Vector
			: public _Vector_val<_Ty, _Ax>
		{	// varying size array of values
	public:
		typedef Vector<_Ty, _Ax> _Myt;
		typedef _Vector_val<_Ty, _Ax> _Mybase;
		typedef typename _Mybase::_Alty _Alloc;
		typedef _Alloc allocator_type;
		typedef typename _Alloc::size_type size_type;
		typedef typename _Alloc::difference_type _Dift;
		typedef _Dift difference_type;
		typedef typename _Alloc::pointer _Tptr;
		typedef typename _Alloc::const_pointer _Ctptr;
		typedef _Tptr pointer;
		typedef _Ctptr const_pointer;
		typedef typename _Alloc::reference _Reft;
		typedef _Reft reference;
		typedef typename _Alloc::const_reference const_reference;
		typedef typename _Alloc::value_type value_type;

	  #define _VEC_ITER_BASE(it)	(it)._Myptr

		typedef _Vector_iterator<_Ty, _Alloc> iterator;
		typedef _Vector_const_iterator<_Ty, _Alloc> const_iterator;

		friend class _Vector_const_iterator<_Ty, _Alloc>;

		Vector()
			: _Mybase()
			{	// construct empty Vector
			_Buy(0);
			}

		explicit Vector(const _Alloc& _Al)
			: _Mybase(_Al)
			{	// construct empty Vector with allocator
			_Buy(0);
			}

		explicit Vector(size_type _Count)
			: _Mybase()
			{	// construct from _Count * _Ty()
			_Construct_n(_Count, _Ty());
			}

		Vector(size_type _Count, const _Ty& _Val)
			: _Mybase()
			{	// construct from _Count * _Val
			_Construct_n(_Count, _Val);
			}

		Vector(size_type _Count, const _Ty& _Val, const _Alloc& _Al)
			: _Mybase(_Al)
			{	// construct from _Count * _Val, with allocator
			_Construct_n(_Count, _Val);
			}

		Vector(const _Myt& _Right)
			: _Mybase(_Right._Alval)
			{	// construct by copying _Right
			if (_Buy(_Right.size()))
				_TRY_BEGIN
				_Mylast = _Ucopy(_Right.begin(), _Right.end(), _Myfirst);
				_CATCH_ALL
				_Tidy();
				
				_CATCH_END
			}

		template<class _Iter>
			Vector(_Iter _First, _Iter _Last)
			: _Mybase()
			{	// construct from [_First, _Last)
			_Construct(_First, _Last, _Iter_cat(_First));
			}

		template<class _Iter>
			Vector(_Iter _First, _Iter _Last, const _Alloc& _Al)
			: _Mybase(_Al)
			{	// construct from [_First, _Last), with allocator
			_Construct(_First, _Last);
			}

		template<class _Iter>
			void _Construct(_Iter _First,_Iter _Last)
			{	// initialize with [_First, _Last), input iterators
			_Buy(0);
			_TRY_BEGIN
			insert(begin(), _First, _Last);
			_CATCH_ALL
			_Tidy();
			
			_CATCH_END
			}

		void _Construct_n(size_type _Count, const _Ty& _Val)
			{	// construct from _Count * _Val
			if (_Buy(_Count))
				{	// nonzero, fill it
				_TRY_BEGIN
				_Mylast = _Ufill(_Myfirst, _Count, _Val);
				_CATCH_ALL
				_Tidy();
				
				_CATCH_END
				}
			}

		~Vector()
			{	// destroy the object
			_Tidy();
			}

		_Myt& operator=(const _Myt& _Right)
			{	// assign _Right
			if (this != &_Right)
				{	// worth doing


				if (_Right.size() == 0)
					clear();	// new sequence empty, erase existing sequence
				else if (_Right.size() <= size())
					{	// enough elements, copy new and destroy old
					pointer _Ptr = unchecked_copy(_Right._Myfirst, _Right._Mylast,
						_Myfirst);	// copy new
					_Destroy(_Ptr, _Mylast);	// destroy old
					_Mylast = _Myfirst + _Right.size();
					}
				else if (_Right.size() <= capacity())
					{	// enough room, copy and construct new
					pointer _Ptr = _Right._Myfirst + size();
					unchecked_copy(_Right._Myfirst, _Ptr, _Myfirst);
					_Mylast = _Ucopy(_Ptr, _Right._Mylast, _Mylast);
					}
				else
					{	// not enough room, allocate new array and construct new
					if (_Myfirst != 0)
						{	// discard old array
						_Destroy(_Myfirst, _Mylast);
						this->_Alval.deallocate(_Myfirst, _Myend - _Myfirst);
						}
					if (_Buy(_Right.size()))
						_Mylast = _Ucopy(_Right._Myfirst, _Right._Mylast,
							_Myfirst);
					}
				}
			return (*this);
			}

		void reserve(size_type _Count)
			{	// determine new minimum length of allocated storage
			if (max_size() < _Count)
				_Xlen();	// result too long
			else if (capacity() < _Count)
				{	// not enough room, reallocate
				pointer _Ptr = this->_Alval.allocate(_Count);

				_TRY_BEGIN
				_Umove(begin(), end(), _Ptr);
				_CATCH_ALL
				this->_Alval.deallocate(_Ptr, _Count);
				
				_CATCH_END

				size_type _Size = size();
				if (_Myfirst != 0)
					{	// destroy and deallocate old array
					_Destroy(_Myfirst, _Mylast);
					this->_Alval.deallocate(_Myfirst, _Myend - _Myfirst);
					}


				_Myend = _Ptr + _Count;
				_Mylast = _Ptr + _Size;
				_Myfirst = _Ptr;
				}
			}

		size_type capacity() const
			{	// return current length of allocated storage
			return (_Myfirst == 0 ? 0 : (size_type)(_Myend - _Myfirst));
			}

		iterator begin()
			{	// return iterator for beginning of mutable sequence
			return (iterator(_Myfirst));
			}

		const_iterator begin() const
			{	// return iterator for beginning of nonmutable sequence
			return (const_iterator(_Myfirst));
			}

		iterator end()
			{	// return iterator for end of mutable sequence
			return (iterator(_Mylast));
			}

		const_iterator end() const
			{	// return iterator for end of nonmutable sequence
			return (const_iterator(_Mylast));
			}

		void resize(size_type _Newsize)
			{	// determine new length, padding with _Ty() elements as needed
			resize(_Newsize, _Ty());
			}

		void resize(size_type _Newsize, _Ty _Val)
			{	// determine new length, padding with _Val elements as needed
			if (size() < _Newsize)
				_Insert_n(end(), _Newsize - size(), _Val);
			else if (_Newsize < size())
				erase(begin() + _Newsize, end());
			}

		size_type size() const
			{	// return length of sequence
			return (_Myfirst == 0 ? 0 : (size_type)(_Mylast - _Myfirst));
			}

		size_type max_size() const
			{	// return maximum possible length of sequence
			return (this->_Alval.max_size());
			}

		bool empty() const
			{	// test if sequence is empty
			return (size() == 0);
			}

		_Alloc get_allocator() const
			{	// return allocator object for values
			return (this->_Alval);
			}

		const_reference at(size_type _Pos) const
			{	// subscript nonmutable sequence with checking
			if (size() <= _Pos)
				_Xran();
			return (*(begin() + _Pos));
			}

		reference at(size_type _Pos)
			{	// subscript mutable sequence with checking
			if (size() <= _Pos)
				_Xran();
			return (*(begin() + _Pos));
			}

		const_reference operator[](size_type _Pos) const
			{	// subscript nonmutable sequence

			return (*(_Myfirst + _Pos));
			}

		reference operator[](size_type _Pos)
			{	// subscript mutable sequence

			return (*(_Myfirst + _Pos));
			}

		reference front()
			{	// return first element of mutable sequence
			return (*begin());
			}

		const_reference front() const
			{	// return first element of nonmutable sequence
			return (*begin());
			}

		reference back()
			{	// return last element of mutable sequence
			return (*(end() - 1));
			}

		const_reference back() const
			{	// return last element of nonmutable sequence
			return (*(end() - 1));
			}

		void push_back(const _Ty& _Val)
			{	// insert element at end
			if (size() < capacity())

				_Mylast = _Ufill(_Mylast, 1, _Val);
			else
				insert(end(), _Val);
			}


		void pop_back()
			{	// erase element at end
			if (!empty())
				{	// erase last element
				_Destroy(_Mylast - 1, _Mylast);
				--_Mylast;
				}
			}

		template<class _Iter>
			void assign(_Iter _First, _Iter _Last)
			{	// assign [_First, _Last)
			_Assign(_First, _Last);
			}

		template<class _Iter>
			void _Assign(_Iter _First, _Iter _Last)
			{	// assign [_First, _Last), input iterators
			erase(begin(), end());
			insert(begin(), _First, _Last);
			}

		void assign(size_type _Count, const _Ty& _Val)
			{	// assign _Count * _Val
			_Assign_n(_Count, _Val);
			}

		iterator insert(iterator _Where, const _Ty& _Val)
			{	// insert _Val at _Where
			size_type _Off = size() == 0 ? 0 : (size_type)(_Where - begin());
			_Insert_n(_Where, (size_type)1, _Val);
			return (begin() + _Off);
			}

		void insert(iterator _Where, size_type _Count, const _Ty& _Val)
			{	// insert _Count * _Val at _Where
			_Insert_n(_Where, _Count, _Val);
			}

		template<class _Iter>
			void insert(iterator _Where, _Iter _First, _Iter _Last)
			{	// insert [_First, _Last) at _Where
			_Insert(_Where, _First, _Last, _Iter_cat(_First));
			}

		template<class _Iter>
			void _Insert(iterator _Where, _Iter _First, _Iter _Last)
			{	// insert _Count * _Val at _Where
			_Insert_n(_Where, (size_type)_First, (_Ty)_Last);
			}

		iterator erase(iterator _Where)
			{	// erase element at where
			unchecked_copy(_VEC_ITER_BASE(_Where) + 1, _Mylast,
				_VEC_ITER_BASE(_Where));
			_Destroy(_Mylast - 1, _Mylast);
			--_Mylast;
			return (_Where);
			}

		iterator erase(iterator _First, iterator _Last)
			{	// erase [_First, _Last)
			if (_First != _Last)
				{	// worth doing, copy down over hole

				pointer _Ptr = unchecked_copy(_VEC_ITER_BASE(_Last), _Mylast,
					_VEC_ITER_BASE(_First));

				_Destroy(_Ptr, _Mylast);
				_Mylast = _Ptr;
				}
			return (_First);
			}

		void clear()
			{	// erase all
			erase(begin(), end());
			}

		void swap(_Myt& _Right)
			{	// exchange contents with _Right
			if (this->_Alval == _Right._Alval)
				{	// same allocator, swap control information

				swap(_Myfirst, _Right._Myfirst);
				swap(_Mylast, _Right._Mylast);
				swap(_Myend, _Right._Myend);
				}
			else
				{	// different allocator, do multiple assigns
				_Myt _Ts = *this; *this = _Right, _Right = _Ts;
				}
			}

	protected:
		void _Assign_n(size_type _Count, const _Ty& _Val)
			{	// assign _Count * _Val
			_Ty _Tmp = _Val;	// in case _Val is in sequence
			erase(begin(), end());
			insert(begin(), _Count, _Tmp);
			}

		bool _Buy(size_type _Capacity)
			{	// allocate array with _Capacity elements
			_Myfirst = 0, _Mylast = 0, _Myend = 0;
			if (_Capacity == 0)
				return (false);
			else if (max_size() < _Capacity)
				_Xlen();	// result too long
			else
				{	// nonempty array, allocate storage
				_Myfirst = this->_Alval.allocate(_Capacity);
				_Mylast = _Myfirst;
				_Myend = _Myfirst + _Capacity;
				}
			return (true);
			}

		void _Destroy(pointer _First, pointer _Last)
			{	// destroy [_First, _Last) using allocator
			_Destroy_range(_First, _Last, this->_Alval);
			}

		void _Tidy()
			{	// free all storage
			if (_Myfirst != 0)
				{	// something to free, destroy and deallocate it

				_Destroy(_Myfirst, _Mylast);
				this->_Alval.deallocate(_Myfirst, (size_type)(_Myend - _Myfirst));
				}
			_Myfirst = 0, _Mylast = 0, _Myend = 0;
			}

		template<class _Iter>
			pointer _Ucopy(_Iter _First, _Iter _Last, pointer _Ptr)
			{	// copy initializing [_First, _Last), using allocator
			return (unchecked_uninitialized_copy(_First, _Last,
				_Ptr, this->_Alval));
			}

		template<class _Iter>
			pointer _Umove(_Iter _First, _Iter _Last, pointer _Ptr)
			{	// move initializing [_First, _Last), using allocator
			return (_Unchecked_uninitialized_move(_First, _Last,
				_Ptr, this->_Alval));
			}

		void _Insert_n(iterator _Where,	size_type _Count, const _Ty& _Val)
			{
			_Ty _Tmp = _Val;	// in case _Val is in sequence
			size_type _Capacity = capacity();

			if (_Count == 0)
				;
			else if (max_size() - size() < _Count)
				_Xlen();	// result too long
			else if (_Capacity < size() + _Count)
				{	// not enough room, reallocate
				_Capacity = max_size() - _Capacity / 2 < _Capacity
					? 0 : _Capacity + _Capacity / 2;	// try to grow by 50%
				if (_Capacity < size() + _Count)
				{
					_Capacity = size() + _Count;
				}

				pointer _Newvec = this->_Alval.allocate(_Capacity);
				pointer _Ptr = _Newvec;

				_TRY_BEGIN
				_Ptr = _Umove(_Myfirst, _VEC_ITER_BASE(_Where),
					_Newvec);	// copy prefix
				_Ptr = _Ufill(_Ptr, _Count, _Tmp);	// add new stuff
				_Umove(_VEC_ITER_BASE(_Where), _Mylast, _Ptr);	// copy suffix
				_CATCH_ALL
				_Destroy(_Newvec, _Ptr);
				this->_Alval.deallocate(_Newvec, _Capacity);
				_CATCH_END

				_Count += size();
				if (_Myfirst != 0)
					{	// destroy and deallocate old array
					_Destroy(_Myfirst, _Mylast);
					this->_Alval.deallocate(_Myfirst, (size_type)(_Myend - _Myfirst));
					}

				_Myend = _Newvec + _Capacity;
				_Mylast = _Newvec + _Count;
				_Myfirst = _Newvec;
				}
			else if ((size_type)(_Mylast - _VEC_ITER_BASE(_Where)) < _Count)
				{	// new stuff spills off end
				_Umove(_VEC_ITER_BASE(_Where), _Mylast,
					_VEC_ITER_BASE(_Where) + _Count);	// copy suffix

				_TRY_BEGIN
				_Ufill(_Mylast, _Count - (_Mylast - _VEC_ITER_BASE(_Where)),
					_Tmp);	// insert new stuff off end
				_CATCH_ALL
				_Destroy(_VEC_ITER_BASE(_Where) + _Count, _Mylast + _Count);
				_CATCH_END

				_Mylast += _Count;

				fill(_VEC_ITER_BASE(_Where), _Mylast - _Count,
					_Tmp);	// insert up to old end
				}
			else
				{	// new stuff can all be assigned
				pointer _Oldend = _Mylast;
				_Mylast = _Umove(_Oldend - _Count, _Oldend,
					_Mylast);	// copy suffix

				_Unchecked_move_backward(_VEC_ITER_BASE(_Where), _Oldend - _Count,
					_Oldend);	// copy hole
				fill(_VEC_ITER_BASE(_Where), _VEC_ITER_BASE(_Where) + _Count,
					_Tmp);	// insert into hole
				}
			}

		pointer _Ufill(pointer _Ptr, size_type _Count, const _Ty &_Val)
			{	// copy initializing _Count * _Val, using allocator
			unchecked_uninitialized_fill_n(_Ptr, _Count, _Val, this->_Alval);
			return (_Ptr + _Count);
			}

		static void _Xlen()
			{	// report a length_error

			}

		static void _Xran()
			{	// report an out_of_range error

			}

		static void _Xinvarg()
			{	// report an invalid_argument error

			}

		pointer _Myfirst;	// pointer to beginning of array
		pointer _Mylast;	// pointer to current end of sequence
		pointer _Myend;	// pointer to end of array
		};

			// Vector TEMPLATE FUNCTIONS
	template<class _Ty,
		class _Alloc> inline
		bool operator==(const Vector<_Ty, _Alloc>& _Left,
			const Vector<_Ty, _Alloc>& _Right)
		{	// test for Vector equality
		return (_Left.size() == _Right.size()
			&& equal(_Left.begin(), _Left.end(), _Right.begin()));
		}

	template<class _Ty,
		class _Alloc> inline
		bool operator!=(const Vector<_Ty, _Alloc>& _Left,
			const Vector<_Ty, _Alloc>& _Right)
		{	// test for Vector inequality
		return (!(_Left == _Right));
		}

	template<class _Ty,
		class _Alloc> inline
		bool operator<(const Vector<_Ty, _Alloc>& _Left,
			const Vector<_Ty, _Alloc>& _Right)
		{	// test if _Left < _Right for Vectors
		return (lexicographical_compare(_Left.begin(), _Left.end(),
			_Right.begin(), _Right.end()));
		}

	template<class _Ty,
		class _Alloc> inline
		bool operator>(const Vector<_Ty, _Alloc>& _Left,
			const Vector<_Ty, _Alloc>& _Right)
		{	// test if _Left > _Right for Vectors
		return (_Right < _Left);
		}

	template<class _Ty,
		class _Alloc> inline
		bool operator<=(const Vector<_Ty, _Alloc>& _Left,
			const Vector<_Ty, _Alloc>& _Right)
		{	// test if _Left <= _Right for Vectors
		return (!(_Right < _Left));
		}

	template<class _Ty,
		class _Alloc> inline
		bool operator>=(const Vector<_Ty, _Alloc>& _Left,
			const Vector<_Ty, _Alloc>& _Right)
		{	// test if _Left >= _Right for Vectors
		return (!(_Left < _Right));
		}

	template<class _Ty,
		class _Alloc> inline
		void swap(Vector<_Ty, _Alloc>& _Left, Vector<_Ty, _Alloc>& _Right)
		{	// swap _Left and _Right Vectors
		_Left.swap(_Right);
		}

	//
	// TEMPLATE CLASS Vector<bool, Alloc> AND FRIENDS
	//
	typedef unsigned _Vbase;	// word type for Vector<bool> representation
	const int _VBITS = 8 * sizeof (_Vbase);	// at least CHAR_BITS bits per word

			// CLASS _Vb_iter_base
	template<class _MycontTy>
		class _Vb_iter_base
			: public _Ranit<BOOL, typename _MycontTy::difference_type,
				bool *, bool>
		{	// store information common to reference and iterators
	public:

		_Vb_iter_base()
			: _Myptr(0), _Myoff(0)
			{	// construct with null pointer
			}

		_Vb_iter_base(const _Vb_iter_base<_MycontTy>& _Right)
			: _Myptr(_Right._Myptr), _Myoff(_Right._Myoff)
			{	// construct with copy of _Right
			}

		_Vb_iter_base(_Vbase *_Ptr)
			: _Myptr(_Ptr), _Myoff(0)
			{	// construct with offset and pointer
			}

		_Vbase *_Myptr;
		typename _MycontTy::size_type _Myoff;

		static void _Xlen()
			{	// report a length_error

				}

		static void _Xran()
			{	// report an out_of_range error

			}

		static void _Xinvarg()
			{	// report an invalid_argument error

			}

		};

			// CLASS _Vb_reference
	template<class _MycontTy>
		class _Vb_reference
			: public _Vb_iter_base<_MycontTy>
		{	// reference to a bit within a base word
	public:
		_Vb_reference()
			{	// construct with null pointer
			}

		_Vb_reference(const _Vb_iter_base<_MycontTy>& _Right)
			: _Vb_iter_base<_MycontTy>(_Right)
			{	// construct with base
			}

		_Vb_reference& operator=(const _Vb_reference<_MycontTy>& _Right)
			{	// assign _Vb_reference _Right to bit
			return (*this = bool(_Right));
			}

		_Vb_reference<_MycontTy>& operator=(bool _Val)
			{	// assign _Val to bit
			if (_Val)
				*_Getptr() |= _Mask();
			else
				*_Getptr() &= ~_Mask();
			return (*this);
			}

		void flip()
			{	// toggle the bit
			*_Getptr() ^= _Mask();
			}

		bool operator~() const
			{	// test if bit is reset
			return (!bool(*this));
			}

		operator bool() const
			{	// test if bit is set
			return ((*_Getptr() & _Mask()) != 0);
			}

		_Vbase *_Getptr() const
			{	// get pointer to base word

			return (this->_Myptr);
			}

	protected:
		_Vbase _Mask() const
			{	// convert offset to mask
			return ((_Vbase)(1 << this->_Myoff));
			}
		};

	template<class _MycontTy>
		void swap(_Vb_reference<_MycontTy> _Left, _Vb_reference<_MycontTy> _Right)
		{	// swap _Left and _Right Vector<bool> elements
		bool _Val = _Left;
		_Left = _Right;
		_Right = _Val;
		}

			// CLASS _Vb_const_iterator
	template<class _MycontTy>
		class _Vb_const_iterator
			: public _Vb_iter_base<_MycontTy>
		{	// iterator for nonmutable Vector<bool>
	public:
		typedef _Vb_reference<_MycontTy> _Reft;
		typedef bool const_reference;

		typedef BOOL value_type;
		typedef typename _MycontTy::size_type size_type;
		typedef typename _MycontTy::difference_type difference_type;
		typedef const_reference *pointer;
		typedef const_reference reference;

		_Vb_const_iterator()
			{	// construct with null reference
			}

		_Vb_const_iterator(const _Vbase *_Ptr)
			: _Vb_iter_base<_MycontTy>((_Vbase *)_Ptr)

			{	// construct with offset and pointer
			}

		const_reference operator*() const
			{	// return (reference to) designated object
			return (_Reft(*this));
			}

		_Vb_const_iterator<_MycontTy>& operator++()
			{	// preincrement
			_Inc();
			return (*this);
			}

		_Vb_const_iterator<_MycontTy> operator++(int)
			{	// postincrement
			_Vb_const_iterator<_MycontTy> _Tmp = *this;
			++*this;
			return (_Tmp);
			}

		_Vb_const_iterator<_MycontTy>& operator--()
			{	// predecrement
			_Dec();
			return (*this);
			}

		_Vb_const_iterator<_MycontTy> operator--(int)
			{	// postdecrement
			_Vb_const_iterator<_MycontTy> _Tmp = *this;
			--*this;
			return (_Tmp);
			}

		_Vb_const_iterator<_MycontTy>& operator+=(difference_type _Off)
			{	// increment by integer
			if (_Off == 0)
				return (*this); // early out


			if (_Off < 0 && this->_Myoff < 0 - (size_type)_Off)
				{	/* add negative increment */
				this->_Myoff += _Off;
				this->_Myptr -= 1 + ((size_type)(-1) - this->_Myoff) / _VBITS;
				this->_Myoff %= _VBITS;
				}
			else
				{	/* add non-negative increment */
				this->_Myoff += _Off;
				this->_Myptr += this->_Myoff / _VBITS;
				this->_Myoff %= _VBITS;
				}
			return (*this);
			}

		_Vb_const_iterator<_MycontTy> operator+(difference_type _Off) const
			{	// return this + integer
			_Vb_const_iterator<_MycontTy> _Tmp = *this;
			return (_Tmp += _Off);
			}

		_Vb_const_iterator<_MycontTy>& operator-=(difference_type _Off)
			{	// decrement by integer
			return (*this += -_Off);
			}

		_Vb_const_iterator<_MycontTy> operator-(difference_type _Off) const
			{	// return this - integer
			_Vb_const_iterator<_MycontTy> _Tmp = *this;
			return (_Tmp -= _Off);
			}

		difference_type operator-(
			const _Vb_const_iterator<_MycontTy>& _Right) const
			{	// return difference of iterators

			return (_VBITS * (this->_Myptr - _Right._Myptr)
				+ (difference_type)this->_Myoff
				- (difference_type)_Right._Myoff);
			}

		const_reference operator[](difference_type _Off) const
			{	// subscript
			return (*(*this + _Off));
			}

		bool operator==(const _Vb_const_iterator<_MycontTy>& _Right) const
			{	// test for iterator equality

		return (this->_Myptr == _Right._Myptr
				&& this->_Myoff == _Right._Myoff);
			}

		bool operator!=(const _Vb_const_iterator<_MycontTy>& _Right) const
			{	// test for iterator inequality
			return (!(*this == _Right));
			}

		bool operator<(const _Vb_const_iterator<_MycontTy>& _Right) const
			{	// test if this < _Right

			return (this->_Myptr < _Right._Myptr
				|| this->_Myptr == _Right._Myptr
					&& this->_Myoff < _Right._Myoff);
			}

		bool operator>(const _Vb_const_iterator<_MycontTy>& _Right) const
			{	// test if this > _Right
			return (_Right < *this);
			}

		bool operator<=(const _Vb_const_iterator<_MycontTy>& _Right) const
			{	// test if this <= _Right
			return (!(_Right < *this));
			}

		bool operator>=(const _Vb_const_iterator<_MycontTy>& _Right) const
			{	// test if this >= _Right
			return (!(*this < _Right));
			}

	protected:

		void _Dec()
			{	// decrement bit position
			if (this->_Myoff != 0)
				{
				--this->_Myoff;
				}
			else
				{
				--this->_Myptr;
				this->_Myoff = _VBITS - 1;
				}
			}

		void _Inc()
			{	// increment bit position
			if (this->_Myoff < _VBITS - 1)
				++this->_Myoff;
			else
				this->_Myoff = 0, ++this->_Myptr;
			}
		};

	template<class _MycontTy>
		_Vb_const_iterator<_MycontTy> operator+(
			typename _Vb_const_iterator<_MycontTy>::difference_type _Off,
			_Vb_const_iterator<_MycontTy> _Right)
			{	// return _Right + integer
			return (_Right += _Off);
			}

		// CLASS _Vb_iterator
	template<class _MycontTy>
		class _Vb_iterator
			: public _Vb_const_iterator<_MycontTy>
		{	// iterator for mutable Vector<bool>
	public:
		typedef _Vb_reference<_MycontTy> _Reft;
		typedef bool const_reference;

		typedef BOOL value_type;
		typedef typename _MycontTy::size_type size_type;
		typedef typename _MycontTy::difference_type difference_type;
		typedef _Reft *pointer;
		typedef _Reft reference;

		_Vb_iterator()
			{	// construct with null reference
			}

		_Vb_iterator( _Vbase *_Ptr)
			: _Vb_const_iterator<_MycontTy>(_Ptr)
			{	// construct with offset and pointer
			}

		reference operator*() const
			{	// return (reference to) designated object
			return (_Reft(*this));
			}

		_Vb_iterator<_MycontTy>& operator++()
			{	// preincrement
			++*(_Vb_const_iterator<_MycontTy> *)this;
			return (*this);
			}

		_Vb_iterator<_MycontTy> operator++(int)
			{	// postincrement
			_Vb_iterator<_MycontTy> _Tmp = *this;
			++*this;
			return (_Tmp);
			}

		_Vb_iterator<_MycontTy>& operator--()
			{	// predecrement
			--*(_Vb_const_iterator<_MycontTy> *)this;
			return (*this);
			}

		_Vb_iterator<_MycontTy> operator--(int)
			{	// postdecrement
			_Vb_iterator<_MycontTy> _Tmp = *this;
			--*this;
			return (_Tmp);
			}

		_Vb_iterator<_MycontTy>& operator+=(difference_type _Off)
			{	// increment by integer
			*(_Vb_const_iterator<_MycontTy> *)this += _Off;
			return (*this);
			}

		_Vb_iterator<_MycontTy> operator+(difference_type _Off) const
			{	// return this + integer
			_Vb_iterator<_MycontTy> _Tmp = *this;
			return (_Tmp += _Off);
			}

		_Vb_iterator<_MycontTy>& operator-=(difference_type _Off)
			{	// decrement by integer
			return (*this += -_Off);
			}

		_Vb_iterator<_MycontTy> operator-(difference_type _Off) const
			{	// return this - integer
			_Vb_iterator<_MycontTy> _Tmp = *this;
			return (_Tmp -= _Off);
			}

		difference_type operator-(const _Vb_const_iterator<_MycontTy>& _Right) const
			{	// return difference of iterators
			return (*(_Vb_const_iterator<_MycontTy> *)this - _Right);
			}

		reference operator[](difference_type _Off) const
			{	// subscript
			return (*(*this + _Off));
			}
		};

	template<class _MycontTy>
	_Vb_iterator<_MycontTy> operator+(typename _Vb_iterator<_MycontTy>::difference_type _Off,	_Vb_iterator<_MycontTy> _Right)
			{	// return _Right + integer
			return (_Right += _Off);
			}
}

#endif
