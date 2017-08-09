#ifndef _FLAGGED_H
#define _FLAGGED_H

#include <assert.h>

#include <vector>
#include <iostream>

#define FI __forceinline

namespace ts {

  class FlaggedT {
  public:
    FlaggedT() : m_flags(0) {}
    FI bool Flag(int i) const {
      return (m_flags & (0x00000001<<i))!=0;
    }
    FI void SetFlag(int i) {
      m_flags |= (0x00000001<<i);
    }
    FI void UnsetFlag(int i) {
      m_flags &= ~(0x00000001<<i);
    }
  private:
    unsigned m_flags;
  };

}

#endif