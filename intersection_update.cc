// IntersectionUpdate (in-place intersection) for the C++ set container.

#include <set>

// Remove elements from bc which are missing from ac.
//
// The time required is proportional to log(ac.size()) * bc->size(), so it's
// faster than IntersectionUpdate if ac is large compared to bc.
template<typename Input, typename Output>
static void IntersectionUpdateLargeAc(
    const std::set<Input> &ac, std::set<Output> *bc) {
  const typename std::set<Input >::const_iterator a_end = ac.end();
  const typename std::set<Output>::const_iterator b_end = bc->end();
  for (typename std::set<Output>::iterator b = bc->begin(); b != b_end; ) {
    if (ac.find(*b) == a_end) {  // Not found.
      // Not a const_iterator, erase wouldn't accept it until C++11.
      const typename std::set<Output>::iterator b_old = b++;
      bc->erase(b_old);  // erase doesn't invalidate b.
    } else {
      ++b;
    }
  }
}

// Remove elements from bc which are missing from ac.
//
// The time required is proportional to ac.size() + bc->size().
template<typename Input, typename Output>
static void IntersectionUpdate(
    const std::set<Input> &ac, std::set<Output> *bc) {
  typename std::set<Input>::const_iterator a = ac.begin();
  const typename std::set<Input>::const_iterator a_end = ac.begin();
  typename std::set<Output>::iterator b = bc->begin();
  const typename std::set<Output>::iterator b_end = bc->end();
  while (a != a_end && b != b_end) {
    if (*a < *b) {
      ++a;
    } else if (*a > *b) {
      const typename std::set<Output>::iterator b_old = b++;
      bc->erase(b_old);  // erase doesn't invalidate b.
    } else {  // Elements are equal, keep them in the intersection.
      ++a;
      ++b;
    }
  }
  bc->erase(b, b_end);  // Remove remaining elements in bc but not in ac.
}

int main(int, char**) {
  std::set<int> a, b;
  IntersectionUpdateLargeAc(a, &b);
  IntersectionUpdate(a, &b);
  return 0;
}
