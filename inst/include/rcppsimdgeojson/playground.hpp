#ifndef __RCPPSIMDGEOJSON__PLAYGROUND_HPP__
#define __RCPPSIMDGEOJSON__PLAYGROUND_HPP__

#include <iterator>
#include <limits>
#include <string_view>
#include <type_traits>
#include <unordered_map>

#include "common.hpp"


namespace rcppsimdgeojson {

namespace defaults {

namespace crs {
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
} // namespace crs

constexpr double Z = 0.0;

} // namespace defaults

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


enum class Dimensions {
  XY,
  XYZ,
  mixed,
};


inline SEXP dispatch_parse(simdjson::dom::object); // forward-declaration

inline constexpr Type get_feature_type(const std::string_view feature_type) {
  using namespace std::literals::string_view_literals;

  if (feature_type == "Point"sv) {
    return Type::Point;
  } else if (feature_type == "MultiPoint"sv) {
    return Type::MultiPoint;
  } else if (feature_type == "LineString"sv) {
    return Type::LineString;
  } else if (feature_type == "MultiLineString"sv) {
    return Type::MultiLineString;
  } else if (feature_type == "Polygon"sv) {
    return Type::Polygon;
  } else if (feature_type == "MultiPolygon"sv) {
    return Type::MultiPolygon;
  } else if (feature_type == "FeatureCollection"sv) {
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
  if (auto [feature_type, error] = obj["type"].get_string(); !error) {
    return get_feature_type(feature_type);
  }
  Rcpp::stop("bad GeoJSON");
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
    return std::vector<Rcpp::NumericMatrix>();
  }
  if constexpr (geom_type == Type::Polygon) {
    return std::vector<Rcpp::NumericMatrix>();
  }
  if constexpr (geom_type == Type::MultiPolygon) {
    return std::vector<std::vector<Rcpp::NumericMatrix>>();
  }
  if constexpr (geom_type == Type::FeatureCollection) {
    return Rcpp::List();
  }
}
template <Type geom_type> using coord_T = decltype(get_coord_T<geom_type>());


template <Type geom_type, Dimensions dims>
inline Rcpp::CharacterVector get_sfg_class() {
  if constexpr (geom_type == Type::Point) {
    if constexpr (dims == Dimensions::XY) {
      return {"XY", "POINT", "sfg"};
    } else {
      return {"XYZ", "POINT", "sfg"};
    }
  }
  if constexpr (geom_type == Type::MultiPoint) {
    if constexpr (dims == Dimensions::XY) {
      return {"XY", "MULTIPOINT", "sfg"};
    } else {
      return {"XYZ", "MULTIPOINT", "sfg"};
    }
  }

  if constexpr (geom_type == Type::LineString) {
    if constexpr (dims == Dimensions::XY) {
      return {"XY", "LINESTRING", "sfg"};
    } else {
      return {"XYZ", "LINESTRING", "sfg"};
    }
  }
  if constexpr (geom_type == Type::MultiLineString) {
    if constexpr (dims == Dimensions::XY) {
      return {"XY", "MULTILINESTRING", "sfg"};
    } else {
      return {"XYZ", "MULTILINESTRING", "sfg"};
    }
  }

  if constexpr (geom_type == Type::Polygon) {
    if constexpr (dims == Dimensions::XY) {
      return {"XY", "POLYGON", "sfg"};
    } else {
      return {"XYZ", "POLYGON", "sfg"};
    }
  }
  if constexpr (geom_type == Type::MultiPolygon) {
    if constexpr (dims == Dimensions::XY) {
      return {"XY", "MULTIPOLYGON", "sfg"};
    } else {
      return {"XYZ", "MULTIPOLYGON", "sfg"};
    }
  }

  if constexpr (geom_type == Type::FeatureCollection) {
    if constexpr (dims == Dimensions::XY) {
      return {"XY", "GEOMETRY", "sfg"};
    } else {
      return {"XYZ", "GEOMETRY", "sfg"};
    }
  }

  if constexpr (geom_type == Type::UNKNOWN) {
    if constexpr (dims == Dimensions::XY) {
      return {"XY", "", "sfg"};
    } else {
      return {"XYZ", "", "sfg"};
    }
  }
}


template <Type geom_type>
inline Dimensions get_dimensions(simdjson::dom::array);

template <>
inline Dimensions get_dimensions<Type::Point>(simdjson::dom::array coords) {
  switch (std::size(coords)) {
    case 2:
      return Dimensions::XY;
    case 3:
      return Dimensions::XYZ;
    default:
      Rcpp::stop("Position/geometry must be of length 2 (XY) or 3 (XYZ).");
  }
};
template <>
inline Dimensions
get_dimensions<Type::MultiPoint>(simdjson::dom::array coords) {
  auto dims = std::unordered_set<Dimensions>();
  for (auto&& point : coords) {
    dims.insert(get_dimensions<Type::Point>(point));
  }
  return std::size(dims) == 1 ? *std::begin(dims) : Dimensions::mixed;
};

template <>
inline Dimensions
get_dimensions<Type::LineString>(simdjson::dom::array coords) {
  auto dims = std::unordered_set<Dimensions>();
  for (auto&& point : coords) {
    dims.insert(get_dimensions<Type::Point>(point));
  }
  return std::size(dims) == 1 ? *std::begin(dims) : Dimensions::mixed;
};
template <>
inline Dimensions
get_dimensions<Type::MultiLineString>(simdjson::dom::array coords) {
  auto dims = std::unordered_set<Dimensions>();
  for (auto&& linestring : coords) {
    dims.insert(get_dimensions<Type::LineString>(linestring));
  }
  return std::size(dims) == 1 ? *std::begin(dims) : Dimensions::mixed;
};


template <>
inline Dimensions get_dimensions<Type::Polygon>(simdjson::dom::array coords) {
  auto dims = std::unordered_set<Dimensions>();
  for (auto&& point : coords.at(0).get_array().value()) { // ignores holes
    dims.insert(get_dimensions<Type::Point>(point));
  }
  return std::size(dims) == 1 ? *std::begin(dims) : Dimensions::mixed;
};
template <>
inline Dimensions
get_dimensions<Type::MultiPolygon>(simdjson::dom::array coords) {
  auto dims = std::unordered_set<Dimensions>();
  for (simdjson::dom::array&& poly : coords) {
    dims.insert(get_dimensions<Type::Polygon>(poly));
  }
  return std::size(dims) == 1 ? *std::begin(dims) : Dimensions::mixed;
};


template <Type geom_type, Dimensions dims>
inline auto parse_coords(simdjson::dom::array coords) -> coord_T<geom_type> {
  if constexpr (geom_type == Type::Point) {
    if constexpr (dims == Dimensions::XY) {
      return coord_T<Type::Point>::create(coords.at(0).get_double().value(),
                                          coords.at(1).get_double().value());
    }

    if constexpr (dims == Dimensions::XYZ) {
      return coord_T<Type::Point>::create(coords.at(0).get_double().value(),
                                          coords.at(1).get_double().value(),
                                          coords.at(2).get_double().value());
    }

    if constexpr (dims == Dimensions::mixed) {
      auto out = coord_T<Type::Point>::create(coords.at(0).get_double().value(),
                                              coords.at(1).get_double().value(),
                                              defaults::Z);
      if (auto [z, error] = coords.at(2).get_double(); !error) {
        out[2] = z;
      }
      return out;
    }
  }


  if constexpr (geom_type == Type::MultiPoint) {
    if constexpr (dims == Dimensions::XY) {
      coord_T<Type::MultiPoint> out(std::size(coords), 2);
      auto x_it = out.column(0).begin();
      auto y_it = out.column(1).begin();
      for (auto&& point : coords) {
        *x_it++ = point.at(0).get_double().value();
        *y_it++ = point.at(1).get_double().value();
      }
      return out;
    }

    if constexpr (dims == Dimensions::XYZ) {
      coord_T<Type::MultiPoint> out(std::size(coords), 2);
      auto x_it = out.column(0).begin();
      auto y_it = out.column(1).begin();
      auto z_it = out.column(2).begin();
      for (auto&& point : coords) {
        *x_it++ = point.at(0).get_double().value();
        *y_it++ = point.at(1).get_double().value();
        *z_it++ = point.at(2).get_double().value();
      }
      return out;
    }

    if constexpr (dims == Dimensions::mixed) {
      coord_T<Type::MultiPoint> out(std::size(coords), 3);
      auto x_it = out.column(0).begin();
      auto y_it = out.column(1).begin();
      auto z_it = out.column(2).begin();
      for (simdjson::dom::array&& point : coords) {
        *x_it++ = point.at(0).get_double().value();
        *y_it++ = point.at(1).get_double().value();
        *z_it++ = std::size(point) == 2 ? defaults::Z
                                        : point.at(2).get_double().value();
      }
      return out;
    }
  }


  if constexpr (geom_type == Type::LineString) {
    if constexpr (dims == Dimensions::XY) {
      coord_T<Type::LineString> out(std::size(coords), 2);
      auto x_it = out.column(0).begin();
      auto y_it = out.column(1).begin();
      for (auto&& point : coords) {
        *x_it++ = point.at(0).get_double().value();
        *y_it++ = point.at(1).get_double().value();
      }
      return out;
    }

    if constexpr (dims == Dimensions::XYZ) {
      coord_T<Type::LineString> out(std::size(coords), 3);
      auto x_it = out.column(0).begin();
      auto y_it = out.column(1).begin();
      auto z_it = out.column(2).begin();
      for (auto&& point : coords) {
        *x_it++ = point.at(0).get_double().value();
        *y_it++ = point.at(1).get_double().value();
        *z_it++ = point.at(2).get_double().value();
      }
      return out;
    }

    if constexpr (dims == Dimensions::mixed) {
      coord_T<Type::LineString> out(std::size(coords), 3);
      auto x_it = out.column(0).begin();
      auto y_it = out.column(1).begin();
      auto z_it = out.column(2).begin();
      for (simdjson::dom::array&& point : coords) {
        *x_it++ = point.at(0).get_double().value();
        *y_it++ = point.at(1).get_double().value();
        *z_it++ = std::size(point) == 2 ? defaults::Z
                                        : point.at(2).get_double().value();
      }
      return out;
    }
  }


  if constexpr (geom_type == Type::MultiLineString) {
    coord_T<Type::MultiLineString> out(std::size(coords));
    auto out_it = std::begin(out);
    for (simdjson::dom::array&& linestring_array : coords) {
      *out_it++ = parse_coords<Type::LineString, dims>(linestring_array);
    }
    return out;
  }


  if constexpr (geom_type == Type::Polygon) {
    if constexpr (dims == Dimensions::XY) {
      coord_T<Type::Polygon> out(std::size(coords));
      auto out_it = std::begin(out);
      for (simdjson::dom::array&& polygon_or_hole_array : coords) {
        Rcpp::NumericMatrix parsed(std::size(polygon_or_hole_array), 2);
        auto x_it = parsed.column(0).begin();
        auto y_it = parsed.column(1).begin();
        for (simdjson::dom::array&& point : polygon_or_hole_array) {
          *x_it++ = point.at(0);
          *y_it++ = point.at(1);
        }
        *out_it++ = parsed;
      }
      return out;
    }

    if constexpr (dims == Dimensions::XYZ) {
      coord_T<Type::Polygon> out(std::size(coords));
      auto out_it = std::begin(out);
      for (simdjson::dom::array&& polygon_or_hole_array : coords) {
        Rcpp::NumericMatrix parsed(std::size(polygon_or_hole_array), 3);
        auto x_it = parsed.column(0).begin();
        auto y_it = parsed.column(1).begin();
        auto z_it = parsed.column(2).begin();
        for (simdjson::dom::array&& point : polygon_or_hole_array) {
          *x_it++ = point.at(0);
          *y_it++ = point.at(1);
          *z_it++ = point.at(2);
        }
        *out_it++ = parsed;
      }
      return out;
    }

    if constexpr (dims == Dimensions::mixed) {
      coord_T<Type::Polygon> out(std::size(coords));
      auto out_it = std::begin(out);
      for (simdjson::dom::array&& polygon_or_hole_array : coords) {
        Rcpp::NumericMatrix parsed(std::size(polygon_or_hole_array), 3);
        auto x_it = parsed.column(0).begin();
        auto y_it = parsed.column(1).begin();
        auto z_it = parsed.column(2).begin();
        for (simdjson::dom::array&& point : polygon_or_hole_array) {
          *x_it++ = point.at(0);
          *y_it++ = point.at(1);
          *z_it++ = std::size(point) == 2 ? defaults::Z
                                          : point.at(2).get_double().value();
        }
        *out_it++ = parsed;
      }
      return out;
    }
  }


  if constexpr (geom_type == Type::MultiPolygon) {
    coord_T<Type::MultiPolygon> out(std::size(coords));
    auto out_it = std::begin(out);
    for (simdjson::dom::array&& linestring_array : coords) {
      *out_it++ = parse_coords<Type::Polygon, dims>(linestring_array);
    }
    return out;
  }
}


struct bounding_box {
  double xmin = std::numeric_limits<double>::infinity();
  double ymin = std::numeric_limits<double>::infinity();
  double zmin = std::numeric_limits<double>::infinity();

