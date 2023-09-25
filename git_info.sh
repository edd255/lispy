#!/bin/bash
COMMIT_HASH=$(git rev-parse --short HEAD)
BRANCH_NAME=$(git rev-parse --abbrev-ref HEAD)
rm --force src/utils/git_info.h
echo "#pragma once" > src/utils/git_info.h
echo "" >> src/utils/git_info.h
echo "#ifndef LISPY_UTILS_GIT_INFO" >> src/utils/git_info.h
echo "#define LISPY_UTILS_GIT_INFO" >> src/utils/git_info.h
echo "" >> src/utils/git_info.h
echo "#define LISPY_GIT_COMMIT_HASH \"$COMMIT_HASH\"" >> src/utils/git_info.h
echo "#define LISPY_GIT_BRANCH_NAME \"$BRANCH_NAME\"" >> src/utils/git_info.h
echo "" >> src/utils/git_info.h
echo "#endif" >> src/utils/git_info.h
