// Assert.C

#include "Assert.H"
#include "App.H"

#include <stdio.h>
#include <stdlib.h>

#define ASSERT_BACKTRACE

#ifdef ASSERT_BACKTRACE

#include <execinfo.h>
#include <cxxabi.h>

void eln_backtrace(int skip) {
  void *backtrace_data[1024];
  int backtrace_count = backtrace(backtrace_data, 1024);
  char **symbols = backtrace_symbols(backtrace_data, backtrace_count);

  fprintf(stderr, "Backtrace:\n");
  size_t dm_length = 1024;
  char demangled[1024];
  for (int k=skip+1; k<backtrace_count; k++) {
    int status;
    char *begin_name=0, *begin_offset=0, *end_offset=0;
    for (char *p=symbols[k]; *p; ++p) {
      if (*p=='(')
	begin_name=p;
      else if (*p=='+')
	begin_offset=p;
      else if (*p==')' && begin_offset && !end_offset)
	end_offset=p;
    }
    if (begin_name && begin_offset && end_offset
	&& begin_name<begin_offset) {
      *begin_name++ = *begin_offset++ = *end_offset = 0;
      abi::__cxa_demangle(begin_name, demangled,
			  &dm_length,&status);
      if (status==0) {
	fprintf(stderr, "  %s +%s\n", demangled, begin_offset);
      } else {
	*--begin_name='(';
	*--begin_offset='+';
	*end_offset=')';
	fprintf(stderr, "  %s (n.d.)\n", symbols[k]);
      }
    } else {
      fprintf(stderr, "  %s (n.d.)\n", symbols[k]);
    }
  }

  free(symbols);
}

#else
void eln_backtrace(int skip) {
  fprintf(stderr, "Backtrace info not available.\n");
}
#endif

void eln_assert(char const *assertion, char const *file, int line) {
  static bool prior_failure = false;

  eln_backtrace(1);
  fprintf(stderr, "\n");
  fprintf(stderr, "Assertion \"%s\" failed in file \"%s\" line %i.\n",
	  assertion, file, line);

  if (prior_failure) {
    fprintf(stderr, "Assertion failed while quitting. Terminating hard.\n");
    exit(2);
  } else {
    prior_failure = true;
    App::quit();
    fprintf(stderr, "Application shut down OK. Terminating.\n");
    exit(1);
  }
}

