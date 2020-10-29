#ifndef __RCPPSIMDGEOJSON__PLAYGROUND_HPP__
#define __RCPPSIMDGEOJSON__PLAYGROUND_HPP__

#include <limits>
#include <string_view>
#include <type_traits>
#include <unordered_set>

#include "common.hpp"
#include "simdjson.h"


namespace rcppsimdgeojson {

namespace default_crs {
constexpr auto input = "4326";
// clang-format off
constexpr auto wkt = R"(GEOGCS["WGS 84",
    DATUM["WGS_1984",
        SPHEROID["WGS 84",6378137,298.257223563,
            AUTHORITY["EPSG","7030"]],
        AUTHORITY["EPSG","6326"]],
    PRIMEM["Greenwich",0,
        AUTHORITY["EPSG","8901"]],
    UNIT["degree",0.0174532925199433,
        AUTHORITY["EPSG","9122"]],
    AUTHORITY["EPSG","4326"]])";
// clang-format on
} // namespace default_crs

enum class Type {
  Point,             // POINT
  MultiPoint,        // MULTIPOINT
  LineString,        // LINESTRING
  MultiLineString,   // MULTILINESTRING
  Polygon,           // POLYGON
  MultiPolygon,      // MULTIPOLYGON
  FeatureCollection, // GEOMETRY ?
  UNKNOWN,
};

inline SEXP dispatch_parse(simdjson::dom::object); // forward-declaration

inline constexpr Type get_feature_type(const std::string_view feature_type) {
  if (feature_type == "Point") {
    return Type::Point;
  } else if (feature_type == "MultiPoint") {
    return Type::MultiPoint;
  } else if (feature_type == "LineString") {
    return Type::LineString;
  } else if (feature_type == "MultiLineString") {
    return Type::MultiLineString;
  } else if (feature_type == "Polygon") {
    return Type::Polygon;
  } else if (feature_type == "MultiPolygon") {
    return Type::MultiPolygon;
  } else if (feature_type == "FeatureCollection") {
    return Type::FeatureCollection;
  } else {
    return Type::UNKNOWN;
  }
}
static_assert(get_feature_type("Point") == Type::Point);
static_assert(get_feature_type("MultiPoint") == Type::MultiPoint);
static_assert(get_feature_type("LineString") == Type::LineString);
static_assert(get_feature_type("MultiLineString") == Type::MultiLineString);
static_assert(get_feature_type("Polygon") == Type::Polygon);
static_assert(get_feature_type("MultiPolygon") == Type::MultiPolygon);
static_assert(get_feature_type("FeatureCollection") == Type::FeatureCollection);
static_assert(get_feature_type("junk") == Type::UNKNOWN);

inline Type get_feature_type(const simdjson::dom::object obj) {
  if (const std::string_view feature_type = obj["type"];
      std::size(feature_type) >= 5) {
    return get_feature_type(feature_type);
  }
  return Type::UNKNOWN;
}


template <Type geom_type> constexpr auto get_coord_T() {
  if constexpr (geom_type == Type::Point) {
    return Rcpp::DoubleVector();
  }
  if constexpr (geom_type == Type::MultiPoint) {
    return Rcpp::NumericMatrix();
  }
  if constexpr (geom_type == Type::LineString) {
    return Rcpp::NumericMatrix();
  }
  if constexpr (geom_type == Type::MultiLineString) {
    return Rcpp::ListOf<Rcpp::NumericMatrix>();
  }
  if constexpr (geom_type == Type::Polygon) {
    return Rcpp::ListOf<Rcpp::NumericMatrix>();
  }
  if constexpr (geom_type == Type::MultiPolygon) {
    return Rcpp::ListOf<Rcpp::ListOf<Rcpp::NumericMatrix>>();
  }
  if constexpr (geom_type == Type::FeatureCollection) {
    return Rcpp::List();
  }
}
template <Type geom_type> using coord_T = decltype(get_coord_T<geom_type>());


template <Type geom_type> inline Rcpp::CharacterVector get_sfg_class() {
  if constexpr (geom_type == Type::Point) {
    return {"XY", "POINT", "sfg"};
  }
  if constexpr (geom_type == Type::MultiPoint) {
    return {"XY", "MULTIPOINT", "sfg"};
  }

  if constexpr (geom_type == Type::LineString) {
    return {"XY", "LINESTRING", "sfg"};
  }
  if constexpr (geom_type == Type::MultiLineString) {
    return {"XY", "MULTILINESTRING", "sfg"};
  }

  if constexpr (geom_type == Type::Polygon) {
    return {"XY", "POLYGON", "sfg"};
  }
  if constexpr (geom_type == Type::MultiPolygon) {
    return {"XY", "MULTIPOLYGON", "sfg"};
  }

  if constexpr (geom_type == Type::FeatureCollection) {
    return {"XY", "GEOMETRY", "sfg"};
  }

  if constexpr (geom_type == Type::UNKNOWN) {
    return {"XY", "", "sfg"};
  }
}


