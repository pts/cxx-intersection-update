// IntersectionUpdate (in-place intersection) for sorted C++ containers.

#if __cplusplus >= 201103 || __GXX_EXPERIMENTAL_CXX0X__
#include <type_traits>
#endif

// Remove elements from bc which are missing from ac. Supported containers for 
// bc: list (only if sorted), vector (only if sorted), set, multiset. Supported
// containers for ac: set, multiset.
//
// The time required is proportional to log(ac.size()) * bc->size(), so it's
// faster than IntersectionUpdate if ac is large compared to bc.
template<typename Input, typename Output>
static void IntersectionUpdateLargeAc(const Input &ac, Output *bc) {
#if __cplusplus >= 201103 || __GXX_EXPERIMENTAL_CXX0X__
  // We could use std::is_convertible (both ways) instead of std::is_same.
  static_assert(std::is_same<typename Input::value_type,
                             typename Output::value_type>::value,
                "the containers passed to IntersectionUpdateLargeAc() need to "
                "have the same value_type");
#endif
  const typename Input::const_iterator a_end = ac.end();
  const typename Output::const_iterator b_end = bc->end();
  for (typename Output::iterator b = bc->begin(); b != b_end; ) {
    if (ac.find(*b) == a_end) {  // Not found.
      // Not a const_iterator, erase wouldn't accept it until C++11.
      const typename Output::iterator b_old = b++;
      bc->erase(b_old);  // erase doesn't invalidate b.
    } else {
      ++b;
    }
  }
}

// Remove elements from bc which are missing from ac. Supported containers for 
// ac and bc: list (only if sorted), vector (only if sorted), set, multiset.
template<typename Input, typename Output>
static void IntersectionUpdate(const Input &ac, Output *bc) {
#if __cplusplus >= 201103 || __GXX_EXPERIMENTAL_CXX0X__
  static_assert(std::is_same<typename Input::value_type,
                             typename Output::value_type>::value,
                "the containers passed to IntersectionUpdate() need to "
                "have the same value_type");
#endif
  typename Input::const_iterator a = ac.begin();
  const typename Input::const_iterator a_end = ac.end();
  typename Output::iterator b = bc->begin();
  // Can't be a const interator, similarly to b_old.
  const typename Output::iterator b_end = bc->end();
  while (a != a_end && b != b_end) {
    if (*a < *b) {
      ++a;
    } else if (*a > *b) {
      // This loop is not necessary, the next instance of the same value would
      // be removed from the multiset in the next iteration.
      //for (;;) {
      //  const typename Output::iterator b_old = b++;
      //  const bool do_break = b == b_end || *b_old != *b;
      //  bc->erase(b_old);  // erase doesn't invalidate b.
      //  if (do_break) break;
      //}
      const typename Output::iterator b_old = b++;
      bc->erase(b_old);  // erase doesn't invalidate b.
    } else {  // Elements are equal, keep it in the intersection.
      // Don't do ++a, in case ac is a multiset.
      ++b;
    }
  }
  bc->erase(b, b_end);  // Remove remaining elements in bc but not in ac.
}

#include <vector>

#if __cplusplus >= 201103 || __GXX_EXPERIMENTAL_CXX0X__
#include <algorithm>  // std::swap.
#endif

// Template specialization for vector output.
template<typename Input, typename T>
static void IntersectionUpdate(const Input &ac, std::vector<T> *bc) {
#if __cplusplus >= 201103 || __GXX_EXPERIMENTAL_CXX0X__
  static_assert(std::is_same<typename Input::value_type, T>::value,
                "the containers passed to IntersectionUpdate() need to "
                "have the same value_type");
#endif
  typename Input::const_iterator a = ac.begin();
  const typename Input::const_iterator a_end = ac.end();
  typename std::vector<T>::iterator b = bc->begin();
  // Elements between b_high an bc->end() will be removed (erased) right before
  // the function returns. We defer their removal to save time.
  typename std::vector<T>::iterator b_high = bc->end();
  while (a != a_end && b != b_high) {
    if (*a < *b) {
      ++a;
    } else if (*a > *b) {
      std::iter_swap(b, --b_high);  // Works even if swapping with itself.
    } else {  // Elements are equal, keep them in the intersection.
      ++a;
      ++b;
    }
  }
  bc->erase(b, bc->end());  // Remove remaining elements in bc but not in ac.
}

#include <list>
#include <set>
#include <vector>

int main(int, char**) {
  std::set<int> s;
  std::multiset<int> ms;
  std::vector<int> v;
  // std::list<unsigned> l;  // Won't work in C++0x and C++11.
  std::list<int> l;
  IntersectionUpdate(s, &ms);
  IntersectionUpdate(ms, &v);
  IntersectionUpdate(v, &l);
  IntersectionUpdate(l, &s);
  IntersectionUpdateLargeAc(s, &ms);
  IntersectionUpdateLargeAc(ms, &v);
  // IntersectionUpdateLargeAc(v, &l);  // v is not good as ac.
  // IntersectionUpdateLargeAc(l, &s);  // l is not good as ac.
  IntersectionUpdateLargeAc(s, &l);
  IntersectionUpdateLargeAc(ms, &s);
  return 0;
}