  double xmax = -std::numeric_limits<double>::infinity();
  double ymax = -std::numeric_limits<double>::infinity();
  double zmax = -std::numeric_limits<double>::infinity();

  bounding_box() = default;

  bounding_box(const double _xmin, const double _ymin, //
               const double _xmax, const double _ymax)
      : xmin(_xmin), ymin(_ymin), //
        xmax(_xmax), ymax(_ymax) {}

  bounding_box(const double _xmin, const double _ymin, const double _zmin,
               const double _xmax, const double _ymax, const double _zmax)
      : xmin(_xmin), ymin(_ymin), zmin(_zmin), //
        xmax(_xmax), ymax(_ymax), zmax(_zmax) {}

  void update(const bounding_box bbox2) {
    this->xmin = bbox2.xmin < this->xmin ? bbox2.xmin : this->xmin;
    this->xmax = bbox2.xmax > this->xmax ? bbox2.xmax : this->xmax;

    this->ymin = bbox2.ymin < this->ymin ? bbox2.ymin : this->ymin;
    this->ymax = bbox2.ymax > this->ymax ? bbox2.ymax : this->ymax;

    this->zmax = bbox2.zmax > this->zmax ? bbox2.zmax : this->zmax;
    this->zmin = bbox2.zmin < this->zmin ? bbox2.zmin : this->zmin;
  }

