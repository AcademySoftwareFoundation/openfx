This directory tree contains a set of plugins and corresponding
guides which take you through the basics of the OFX
Image Effects Plugin API.

There are two sub-directories...
   - Code - which contains the example plugins source files,
   - Doc  - has a guide to each plugin.

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BUILDING THE PLUGINS

The plugins and support libs are set up to use cmake to generate and run builds.
See the top-level README.md in the repo, and use `scripts/build-cmake.sh`.

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BUILDING THE DOCUMENTATION

See the file Documentation/README, and use `Documentation/build.sh`.
The docs are also auto-generated on commit to the main branch on github.
