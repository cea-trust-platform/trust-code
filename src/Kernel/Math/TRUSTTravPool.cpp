/****************************************************************************
* Copyright (c) 2024, CEA
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
* 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
* 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*****************************************************************************/

#include <TRUSTTravPool.h>
#include <unordered_map>
#include <list>
#include <iostream>
#include <cassert>
#include <Process.h>
#include <EntreeSortie.h>

/*! The shared pools of memory - visibility: here only.
 *
 * Implementation is done as a map of lists. Key is the array size, and the list simply contains pointers to
 * available free blocks. So a block is only registered in the list when it is released.
 *
 */
template<typename _TYPE_>
struct PoolImpl_
{
  using ptr_t = std::shared_ptr<std::vector<_TYPE_>>;
  using list_t = std::list<ptr_t>;
  using pool_t = std::unordered_map<size_t, list_t>;

  /*! A pool is a map of lists:
  *   - key is the size of the block,
  *   - value is a list giving all free blocks (the last one is picked when requesting a free block)
  */
  static pool_t Free_blocks_;

#ifndef NDEBUG
  //! Total allocation requests:
  static size_t req_sz_;
  //! Actual alloc performed:
  static size_t actual_sz_;
#endif
};

//
// Instanciations of the static pools themselves (protect your eyes, it hurts :-) )
//
template<> PoolImpl_<int>::pool_t    PoolImpl_<int>::Free_blocks_    = PoolImpl_<int>::pool_t();
template<> PoolImpl_<float>::pool_t  PoolImpl_<float>::Free_blocks_  = PoolImpl_<float>::pool_t();
template<> PoolImpl_<double>::pool_t PoolImpl_<double>::Free_blocks_ = PoolImpl_<double>::pool_t();

#ifndef NDEBUG
template<> size_t PoolImpl_<int>::req_sz_ = 0;
template<> size_t PoolImpl_<float>::req_sz_ = 0;
template<> size_t PoolImpl_<double>::req_sz_ = 0;

template<> size_t PoolImpl_<int>::actual_sz_ = 0;
template<> size_t PoolImpl_<float>::actual_sz_ = 0;
template<> size_t PoolImpl_<double>::actual_sz_ = 0;
#endif


/*! Handy method to get the proper list corresponding to a given size.
 * The list is created if this is the first time the corresponding key (=size) is encountered
 */
template<typename _TYPE_>
typename PoolImpl_<_TYPE_>::list_t& GetOrCreateList(size_t sz)
{
  using lst_t = typename PoolImpl_<_TYPE_>::list_t;
  auto& ze_pool = PoolImpl_<_TYPE_>::Free_blocks_;

  auto it = ze_pool.find(sz);
  if (it == ze_pool.end())
    {
      auto pr = ze_pool.emplace(sz, lst_t());
      // emplace returns a pair { iterator, bool } - the iterator itself is a pair (since this is a map) whose 'second' is the value.
      return pr.first->second;
    }
  else
    return it->second;
}


/*! Retrieve a free block of size sz.
 *
 * This takes the last available block from the list of the corresponding size, or returns a newly allocated block if none is available in the
 * block.
 */
template<typename _TYPE_>
typename TRUSTTravPool<_TYPE_>::block_ptr_t TRUSTTravPool<_TYPE_>::GetFreeBlock(int sz)
{
  using vec_t = std::vector<_TYPE_>;
  using ptr_t = typename PoolImpl_<_TYPE_>::ptr_t;
  using lst_t = typename PoolImpl_<_TYPE_>::list_t;

#ifndef NDEBUG
  PoolImpl_<_TYPE_>::req_sz_ += sz;
#endif

  lst_t& lst = GetOrCreateList<_TYPE_>(sz);
  // Is there an available block?
  if (lst.size())
    {
      // Yes - pop it from the list and returns it:
      ptr_t ret = lst.back();
      lst.pop_back();
      return ret;
    }
  else // No, must create a new one - it will be registered in the pool when released in ~TRUSTArray()
    {
#ifndef NDEBUG
      PoolImpl_<_TYPE_>::actual_sz_ += sz;
#endif
      return std::make_shared<vec_t>(vec_t(sz));
    }
}

/*! "Resize" a temporary Trav block - two possible strategies:
 *  Strategy 1
 *    - get a new free block of the new size
 *    - copy the former data into it
 *    - release the former small block (and so make it available for future potential use by another Trav)
 *  Strategy 2
 *    - simply resize the underlying vector, hence completely forgetting the previous block which is never registered in the pool.
 *
 *  Strategy 2 is retained, since in part of the code (e.g. Assembleur PolyMAC) we have aggressive 'append_line' usage which
 *  leads to a clottering of the pool with blocks of intermediate sizes if using strategy 1.
 */
template<typename _TYPE_>
typename TRUSTTravPool<_TYPE_>::block_ptr_t TRUSTTravPool<_TYPE_>::ResizeBlock(typename TRUSTTravPool<_TYPE_>::block_ptr_t p, int new_sz)
{
  assert(p != nullptr);
  assert(p->size() > 0);
  assert(new_sz > 0);  // new_sz == 0 should never happen, see TRUSTArray::resize_array_()

  // Resize ... and that's it!
  p->resize(new_sz);

// Strategy 1 - to be avoided!! See comments above.
//  // Get new bigger block
//  block_ptr_t new_blk = GetFreeBlock_impl_<_TYPE_>(new_sz);
//  // Copy data
//  std::copy(p->begin(), p->end(), new_blk->begin());
//  // Release small block
//  ReleaseBlock_impl_<_TYPE_>(p);

  return p;
}

/*! Release a block.
 *
 * This is invoked from the dtor of TRUSTArray and makes the memory block available again by registering it in the pool of
 * free blocks.
 * We don't register blocks of size 0.
 */
template<typename _TYPE_>
void TRUSTTravPool<_TYPE_>::ReleaseBlock(typename TRUSTTravPool<_TYPE_>::block_ptr_t p)
{
  using lst_t = typename PoolImpl_<_TYPE_>::list_t;

  assert(p != nullptr);
  size_t sz = p->size();

  if (sz)
    {
      lst_t& lst = GetOrCreateList<_TYPE_>(sz);
      // Append the free block pointer to the list corresponding to its size:
      lst.push_back(p);
    }
}

/*!
 * Debug method printing useful stats.
 */
template<typename _TYPE_>
void TRUSTTravPool<_TYPE_>::PrintStats()
{
#ifdef NDEBUG
  Cerr << "TRUSTTravPool stats are only available in debug mode for performance reasons!!" << finl;
  Process::exit(-1);
#else
  using pi = PoolImpl_<_TYPE_>;

  Cerr << "Total requested  (MB): " << (double)pi::req_sz_ / (1024.0 * 1024.0) << finl;
  Cerr << "Total allocated  (MB): " << (double)pi::actual_sz_ / (1024.0 * 1024.0) << finl;
//  std::cout << "-- Top five blocks:" << std::endl;
//  std::cout << "   Size" << std::setw(40) << "Nb instances" << std::finl;
//  int cnt = 0;
//  for (auto& )
//    {
//      if(++cnt >= 5) break;
//
//    }
#endif
}

//
// Explicit instanciations of templates
//
template class TRUSTTravPool<double>;
template class TRUSTTravPool<int>;
template class TRUSTTravPool<float>;