  void update(const double _xmin, const double _ymin, //
              const double _xmax, const double _ymax) {
    this->xmin = _xmin < this->xmin ? _xmin : this->xmin;
    this->ymin = _ymin < this->ymin ? _ymin : this->ymin;

    this->xmax = _xmax > this->xmax ? _xmax : this->xmax;
    this->ymax = _ymax > this->ymax ? _ymax : this->ymax;
  }


  void update(const double _xmin, const double _ymin, const double _zmin,
              const double _xmax, const double _ymax, const double _zmax) {
    this->xmin = _xmin < this->xmin ? _xmin : this->xmin;
    this->ymin = _ymin < this->ymin ? _ymin : this->ymin;
    this->zmin = _zmin < this->zmin ? _zmin : this->zmin;

    this->xmax = _xmax > this->xmax ? _xmax : this->xmax;
    this->ymax = _ymax > this->ymax ? _ymax : this->ymax;
    this->zmax = _zmax > this->zmax ? _zmax : this->zmax;
  }

  Rcpp::NumericVector to_r() const {
    using Rcpp::_;
    auto out = Rcpp::NumericVector::create(_["xmin"] = xmin, _["ymin"] = ymin,
                                           _["xmax"] = xmax, _["ymax"] = ymax);
    out.attr("class") = "bbox";
    return out;
  }

