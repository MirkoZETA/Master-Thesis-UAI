#include <fstream>
#include <armadillo>
#include "./src/gsnr_values/CL.hpp"

// Def
#define CL 0
#define CLE 1
#define CLS 2

// Global parameters
int escenario = CLE;
std::ofstream file;

// MB
std::vector<char> ORDEN(int largoRuta, int escenario){
    switch (escenario)
    {
    case CLE:
        // if (largoRuta <= 400) return {'E', 'L', 'C'};
        // else return {'C', 'L', 'E'};
        return {'E', 'C', 'L'};
        break;

    case CLS:
        //if (largoRuta <= 2000) return {'S', 'L', 'C'};
        //else return {'L', 'C', 'S'};
        return {'S', 'L', 'C'};
        break;
    default:
        return {'L', 'C'};
    }
}

// BBP
std::map<float, int> mapaBitRate {{ 100.0 , 0 }, { 200.0 , 1 }, { 300.0 , 2 }, { 400.0 , 3 }};
double pesoBitRate[4];
double totalBitRate[4];
double bloqueadosBitRate[4];

double BBP(double totalBitRate[4], double bloqueadosBitRate[4], double pesoBitRate[4]){
    double BBP = 0;

    // Suma de pesos:
    double sumaPesos = 0;
    for (int b = 0; b < 4; b++){
        if (totalBitRate[b] == 0) continue;
        sumaPesos += pesoBitRate[b];
    }

    for (int b = 0; b < 4; b++){
        if (totalBitRate[b] == 0) continue;
        BBP += (bloqueadosBitRate[b] / totalBitRate[b]) * pesoBitRate[b];
    }
    return (BBP/sumaPesos);
}

// Cantidad de conexiones por banda
std::map<char, int> conexionesPorBanda;






























// GNPY: Olvidalo, dejalo atras.

// Metricas
double blockedByGSNR = 0;

std::map<char, int> bands_index_alphabetical = {
    {'S', 3},
    {'L', 2},
    {'E', 1},
    {'C', 0}
};

// GSNR tresholds          128QAM   64QAM   32QAM   16QAM   8QAM    QPSK    BPSK
//float gsnrModulation[7] = {23.78 , 20.88 , 17.96 , 14.98 , 12.313,  8.32 ,  5.32 };
float gsnrModulation[4] = {14.98 , 12.313,  8.32 ,  5.32 };

// Get the index of the first element in the distances array that is >= target_distance
int find_index_for_distance(double target_distance) {
    // Define the distances array within the function
    const std::vector<double> distances = {
        3.000e+00, 5.000e+00, 1.000e+01, 2.000e+01, 4.000e+01, 6.000e+01,
        7.500e+01, 1.500e+02, 2.250e+02, 3.000e+02, 3.750e+02, 4.500e+02,
        5.250e+02, 6.000e+02, 6.750e+02, 7.500e+02, 8.250e+02, 9.000e+02,
        9.750e+02, 1.050e+03, 1.125e+03, 1.200e+03, 1.275e+03, 1.350e+03,
        1.425e+03, 1.500e+03, 1.650e+03, 1.800e+03, 1.950e+03, 2.100e+03,
        2.250e+03, 2.400e+03, 2.550e+03, 2.700e+03, 2.850e+03, 3.000e+03,
        3.150e+03, 3.300e+03, 3.450e+03, 3.600e+03, 3.750e+03, 3.900e+03,
        4.050e+03, 4.200e+03, 4.350e+03, 4.500e+03, 4.650e+03, 4.800e+03,
        4.950e+03, 5.100e+03, 5.250e+03, 5.400e+03, 5.550e+03, 5.700e+03,
        5.850e+03, 6.000e+03, 6.150e+03, 6.600e+03, 6.750e+03
    };

    // Use lower_bound to find the first element that is >= target_distance
    auto it = std::lower_bound(distances.begin(), distances.end(), target_distance);

    // Check if we found a valid element
    if (it != distances.end()) {
        // Calculate and return the index
        return std::distance(distances.begin(), it);
    } else {
        // If no such element exists, return -1 (or handle as appropriate)
        return -1;
    }
}

float get_gsnr_value_slot(int distance, size_t slot_index, char band){
    // Get the index for the distance
    int distance_index = find_index_for_distance(distance);
    if (distance_index == -1) {
        std::cerr << "Distance not found in the distances array." << std::endl;
        return -1;
    }

    // Map of bands to index Assuming C,E,L,S
    size_t band_index = bands_index_alphabetical[band];

    // Get the GSNR value from the matrix
    switch (escenario)
    {
    case CL:
        return gsnr_CL[distance_index][band_index][slot_index];
        break;
    case CLE:
        //return gsnrCLE[distance_index][slot_index][band];
        break;
    case CLS:
        //return gsnrCLS[distance_index][slot_index][band];
        break;
    }
    return -1;
}

float get_gsnr_value_band(int distance, char band){

    // Get the index for the distance
    int distance_index = find_index_for_distance(distance);
    if (distance_index == -1) {
        std::cerr << "Distance not found in the distances array." << std::endl;
        return -1;
    }

    // Map of bands to index Assuming C,E,L,S
    size_t band_index = bands_index_alphabetical[band];

    // Get the GSNR value from the matrix
    switch (escenario)
    {
    case CL:
        return gsnr_worst_case_CL[distance_index][band_index];
        break;
    case CLE:
        //return gsnr_worst_case_CLE[distance_index][slot_index];
        break;
    case CLS:
        //return gsnr_worst_case_CLS[distance_index][slot_index];
        break;
    }
    return -1;
}