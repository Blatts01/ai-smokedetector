#ifndef NEUTON_MODEL_MODEL_H
#define NEUTON_MODEL_MODEL_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Model info */
#define NEUTON_MODEL_HEADER_VERSION 3
#define NEUTON_MODEL_QLEVEL 32
#define NEUTON_MODEL_FLOAT_SUPPORT 1
#define NEUTON_MODEL_TASK_TYPE 1  // binary classification
#define NEUTON_MODEL_NEURONS_COUNT 23
#define NEUTON_MODEL_WEIGHTS_COUNT 158
#define NEUTON_MODEL_INPUTS_COUNT 12
#define NEUTON_MODEL_INPUT_LIMITS_COUNT 12
#define NEUTON_MODEL_OUTPUTS_COUNT 2
#define NEUTON_MODEL_LOG_SCALE_OUTPUTS 0
#define NEUTON_MODEL_HAS_CLASSES_RATIO 0
#define NEUTON_MODEL_HAS_NEGPOS_RATIO 0

/* Preprocessing */
#define NEUTON_PREPROCESSING_ENABLED 0
#define NEUTON_DROP_ORIGINAL_FEATURES 0
#define NEUTON_BITMASK_ENABLED 1
#define NEUTON_INPUTS_IS_INTEGER 0
#define NEUTON_MODEL_SA_PRECISION 24

/* Types */
typedef float input_t;
typedef float extracted_feature_t;
typedef float coeff_t;
typedef float weight_t;
typedef double acc_signed_t;
typedef double acc_unsigned_t;
typedef uint8_t sources_size_t;
typedef uint8_t weights_size_t;
typedef uint8_t neurons_size_t;

/* Limits */
static const input_t modelInputMin[] = {
	-22.01, 10.74, 0, 10668, 15317, 930.85199, 0, 0, 0, 0, 0, 1.6547122e+09 };
static const input_t modelInputMax[] = {
	59.93, 75.199997, 60000, 13803, 21410, 939.86102, 45432.262, 61482.031,
	51914.68, 30026.438, 1, 1.6551301e+09 };

static const uint8_t modelUsedInputsMask[] = { 0xff, 0x0e };

/* Structure */
static const weight_t modelWeights[] = {
	-0.14723101, 0.42276001, -0.97177482, -1, 0.045457635, 0.39131272, 0.54569161,
	0.87854105, 0.52345347, -0.5898993, 0.022142299, -1, 0, -0.79677242, -0.50641638,
	-0.40433171, 0.35659119, 0.40795952, 0.26390839, -0.61941022, -0.875, -1,
	-1, 1, -1, 0.0028047562, -1, -0.5, -0.5, -0.5, -0.5, 0.5, -0.984375, -1,
	0.32739455, 0.97683311, -0.68729115, 0.031331778, -1, 0.93561596, -1, 0.78241533,
	0.20357724, -0.99804688, 0.44779828, 0.40006912, -0.95535082, 0.21854335,
	-0.35345733, -1, 0.0390625, -1, -1, 0.24123444, -1, -0.74958092, 0.62082136,
	0.011401023, 1, -0.50977409, -0.77540159, 0.64093196, 0.18762732, -0.91592169,
	-0.73635143, -0.45428532, 0.60049218, -0.32104141, 0.1371212, 0.625, 0.059296925,
	-0.51720101, 0.93641371, 0.59463549, -1, -0.99999368, -0.14923662, -0.0060753236,
	0.99804652, 0.40841684, -0.0043673804, -0.011712535, -0.23266408, -0.99609363,
	0.03964771, -0.6513952, 0.44919169, -0.74993265, -0.22468252, -0.097089648,
	0.11390895, 1, -0.0057103559, -0.12825337, 0.77593625, 0.24375403, -0.28912482,
	0.96567106, -0.081430689, -0.097550437, 0.99999619, 0.2990081, 0.68650901,
	0.22298035, -0.57393897, -0.20379639, 0, 0.5, 0.5, -0.5, 0.5, -0.5, -0.26106986,
	-0.42514747, -1, -0.41758108, 0.27973098, 0.1730957, 0.0076183379, -0.89169979,
	1, 0.083969891, 0.39673018, -0.20741583, -0.99999988, -0.33006766, 0.72539967,
	-0.13131644, 0.65462667, 0.13613161, -0.9750663, -0.58202791, 1, 0.5, -1,
	-1, -1, -1, -1, -0.48035786, 0.54993325, -1, 0.65260881, -0.90368974, 0.0045375824,
	0.83426547, 0.62294483, 0.28454763, -0.84872538, 0.875, -0.88664484, 0.16651534,
	1, -0.56436849, 0.70830262, -1, -0.28629696, -0.41533047 };

static const sources_size_t modelLinks[] = {
	0, 3, 4, 10, 11, 12, 0, 2, 3, 4, 6, 10, 12, 1, 0, 1, 5, 6, 12, 0, 2, 7,
	9, 10, 11, 12, 0, 2, 3, 5, 6, 12, 1, 3, 4, 4, 5, 12, 0, 1, 3, 1, 5, 12,
	2, 5, 0, 1, 5, 6, 12, 1, 3, 5, 7, 1, 12, 1, 7, 1, 4, 5, 12, 1, 4, 8, 9,
	4, 5, 12, 6, 0, 1, 4, 10, 12, 9, 10, 11, 0, 3, 7, 12, 0, 8, 12, 1, 6, 12,
	0, 2, 3, 5, 6, 7, 10, 12, 13, 12, 6, 7, 9, 13, 3, 11, 12, 5, 12, 0, 4,
	5, 12, 0, 2, 3, 4, 6, 11, 12, 0, 3, 5, 6, 10, 2, 12, 3, 16, 18, 5, 11,
	12, 1, 4, 8, 1, 7, 12, 0, 5, 7, 11, 12, 16, 12, 1, 4, 8, 9, 11, 15, 16,
	17, 18, 19, 20, 21, 12 };

static const weights_size_t modelIntLinksBoundaries[] = {
	0, 7, 14, 20, 29, 35, 41, 46, 55, 59, 67, 71, 79, 86, 98, 103, 108, 116,
	124, 129, 135, 144, 157 };
static const weights_size_t modelExtLinksBoundaries[] = {
	6, 13, 19, 26, 32, 38, 44, 51, 57, 63, 70, 76, 83, 89, 99, 106, 112, 119,
	126, 132, 138, 145, 158 };

static const coeff_t modelFuncCoeffs[] = {
	40, 39.746727, 39.999947, 40, 39.999985, 39.999985, 40, 40, 40, 40, 23.262875,
	40, 39.999947, 39.999992, 39.06485, 37.418579, 39.999962, 40, 40, 35.012512,
	40, 40, 39.999969 };
static const uint8_t modelFuncTypes[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

static const neurons_size_t modelOutputNeurons[] = { 14, 22 };

#ifdef __cplusplus
}
#endif

#endif // NEUTON_MODEL_MODEL_H