  bool needs_z_range() const {
    return zmin != std::numeric_limits<double>::infinity() ||
           zmax != -std::numeric_limits<double>::infinity();
  }

  Rcpp::DoubleVector z_range() const {
    using Rcpp::_;
    auto out = Rcpp::DoubleVector::create(
        _["zmin"] =
            zmin == std::numeric_limits<double>::infinity() ? 0.0 : zmin,
        _["zmax"] =
            -zmax == std::numeric_limits<double>::infinity() ? 0.0 : zmax);
    out.attr("class") = "z_range";

    return out;
  }
};


template <Type geom_type, Dimensions dims> struct sfg {
  coord_T<geom_type> coords;

  sfg(simdjson::dom::array coordinates)
      : coords(parse_coords<geom_type, dims>(coordinates)){};

  bounding_box bbox() const {
    if constexpr (geom_type == Type::Point) {
      if constexpr (dims == Dimensions::XY) {
        return {coords[0], coords[1], coords[0], coords[1]};
      } else {
        return {coords[0], coords[1], coords[2],
                coords[0], coords[1], coords[2]};
      }
    }
    if constexpr (geom_type == Type::MultiPoint ||
                  geom_type == Type::LineString) {
      if constexpr (dims == Dimensions::XY) {
        return {Rcpp::min(coords.column(0)), Rcpp::min(coords.column(1)),
                //
                Rcpp::max(coords.column(0)), Rcpp::max(coords.column(1))};
      } else {
        return {Rcpp::min(coords.column(0)), Rcpp::min(coords.column(1)),
                Rcpp::min(coords.column(2)),
                //
                Rcpp::max(coords.column(0)), Rcpp::max(coords.column(1)),
                Rcpp::max(coords.column(2))};
      }
    }
    if constexpr (geom_type == Type::MultiLineString) {
      auto out = bounding_box();
      for (auto&& mat : coords) {
        if constexpr (dims == Dimensions::XY) {
          out.update(Rcpp::min(mat.column(0)), Rcpp::min(mat.column(1)),
                     //
                     Rcpp::max(mat.column(0)), Rcpp::max(mat.column(1)));
        } else {
          out.update(Rcpp::min(mat.column(0)), Rcpp::min(mat.column(1)),
                     Rcpp::min(mat.column(2)),
                     //
                     Rcpp::max(mat.column(0)), Rcpp::max(mat.column(1)),
                     Rcpp::max(mat.column(2)));
        }
      }
      return out;
    }
    if constexpr (geom_type == Type::Polygon) {
      // coords[0], because subsequent matrices are holes in the polygon
      if constexpr (dims == Dimensions::XY) {
        return {Rcpp::min(coords[0].column(0)), Rcpp::min(coords[0].column(1)),
                //
                Rcpp::max(coords[0].column(0)), Rcpp::max(coords[0].column(1))};
      } else {
        return {Rcpp::min(coords[0].column(0)), Rcpp::min(coords[0].column(1)),
                Rcpp::min(coords[0].column(2)),
                //
                Rcpp::max(coords[0].column(0)), Rcpp::max(coords[0].column(1)),
                Rcpp::max(coords[0].column(2))};
      }
    }
    if constexpr (geom_type == Type::MultiPolygon) {
      auto out = bounding_box();
      for (auto&& list_of_mats : coords) {
        if constexpr (dims == Dimensions::XY) {
          out.update(Rcpp::min(list_of_mats[0].column(0)),
                     Rcpp::min(list_of_mats[0].column(1)),
                     //
                     Rcpp::max(list_of_mats[0].column(0)),
                     Rcpp::max(list_of_mats[0].column(1)));
        } else {
          out.update(Rcpp::min(list_of_mats[0].column(0)),
                     Rcpp::min(list_of_mats[0].column(1)),
                     Rcpp::min(list_of_mats[0].column(2)),
                     //
                     Rcpp::max(list_of_mats[0].column(0)),
                     Rcpp::max(list_of_mats[0].column(1)),
                     Rcpp::max(list_of_mats[0].column(2)));
        }
      }
      return out;
    }
  }

  Rcpp::CharacterVector sfg_class() const {
    return Rcpp::CharacterVector::create(get_sfg_class<geom_type, dims>());
  }

  SEXP to_r() {
    if constexpr (geom_type == Type::Point || geom_type == Type::MultiPoint ||
                  geom_type == Type::LineString) {
      coords.attr("class") = get_sfg_class<geom_type, dims>();
      return coords;
    } else {
      Rcpp::List out = Rcpp::wrap(coords);
      out.attr("class") = get_sfg_class<geom_type, dims>();
      return out;
    }
  }
};


struct sfc {
  Rcpp::List sfgs;
  decltype(std::begin(sfgs)) it_sfgs = std::begin(sfgs);

