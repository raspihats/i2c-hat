#!/usr/bin/env bash
#
# bump.sh - bump a board's firmware version in its board.h
#
#   tools/bump.sh <board> <patch|minor|major>
#   tools/bump.sh dq8rly patch
#
# SemVer rules (see RELEASING.md):
#   patch  x.y.z -> x.y.(z+1)   bug fix, host interface unchanged
#   minor  x.y.z -> x.(y+1).0   additive command / capability
#   major  x.y.z -> (x+1).0.0   breaking change to the I2C command set
#
# Nothing clever here on purpose: it just rewrites the three FW_VERSION_* lines
# so you can always do the same edit by hand.

set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

board="${1:-}"
kind="${2:-}"

if [[ -z "$board" || -z "$kind" ]]; then
    echo "usage: tools/bump.sh <board> <patch|minor|major>" >&2
    echo "boards: $(cd "$REPO_ROOT/boards" && ls -d */ | tr -d '/' | tr '\n' ' ')" >&2
    exit 2
fi

hdr="$REPO_ROOT/boards/$board/board.h"
[[ -f "$hdr" ]] || { echo "error: no such board.h: $hdr" >&2; exit 1; }

read_field() { # read_field MAJOR|MINOR|PATCH
    grep -E "^#define[[:space:]]+FW_VERSION_$1" "$hdr" \
        | grep -oE '\(([0-9]+)\)' | tr -d '()'
}

major="$(read_field MAJOR)"
minor="$(read_field MINOR)"
patch="$(read_field PATCH)"

if [[ -z "$major" || -z "$minor" || -z "$patch" ]]; then
    echo "error: could not parse FW_VERSION_* from $hdr" >&2
    exit 1
fi

old="$major.$minor.$patch"
case "$kind" in
    major) major=$((major + 1)); minor=0; patch=0 ;;
    minor) minor=$((minor + 1)); patch=0 ;;
    patch) patch=$((patch + 1)) ;;
    *) echo "error: kind must be patch|minor|major (got '$kind')" >&2; exit 2 ;;
esac
new="$major.$minor.$patch"

set_field() { # set_field MAJOR value
    sed -i -E "s/^(#define[[:space:]]+FW_VERSION_$1[[:space:]]+)\([0-9]+\)/\1($2)/" "$hdr"
}
set_field MAJOR "$major"
set_field MINOR "$minor"
set_field PATCH "$patch"

echo "$board: $old -> $new ($kind)"
