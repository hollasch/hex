hex - Change Log
====================================================================================================

# v1.1.1 (In Progress)

### Changed
  - Tweaked format of version line


----------------------------------------------------------------------------------------------------
# v1.1.0  (2020-10-03)

### Deleted
  - Removed options `/h` and `-help`

### Added
  - Added long option forms `--help`, `--byte`, `--word`, `--longword`, `--quadword`, `--octword`,
    `--compact`, `--start`, and `--end`
  - Added new option `--version`

### Changed
  - Various refactorings
  - Converted project to CMake


----------------------------------------------------------------------------------------------------
# v1.0.1  (2018-06-24)

### Changed
  - Retarget Windows SDK 10.0.17134.0
  - Tweaked help text


----------------------------------------------------------------------------------------------------
# v1.0.0  (2016-10-16)

First formal release

  - Supports grouping options -b, -w, -l, -q and -o.
  - Supports -c option to compact duplicate lines.
  - Supports -s (start) and -e (end) address options.
