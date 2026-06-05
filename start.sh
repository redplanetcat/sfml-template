#!/usr/bin/env bash

GALLIUM_DRIVER=virpipe MESA_GL_VERSION_OVERRIDE=4.3 LD_LIBRARY_PATH="../SFML/lib" ./debug/main
