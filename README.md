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
geojson_files <- list.files(system.file("geojsonexamples", package = "rcppsimdgeojson"),
                            recursive = TRUE, pattern = "\\.geojson$", full.names = TRUE)
geojson_files <- geojson_files[!grepl("largeMixedTest.geojson", geojson_files, fixed = TRUE)] # invalid

lapply(geojson_files, rcppsimdgeojson:::.read_geojson)
```

    #> [[1]]
    #> [[1]][[1]]
    #> [1] 102.0   0.5
    #> attr(,"class")
    #> [1] "XY"    "POINT" "sfg"  
    #> 
    #> [[1]][[2]]
    #>      [,1] [,2]
    #> [1,]  102    0
    #> [2,]  103    1
    #> [3,]  104    0
    #> [4,]  105    1
    #> attr(,"class")
    #> [1] "XY"         "LINESTRING" "sfg"       
    #> 
    #> [[1]][[3]]
    #> [[1]]
    #>      [,1] [,2]
    #> [1,]  100    0
    #> [2,]  101    0
    #> [3,]  101    1
    #> [4,]  100    1
    #> [5,]  100    0
    #> 
    #> attr(,"class")
    #> [1] "XY"      "POLYGON" "sfg"    
    #> 
    #> attr(,"bbox")
    #> xmin ymin xmax ymax 
    #>  100    0  105    1 
    #> attr(,"n_empty")
    #> [1] 0
    #> attr(,"crs")
    #> $input
    #> [1] "4326"
    #> 
    #> $wkt
    #> [1] "GEOGCS[\"WGS 84\",\n    DATUM[\"WGS_1984\",\n        SPHEROID[\"WGS 84\",6378137,298.257223563,\n            AUTHORITY[\"EPSG\",\"7030\"]],\n        AUTHORITY[\"EPSG\",\"6326\"]],\n    PRIMEM[\"Greenwich\",0,\n        AUTHORITY[\"EPSG\",\"8901\"]],\n    UNIT[\"degree\",0.0174532925199433,\n        AUTHORITY[\"EPSG\",\"9122\"]],\n    AUTHORITY[\"EPSG\",\"4326\"]]"
    #> 
    #> attr(,"class")
    #> [1] "crs"
    #> attr(,"precision")
    #> [1] 0
    #> 
    #> [[2]]
    #> [[2]][[1]]
    #> [[1]]
    #> [[1]][[1]]
    #>      [,1] [,2]
    #> [1,]   40   40
    #> [2,]   20   45
    #> [3,]   45   30
    #> [4,]   40   40
    #> 
    #> 
    #> [[2]]
    #> [[2]][[1]]
    #>      [,1] [,2]
    #> [1,]   20   35
    #> [2,]   10   30
    #> [3,]   10   10
    #> [4,]   30    5
    #> [5,]   45   20
    #> [6,]   20   35
    #> 
    #> [[2]][[2]]
    #>      [,1] [,2]
    #> [1,]   30   20
    #> [2,]   20   15
    #> [3,]   20   25
    #> [4,]   30   20
    #> 
    #> 
    #> attr(,"class")
    #> [1] "XY"           "MULTIPOLYGON" "sfg"         
    #> 
    #> attr(,"bbox")
    #> xmin ymin xmax ymax 
    #>   10    5   45   45 
    #> attr(,"n_empty")
    #> [1] 0
    #> attr(,"crs")
    #> $input
    #> [1] "4326"
    #> 
    #> $wkt
    #> [1] "GEOGCS[\"WGS 84\",\n    DATUM[\"WGS_1984\",\n        SPHEROID[\"WGS 84\",6378137,298.257223563,\n            AUTHORITY[\"EPSG\",\"7030\"]],\n        AUTHORITY[\"EPSG\",\"6326\"]],\n    PRIMEM[\"Greenwich\",0,\n        AUTHORITY[\"EPSG\",\"8901\"]],\n    UNIT[\"degree\",0.0174532925199433,\n        AUTHORITY[\"EPSG\",\"9122\"]],\n    AUTHORITY[\"EPSG\",\"4326\"]]"
    #> 
    #> attr(,"class")
    #> [1] "crs"
    #> attr(,"precision")
    #> [1] 0
    #> 
    #> [[3]]
    #> [[3]][[1]]
    #> [[1]]
    #>      [,1] [,2]
    #> [1,]   35   10
    #> [2,]   45   45
    #> [3,]   15   40
    #> [4,]   10   20
    #> [5,]   35   10
    #> 
    #> [[2]]
    #>      [,1] [,2]
    #> [1,]   20   30
    #> [2,]   35   35
    #> [3,]   30   20
    #> [4,]   20   30
    #> 
    #> attr(,"class")
    #> [1] "XY"      "POLYGON" "sfg"    
    #> 
    #> attr(,"bbox")
    #> xmin ymin xmax ymax 
    #>   10   10   45   45 
    #> attr(,"n_empty")
    #> [1] 0
    #> attr(,"crs")
    #> $input
    #> [1] "4326"
    #> 
    #> $wkt
    #> [1] "GEOGCS[\"WGS 84\",\n    DATUM[\"WGS_1984\",\n        SPHEROID[\"WGS 84\",6378137,298.257223563,\n            AUTHORITY[\"EPSG\",\"7030\"]],\n        AUTHORITY[\"EPSG\",\"6326\"]],\n    PRIMEM[\"Greenwich\",0,\n        AUTHORITY[\"EPSG\",\"8901\"]],\n    UNIT[\"degree\",0.0174532925199433,\n        AUTHORITY[\"EPSG\",\"9122\"]],\n    AUTHORITY[\"EPSG\",\"4326\"]]"
    #> 
    #> attr(,"class")
    #> [1] "crs"
    #> attr(,"precision")
    #> [1] 0
    #> 
    #> [[4]]
    #> [[4]][[1]]
    #>      [,1] [,2]
    #> [1,]   30   10
    #> [2,]   10   30
    #> [3,]   40   40
    #> attr(,"class")
    #> [1] "XY"         "LINESTRING" "sfg"       
    #> 
    #> attr(,"bbox")
    #> xmin ymin xmax ymax 
    #>   10   10   40   40 
    #> attr(,"n_empty")
    #> [1] 0
    #> attr(,"crs")
    #> $input
    #> [1] "4326"
    #> 
    #> $wkt
    #> [1] "GEOGCS[\"WGS 84\",\n    DATUM[\"WGS_1984\",\n        SPHEROID[\"WGS 84\",6378137,298.257223563,\n            AUTHORITY[\"EPSG\",\"7030\"]],\n        AUTHORITY[\"EPSG\",\"6326\"]],\n    PRIMEM[\"Greenwich\",0,\n        AUTHORITY[\"EPSG\",\"8901\"]],\n    UNIT[\"degree\",0.0174532925199433,\n        AUTHORITY[\"EPSG\",\"9122\"]],\n    AUTHORITY[\"EPSG\",\"4326\"]]"
    #> 
    #> attr(,"class")
    #> [1] "crs"
    #> attr(,"precision")
    #> [1] 0
    #> 
    #> [[5]]
    #> [[5]][[1]]
    #> [[1]]
    #>      [,1] [,2]
    #> [1,]   10   10
    #> [2,]   20   20
    #> [3,]   10   40
    #> 
    #> [[2]]
    #>      [,1] [,2]
    #> [1,]   40   40
    #> [2,]   30   30
    #> [3,]   40   20
    #> [4,]   30   10
    #> 
    #> attr(,"class")
    #> [1] "XY"              "MULTILINESTRING" "sfg"            
    #> 
    #> attr(,"bbox")
    #> xmin ymin xmax ymax 
    #>   10   10   40   40 
    #> attr(,"n_empty")
    #> [1] 0
    #> attr(,"crs")
    #> $input
    #> [1] "4326"
    #> 
    #> $wkt
    #> [1] "GEOGCS[\"WGS 84\",\n    DATUM[\"WGS_1984\",\n        SPHEROID[\"WGS 84\",6378137,298.257223563,\n            AUTHORITY[\"EPSG\",\"7030\"]],\n        AUTHORITY[\"EPSG\",\"6326\"]],\n    PRIMEM[\"Greenwich\",0,\n        AUTHORITY[\"EPSG\",\"8901\"]],\n    UNIT[\"degree\",0.0174532925199433,\n        AUTHORITY[\"EPSG\",\"9122\"]],\n    AUTHORITY[\"EPSG\",\"4326\"]]"
    #> 
    #> attr(,"class")
    #> [1] "crs"
    #> attr(,"precision")
    #> [1] 0
    #> 
    #> [[6]]
    #> [[6]][[1]]
    #>      [,1] [,2]
    #> [1,]   10   40
    #> [2,]   40   30
    #> [3,]   20   20
    #> [4,]   30   10
    #> attr(,"class")
    #> [1] "XY"         "MULTIPOINT" "sfg"       
    #> 
    #> attr(,"bbox")
    #> xmin ymin xmax ymax 
    #>   10   10   40   40 
    #> attr(,"n_empty")
    #> [1] 0
    #> attr(,"crs")
    #> $input
    #> [1] "4326"
    #> 
    #> $wkt
    #> [1] "GEOGCS[\"WGS 84\",\n    DATUM[\"WGS_1984\",\n        SPHEROID[\"WGS 84\",6378137,298.257223563,\n            AUTHORITY[\"EPSG\",\"7030\"]],\n        AUTHORITY[\"EPSG\",\"6326\"]],\n    PRIMEM[\"Greenwich\",0,\n        AUTHORITY[\"EPSG\",\"8901\"]],\n    UNIT[\"degree\",0.0174532925199433,\n        AUTHORITY[\"EPSG\",\"9122\"]],\n    AUTHORITY[\"EPSG\",\"4326\"]]"
    #> 
    #> attr(,"class")
    #> [1] "crs"
    #> attr(,"precision")
    #> [1] 0
    #> 
    #> [[7]]
    #> [[7]][[1]]
    #> [[1]]
    #> [[1]][[1]]
    #>      [,1] [,2]
    #> [1,]   30   20
    #> [2,]   45   40
    #> [3,]   10   40
    #> [4,]   30   20
    #> 
    #> 
    #> [[2]]
    #> [[2]][[1]]
    #>      [,1] [,2]
    #> [1,]   15    5
    #> [2,]   40   10
    #> [3,]   10   20
    #> [4,]    5   10
    #> [5,]   15    5
    #> 
    #> 
    #> attr(,"class")
    #> [1] "XY"           "MULTIPOLYGON" "sfg"         
    #> 
    #> attr(,"bbox")
    #> xmin ymin xmax ymax 
    #>    5    5   45   40 
    #> attr(,"n_empty")
    #> [1] 0
    #> attr(,"crs")
    #> $input
    #> [1] "4326"
    #> 
    #> $wkt
    #> [1] "GEOGCS[\"WGS 84\",\n    DATUM[\"WGS_1984\",\n        SPHEROID[\"WGS 84\",6378137,298.257223563,\n            AUTHORITY[\"EPSG\",\"7030\"]],\n        AUTHORITY[\"EPSG\",\"6326\"]],\n    PRIMEM[\"Greenwich\",0,\n        AUTHORITY[\"EPSG\",\"8901\"]],\n    UNIT[\"degree\",0.0174532925199433,\n        AUTHORITY[\"EPSG\",\"9122\"]],\n    AUTHORITY[\"EPSG\",\"4326\"]]"
    #> 
    #> attr(,"class")
    #> [1] "crs"
    #> attr(,"precision")
    #> [1] 0
    #> 
    #> [[8]]
    #> [[8]][[1]]
    #> [1] 30 10
    #> attr(,"class")
    #> [1] "XY"    "POINT" "sfg"  
    #> 
    #> attr(,"bbox")
    #> xmin ymin xmax ymax 
    #>   30   10   30   10 
    #> attr(,"n_empty")
    #> [1] 0
    #> attr(,"crs")
    #> $input
    #> [1] "4326"
    #> 
    #> $wkt
    #> [1] "GEOGCS[\"WGS 84\",\n    DATUM[\"WGS_1984\",\n        SPHEROID[\"WGS 84\",6378137,298.257223563,\n            AUTHORITY[\"EPSG\",\"7030\"]],\n        AUTHORITY[\"EPSG\",\"6326\"]],\n    PRIMEM[\"Greenwich\",0,\n        AUTHORITY[\"EPSG\",\"8901\"]],\n    UNIT[\"degree\",0.0174532925199433,\n        AUTHORITY[\"EPSG\",\"9122\"]],\n    AUTHORITY[\"EPSG\",\"4326\"]]"
    #> 
    #> attr(,"class")
    #> [1] "crs"
    #> attr(,"precision")
    #> [1] 0
    #> 
    #> [[9]]
    #> [[9]][[1]]
    #> [[1]]
    #>      [,1] [,2]
    #> [1,]   30   10
    #> [2,]   40   40
    #> [3,]   20   40
    #> [4,]   10   20
    #> [5,]   30   10
    #> 
    #> attr(,"class")
    #> [1] "XY"      "POLYGON" "sfg"    
    #> 
    #> attr(,"bbox")
    #> xmin ymin xmax ymax 
    #>   10   10   40   40 
    #> attr(,"n_empty")
    #> [1] 0
    #> attr(,"crs")
    #> $input
    #> [1] "4326"
    #> 
    #> $wkt
    #> [1] "GEOGCS[\"WGS 84\",\n    DATUM[\"WGS_1984\",\n        SPHEROID[\"WGS 84\",6378137,298.257223563,\n            AUTHORITY[\"EPSG\",\"7030\"]],\n        AUTHORITY[\"EPSG\",\"6326\"]],\n    PRIMEM[\"Greenwich\",0,\n        AUTHORITY[\"EPSG\",\"8901\"]],\n    UNIT[\"degree\",0.0174532925199433,\n        AUTHORITY[\"EPSG\",\"9122\"]],\n    AUTHORITY[\"EPSG\",\"4326\"]]"
    #> 
    #> attr(,"class")
    #> [1] "crs"
    #> attr(,"precision")
    #> [1] 0

<!-- ```{r} -->

<!-- microbenchmark::microbenchmark( -->

<!--   rcppsimdgeojson = rcppsimdgeojson:::.read_geojson(path.expand("~/Downloads/bay_delta_89b.geojson")), -->

<!--   geojsonsf = geojsonsf::geojson_sfc("~/Downloads/bay_delta_89b.geojson"), -->

<!--   sf = sf::read_sf("~/Downloads/bay_delta_89b.geojson") -->

<!--   , -->

<!--   times = 5 -->

<!-- ) -->

<!-- ``` -->
