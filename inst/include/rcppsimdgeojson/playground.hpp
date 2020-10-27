#ifndef __RCPPSIMDGEOJSON__PLAYGROUND_HPP__
#define __RCPPSIMDGEOJSON__PLAYGROUND_HPP__

#include <limits>
#include <string_view>
#include <unordered_set>
#include "common.hpp"


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


inline Rcpp::CharacterVector
get_sfc_class(std::unordered_set<Type> geom_types) {
  switch (std::size(geom_types)) {
    case 0:
      return Rcpp::CharacterVector::create(NA_STRING, "sfc");

    case 1:

      switch (*std::begin(geom_types)) {
        case Type::Point:
          return Rcpp::CharacterVector::create("sfc_POINT", "sfc");

        case Type::MultiPoint:
          return Rcpp::CharacterVector::create("sfc_MULTIPOINT", "sfc");

        case Type::LineString:
          return Rcpp::CharacterVector::create("sfc_LINESTRING", "sfc");

        case Type::MultiLineString:
          return Rcpp::CharacterVector::create("sfc_MULTILINESTRING", "sfc");

        case Type::Polygon:
          return Rcpp::CharacterVector::create("sfc_POLYGON", "sfc");

        case Type::MultiPolygon:
          return Rcpp::CharacterVector::create("sfc_MULTIPOLYGON", "sfc");

        default:
          return Rcpp::CharacterVector::create(NA_STRING, "sfc");
      }

    default:
      return Rcpp::CharacterVector::create("sfc_GEOMETRY", "sfc");
  }
}


struct Bbox {
  double xmin = std::numeric_limits<double>::max();
  double ymin = std::numeric_limits<double>::max();

  double xmax = std::numeric_limits<double>::min();
  double ymax = std::numeric_limits<double>::min();

  Bbox() = default;
  void update(const double x, const double y) {
    this->xmin = x < this->xmin ? x : this->xmin;
    this->xmax = x > this->xmax ? x : this->xmax;
    this->ymin = y < this->ymin ? y : this->ymin;
    this->ymax = y > this->ymax ? y : this->ymax;
  }
  SEXP sf_bbox() const {
    using Rcpp::_;
    auto out = Rcpp::DoubleVector::create(_["xmin"] = xmin, _["ymin"] = ymin,
                                          _["xmax"] = xmax, _["ymax"] = ymax);
    out.attr("class") = "bbox";
    return out;
  };
};


template <Type geom_type, bool add_class = true>
inline SEXP parse_feature(simdjson::dom::array array, Bbox& bbox) {
  if constexpr (geom_type == Type::Point) {
    const double lon = array.at(0);
    const double lat = array.at(1);
    bbox.update(lon, lat);
    auto out = Rcpp::DoubleVector::create(lon, lat);
    if constexpr (add_class) {
      out.attr("class") = Rcpp::CharacterVector::create("XY", "POINT", "sfg");
    }
    return out;
  }

  if constexpr (geom_type == Type::MultiPoint) {
    Rcpp::NumericMatrix out(std::size(array), 2);
    R_xlen_t i_row = 0;
    for (auto&& point : array) {
      const double lon = point.at(0);
      const double lat = point.at(1);
      bbox.update(lon, lat);

      out(i_row, 0) = lon;
      out(i_row++, 1) = lat;
    }
    if constexpr (add_class) {
      out.attr("class") =
          Rcpp::CharacterVector::create("XY", "MULTIPOINT", "sfg");
    }
    return out;
  }


  if constexpr (geom_type == Type::LineString) {
    Rcpp::NumericMatrix out(std::size(array), 2);
    R_xlen_t i_row = 0;
    for (auto&& point : array) {
      const double lon = point.at(0);
      const double lat = point.at(1);
      bbox.update(lon, lat);

      out(i_row, 0) = lon;
      out(i_row++, 1) = lat;
    }
    if constexpr (add_class) {
      out.attr("class") =
          Rcpp::CharacterVector::create("XY", "LINESTRING", "sfg");
    }
    return out;
  }

  if constexpr (geom_type == Type::MultiLineString) {
    Rcpp::List out(std::size(array));
    auto it = std::begin(out);
    for (auto&& point : array) {
      *it++ = parse_feature<Type::Point, false>(point, bbox);
    }
    if constexpr (add_class) {
      out.attr("class") =
          Rcpp::CharacterVector::create("XY", "MULTILINESTRING", "sfg");
    }
    return out;
  }


  if constexpr (geom_type == Type::Polygon) {
    Rcpp::List out(std::size(array));
    auto it = std::begin(out);

    for (simdjson::dom::array&& inner_array : array) {
      Rcpp::NumericMatrix coords(std::size(inner_array), 2);
      R_xlen_t i_row = 0;

      for (auto&& point : inner_array) {
        const double lon = point.at(0);
        const double lat = point.at(1);
        bbox.update(lon, lat);

        coords(i_row, 0) = lon;
        coords(i_row++, 1) = lat;
      }

      *it++ = coords;
    }

    if constexpr (add_class) {
      out.attr("class") = Rcpp::CharacterVector::create("XY", "POLYGON", "sfg");
    }
    return out;
  }

  if constexpr (geom_type == Type::MultiPolygon) {
    Rcpp::List out(std::size(array));
    auto it = std::begin(out);
    for (auto&& point : array) {
      *it++ = parse_feature<Type::Polygon, false>(point, bbox);
    }
    if constexpr (add_class) {
      out.attr("class") =
          Rcpp::CharacterVector::create("XY", "MULTIPOLYGON", "sfg");
    }
    return out;
  }


  return R_NilValue;
}


