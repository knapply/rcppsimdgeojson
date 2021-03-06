// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include <Rcpp.h>

using namespace Rcpp;

// hello
bool hello();
RcppExport SEXP _rcppsimdgeojson_hello() {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    rcpp_result_gen = Rcpp::wrap(hello());
    return rcpp_result_gen;
END_RCPP
}
// fparse_sfc
SEXP fparse_sfc(const Rcpp::CharacterVector& x, const bool expand_geometries);
RcppExport SEXP _rcppsimdgeojson_fparse_sfc(SEXP xSEXP, SEXP expand_geometriesSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const Rcpp::CharacterVector& >::type x(xSEXP);
    Rcpp::traits::input_parameter< const bool >::type expand_geometries(expand_geometriesSEXP);
    rcpp_result_gen = Rcpp::wrap(fparse_sfc(x, expand_geometries));
    return rcpp_result_gen;
END_RCPP
}
// fload_sfc
SEXP fload_sfc(const Rcpp::CharacterVector& x, const bool expand_geometries);
RcppExport SEXP _rcppsimdgeojson_fload_sfc(SEXP xSEXP, SEXP expand_geometriesSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const Rcpp::CharacterVector& >::type x(xSEXP);
    Rcpp::traits::input_parameter< const bool >::type expand_geometries(expand_geometriesSEXP);
    rcpp_result_gen = Rcpp::wrap(fload_sfc(x, expand_geometries));
    return rcpp_result_gen;
END_RCPP
}

static const R_CallMethodDef CallEntries[] = {
    {"_rcppsimdgeojson_hello", (DL_FUNC) &_rcppsimdgeojson_hello, 0},
    {"_rcppsimdgeojson_fparse_sfc", (DL_FUNC) &_rcppsimdgeojson_fparse_sfc, 2},
    {"_rcppsimdgeojson_fload_sfc", (DL_FUNC) &_rcppsimdgeojson_fload_sfc, 2},
    {NULL, NULL, 0}
};

RcppExport void R_init_rcppsimdgeojson(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
