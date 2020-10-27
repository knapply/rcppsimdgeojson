`{rcppsimdgeojson}`
================

  - [Installation](#installation)

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
[![HitCount](http://hits.dwyl.io/knapply/rcppsimdgeojson.svg)](http://hits.dwyl.io/knapply/rcppsimdgeojson)
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

### (Initial) Benchmarking

``` r
microbenchmark::microbenchmark(
  rcppsimdgeojson = rcppsimdgeojson:::.read_geojson(path.expand("~/Downloads/bay_delta_89b.geojson")),
  geojsonsf = geojsonsf::geojson_sf("~/Downloads/bay_delta_89b.geojson"),
  sf = sf::read_sf("~/Downloads/bay_delta_89b.geojson")
  ,
  times = 5
)
```

    #> Unit: milliseconds
    #>             expr       min       lq     mean   median       uq      max neval
    #>  rcppsimdgeojson  881.9412 1008.401 1020.374 1053.282 1065.275 1092.973     5
    #>        geojsonsf 1998.6408 2224.385 2287.515 2243.745 2429.686 2541.117     5
    #>               sf 7072.5232 7415.354 7473.168 7419.669 7598.604 7859.688     5