template <rcppsimdjson::deserialize::Type_Policy type_policy,
          rcppsimdjson::utils::Int64_R_Type int64_opt>
inline std::pair<
    std::unordered_set<Type>,
    rcppsimdjson::deserialize::Column_Schema<type_policy, int64_opt>>
diagnose_features(simdjson::dom::array array) {
  auto cols =
      rcppsimdjson::deserialize::Column_Schema<type_policy, int64_opt>();
  R_xlen_t col_index(0L);
  std::unordered_set<Type> geom_types;

  for (simdjson::dom::object&& obj : array) {
    geom_types.insert(get_feature_type(obj["geometry"].get_object().first));

    if (auto [object, error] = obj["properties"].get<simdjson::dom::object>();
        !error) {
      for (auto&& [key, value] : object) {
        if (key != "coordinates" &&
            cols.schema.find(key) == std::end(cols.schema)) {
          cols.schema[key] = rcppsimdjson::deserialize::Column<type_policy,
                                                               int64_opt>{
              col_index++,
              rcppsimdjson::deserialize::Type_Doctor<type_policy, int64_opt>()};
        }
        cols.schema[key].schema.add_element(value);
      }
    }
  }

  return std::make_pair(geom_types, cols);
}

template <int RTYPE, typename scalar_T, rcppsimdjson::rcpp_T R_Type,
          rcppsimdjson::deserialize::Type_Policy type_policy,
          rcppsimdjson::utils::Int64_R_Type int64_opt>
inline Rcpp::Vector<RTYPE> build_feature_col(
    simdjson::dom::array array, const std::string_view key,
    const rcppsimdjson::deserialize::Type_Doctor<type_policy, int64_opt>&
        type_doc) {
  auto out =
      Rcpp::Vector<RTYPE>(std::size(array), rcppsimdjson::na_val<R_Type>());
  R_xlen_t i_row = 0;

  if (type_doc.is_homogeneous()) {
    if (type_doc.has_null()) {
      for (simdjson::dom::object&& object : array) {
        auto [element, error] = object["properties"][key];
        if (!error) {
          out[i_row] = rcppsimdjson::deserialize::get_scalar<
              scalar_T, R_Type, rcppsimdjson::deserialize::HAS_NULLS>(element);
        }
        i_row++;
      }

    } else {

      for (simdjson::dom::object&& object : array) {
        if (auto [element, error] = object["properties"][key]; !error) {
          out[i_row] = rcppsimdjson::deserialize::get_scalar<
              scalar_T, R_Type, rcppsimdjson::deserialize::NO_NULLS>(element);
        }
        i_row++;
      }
    }

  } else {

    for (auto&& object : array) {
      if (auto [element, error] = object["properties"][key]; !error) {
        out[i_row] =
            rcppsimdjson::deserialize::get_scalar_dispatch<RTYPE>(element);
      }
      i_row++;
    }
  }

  return out;
}


