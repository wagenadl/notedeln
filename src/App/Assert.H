// Assert.H

#ifndef ASSERT_H

#define ASSERT_H

#include <QObject>

void eln_backtrace(int skip=0);

void eln_assert(char const *assertion, char const *file, int line);

inline void eln_noop() { }

#define ASSERT(cond) ((!(cond)) ? eln_assert(#cond, __FILE__, __LINE__) : eln_noop())

#define ASSERTX(cond, str) ((!(cond)) ? eln_assert(str, __FILE__, __LINE__) : eln_noop())

#endif
