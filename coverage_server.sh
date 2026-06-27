#!/bin/sh
mkdir -p coverage_report
gcovr --gcov-executable "llvm-cov gcov" -r . -f 'server/src/' \
      --txt --html-details coverage_report/index.html