template <Type geom_type>
inline auto parse_coords(simdjson::dom::array) -> coord_T<geom_type>;

template <>
inline auto parse_coords<Type::Point>(simdjson::dom::array coords)
    -> coord_T<Type::Point> {

  return coord_T<Type::Point>::create(coords.at(0), coords.at(1));
}

template <>
inline auto parse_coords<Type::MultiPoint>(simdjson::dom::array coords)
    -> coord_T<Type::MultiPoint> {

  coord_T<Type::MultiPoint> out(std::size(coords), 2);
  auto lon_it = out.column(0).begin();
  auto lat_it = out.column(1).begin();

  for (auto&& point : coords) {
    *lon_it++ = point.at(0);
    *lat_it++ = point.at(1);
  }

  return out;
}

template <>
inline auto parse_coords<Type::LineString>(simdjson::dom::array coords)
    -> coord_T<Type::LineString> {

  coord_T<Type::LineString> out(std::size(coords), 2);
  auto lon_it = out.column(0).begin();
  auto lat_it = out.column(1).begin();

  for (auto&& point : coords) {
    *lon_it++ = point.at(0);
    *lat_it++ = point.at(1);
  }

  return out;
}

template <>
inline auto parse_coords<Type::MultiLineString>(simdjson::dom::array coords)
    -> coord_T<Type::MultiLineString> {

  Rcpp::List out(std::size(coords));
  auto out_it = out.begin();

  for (simdjson::dom::array&& linestring_array : coords) {
    coord_T<Type::LineString> linestring(std::size(linestring_array), 2);
    auto lon_it = linestring.column(0).begin();
    auto lat_it = linestring.column(1).begin();

    for (simdjson::dom::array&& point : linestring_array) {
      *lon_it++ = point.at(0).get_double();
      *lat_it++ = point.at(1).get_double();
    }

    *out_it++ = parse_coords<Type::LineString>(linestring_array);
  }

  return out;
}

template <>
inline auto parse_coords<Type::Polygon>(simdjson::dom::array coords)
    -> coord_T<Type::Polygon> {
  Rcpp::List out(std::size(coords));
  auto out_it = std::begin(out);

  for (simdjson::dom::array&& polygon_or_hole_array : coords) {
    Rcpp::NumericMatrix parsed_linestring(std::size(polygon_or_hole_array), 2);
    auto lon_it = parsed_linestring.column(0).begin();
    auto lat_it = parsed_linestring.column(1).begin();

    for (simdjson::dom::array&& point : polygon_or_hole_array) {
      *lon_it++ = point.at(0);
      *lat_it++ = point.at(1);
    }

    *out_it++ = parsed_linestring;
  }

  return out;
}

template <>
inline auto parse_coords<Type::MultiPolygon>(simdjson::dom::array coords)
    -> coord_T<Type::MultiPolygon> {

  Rcpp::List out(std::size(coords));
  auto out_it = std::begin(out);

  for (simdjson::dom::array&& polygon_array : coords) {
    *out_it++ = parse_coords<Type::Polygon>(polygon_array);
  }

  return out;
}

struct bounding_box {
  double xmin = std::numeric_limits<double>::max();
  double ymin = std::numeric_limits<double>::max();

  double xmax = std::numeric_limits<double>::min();
  double ymax = std::numeric_limits<double>::min();

  bounding_box() = default;

  bounding_box(const double _xmin, const double _ymin, const double _xmax,
               const double _ymax)
      : xmin(_xmin), ymin(_ymin), xmax(_xmax), ymax(_ymax) {}

  void update(bounding_box bbox2) {
    this->xmin = bbox2.xmin < this->xmin ? bbox2.xmin : this->xmin;
    this->ymin = bbox2.ymin < this->ymin ? bbox2.ymin : this->ymin;

    this->xmax = bbox2.xmax > this->xmax ? bbox2.xmax : this->xmax;
    this->ymax = bbox2.ymax > this->ymax ? bbox2.ymax : this->ymax;
  }

  Rcpp::NumericVector to_r() const {
    using Rcpp::_;
    return Rcpp::NumericVector::create(_["xmin"] = xmin, _["ymin"] = ymin,
                                       _["xmax"] = xmax, _["ymax"] = ymax);
  }
};

