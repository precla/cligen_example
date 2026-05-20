#!/bin/bash

# get $1 from 'uname -m' output on the target board

# For a full static build get $2 from
# 'zig targets | awk '/^\s*\.libc = \./,/^\s*\},/' | sed '1d;$d' | grep musl'

# For a build with dynamically linked glibc with a specific version get $2 from
# zig targets | awk '/^\s*\.libc = \./,/^\s*\},/' | sed '1d;$d'` | grep gnu
# +
# zig targets | awk '/^\s*\.glibc = \./,/^\s*\},/' | sed '1d;$d'

CURRENT_DIRECTROY="$(cd "$(dirname "$0")" && pwd)"

cmake \
-DCMAKE_SYSTEM_NAME="Linux" \
-DCMAKE_SYSTEM_PROCESSOR="$1" \
-DCMAKE_C_COMPILER="${CURRENT_DIRECTROY}/zig-cc.sh" \
-DCMAKE_C_FLAGS_INIT="-target $2" \
-DCMAKE_AR="${CURRENT_DIRECTROY}/zig-ar.sh" \
-DCMAKE_RANLIB="${CURRENT_DIRECTROY}/zig-ranlib.sh" \
-DCMAKE_LINK_DEPENDS_USE_LINKER=FALSE \
-DCLIGEN_DEPENDENCIES=ON \
${@:3}