template <rcppsimdjson::deserialize::Type_Policy type_policy,
          rcppsimdjson::utils::Int64_R_Type int64_opt>
inline auto build_col_int64(
    simdjson::dom::array array, const std::string_view key,
    const rcppsimdjson::deserialize::Type_Doctor<type_policy, int64_opt>
        type_doc) -> SEXP {

  if constexpr (int64_opt == rcppsimdjson::utils::Int64_R_Type::Double) {
    return build_feature_col<REALSXP, int64_t, rcppsimdjson::rcpp_T::dbl,
                             type_policy>(array, key, type_doc);
  }

  if constexpr (int64_opt == rcppsimdjson::utils::Int64_R_Type::String) {
    return build_feature_col<STRSXP, int64_t, rcppsimdjson::rcpp_T::chr,
                             type_policy>(array, key, type_doc);
  }

  if constexpr (int64_opt == rcppsimdjson::utils::Int64_R_Type::Integer64 ||
                int64_opt == rcppsimdjson::utils::Int64_R_Type::Always) {
    auto stl_vec =
        std::vector<int64_t>(std::size(array), rcppsimdjson::NA_INTEGER64);
    auto i_row = std::size_t(0ULL);

    if (type_doc.is_homogeneous()) {
      if (type_doc.has_null()) {
        for (auto&& object : array) {
          if (auto [element, error] = object["properties"][key]; !error) {
            stl_vec[i_row] = rcppsimdjson::deserialize::get_scalar<
                int64_t, rcppsimdjson::rcpp_T::i64,
                rcppsimdjson::deserialize::HAS_NULLS>(element);
          }
          i_row++;
        }

      } else {
        for (auto&& object : array) {
          if (auto [element, error] = object["properties"][key]; !error) {
            stl_vec[i_row] = rcppsimdjson::deserialize::get_scalar<
                int64_t, rcppsimdjson::rcpp_T::i64,
                rcppsimdjson::deserialize::NO_NULLS>(element);
          }
          i_row++;
        }
      }

    } else {
      for (auto&& object : array) {
        if (auto [element, error] = object["properties"][key]; !error) {
          switch (element.type()) {
            case simdjson::dom::element_type::INT64:
              stl_vec[i_row] = rcppsimdjson::deserialize::get_scalar<
                  int64_t, rcppsimdjson::rcpp_T::i64,
                  rcppsimdjson::deserialize::NO_NULLS>(element);
              break;

            case simdjson::dom::element_type::BOOL:
              stl_vec[i_row] = rcppsimdjson::deserialize::get_scalar<
                  bool, rcppsimdjson::rcpp_T::i64,
                  rcppsimdjson::deserialize::NO_NULLS>(element);
              break;

            default:
              break;
          }
        }
        i_row++;
      }
    }
    return rcppsimdjson::utils::as_integer64(stl_vec);
  }
}


template <rcppsimdjson::deserialize::Type_Policy type_policy,
          rcppsimdjson::utils::Int64_R_Type int64_opt>
