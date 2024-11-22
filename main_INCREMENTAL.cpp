#include "./src/functions.hpp"
#include "./src/simulator_incremental.hpp"
#include <random>
#include <chrono>



// CAPACIDAD
double bloqueadosTotal;
double conectadosTotal;
double bitRateTotal;
std::vector<double> valoresTotales;
std::vector<double> valoresConexiones;
std::vector<double> valoresBitRate;
bool calculado = false;

BEGIN_ALLOC_FUNCTION(FirstFit) {
    // Variables de control
    int largoRuta;
    std::vector<char> ordenBandas;
    std::map<char, std::vector<bool>> estadoSlots;
    int requerido;
    int total;
    int indice;
    
    for (size_t r = 0; r < NUMBER_OF_ROUTES; r++){ // RBMLSA: -> R (Por cada ruta).

        // Obtenemos el largo de la ruta y el orden de las bandas.
        largoRuta = 0;
        for (size_t l = 0; l < NUMBER_OF_LINKS(r); l++){ 
            largoRuta += LINK_IN_ROUTE(r, l)->getLength();
        }
        ordenBandas = ORDEN(largoRuta, escenario);

        for (size_t m = 0; m < NUMBER_OF_MODULATIONS; m++){ // RBMLSA: -> ML (Por cada modulación)
            for (size_t b = 0; b < NUMBER_OF_BANDS(r, 0); b++){ // RBMLSA: -> B (Por cada banda).

                // Verificamos si la modulación es factible.
                if (REQ_REACH_BDM(m, REQ_POS_BANDS(m)[ordenBandas[b]]) < largoRuta) continue;

                // Slots requeridos por la modulación y bit rate actual
                requerido = REQ_SLOTS_BDM(m, REQ_POS_BANDS(m)[ordenBandas[b]]);

                // Obtenemos el vector representativo de los slots ocupados en la ruta (S).
                if (estadoSlots.find(ordenBandas[b]) == estadoSlots.end()) {
                    // Inicializamos el vector de slots para la banda actual si no existe en el mapa
                    estadoSlots[ordenBandas[b]] = std::vector<bool>(LINK_IN_ROUTE(r, 0)->getSlots(ordenBandas[b]), false);

                    // Actualizamos el estado de cada slot en función de cada enlace en la ruta
                    for (size_t l = 0; l < NUMBER_OF_LINKS(r); l++) {
                        for (size_t s = 0; s < LINK_IN_ROUTE(r, l)->getSlots(ordenBandas[b]); s++) { // Por cada slot
                            estadoSlots[ordenBandas[b]][s] = estadoSlots[ordenBandas[b]][s] | LINK_IN_ROUTE(r, l)->getSlot(s, ordenBandas[b]);
                        }
                    }
                }

                // Buscamos slots consecutivos libres para la asignación.
                total = 0;
                indice = 0;
                for (size_t s = 0; s < estadoSlots[ordenBandas[b]].size(); s++){ // RBMLSA: ->SA (Por cada slot).
                    if (estadoSlots[ordenBandas[b]][s] == false){
                        total++;
                    }
                    else {
                        total = 0;
                        indice = s + 1;
                    }
                    if (total == requerido){
                        // Asignar slots en caso de encontrar lighpath factible.
                        for (size_t l = 0; l < NUMBER_OF_LINKS(r); l++){
                            ALLOC_SLOTS_BDM(LINK_IN_ROUTE_ID(r, l), ordenBandas[b], indice, requerido);
                        }
                        conectadosTotal++;
                        bitRateTotal += REQ_BITRATE;
                        if (calculado == false) conexionesPorBanda[ordenBandas[b]]++;
                        return ALLOCATED;
                    }
                }
            }
        }       
    }
    bloqueadosTotal++;
    if (bloqueadosTotal / (bloqueadosTotal+conectadosTotal) >= 0.10 && calculado == false) {
        calculado = true;
        valoresTotales.push_back(bloqueadosTotal+conectadosTotal);
        valoresBitRate.push_back(bitRateTotal);
        valoresConexiones.push_back(conectadosTotal);
    }
    return NOT_ALLOCATED;
}
END_ALLOC_FUNCTION

