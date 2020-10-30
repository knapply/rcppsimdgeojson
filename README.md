`{rcppsimdgeojson}`
================

  - [Installation](#installation)
  - [Conformance](#conformance)
  - [Benchmarking](#benchmarking)

<!-- README.Rmd generates README.md. -->

<!-- badges: start -->

[![GitHub last
commit](https://img.shields.io/github/last-commit/knapply/rcppsimdgeojson.svg)](https://github.com/knapply/rcppsimdgeojson/commits/master)
[![Lifecycle](https://img.shields.io/badge/lifecycle-experimental-orange.svg)](https://www.tidyverse.org/lifecycle/#experimental)
[![R build
status](https://github.com/knapply/rcppsimdgeojson/workflows/R-CMD-check/badge.svg)](https://github.com/knapply/rcppsimdgeojson/actions?workflow=R-CMD-check)
[![Codecov test
coverage](https://codecov.io/gh/knapply/rcppsimdgeojson/branch/master/graph/badge.svg)](https://codecov.io/gh/knapply/rcppsimdgeojson?branch=master)
[![License: GPL
v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Depends-R](https://img.shields.io/badge/Depends-GNU_R%20%3E=%204.0-blue.svg)](https://www.r-project.org/)
[![Depends-C++](https://img.shields.io/badge/Depends-C++17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![Depends-GCC](https://img.shields.io/badge/Depends-GCC%20%3E=%207-red.svg)](https://gcc.gnu.org/)
[![Depends-CLang](https://img.shields.io/badge/Depends-Clang%20%3E=%207-red.svg)](https://clang.llvm.org/)
[![Depends-RTools](https://img.shields.io/badge/Depends-RTools%20%3E=%2040-red.svg)](https://cran.r-project.org/bin/windows/Rtools/)

<!-- badges: end -->

***WIP: This is still not useful…***

## Installation

``` r
if (!requireNamespace("remotes", quietly = TRUE)) install.packages("remotes")
remotes::install_github("knapply/rcppsimdgeojson")
```

``` r
geojson_files <- list.files(system.file("geojsonexamples", package = "rcppsimdgeojson"),
                            recursive = TRUE, pattern = "\\.geojson$", full.names = TRUE)
geojson_files <- geojson_files[!grepl("largeMixedTest", geojson_files, fixed = TRUE)]

names(geojson_files) <- basename(geojson_files)
```

## Conformance

``` r
identical_w_sf <- vapply(
  geojson_files, function(.file) {
  identical(rcppsimdgeojson:::.fload_sfc(.file), sf::read_sf(.file)$geometry)
  }, 
  logical(1L)
)

stopifnot(all(identical_w_sf))
as.matrix(identical_w_sf)
```

    #>                            [,1]
    #> featureCollection.geojson  TRUE
    #> holeyMultiPolygon.geojson  TRUE
    #> holeyPolygon.geojson       TRUE
    #> lineString.geojson         TRUE
    #> multiLineString.geojson    TRUE
    #> multiPoint.geojson         TRUE
    #> multiPolygon.geojson       TRUE
    #> point.geojson              TRUE
    #> polygon.geojson            TRUE
    #> GeometryCollection.geojson TRUE

``` r
big_geojson <- path.expand("~/Downloads/bay_delta_89b.geojson")

test <- rcppsimdgeojson:::.fload_sfc(big_geojson)
target <- sf::read_sf(big_geojson)$geometry
stopifnot(identical(test, target))

test
```

    #> Geometry set for 258032 features 
    #> geometry type:  MULTIPOLYGON
    #> dimension:      XY
    #> bbox:           xmin: -123.0052 ymin: 37.40251 xmax: -121.2558 ymax: 38.80345
    #> CRS:            4326
    #> First 5 geometries:

    #> MULTIPOLYGON (((-121.7215 38.80249, -121.7211 3...

    #> MULTIPOLYGON (((-121.7215 38.80257, -121.7211 3...

    #> MULTIPOLYGON (((-121.7215 38.80234, -121.7211 3...

    #> MULTIPOLYGON (((-121.7211 38.80249, -121.7207 3...

    #> MULTIPOLYGON (((-121.7211 38.80255, -121.7207 3...

``` r
plot(test)
```

<img src="man/figures/unnamed-chunk-4-1.png" style="display: block; margin: auto;" />

## Benchmarking

### `<sfc>`

#### String

``` r
geojson_strings <- vapply(geojson_files, function(.file) {
  readChar(.file, nchars = file.size(.file))
}, character(1L))


lapply(geojson_strings, function(.geojson) {
  microbenchmark::microbenchmark(
    rcppsimdgeojson = rcppsimdgeojson:::.fparse_sfc(.geojson),
    geojsonsf = geojsonsf:::rcpp_geojson_to_sfc(.geojson, expand_geometries = FALSE),
    unit = "relative"
  )
})
```

    #> $featureCollection.geojson
    #> Unit: relative
    #>             expr      min       lq     mean   median       uq      max neval
    #>  rcppsimdgeojson 1.000000 1.000000 1.000000 1.000000 1.000000  1.00000   100
    #>        geojsonsf 1.708294 1.706211 3.750529 1.582346 1.573242 55.20384   100
    #> 
    #> $holeyMultiPolygon.geojson
    #> Unit: relative
    #>             expr      min       lq     mean median       uq       max neval
    #>  rcppsimdgeojson 1.000000 1.000000 1.000000 1.0000 1.000000 1.0000000   100
    #>        geojsonsf 1.390692 1.366719 1.266721 1.3485 1.331571 0.7537005   100
    #> 
    #> $holeyPolygon.geojson
    #> Unit: relative
    #>             expr      min       lq     mean   median       uq       max neval
    #>  rcppsimdgeojson 1.000000 1.000000 1.000000 1.000000 1.000000 1.0000000   100
    #>        geojsonsf 1.392627 1.393801 1.333468 1.367604 1.368263 0.5311835   100
    #> 
    #> $lineString.geojson
    #> Unit: relative
    #>             expr      min       lq     mean   median       uq       max neval
    #>  rcppsimdgeojson 1.000000 1.000000 1.000000 1.000000 1.000000 1.0000000   100
    #>        geojsonsf 1.346955 1.318726 1.270599 1.283767 1.286882 0.6452351   100
    #> 
    #> $multiLineString.geojson
    #> Unit: relative
    #>             expr      min       lq     mean   median       uq      max neval
    #>  rcppsimdgeojson 1.000000 1.000000 1.000000 1.000000 1.000000 1.000000   100
    #>        geojsonsf 1.274656 1.290239 1.293692 1.285171 1.297341 2.097094   100
    #> 
    #> $multiPoint.geojson
    #> Unit: relative
    #>             expr      min       lq     mean   median       uq       max neval
    #>  rcppsimdgeojson 1.000000 1.000000 1.000000 1.000000 1.000000 1.0000000   100
    #>        geojsonsf 1.341292 1.349313 1.265935 1.295201 1.286315 0.9176263   100
    #> 
    #> $multiPolygon.geojson
    #> Unit: relative
    #>             expr      min       lq     mean   median      uq      max neval
    #>  rcppsimdgeojson 1.000000 1.000000 1.000000 1.000000 1.00000 1.000000   100
    #>        geojsonsf 1.331286 1.344475 1.275695 1.312075 1.30321 1.804065   100
    #> 
    #> $point.geojson
    #> Unit: relative
    #>             expr      min       lq     mean   median       uq      max neval
    #>  rcppsimdgeojson 1.000000 1.000000 1.000000 1.000000 1.000000 1.000000   100
    #>        geojsonsf 1.319239 1.278732 1.323499 1.243311 1.221639 3.078142   100
    #> 
    #> $polygon.geojson
    #> Unit: relative
    #>             expr      min       lq    mean   median       uq      max neval
    #>  rcppsimdgeojson 1.000000 1.000000 1.00000 1.000000 1.000000 1.000000   100
    #>        geojsonsf 1.359964 1.366889 1.38734 1.299311 1.289514 1.727529   100
    #> 
    #> $GeometryCollection.geojson
    #> Unit: relative
    #>             expr      min       lq     mean   median       uq       max neval
    #>  rcppsimdgeojson 1.000000 1.000000 1.000000 1.000000 1.000000 1.0000000   100
    #>        geojsonsf 1.350152 1.350065 1.268412 1.319147 1.291335 0.5555074   100

#### File

``` r
lapply(geojson_files, function(.file) {
  microbenchmark::microbenchmark(
    rcppsimdgeojson = rcppsimdgeojson:::.fload_sfc(.file),
    geojsonsf = geojsonsf:::rcpp_read_sfc_file(.file, mode = "rb",
                                               flatten_geometries = FALSE),
    unit = "relative"
  )
})
```

    #> $featureCollection.geojson
    #> Unit: relative
    #>             expr      min       lq     mean   median       uq      max neval
    #>  rcppsimdgeojson 1.000000 1.000000 1.000000 1.000000 1.000000 1.000000   100
    #>        geojsonsf 1.609552 1.588139 1.559423 1.455929 1.422411 2.863556   100
    #> 
    #> $holeyMultiPolygon.geojson
    #> Unit: relative
    #>             expr      min       lq     mean   median       uq      max neval
    #>  rcppsimdgeojson 1.000000 1.000000 1.000000 1.000000 1.000000 1.000000   100
    #>        geojsonsf 1.331092 1.324711 1.321235 1.273143 1.244646 2.720689   100
    #> 
    #> $holeyPolygon.geojson
    #> Unit: relative
    #>             expr      min       lq     mean median       uq     max neval
    #>  rcppsimdgeojson 1.000000 1.000000 1.000000 1.0000 1.000000 1.00000   100
    #>        geojsonsf 1.303239 1.309057 1.303131 1.2543 1.226838 2.97318   100
    #> 
    #> $lineString.geojson
    #> Unit: relative
    #>             expr      min      lq     mean   median       uq       max neval
    #>  rcppsimdgeojson 1.000000 1.00000 1.000000 1.000000 1.000000 1.0000000   100
    #>        geojsonsf 1.275821 1.26404 1.131551 1.194256 1.178014 0.6378789   100
    #> 
    #> $multiLineString.geojson
    #> Unit: relative
    #>             expr     min     lq     mean   median       uq       max neval
    #>  rcppsimdgeojson 1.00000 1.0000 1.000000 1.000000 1.000000 1.0000000   100
    #>        geojsonsf 1.23573 1.2303 1.128375 1.189969 1.176542 0.5372709   100
    #> 
    #> $multiPoint.geojson
    #> Unit: relative
    #>             expr      min      lq     mean   median      uq      max neval
    #>  rcppsimdgeojson 1.000000 1.00000 1.000000 1.000000 1.00000 1.000000   100
    #>        geojsonsf 1.270341 1.24851 1.243462 1.193452 1.15467 1.528522   100
    #> 
    #> $multiPolygon.geojson
    #> Unit: relative
    #>             expr     min       lq     mean   median       uq       max neval
    #>  rcppsimdgeojson 1.00000 1.000000 1.000000 1.000000 1.000000 1.0000000   100
    #>        geojsonsf 1.27095 1.257271 1.265028 1.459767 1.267501 0.5011639   100
    #> 
    #> $point.geojson
    #> Unit: relative
    #>             expr      min      lq     mean   median       uq      max neval
    #>  rcppsimdgeojson 1.000000 1.00000 1.000000 1.000000 1.000000 1.000000   100
    #>        geojsonsf 1.281824 1.26817 1.296168 1.197473 1.174644 1.086907   100
    #> 
    #> $polygon.geojson
    #> Unit: relative
    #>             expr      min       lq     mean   median       uq      max neval
    #>  rcppsimdgeojson 1.000000 1.000000 1.000000 1.000000 1.000000 1.000000   100
    #>        geojsonsf 1.268324 1.271186 1.325434 1.202063 1.188934 2.689661   100
    #> 
    #> $GeometryCollection.geojson
    #> Unit: relative
    #>             expr      min       lq     mean   median       uq       max neval
    #>  rcppsimdgeojson 1.000000 1.000000 1.000000 1.000000 1.000000 1.0000000   100
    #>        geojsonsf 1.318199 1.310385 1.225802 1.242727 1.227966 0.6305231   100

##### Big File

``` r
sprintf("%f MB", file.size(big_geojson) * 1e-6)
```

    #> [1] "103.882182 MB"

``` r
microbenchmark::microbenchmark(
  rcppsimdgeojson = rcppsimdgeojson:::.fload_sfc(big_geojson),
  geojsonsf = geojsonsf:::rcpp_read_sfc_file(big_geojson, mode = "rb",
                                             flatten_geometries = FALSE)
  ,
  times = 5
)
```

    #> Unit: milliseconds
    #>             expr       min        lq     mean    median        uq       max neval
    #>  rcppsimdgeojson  427.7686  432.3005  540.547  459.4846  691.2816  691.8997     5
    #>        geojsonsf 1261.7060 1434.2256 1540.144 1530.7067 1708.0215 1766.0586     5
