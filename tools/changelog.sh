#!/usr/bin/env bash
#
# changelog.sh - prepend a dated entry to CHANGELOG.md
#
#   tools/changelog.sh "<message>" <board> [board...]
#   tools/changelog.sh "I2C clock-stretch fix (core)" dq8rly di16ac dq10rly
#
# It reads each board's CURRENT version straight from its board.h, so the
# changelog can't drift from what's actually in the source. Run it AFTER
# bumping. With no boards listed it applies to all boards (a core change).

set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$REPO_ROOT"

msg="${1:-}"
shift || true
[[ -n "$msg" ]] || { echo "usage: tools/changelog.sh \"<message>\" <board> [board...]" >&2; exit 2; }

boards=("$@")
if [[ ${#boards[@]} -eq 0 ]]; then
    mapfile -t boards < <(cd boards && ls -d */ | tr -d '/')
fi

version_of() { # version_of <board>
    local h="boards/$1/board.h" M m p
    M=$(grep -E '^#define[[:space:]]+FW_VERSION_MAJOR' "$h" | grep -oE '[0-9]+')
    m=$(grep -E '^#define[[:space:]]+FW_VERSION_MINOR' "$h" | grep -oE '[0-9]+')
    p=$(grep -E '^#define[[:space:]]+FW_VERSION_PATCH' "$h" | grep -oE '[0-9]+')
    echo "$M.$m.$p"
}

# Date: prefer git commit date if available for reproducibility, else today.
today="$(date +%Y-%m-%d 2>/dev/null || echo 'unreleased')"

entry="## ${today}  ${msg}"$'\n'
entry+="Applied to:"
for b in "${boards[@]}"; do
    entry+=" ${b} $(version_of "$b"),"
done
entry="${entry%,}"$'\n'

[[ -f CHANGELOG.md ]] || printf '# Changelog\n\n' > CHANGELOG.md

# Insert the new entry right after the "# Changelog" header.
tmp="$(mktemp)"
awk -v entry="$entry" '
    NR==1 && /^# Changelog/ { print; print ""; printf "%s\n", entry; next }
    { print }
' CHANGELOG.md > "$tmp"
mv "$tmp" CHANGELOG.md

echo "changelog updated:"
printf '%s\n' "$entry"