template <Type geom_type> struct sfg {
  coord_T<geom_type> coords;

  sfg(simdjson::dom::array coordinates)
      : coords(parse_coords<geom_type>(coordinates)){};

  bounding_box bbox() const {
    if constexpr (geom_type == Type::Point) {
      return {coords[0], coords[1], coords[0], coords[1]};
    }
    if constexpr (geom_type == Type::MultiPoint ||
                  geom_type == Type::LineString) {
      return {Rcpp::min(coords.column(0)), Rcpp::min(coords.column(1)),
              Rcpp::max(coords.column(0)), Rcpp::max(coords.column(1))};
    }
    if constexpr (geom_type == Type::MultiLineString) {
      auto out = bounding_box();
      for (Rcpp::NumericMatrix&& mat : coords) {
        out.update({Rcpp::min(mat.column(0)), Rcpp::min(mat.column(1)),
                    Rcpp::max(mat.column(0)), Rcpp::max(mat.column(1))});
      }
      return out;
    }
    if constexpr (geom_type == Type::Polygon) {
      // coords[0], because subsequent matrices are holes in the polygon
      return {Rcpp::min(coords[0].column(0)), Rcpp::min(coords[0].column(1)),
              Rcpp::max(coords[0].column(0)), Rcpp::max(coords[0].column(1))};
    }
    if constexpr (geom_type == Type::MultiPolygon) {
      auto out = bounding_box();
      for (R_xlen_t i = 0; i < std::size(coords); ++i) {
        // what is this dark-ages nonsense?
        out.update({Rcpp::min(coords[i][0].column(0)),
                    Rcpp::min(coords[i][0].column(1)),
                    Rcpp::max(coords[i][0].column(0)),
                    Rcpp::max(coords[i][0].column(1))});
      }
      return out;
    }
  }

  Rcpp::CharacterVector sfg_class() const {
    return Rcpp::CharacterVector::create(get_sfg_class<geom_type>());
  }

  SEXP to_r() {
    if constexpr (std::is_same_v<decltype(coords), Rcpp::DoubleVector> ||
                  std::is_same_v<decltype(coords), Rcpp::NumericMatrix>) {
      coords.attr("class") = get_sfg_class<geom_type>();
      return coords;
    } else {
      Rcpp::List out(coords);
      out.attr("class") = get_sfg_class<geom_type>();
      return out;
    }
  }
};


struct sfc {
  Rcpp::List sfgs;
  decltype(std::begin(sfgs)) it_sfgs = std::begin(sfgs);

  bounding_box bbox;
  std::unordered_set<std::string_view> classes;

  template <Type geom_type> void add_sfg(simdjson::dom::array coordinates) {
    auto geom = sfg<geom_type>(coordinates);
    bbox.update(geom.bbox());
    *it_sfgs++ = geom.to_r();
  }

  sfc(simdjson::dom::object feature) : sfgs(1) {
    switch (get_feature_type(feature)) {
      case Type::Point: {
        add_sfg<Type::Point>(feature["coordinates"]);
        break;
      }

      case Type::MultiPoint: {
        add_sfg<Type::MultiPoint>(feature["coordinates"]);
        break;
      }

      case Type::LineString: {
        add_sfg<Type::LineString>(feature["coordinates"]);
        break;
      }

      case Type::MultiLineString: {
        add_sfg<Type::MultiLineString>(feature["coordinates"]);
        break;
      }

      case Type::Polygon: {
        add_sfg<Type::Polygon>(feature["coordinates"]);
        break;
      }

      case Type::MultiPolygon: {
        add_sfg<Type::MultiPolygon>(feature["coordinates"]);
        break;
      }

      default:
        Rcpp::stop("wtf");
    }
  }

  sfc(simdjson::dom::array features) : sfgs(std::size(features)), bbox() {
    for (simdjson::dom::object&& feature : features) {
      simdjson::dom::object geometry = feature["geometry"];
      simdjson::dom::array coordinates = geometry["coordinates"];

      switch (get_feature_type(geometry["type"].get_string())) {
        case Type::Point: {
          add_sfg<Type::Point>(coordinates);
          break;
        }

        case Type::MultiPoint: {
          add_sfg<Type::MultiPoint>(coordinates);
          break;
        }

        case Type::LineString: {
          add_sfg<Type::LineString>(coordinates);
          break;
        }

        case Type::MultiLineString: {
          add_sfg<Type::MultiLineString>(coordinates);
          break;
        }

        case Type::Polygon: {
          add_sfg<Type::Polygon>(coordinates);
          break;
        }

        case Type::MultiPolygon: {
          add_sfg<Type::MultiPolygon>(coordinates);
          break;
        }

        default:
          Rcpp::stop("wtf");
      }
    }
  }

  decltype(sfgs) to_r() {
    sfgs.attr("bbox") = bbox.to_r();
    sfgs.attr("n_empty") = 0;

    using Rcpp::_;
    auto crs = Rcpp::List::create(_["input"] = default_crs::input,
                                  _["wkt"] = default_crs::wkt);
    crs.attr("class") = "crs";
    sfgs.attr("crs") = crs;
    // sfgs.attr("class") = get_sfc_class(geom_types);
    sfgs.attr("precision") = 0.0;

    return sfgs;
  }
};


inline SEXP dispatch_parse(simdjson::dom::object obj) {
  if (std::string_view type = obj["type"]; type == "FeatureCollection") {
    return sfc(obj["features"].get_array()).to_r();
  } else {
    return sfc(obj).to_r();
  }
}


} // namespace rcppsimdgeojson


#endif