BEGIN_ALLOC_FUNCTION(BestFit) {
    // Variables de control
    int largoRuta;
    std::vector<char> ordenBandas;
    std::map<char, std::vector<bool>> estadoSlots;
    int requerido;
    int total;
    int indice;
    int mejorTotal;
    int mejorIndice;
    
    for (size_t r = 0; r < NUMBER_OF_ROUTES; r++){ // RBMLSA: -> R (Por cada ruta).

        // Obtenemos el largo de la ruta y el orden de las bandas.
        largoRuta = 0;
        for (size_t l = 0; l < NUMBER_OF_LINKS(r); l++){ 
            largoRuta += LINK_IN_ROUTE(r, l)->getLength();
        }
        ordenBandas = ORDEN(largoRuta, escenario);

        for (size_t m = 0; m < NUMBER_OF_MODULATIONS; m++){ // RBMLSA: -> ML (Por cada modulación)
            for (size_t b = 0; b < NUMBER_OF_BANDS(r, 0); b++){ // RBMLSA: -> B (Por cada banda).

                // Verificamos si la modulación es factible.
                if (REQ_REACH_BDM(m, REQ_POS_BANDS(m)[ordenBandas[b]]) < largoRuta) continue;

                // Slots requeridos por la modulación y bit rate actual
                requerido = REQ_SLOTS_BDM(m, REQ_POS_BANDS(m)[ordenBandas[b]]);

                // Obtenemos el vector representativo de los slots ocupados en la ruta (S).
                if (estadoSlots.find(ordenBandas[b]) == estadoSlots.end()) {
                    // Inicializamos el vector de slots para la banda actual si no existe en el mapa
                    estadoSlots[ordenBandas[b]] = std::vector<bool>(LINK_IN_ROUTE(r, 0)->getSlots(ordenBandas[b]), false);

                    // Actualizamos el estado de cada slot en función de cada enlace en la ruta
                    for (size_t l = 0; l < NUMBER_OF_LINKS(r); l++) {
                        for (size_t s = 0; s < LINK_IN_ROUTE(r, l)->getSlots(ordenBandas[b]); s++) { // Por cada slot
                            estadoSlots[ordenBandas[b]][s] = estadoSlots[ordenBandas[b]][s] | LINK_IN_ROUTE(r, l)->getSlot(s, ordenBandas[b]);
                        }
                    }
                }

                // Buscamos slots consecutivos libres para la asignación.
                total = 0;
                indice = 0;
                mejorTotal = std::numeric_limits<int>::max();
                mejorIndice = -1;
                for (size_t s = 0; s < estadoSlots[ordenBandas[b]].size(); s++){ // RBMLSA: ->SA (Por cada slot).
                    if (estadoSlots[ordenBandas[b]][s] == false){
                        total++;
                    }
                    else {
                        if (total == requerido){
                            // Asignar slots en caso de encontrar lighpath exacto.
                            for (size_t l = 0; l < NUMBER_OF_LINKS(r); l++){
                                ALLOC_SLOTS_BDM(LINK_IN_ROUTE_ID(r, l), ordenBandas[b], indice, requerido);
                            }
                            conectadosTotal++;
                            bitRateTotal += REQ_BITRATE;
                            if (calculado == false) conexionesPorBanda[ordenBandas[b]]++;
                            return ALLOCATED;
                        }
                        if (total >= requerido && total < mejorTotal){
                            mejorTotal = total;
                            mejorIndice = indice;
                        }
                        total = 0;
                        indice = s + 1;
                    }
                }
                if (total >= requerido && total < mejorTotal){
                    mejorTotal = total;
                    mejorIndice = indice;
                }
                // Asignamos al mejor bloque de slots encontrado.
                if (mejorIndice != -1){
                    for (size_t l = 0; l < NUMBER_OF_LINKS(r); l++){
                        ALLOC_SLOTS_BDM(LINK_IN_ROUTE_ID(r, l), ordenBandas[b], mejorIndice, requerido);
                    }
                    conectadosTotal++;
                    bitRateTotal += REQ_BITRATE;
                    if (calculado == false) conexionesPorBanda[ordenBandas[b]]++;
                    return ALLOCATED;
                }
            }
        }       
    }
    bloqueadosTotal++;
    if (bloqueadosTotal / (bloqueadosTotal+conectadosTotal) >= 0.10 && calculado == false) {
        calculado = true;
        valoresTotales.push_back(bloqueadosTotal+conectadosTotal);
        valoresBitRate.push_back(bitRateTotal);
        valoresConexiones.push_back(conectadosTotal);
    }
    return NOT_ALLOCATED;
}
END_ALLOC_FUNCTION