  bounding_box bbox = bounding_box();
  R_xlen_t class_index = 0;
  std::unordered_map<Type, R_xlen_t> classes_;

  template <Type geom_type, Dimensions dims>
  void add_sfg(simdjson::dom::array coordinates) {
    if (classes_.find(geom_type) == std::end(classes_)) {
      classes_[geom_type] = class_index++;
    }
    auto geom = sfg<geom_type, dims>(coordinates);
    bbox.update(geom.bbox());
    *it_sfgs++ = geom.to_r();
  }

  Rcpp::CharacterVector classes() const {
    Rcpp::CharacterVector out(std::size(classes_));
    for (auto&& c : classes_) {
      switch (c.first) {
        case Type::Point: {
          out[c.second] = "POINT";
          break;
        }
        case Type::MultiPoint: {
          out[c.second] = "MULTIPOINT";
          break;
        }
        case Type::LineString: {
          out[c.second] = "LINESTRING";
          break;
        }
        case Type::MultiLineString: {
          out[c.second] = "MULTILINESTRING";
          break;
        }
        case Type::Polygon: {
          out[c.second] = "POLYGON";
          break;
        }
        case Type::MultiPolygon: {
          out[c.second] = "MULTIPOLYGON";
          break;
        }
        default:
          Rcpp::stop("bad class");
      }
    }
    return out;
  }


