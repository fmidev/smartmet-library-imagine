# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

Full developer documentation: `docs/developer-guide.md`.

`smartmet-library-imagine` is a 2D graphics rendering library (`libsmartmet-imagine.so`) used by SmartMet Server rendering tools (notably `qdcontour`). It lives in the `Imagine` namespace. The successor library is `imagine2`, but this one is still actively maintained.

## Build commands

```bash
make                # Build libsmartmet-imagine.so
make test           # Build and run all tests in test/
make format         # Run clang-format on all source and test files
make clean          # Clean build artifacts
make rpm            # Build RPM package
make install        # Install headers to smartmet/imagine/ and .so to libdir
```

To run a single test:
```bash
cd test && make NFmiBezierToolsTest && ./NFmiBezierToolsTest
```

Tests link against the locally-built `../libsmartmet-imagine.so`, not the system-installed version.

## Dependencies

Build requires: `macgyver`, `newbase` (SmartMet libraries), GDAL, fmt 12.x, FreeType2, libjpeg, libpng, zlib, Boost (regex, thread).

`REQUIRES = gdal fmt` in the Makefile drives pkg-config resolution.

## Architecture

### Rendering backend

The library has a single rendering backend: the self-made rasteriser drawing into `NFmiImage`. The never-enabled Cairo backend (`IMAGINE_WITH_CAIRO`, `ImagineXr`, `imagine-config.h`) was removed in 26.10.13; Cairo rendering is in imagine2.

### Core class hierarchy

`NFmiDrawable` is the abstract base for objects that can fill/stroke onto an `NFmiImage`. Key inheritors:

- **`NFmiImage`** -- RGBA pixel buffer with JPEG/PNG/PNM/PGM/GIF read/write, compositing, and basic line drawing. Colors are `int` (0xAARRGGBB format, alpha 0-127 where 0=opaque).
- **`NFmiPath`** -- PostScript-style path (deque of `NFmiPathElement`). Supports moveto/lineto/ghostlineto/conicto/cubicto, affine transforms, projection, SVG output, Douglas-Peucker simplification, clipping, and Pacific/Atlantic view conversion.
- **`NFmiEdgeTree`** -- Balanced binary tree of unique edges, used for contour polygon assembly. Duplicate edges (shared between adjacent cells) cancel out.
- **`NFmiContourTree`** (extends `NFmiEdgeTree`) -- Contours 2D data matrices into polygons using linear, nearest-neighbor, or discrete interpolation.
- **`NFmiGeoShape`** -- Reads ESRI shapefiles and projects/renders them.

### Subsystems

- **Contouring**: `NFmiContourTree` + `NFmiEdgeTree` + `NFmiEdge` + `NFmiDataHints`. Grid cells are decomposed into triangles/rectangles; edges are deduplicated in a set; remaining edges are assembled into closed polygons via `Path()`.
- **ESRI Shapefile I/O**: `NFmiEsriShape` + `NFmiEsriElement` subclasses (Point, PolyLine, Polygon, with M/Z variants, MultiPatch) + `NFmiEsriBox` + `NFmiEsriBuffer` + `NFmiEsriTools`. Full read/write of .shp/.shx/.dbf files including dBASE attribute tables.
- **Color blending**: `NFmiColorTools` (color construction/decomposition, blend rule dispatch) + `NFmiColorBlend.h` (Porter-Duff and other blend rule template structs). Blend rules are resolved at compile time via `Blend2Type` template dispatch.
- **Bezier fitting**: `NFmiApproximateBezierFit`, `NFmiCardinalBezierFit`, `NFmiTightBezierFit`, `NFmiBezierTools` -- different curve-fitting strategies for smoothing paths.
- **Image I/O**: Built into `NFmiImage`. Format support controlled by `IMAGINE_FORMAT_JPEG` / `IMAGINE_FORMAT_PNG` defines (both enabled by default). `NFmiColorReduce` handles palette reduction for GIF/PNG output.
- **Text rendering**: `NFmiFreeType` (singleton) renders TrueType fonts onto `NFmiImage` via FreeType2. Unix-only.
- **Geometry**: `NFmiAffine` (2D affine transforms), `NFmiAlignment` (anchor point alignment), `NFmiFillMap` (scanline fill), `NFmiSimplifier` (Douglas-Peucker), `NFmiNearTree` (spatial nearest-neighbor search).
