#include "main.hpp"
#include "./src/simulator.hpp"
#include <random>
#include <chrono>

// Temp diccionario de codigos debug:
// std::cout << "DEBUG: 1" << std::endl;

BEGIN_ALLOC_FUNCTION(FirstFit) {
    // Variables de control
    int largoRuta;
    std::vector<char> ordenBandas;
    std::map<char, std::vector<bool>> estadoSlots;
    int requerido;
    int total;
    int indice;

    // Métricas
    totalBitRate[mapaBitRate[REQ_BITRATE]]++;
    
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
                // std::cout << "Bit Rate: " << REQ_BITRATE_STR << " - Modulation: " << REQ_MODULATION(m) << " - Band: " << ordenBandas[b] << " - Slots: " << requerido << std::endl;

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
                        conexionesPorBanda[ordenBandas[b]]++;
                        return ALLOCATED;
                    }
                }
            }
        }       
    }
    bloqueadosBitRate[mapaBitRate[REQ_BITRATE]]++;
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

    // Métricas
    totalBitRate[mapaBitRate[REQ_BITRATE]]++;
    
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
                // std::cout << "Bit Rate: " << REQ_BITRATE_STR << " - Modulation: " << REQ_MODULATION(m) << " - Band: " << ordenBandas[b] << " - Slots: " << requerido << std::endl;

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
                            conexionesPorBanda[ordenBandas[b]]++;
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
                // Asignamos al mejor bloque de slots encontrado.
                if (total >= requerido && total < mejorTotal) {
                    mejorTotal = total;
                    mejorIndice = indice;
                }
                if (mejorIndice != -1){
                    for (size_t l = 0; l < NUMBER_OF_LINKS(r); l++){
                        ALLOC_SLOTS_BDM(LINK_IN_ROUTE_ID(r, l), ordenBandas[b], mejorIndice, requerido);
                    }
                    conexionesPorBanda[ordenBandas[b]]++;
                    return ALLOCATED;
                }
            }
        }       
    }
    bloqueadosBitRate[mapaBitRate[REQ_BITRATE]]++;
    return NOT_ALLOCATED;
}
END_ALLOC_FUNCTION

BEGIN_UNALLOC_CALLBACK_FUNCTION{
    int z; // TO DO: Indignado porque tengo que hacer esto, una locura.
}
END_UNALLOC_CALLBACK_FUNCTION