  Rcpp::CharacterVector sfc_classe() const {
    switch (std::size(classes_)) {
      case 0:
        Rcpp::stop("no classes?");
      case 1: {
        switch (std::begin(classes_)->first) {
          case Type::Point:
            return {"sfc_POINT", "sfc"};
          case Type::MultiPoint:
            return {"sfc_MULTIPOINT", "sfc"};
          case Type::LineString:
            return {"sfc_LINESTRING", "sfc"};
          case Type::MultiLineString:
            return {"sfc_MULTILINESTRING", "sfc"};
          case Type::Polygon:
            return {"sfc_POLYGON", "sfc"};
          case Type::MultiPolygon:
            return {"sfc_MULTIPOLYGON", "sfc"};
          default:
            Rcpp::stop("bad class");
        }
        default:
          return {"sfc_GEOMETRY", "sfc"};
      }
    }
  }


  void add_sfg(simdjson::dom::object feature) {
    switch (get_feature_type(feature)) {
      case Type::Point: {
        switch (simdjson::dom::array coords = feature["coordinates"];
                get_dimensions<Type::Point>(coords)) {
          case Dimensions::XY: {
            add_sfg<Type::Point, Dimensions::XY>(coords);
            break;
          }
          case Dimensions::XYZ: {
            add_sfg<Type::Point, Dimensions::XYZ>(coords);
            break;
          }
          case Dimensions::mixed: {
            add_sfg<Type::Point, Dimensions::mixed>(coords);
            break;
          }
        }
        break;
      }


      case Type::MultiPoint: {
        switch (simdjson::dom::array coords = feature["coordinates"];
                get_dimensions<Type::MultiPoint>(coords)) {
          case Dimensions::XY: {
            add_sfg<Type::MultiPoint, Dimensions::XY>(coords);
            break;
          }
          case Dimensions::XYZ: {
            add_sfg<Type::MultiPoint, Dimensions::XYZ>(coords);
            break;
          }
          case Dimensions::mixed: {
            add_sfg<Type::MultiPoint, Dimensions::mixed>(coords);
            break;
          }
        }
        break;
      }


      case Type::LineString: {
        switch (simdjson::dom::array coords = feature["coordinates"];
                get_dimensions<Type::LineString>(coords)) {
          case Dimensions::XY: {
            add_sfg<Type::LineString, Dimensions::XY>(coords);
            break;
          }
          case Dimensions::XYZ: {
            add_sfg<Type::LineString, Dimensions::XYZ>(coords);
            break;
          }
          case Dimensions::mixed: {
            add_sfg<Type::LineString, Dimensions::mixed>(coords);
            break;
          }
        }
        break;
      }


      case Type::MultiLineString: {
        switch (simdjson::dom::array coords = feature["coordinates"];
                get_dimensions<Type::MultiLineString>(coords)) {
          case Dimensions::XY: {
            add_sfg<Type::MultiLineString, Dimensions::XY>(coords);
            break;
          }
          case Dimensions::XYZ: {
            add_sfg<Type::MultiLineString, Dimensions::XYZ>(coords);
            break;
          }
          case Dimensions::mixed: {
            add_sfg<Type::MultiLineString, Dimensions::mixed>(coords);
            break;
          }
        }
        break;
      }


      case Type::Polygon: {
        switch (simdjson::dom::array coords = feature["coordinates"];
                get_dimensions<Type::Polygon>(coords)) {
          case Dimensions::XY: {
            add_sfg<Type::Polygon, Dimensions::XY>(coords);
            break;
          }
          case Dimensions::XYZ: {
            add_sfg<Type::Polygon, Dimensions::XYZ>(coords);
            break;
          }
          case Dimensions::mixed: {
            add_sfg<Type::Polygon, Dimensions::mixed>(coords);
            break;
          }
        }
        break;
      }


      case Type::MultiPolygon: {
        switch (simdjson::dom::array coords = feature["coordinates"];
                get_dimensions<Type::MultiPolygon>(coords)) {
          case Dimensions::XY: {
            add_sfg<Type::MultiPolygon, Dimensions::XY>(coords);
            break;
          }
          case Dimensions::XYZ: {
            add_sfg<Type::MultiPolygon, Dimensions::XYZ>(coords);
            break;
          }
          case Dimensions::mixed: {
            add_sfg<Type::MultiPolygon, Dimensions::mixed>(coords);
            break;
          }
        }
        break;
      }

      default:
        Rcpp::stop("wtf");
    }
  }


  sfc(simdjson::dom::object feature) : sfgs(1) { add_sfg(feature); }

  sfc(simdjson::dom::array features) : sfgs(std::size(features)) {
    for (simdjson::dom::object&& obj : features) {
      add_sfg(obj["geometry"].get_object().value());
    }
  }


  decltype(sfgs) to_r() {
    sfgs.attr("n_empty") = 0;

    using Rcpp::_;
    auto crs = Rcpp::List::create(_["input"] = defaults::crs::input,
                                  _["wkt"] = defaults::crs::wkt);
    crs.attr("class") = "crs";
    sfgs.attr("crs") = crs;
    sfgs.attr("class") = sfc_classe();
    sfgs.attr("precision") = 0.0;
    sfgs.attr("bbox") = bbox.to_r();

    if (bbox.needs_z_range()) {
      sfgs.attr("z_range") = bbox.z_range();
    }
    if (std::size(classes_) != 1) {
      sfgs.attr("classes") = classes();
    }

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