inline SEXP build_features(
    simdjson::dom::array array,
    const std::unordered_map<
        std::string_view,
        rcppsimdjson::deserialize::Column<type_policy, int64_opt>>& cols) {

  const R_xlen_t n_rows = std::size(array);
  Rcpp::List out(std::size(cols));
  Rcpp::CharacterVector out_names(std::size(cols));

  for (auto&& [key, col] : cols) {
    out_names[col.index] = Rcpp::String(std::string(key));

    switch (col.schema.common_R_type()) {
      case rcppsimdjson::rcpp_T::chr: {
        out[col.index] =
            build_feature_col<STRSXP, std::string, rcppsimdjson::rcpp_T::chr,
                              type_policy>(array, key, col.schema);
        break;
      }

      case rcppsimdjson::rcpp_T::dbl: {
        out[col.index] =
            build_feature_col<REALSXP, double, rcppsimdjson::rcpp_T::dbl,
                              type_policy>(array, key, col.schema);
        break;
      }

      case rcppsimdjson::rcpp_T::i64: {
        out[col.index] =
            build_col_int64<type_policy, int64_opt>(array, key, col.schema);
        break;
      }

      case rcppsimdjson::rcpp_T::i32: {
        out[col.index] =
            build_feature_col<INTSXP, int64_t, rcppsimdjson::rcpp_T::i32,
                              type_policy>(array, key, col.schema);
        break;
      }

      case rcppsimdjson::rcpp_T::lgl: {
        out[col.index] =
            build_feature_col<LGLSXP, bool, rcppsimdjson::rcpp_T::lgl,
                              type_policy>(array, key, col.schema);
        break;
      }

      case rcppsimdjson::rcpp_T::null: {
        out[col.index] = Rcpp::LogicalVector(n_rows, NA_LOGICAL);
        break;
      }

      default: {
        Rcpp::Vector<VECSXP> this_col(n_rows);
        R_xlen_t i_row(0L);
        for (simdjson::dom::object&& obj : array) {
          if (auto [value, error] = obj["properties"][key]; !error) {
            this_col[i_row++] = rcppsimdjson::deserialize::simplify_element<
                type_policy, int64_opt,
                rcppsimdjson::deserialize::Simplify_To::data_frame>(
                value, R_NilValue, R_NilValue, R_NilValue);
          } else {
            this_col[i_row++] = NA_LOGICAL;
          }
        }
        out[col.index] = this_col;
      }
    }
  }

  out.attr("names") = out_names;

  return out;
}


inline SEXP dispatch_parse(simdjson::dom::object obj) {
  auto bbox = Bbox();

  switch (get_feature_type(obj)) {
    case Type::Point:
      return parse_feature<Type::Point>(obj["coordinates"], bbox);

    case Type::MultiPoint:
      return parse_feature<Type::MultiPoint>(obj["coordinates"], bbox);

    case Type::LineString:
      return parse_feature<Type::LineString>(obj["coordinates"], bbox);

    case Type::MultiLineString:
      return parse_feature<Type::MultiLineString>(obj["coordinates"], bbox);

    case Type::Polygon:
      return parse_feature<Type::Polygon>(obj["coordinates"], bbox);

    case Type::MultiPolygon:
      return parse_feature<Type::MultiPolygon>(obj["coordinates"], bbox);

    case Type::FeatureCollection: {
      simdjson::dom::array features = obj["features"];

      const auto [geom_types, col_schema] = diagnose_features<
          rcppsimdjson::deserialize::Type_Policy::anything_goes,
          rcppsimdjson::utils::Int64_R_Type::Double>(features);

      Rcpp::List out =
          build_features<rcppsimdjson::deserialize::Type_Policy::anything_goes,
                         rcppsimdjson::utils::Int64_R_Type::Double>(
              features, col_schema.schema);


      Rcpp::List geometry(std::size(features));
      auto it = std::begin(geometry);
      for (simdjson::dom::object&& feature : features) {
        *it++ = dispatch_parse(feature["geometry"]);
      }

      geometry.attr("n_empty") = 0;

      using Rcpp::_;
      auto crs = Rcpp::List::create(_["input"] = default_crs::input,
                                    _["wkt"] = default_crs::wkt);
      crs.attr("class") = "crs";
      geometry.attr("crs") = crs;

      geometry.attr("class") = get_sfc_class(geom_types);

      geometry.attr("precision") = double(0);

      geometry.attr("bbox") = bbox.sf_bbox();


      out["geometry"] = geometry;
      out.attr("row.names") = Rcpp::seq(1, std::size(features));
      out.attr("class") = "data.frame";

      return out;
    }

    default:
      return R_NilValue;
  }
}


} // namespace rcppsimdgeojson


#endif
