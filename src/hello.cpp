
#include <rcppsimdgeojson.hpp>

// [[Rcpp::export(.hello)]]
bool hello() { return true; }


// [[Rcpp::export(.parse_geojson)]]
SEXP parse_geojson(const Rcpp::CharacterVector& x) {
  if (std::size(x) > 0) {
    simdjson::dom::parser parser;

    if (const auto [parsed, error] = parser.parse(std::string_view(x[0]));
        !error) {
      return rcppsimdgeojson::dispatch_parse(parsed);
    }
  }

  return R_NilValue;
}


// [[Rcpp::export(.read_geojson)]]
SEXP read_geojson(const Rcpp::CharacterVector& x) {

  if (std::size(x) > 0) {
    simdjson::dom::parser parser;

    if (const auto [parsed, error] = parser.load(std::string(x[0])); !error) {
      return rcppsimdgeojson::dispatch_parse(parsed);
    }
  }

  return R_NilValue;
}
