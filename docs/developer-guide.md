# imagine developer guide

This guide is for developers who change `smartmet-library-imagine`. imagine is FMI's older 2D
graphics library: paths and their projection, contouring, ESRI shapefiles, colour blending,
text and image files. It is used by command-line tools, not by the server: `qdcontour`, `shapetools` and `qdtools`.

[imagine2](https://github.com/fmidev/smartmet-library-imagine2) is the Cairo-based variant of the same code, used by `qdcontour2`. The two have the same classes, but their sources have diverged.

[CLAUDE.md](../CLAUDE.md) has the class overview.

## Contents

1. [Building and testing](#1-building-and-testing)
2. [Rendering backend](#2-rendering-backend)
3. [Colours and images](#3-colours-and-images)
4. [Paths](#4-paths)
5. [Contouring](#5-contouring)
6. [Shapefiles and coastlines](#6-shapefiles-and-coastlines)
7. [Text](#7-text)
8. [Compatibility](#8-compatibility)
9. [Known pitfalls](#9-known-pitfalls)

---

## 1. Building and testing

```bash
make
make test
make -C test NFmiBezierToolsTest && ./test/NFmiBezierToolsTest
```

The tests (`regression/tframe.h`) cover only the Bezier fitting, the counter and the data
hints. Drawing, contouring and image output are tested through those tools: compare their
output images before and after a change.

## 2. Rendering backend

`imagine-config.h` decides the backend, and must not be overridden from a Makefile:
in imagine, `IMAGINE_WITH_CAIRO` is **not** defined, so drawing goes to `NFmiImage` with the library's own rasteriser (`NFmiDrawable`, `NFmiFillMap`).

`ImagineXr_or_NFmiImage` is the type that drawing functions take in the active build.
Code for the other backend stays under `#ifdef IMAGINE_WITH_CAIRO`; keep both branches
compiling, since the Windows workstation build uses them too.

## 3. Colours and images

* A colour is an `int` `0xAARRGGBB` in which **alpha is 0…127 and 0 means opaque**
  (`NFmiColorTools::Opaque`, `Transparent`, `MaxAlpha`), as in the GD library. Build colours
  with `NFmiColorTools::MakeColor(r, g, b, a)`.
* `NFmiColorTools` implements the blending rules (Porter-Duff and others, about 20); the
  blend functions are templates selected per rule at compile time (`NFmiColorBlend.h`).
* `NFmiImage` is an RGBA pixel buffer that reads and writes PNG, JPEG, GIF, PNM and PGM,
  and composites images; `NFmiColorReduce` reduces colours for palette output.

## 4. Paths

`NFmiPath` is a PostScript-style path of `moveto`, `lineto`, `ghostlineto` (an invisible
segment, used to connect the parts of clipped shapes), `conicto` and `cubicto` elements.

* `Project(area)` converts lon/lat coordinates to the image coordinates of a newbase
  `NFmiArea`; `InvProject()` does the reverse.
* `Clip()`, `Simplify()` (Douglas–Peucker), `PacificView()`, affine transformations,
  Bezier smoothing, and `SVG()` output.
* `Stroke()` and `Fill()` draw the path onto the image.

## 5. Contouring

`NFmiContourTree` contours a data matrix into polygons for a range `[lolimit, hilimit]`
(each end open, exact or infinite), with linear, nearest-neighbour or discrete
interpolation. Each cell is split into triangles or handled as a rectangle; the resulting
edges are stored in an `NFmiEdgeTree`, where an edge that appears twice (shared by two
cells) cancels out, and the remaining edges are joined into closed paths. `NFmiDataHints`
speeds up finding the cells that can contain a given range.

The server does not use this contourer: it uses the trax library, which is faster and
handles more special cases.

## 6. Shapefiles and coastlines

`NFmiEsriShape` reads and writes ESRI shapefiles (`.shp`, `.shx`, `.dbf`), with all element
types (points, polylines, polygons, their M and Z variants, multipatches) and the dBASE
attributes. `NFmiGeoShape` projects and draws shapes, and `NFmiGshhsTools` reads GSHHS
coastline data.

## 7. Text

`NFmiFreeType::Instance()` renders text with FreeType onto images; `NFmiFace` holds a
font face and its size. The text is UTF-8. The FreeType library is initialised when the
instance is first used.

## 8. Compatibility

The headers are installed under `smartmet/imagine/` and used by `qdcontour`, `shapetools` and `qdtools`. Many classes have virtual
methods and inline code, so a change requires rebuilding those tools. The library builds
on Windows as well (`make.cmd`, `CMakeLists.txt`).

## 9. Known pitfalls

* **Alpha 0 is opaque** and 127 transparent (§3), the opposite of the usual convention.
* **The two libraries have diverged.** imagine and imagine2 have the same classes but the
  sources differ; for example imagine wraps its functions in `Fmi::Exception` handlers, imagine2 does not; a fix in one is not automatically in the other. Port fixes to both when both are affected.
* **Only a few classes have tests** (§1).
