# I2C-HAT firmware - thin wrapper over CMake.
#
# Everything here is a one-liner you could also type by hand; the Makefile just
# collects the commands so you don't have to remember them. See RELEASING.md.
#
#   make build BOARD=dq8rly            # configure + build one board
#   make build-all                     # build every board (the CI gate)
#   make bump BOARD=dq8rly KIND=patch  # bump that board's version
#   make release-core KIND=patch       # bump ALL boards + build them all
#   make changelog BOARD=dq8rly MSG="..."
#   make clean

BOARDS      := $(notdir $(wildcard boards/*))
BUILD_DIR   := build
TOOLCHAIN   := cmake/arm-none-eabi-toolchain.cmake
BUILD_TYPE  ?= Debug

.PHONY: help build build-all bump release-core changelog clean list

help:
	@echo "Boards: $(BOARDS)"
	@echo "Targets:"
	@echo "  make build BOARD=<name>            configure + build one board"
	@echo "  make build-all                     build every board"
	@echo "  make bump BOARD=<name> KIND=patch  bump version (patch|minor|major)"
	@echo "  make release-core KIND=patch       bump ALL boards then build all"
	@echo "  make changelog BOARD=<name> MSG=.. append a changelog entry"
	@echo "  make clean"

list:
	@echo $(BOARDS)

# --- build one board -------------------------------------------------------
build:
	@test -n "$(BOARD)" || { echo "set BOARD=<name>. boards: $(BOARDS)"; exit 2; }
	cmake -B $(BUILD_DIR)/$(BOARD) -G "Unix Makefiles" \
	      -DCMAKE_TOOLCHAIN_FILE=$(TOOLCHAIN) \
	      -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) \
	      -DBOARD=$(BOARD)
	cmake --build $(BUILD_DIR)/$(BOARD)

# --- build all boards (fails on the first that breaks) ---------------------
build-all:
	@set -e; for b in $(BOARDS); do \
	    echo "==== building $$b ===="; \
	    $(MAKE) --no-print-directory build BOARD=$$b; \
	done
	@echo "All boards built."

# --- version bump ----------------------------------------------------------
bump:
	@test -n "$(BOARD)" || { echo "set BOARD=<name>"; exit 2; }
	@test -n "$(KIND)"  || { echo "set KIND=patch|minor|major"; exit 2; }
	tools/bump.sh $(BOARD) $(KIND)

# --- core change: bump every board the same way, then build them all -------
release-core:
	@test -n "$(KIND)" || { echo "set KIND=patch|minor|major"; exit 2; }
	@for b in $(BOARDS); do tools/bump.sh $$b $(KIND); done
	@$(MAKE) --no-print-directory build-all
	@echo "All boards bumped ($(KIND)) and built. Review, run 'make changelog', commit, flash."

changelog:
	@test -n "$(BOARD)" || { echo "set BOARD=<name> (or edit CHANGELOG.md by hand)"; exit 2; }
	@test -n "$(MSG)"   || { echo 'set MSG="..."'; exit 2; }
	tools/changelog.sh "$(MSG)" $(BOARD)

clean:
	rm -rf $(BUILD_DIR)