int main(int argc, char* argv[]) {

    escenario = CL;

        // Lista de nombres de archivos:
    std::vector<std::string> archivosSalida = {
        "./results/NSFNet_CL_50_BestFit.csv",
        "./results/NSFNet_CL_50_FirstFit.csv",
        "./results/EuroCore_CL_50_BestFit.csv",
        "./results/EuroCore_CL_50_FirstFit.csv",
        "./results/UKNet_CL_50_BestFit.csv",
        "./results/UKNet_CL_50_FirstFit.csv",
    };

    // Archivos topologia y bitrate
    std::vector<std::string> topologias = {
        "./src/topologies/NSFNet_CL_50.json",
        "./src/topologies/NSFNet_CL_50.json",
        "./src/topologies/EuroCore_CL_50.json",
        "./src/topologies/EuroCore_CL_50.json",
        "./src/topologies/UKNet_CL_50.json",
        "./src/topologies/UKNet_CL_50.json",
    };

    std::vector<std::string> rutas = {
        "./src/topologies/NSFNet_routes.json",
        "./src/topologies/NSFNet_routes.json",
        "./src/topologies/EuroCore_routes.json",
        "./src/topologies/EuroCore_routes.json",
        "./src/topologies/UKNet_routes.json",
        "./src/topologies/UKNet_routes.json",
    };

    // Vector de vectores int con cargas de trafico con el formato {500,500,500}
    std::vector<std::vector<int>> traficos = {
        {1000, 4000, 250},
        {1000, 4000, 250},
        {2500, 8000, 500},
        {2500, 8000, 500},
        {2500, 8000, 500},
        {2500, 8000, 500},
    };

    // 625
    // pesoBitRate[0] = 3.5 * 0.0625;
    // pesoBitRate[1] = 6.83 * 0.0625;
    // pesoBitRate[2] = 9.83 * 0.0625;
    // pesoBitRate[3] = 13.33 * 0.0625;
    // 125
    // pesoBitRate[0] = 1.83 * 0.125;
    // pesoBitRate[1] = 3.50 * 0.125;
    // pesoBitRate[2] = 5.0 * 0.125;
    // pesoBitRate[3] = 5.50 * 0.125;
    // 50
    pesoBitRate[0] = 1.0 * 0.5;
    pesoBitRate[1] = 1.5 * 0.5;
    pesoBitRate[2] = 3.0 * 0.5;
    pesoBitRate[3] = 2.25 * 0.5;

    // Para las semillas
    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());

    for (size_t a = 0; a < 6; a+=2){
        // Archivo salida
        file.open(archivosSalida[a], std::ios_base::app);

        // Setear variables de BBP
        for (size_t i = 0; i < 4; i++) {
            totalBitRate[i] = 0.0;
            bloqueadosBitRate[i] = 0.0;
        }

        // Simulacion
        for (int lambda = traficos[a][0]; lambda <= traficos[a][1]; lambda+=traficos[a][2]) {

            // Setear conexiones por banda
            conexionesPorBanda['C'] = 0;
            conexionesPorBanda['L'] = 0;
            conexionesPorBanda['E'] = 0;
            conexionesPorBanda['S'] = 0;

            // Crear semillas
            int seedArrive = rng();
            int seedDeparture = rng();
            int seedBitRate = rng();
            int seedDst = rng();
            int seedSrc = rng();

            Simulator sim =
                Simulator(
                    topologias[a],
                    rutas[a],
                    "./src/profiles/bitrates_CL_50.json",
                    BDM);

            USE_ALLOC_FUNCTION(BestFit, sim);
            USE_UNALLOC_FUNCTION_BDM(sim);
            sim.setGoalConnections(1e6);
            sim.setLambda(lambda);
            sim.setMu(1);

            // Set Semillas
            sim.setSeedArrive(seedArrive);
            sim.setSeedDeparture(seedArrive);
            sim.setSeedBitRate(seedBitRate);
            sim.setSeedDst(seedDst);
            sim.setSeedSrc(seedSrc);

            sim.init();
            sim.run();

            // Guardar datos
            file << lambda << "\t" << sim.getBlockingProbability() << "\t" << sim.wilsonCI() << "\t" << sim.wilsonCI() << "\\\\  " 
                << BBP(totalBitRate, bloqueadosBitRate, pesoBitRate) 
                << std::endl;
            // Guardar asignaciones por banda
            // file    << "C: " << conexionesPorBanda['C'] 
            //         << " L: " << conexionesPorBanda['L'] 
            //         << " E: " << conexionesPorBanda['E'] 
            //         << " S: " << conexionesPorBanda['S'] 
            //         << std::endl;
        }
        file.close();

        // Archivo salida
        file.open(archivosSalida[a+1], std::ios_base::app);

        // Setear variables de BBP
        for (size_t i = 0; i < 4; i++) {
            totalBitRate[i] = 0.0;
            bloqueadosBitRate[i] = 0.0;
        }

        // Simulacion
        for (int lambda = traficos[a+1][0]; lambda <= traficos[a+1][1]; lambda+=traficos[a+1][2]) {

            // Setear conexiones por banda
            conexionesPorBanda['C'] = 0;
            conexionesPorBanda['L'] = 0;
            conexionesPorBanda['E'] = 0;
            conexionesPorBanda['S'] = 0;

            // Crear semillas
            int seedArrive = rng();
            int seedDeparture = rng();
            int seedBitRate = rng();
            int seedDst = rng();
            int seedSrc = rng();

            Simulator sim =
                Simulator(
                    topologias[a+1],
                    rutas[a+1],
                    "./src/profiles/bitrates_CL_50.json",
                    BDM);

            USE_ALLOC_FUNCTION(FirstFit, sim);
            USE_UNALLOC_FUNCTION_BDM(sim);
            sim.setGoalConnections(1e6);
            sim.setLambda(lambda);
            sim.setMu(1);

            // Set Semillas
            sim.setSeedArrive(seedArrive);
            sim.setSeedDeparture(seedArrive);
            sim.setSeedBitRate(seedBitRate);
            sim.setSeedDst(seedDst);
            sim.setSeedSrc(seedSrc);


            sim.init();
            sim.run();

            // Guardar datos
            file << lambda << "\t" << sim.getBlockingProbability() << "\t" << sim.wilsonCI() << "\t" << sim.wilsonCI() << "\\\\  " 
                << BBP(totalBitRate, bloqueadosBitRate, pesoBitRate) 
                << std::endl;
            // Guardar asignaciones por banda
            // file    << "C: " << conexionesPorBanda['C'] 
            //         << " L: " << conexionesPorBanda['L'] 
            //         << " E: " << conexionesPorBanda['E'] 
            //         << " S: " << conexionesPorBanda['S'] 
            //         << std::endl;
        }
        file.close();
    }
    return 0;
}