BEGIN_UNALLOC_CALLBACK_FUNCTION{
}
END_UNALLOC_CALLBACK_FUNCTION

int main(int argc, char* argv[]) {

    // Para las semillas
    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());

    // Listado de nombres de escenarios (primeros dos numeros indican topologia y bitrates)
    std::vector<std::string> encabezados = {
        "00 FirstFit - EuroCore - CL_625",
        "00 BestFit - EuroCore - CL_625",
        "01 FirstFit - EuroCore - CL_125",
        "01 BestFit - EuroCore - CL_125",
        "02 FirstFit - EuroCore - CL_50",
        "02 BestFit - EuroCore - CL_50",
        "03 FirstFit - EuroCore - CLE",
        "03 BestFit - EuroCore - CLE",
        "04 FirstFit - EuroCore - CLS",
        "04 BestFit - EuroCore - CLS",
        "10 FirstFit - UKNet - CL_625",
        "10 BestFit - UKNet - CL_625",
        "11 FirstFit - UKNet - CL_125",
        "11 BestFit - UKNet - CL_125",
        "12 FirstFit - UKNet - CL_50",
        "12 BestFit - UKNet - CL_50",
        "13 FirstFit - UKNet - CLE",
        "13 BestFit - UKNet - CLE",
        "14 FirstFit - UKNet - CLS",
        "14 BestFit - UKNet - CLS",
        "20 FirstFit - NSFNet - CL_625",
        "20 BestFit - NSFNet - CL_625",
        "21 FirstFit - NSFNet - CL_125",
        "21 BestFit - NSFNet - CL_125",
        "22 FirstFit - NSFNet - CL_50",
        "22 BestFit - NSFNet - CL_50",
        "23 FirstFit - NSFNet - CLE",
        "23 BestFit - NSFNet - CLE",
        "24 FirstFit - NSFNet - CLS",
        "24 BestFit - NSFNet - CLS"
    };

    // Configuración por escenario
    std::vector<int> escenarios = {
        CL,CL,CL,CL,CL,CL,CLE,CLE,CLS,CLS,
        CL,CL,CL,CL,CL,CL,CLE,CLE,CLS,CLS,
        CL,CL,CL,CL,CL,CL,CLE,CLE,CLS,CLS
    };

    // Abrir archivo salida
    file.open("./results/results_INCREMENTAL.csv", std::ios_base::app);

    // Archivos topologia y bitrate
    std::vector<std::string> topologias = {
        "./src/topologies/EuroCore_CL_625.json",
        "./src/topologies/EuroCore_CL_625.json",
        "./src/topologies/EuroCore_CL_125.json",
        "./src/topologies/EuroCore_CL_125.json",
        "./src/topologies/EuroCore_CL_50.json",
        "./src/topologies/EuroCore_CL_50.json",
        "./src/topologies/EuroCore_CLE.json",
        "./src/topologies/EuroCore_CLE.json",
        "./src/topologies/EuroCore_CLS.json",
        "./src/topologies/EuroCore_CLS.json",
        "./src/topologies/UKNet_CL_625.json",
        "./src/topologies/UKNet_CL_625.json",
        "./src/topologies/UKNet_CL_125.json",
        "./src/topologies/UKNet_CL_125.json",
        "./src/topologies/UKNet_CL_50.json",
        "./src/topologies/UKNet_CL_50.json",
        "./src/topologies/UKNet_CLE.json",
        "./src/topologies/UKNet_CLE.json",
        "./src/topologies/UKNet_CLS.json",
        "./src/topologies/UKNet_CLS.json",
        "./src/topologies/NSFNet_CL_625.json",
        "./src/topologies/NSFNet_CL_625.json",
        "./src/topologies/NSFNet_CL_125.json",
        "./src/topologies/NSFNet_CL_125.json",
        "./src/topologies/NSFNet_CL_50.json",
        "./src/topologies/NSFNet_CL_50.json",
        "./src/topologies/NSFNet_CLE.json",
        "./src/topologies/NSFNet_CLE.json",
        "./src/topologies/NSFNet_CLS.json",
        "./src/topologies/NSFNet_CLS.json"
    };

    std::vector<std::string> rutas = {
        "./src/topologies/EuroCore_routes.json",
        "./src/topologies/UKNet_routes.json",
        "./src/topologies/NSFNet_routes.json"
    };

    std::vector<std::string> bitrates = {
        "./src/profiles/bitrates_CL_625.json",
        "./src/profiles/bitrates_CL_125.json",
        "./src/profiles/bitrates_CL_50.json",
        "./src/profiles/bitrates_CLE.json",
        "./src/profiles/bitrates_CLS.json"
    };

    for (size_t a = 0; a < 29; a+=2){
        // Header en archivo de salida
        file << encabezados[a].substr(3) << std::endl;

        // Dependiendo de los primeros dos strings de encabezados, se selecciona la topología, el perfil de bitrates y escenario
        std::string topologia = topologias[a];
        std::string ruta = rutas[encabezados[a][0] - '0'];
        std::string bitrate = bitrates[encabezados[a][1] - '0'];
        escenario = escenarios[a];

        // Setear conexiones por banda para metricas
        conexionesPorBanda['C'] = 0;
        conexionesPorBanda['L'] = 0;
        conexionesPorBanda['E'] = 0;
        conexionesPorBanda['S'] = 0;

        for (size_t i = 0; i < 30; i++){
            
            // Crear semillas
            int seedArrive = rng();
            int seedDeparture = rng();
            int seedBitRate = rng();
            int seedDst = rng();
            int seedSrc = rng();

            // Setear variables de CAPACIDAD
            bloqueadosTotal = 0;
            bitRateTotal = 0;
            conectadosTotal = 0;
            calculado = false;

            // Simulacion
            Simulator sim =
                Simulator(topologia,
                    ruta,
                    bitrate,
                    BDM);

            USE_ALLOC_FUNCTION(FirstFit, sim);
            USE_UNALLOC_FUNCTION_BDM(sim);
            sim.setGoalConnections(2e4);
            sim.setLambda(10);
            sim.setMu(1);

            // Set Semillas
            sim.setSeedArrive(seedArrive);
            sim.setSeedDeparture(seedArrive);
            sim.setSeedBitRate(seedBitRate);
            sim.setSeedDst(seedDst);
            sim.setSeedSrc(seedSrc);

            // Simular
            sim.init();
            sim.run();
        }

        // Media
        double mediaTotal = 0;
        double mediaBitRate = 0;
        double mediaConexiones = 0;
        for (size_t i = 0; i < 30; i++){
            mediaTotal += valoresTotales[i];
            mediaBitRate += valoresBitRate[i];
            mediaConexiones += valoresConexiones[i];
        }
        mediaTotal /= 30;
        mediaBitRate /= 30;
        mediaConexiones /= 30;
        // Desviación estándar
        double desviacionTotal = 0;
        double desviacionBitRate = 0;
        double desviacionConexiones = 0;
        for (size_t i = 0; i < 30; i++){
            desviacionTotal += pow(valoresTotales[i] - mediaTotal, 2);
            desviacionBitRate += pow(valoresBitRate[i] - mediaBitRate, 2);
            desviacionConexiones += pow(valoresConexiones[i] - mediaConexiones, 2);
        }
        desviacionTotal = sqrt(desviacionTotal / 30);
        desviacionBitRate = sqrt(desviacionBitRate / 30);
        desviacionConexiones = sqrt(desviacionConexiones / 30);
        double errorEstandarTotal = desviacionTotal / sqrt(30);
        double errorEstandarBitRate = desviacionBitRate / sqrt(30);
        double errorEstandarConexiones = desviacionConexiones / sqrt(30);
        double intervaloTotal = 2.045 * errorEstandarTotal;
        double intervaloBitRate = 2.045 * errorEstandarBitRate;
        double intervaloConexiones = 2.045 * errorEstandarConexiones;

        file << ", " << mediaConexiones << ") +- (" << intervaloConexiones << ", " << intervaloConexiones << ")" << std::endl;
        //file << ", " << mediaTotal << ") +- (" << intervaloTotal << ", " << intervaloTotal << ")" << std::endl;
        file << ", " << mediaBitRate << ") +- (" << intervaloBitRate << ", " << intervaloBitRate << ")" << std::endl;

        // Asignaciones por banda
        file    << "C: ," << conexionesPorBanda['C']/30 << ")\n"
                << "L: ," << conexionesPorBanda['L']/30 << ")\n"
                << "E: ," << conexionesPorBanda['E']/30  << ")\n"
                << "S: ," << conexionesPorBanda['S']/30  << ")"
                << "\n\n"
                << std::endl;

        // Limpiar vectores
        valoresTotales.clear();
        valoresBitRate.clear();
        valoresConexiones.clear();

        // Setear conexiones por banda para metricas
        conexionesPorBanda['C'] = 0;
        conexionesPorBanda['L'] = 0;
        conexionesPorBanda['E'] = 0;
        conexionesPorBanda['S'] = 0;

        
        // Header en archivo de salida
        file << encabezados[a].substr(3) << std::endl;

        // Selecciona escenario
        escenario = escenarios[a+1];
        
        for (size_t i = 0; i < 30; i++){
            
            // Crear semillas
            int seedArrive = rng();
            int seedDeparture = rng();
            int seedBitRate = rng();
            int seedDst = rng();
            int seedSrc = rng();

            // Setear variables de CAPACIDAD
            bloqueadosTotal = 0;
            bitRateTotal = 0;
            conectadosTotal = 0;
            calculado = false;

            // Simulacion
            Simulator sim =
                Simulator(topologia,
                    ruta,
                    bitrate,
                    BDM);

            USE_ALLOC_FUNCTION(BestFit, sim);
            USE_UNALLOC_FUNCTION_BDM(sim);
            sim.setGoalConnections(1e4);
            sim.setLambda(10);
            sim.setMu(1);

            // Set Semillas
            sim.setSeedArrive(seedArrive);
            sim.setSeedDeparture(seedArrive);
            sim.setSeedBitRate(seedBitRate);
            sim.setSeedDst(seedDst);
            sim.setSeedSrc(seedSrc);
            
            // Simular
            sim.init();
            sim.run();
        }

        // Media
        mediaTotal = 0;
        mediaBitRate = 0;
        mediaConexiones = 0;
        for (size_t i = 0; i < 30; i++){
            mediaTotal += valoresTotales[i];
            mediaBitRate += valoresBitRate[i];
            mediaConexiones += valoresConexiones[i];
        }
        mediaTotal /= 30;
        mediaBitRate /= 30;
        mediaConexiones /= 30;
        // Desviación estándar
        desviacionTotal = 0;
        desviacionBitRate = 0;
        desviacionConexiones = 0;
        for (size_t i = 0; i < 30; i++){
            desviacionTotal += pow(valoresTotales[i] - mediaTotal, 2);
            desviacionBitRate += pow(valoresBitRate[i] - mediaBitRate, 2);
            desviacionConexiones += pow(valoresConexiones[i] - mediaConexiones, 2);
        }
        desviacionTotal = sqrt(desviacionTotal / 30);
        desviacionBitRate = sqrt(desviacionBitRate / 30);
        desviacionConexiones = sqrt(desviacionConexiones / 30);
        errorEstandarTotal = desviacionTotal / sqrt(30);
        errorEstandarBitRate = desviacionBitRate / sqrt(30);
        errorEstandarConexiones = desviacionConexiones / sqrt(30);
        intervaloTotal = 2.045 * errorEstandarTotal;
        intervaloBitRate = 2.045 * errorEstandarBitRate;
        intervaloConexiones = 2.045 * errorEstandarConexiones;

        file << ", " << mediaConexiones << ") +- (" << intervaloConexiones << ", " << intervaloConexiones << ")" << std::endl;
        //file << ", " << mediaTotal << ") +- (" << intervaloTotal << ", " << intervaloTotal << ")" << std::endl;
        file << ", " << mediaBitRate << ") +- (" << intervaloBitRate << ", " << intervaloBitRate << ")" << std::endl;

        // Asignaciones por banda
        file    << "C: ," << conexionesPorBanda['C']/30 << ")\n"
                << "L: ," << conexionesPorBanda['L']/30 << ")\n"
                << "E: ," << conexionesPorBanda['E']/30  << ")\n"
                << "S: ," << conexionesPorBanda['S']/30  << ")"
                << "\n\n"
                << std::endl;

        // Limpiar vectores
        valoresTotales.clear();
        valoresBitRate.clear();
        valoresConexiones.clear();

    }

    // Cerramos archivo
    file.close();
    
    return 0;
}