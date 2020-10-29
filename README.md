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

all(identical_w_sf)
```

    #> [1] TRUE

``` r
as.matrix(identical_w_sf)
```

    #>                           [,1]
    #> featureCollection.geojson TRUE
    #> holeyMultiPolygon.geojson TRUE
    #> holeyPolygon.geojson      TRUE
    #> lineString.geojson        TRUE
    #> multiLineString.geojson   TRUE
    #> multiPoint.geojson        TRUE
    #> multiPolygon.geojson      TRUE
    #> point.geojson             TRUE
    #> polygon.geojson           TRUE

``` r
big_geojson <- path.expand("~/Downloads/bay_delta_89b.geojson")

test <- rcppsimdgeojson:::.fload_sfc(big_geojson)
target <- sf::read_sf(big_geojson)$geometry

test
```

    #> Geometry set for 258032 features 
    #> geometry type:  MULTIPOLYGON
    #> dimension:      XY
    #> bbox:           xmin: -123.0052 ymin: 37.40251 xmax: -121.2558 ymax: 38.80345
    #> CRS:            4326
    #> First 5 geometries:

``` r
identical(test, target)
```

    #> [1] TRUE

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
    #>        geojsonsf 1.874076 1.834803 4.121041 1.737544 1.573296 56.86297   100
    #> 
    #> $holeyMultiPolygon.geojson
    #> Unit: relative
    #>             expr      min       lq     mean   median       uq      max neval
    #>  rcppsimdgeojson 1.000000 1.000000 1.000000 1.000000 1.000000 1.000000   100
    #>        geojsonsf 1.430164 1.382618 1.443046 1.299463 1.300615 2.054296   100
    #> 
    #> $holeyPolygon.geojson
    #> Unit: relative
    #>             expr      min       lq     mean   median       uq      max neval
    #>  rcppsimdgeojson 1.000000 1.000000 1.000000 1.000000 1.000000 1.000000   100
    #>        geojsonsf 1.361085 1.381617 1.412262 1.335363 1.291944 1.469476   100
    #> 
    #> $lineString.geojson
    #> Unit: relative
    #>             expr     min       lq     mean   median       uq       max neval
    #>  rcppsimdgeojson 1.00000 1.000000 1.000000 1.000000 1.000000 1.0000000   100
    #>        geojsonsf 1.34042 1.323556 1.237217 1.253763 1.221113 0.6764029   100
    #> 
    #> $multiLineString.geojson
    #> Unit: relative
    #>             expr      min       lq    mean   median       uq      max neval
    #>  rcppsimdgeojson 1.000000 1.000000 1.00000 1.000000 1.000000 1.000000   100
    #>        geojsonsf 1.285808 1.274137 1.32059 1.229296 1.220007 2.340503   100
    #> 
    #> $multiPoint.geojson
    #> Unit: relative
    #>             expr      min       lq     mean   median       uq      max neval
    #>  rcppsimdgeojson 1.000000 1.000000 1.000000 1.000000 1.000000 1.000000   100
    #>        geojsonsf 1.352192 1.341144 1.303338 1.271494 1.268141 1.601252   100
    #> 
    #> $multiPolygon.geojson
    #> Unit: relative
    #>             expr     min       lq     mean   median      uq       max neval
    #>  rcppsimdgeojson 1.00000 1.000000 1.000000 1.000000 1.00000 1.0000000   100
    #>        geojsonsf 1.24821 1.240183 1.221959 1.212003 1.21034 0.8049568   100
    #> 
    #> $point.geojson
    #> Unit: relative
    #>             expr      min       lq     mean   median       uq       max neval
    #>  rcppsimdgeojson 1.000000 1.000000 1.000000 1.000000 1.000000 1.0000000   100
    #>        geojsonsf 1.378581 1.296821 1.221089 1.261387 1.241391 0.6298718   100
    #> 
    #> $polygon.geojson
    #> Unit: relative
    #>             expr      min       lq     mean   median       uq      max neval
    #>  rcppsimdgeojson 1.000000 1.000000 1.000000 1.000000 1.000000 1.000000   100
    #>        geojsonsf 1.376806 1.365994 1.338474 1.308208 1.264657 2.372659   100

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
    #>             expr      min       lq     mean   median       uq       max neval
    #>  rcppsimdgeojson 1.000000 1.000000 1.000000 1.000000 1.000000 1.0000000   100
    #>        geojsonsf 1.589574 1.417262 1.451384 1.677049 1.600031 0.6282627   100
    #> 
    #> $holeyMultiPolygon.geojson
    #> Unit: relative
    #>             expr      min       lq     mean   median       uq      max neval
    #>  rcppsimdgeojson 1.000000 1.000000 1.000000 1.000000 1.000000 1.000000   100
    #>        geojsonsf 1.275708 1.288546 1.150914 1.252144 1.239742 0.669996   100
    #> 
    #> $holeyPolygon.geojson
    #> Unit: relative
    #>             expr      min       lq     mean   median       uq      max neval
    #>  rcppsimdgeojson 1.000000 1.000000 1.000000 1.000000 1.000000 1.000000   100
    #>        geojsonsf 1.286434 1.275782 1.344583 1.245294 1.234352 2.546523   100
    #> 
    #> $lineString.geojson
    #> Unit: relative
    #>             expr      min       lq     mean   median       uq       max neval
    #>  rcppsimdgeojson 1.000000 1.000000 1.000000 1.000000 1.000000 1.0000000   100
    #>        geojsonsf 1.248774 1.236203 1.087627 1.186793 1.178781 0.5228065   100
    #> 
    #> $multiLineString.geojson
    #> Unit: relative
    #>             expr      min       lq     mean   median      uq       max neval
    #>  rcppsimdgeojson 1.000000 1.000000 1.000000 1.000000 1.00000 1.0000000   100
    #>        geojsonsf 1.206777 1.227156 1.255611 1.219694 1.19505 0.5102791   100
    #> 
    #> $multiPoint.geojson
    #> Unit: relative
    #>             expr      min     lq     mean   median       uq       max neval
    #>  rcppsimdgeojson 1.000000 1.0000 1.000000 1.000000 1.000000 1.0000000   100
    #>        geojsonsf 1.227284 1.2185 1.144125 1.181237 1.181372 0.6567647   100
    #> 
    #> $multiPolygon.geojson
    #> Unit: relative
    #>             expr      min       lq     mean  median       uq      max neval
    #>  rcppsimdgeojson 1.000000 1.000000 1.000000 1.00000 1.000000 1.000000   100
    #>        geojsonsf 1.127493 1.125216 1.153369 1.07494 1.162353 1.567482   100
    #> 
    #> $point.geojson
    #> Unit: relative
    #>             expr      min       lq     mean   median       uq      max neval
    #>  rcppsimdgeojson 1.000000 1.000000 1.000000 1.000000 1.000000 1.000000   100
    #>        geojsonsf 1.220664 1.209948 1.180893 1.173892 1.147677 2.137868   100
    #> 
    #> $polygon.geojson
    #> Unit: relative
    #>             expr      min       lq    mean   median       uq      max neval
    #>  rcppsimdgeojson 1.000000 1.000000 1.00000 1.000000 1.000000 1.000000   100
    #>        geojsonsf 1.248989 1.220673 1.20538 1.180559 1.181003 1.953069   100

##### Big File

``` r
sprintf("%f MB", file.size(big_geojson) * 1e-6)
```

    #> [1] "103.882182 MB"

``` r
microbenchmark::microbenchmark(
  rcppsimdgeojson = rcppsimdgeojson:::.fload_sfc(big_geojson),
  geojsonsf = geojsonsf::geojson_sfc(big_geojson)
  ,
  times = 5
)
```

    #> Unit: milliseconds
    #>             expr       min        lq      mean    median        uq       max neval
    #>  rcppsimdgeojson  557.1922  609.4428  702.2059  705.4532  813.1291  825.8122     5
    #>        geojsonsf 1369.2176 1581.7968 1801.1519 1800.0858 1858.8099 2395.8495     5
