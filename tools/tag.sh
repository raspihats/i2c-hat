#!/usr/bin/env bash
#
# tag.sh - create an annotated release tag for a board, named from its board.h
#
#   tools/tag.sh <board> [message] [commit]
#   tools/tag.sh di16ac
#   tools/tag.sh di16ac "regression 11/11 on the bench" abd541c
#
# Tag name: <board>-v<major.minor.patch>. The version is read from the board's
# board.h AT THE TAGGED COMMIT - the same single source of truth the firmware
# reports over I2C (GET_FIRMWARE_VERSION 0x11), so tag name and binary cannot
# drift. Default message: "<board> firmware <version>". Default commit: HEAD.

set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

board="${1:-}"
msg="${2:-}"
commit="${3:-HEAD}"

if [[ -z "$board" ]]; then
    echo "usage: tools/tag.sh <board> [message] [commit]" >&2
    echo "boards: $(cd "$REPO_ROOT/boards" && ls -d */ | tr -d '/' | tr '\n' ' ')" >&2
    exit 2
fi

hdr_content="$(git -C "$REPO_ROOT" show "$commit:boards/$board/board.h")" \
    || { echo "error: no boards/$board/board.h at $commit" >&2; exit 1; }

read_field() { # read_field MAJOR|MINOR|PATCH
    grep -E "^#define[[:space:]]+FW_VERSION_$1" <<<"$hdr_content" \
        | grep -oE '\(([0-9]+)\)' | tr -d '()'
}

major="$(read_field MAJOR)"
minor="$(read_field MINOR)"
patch="$(read_field PATCH)"

if [[ -z "$major" || -z "$minor" || -z "$patch" ]]; then
    echo "error: could not parse FW_VERSION_* from boards/$board/board.h at $commit" >&2
    exit 1
fi

version="$major.$minor.$patch"
tag="$board-v$version"
[[ -n "$msg" ]] || msg="$board firmware $version"

git -C "$REPO_ROOT" tag -a "$tag" -m "$msg" "$commit"
echo "tagged $(git -C "$REPO_ROOT" rev-parse --short "$commit"): $tag"
