#include "../functions.hpp"
#include "../simulator.hpp"
#include <random>
#include <chrono>



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

}
END_UNALLOC_CALLBACK_FUNCTION

int main(int argc, char* argv[]) {

    escenario = CLE;

        // Lista de nombres de archivos a guardar
    std::vector<std::string> archivosSalida = {
        "./results/NSFNet_CLE_BestFit.csv",
        "./results/NSFNet_CLE_FirstFit.csv",
        "./results/EuroCore_CLE_BestFit.csv",
        "./results/EuroCore_CLE_FirstFit.csv",
        "./results/UKNet_CLE_BestFit.csv",
        "./results/UKNet_CLE_FirstFit.csv",
    };

    // Archivos topologia y bitrate
    std::vector<std::string> topologias = {
        "./src/topologies/NSFNet_CLE.json",
        "./src/topologies/NSFNet_CLE.json",
        "./src/topologies/EuroCore_CLE.json",
        "./src/topologies/EuroCore_CLE.json",
        "./src/topologies/UKNet_CLE.json",
        "./src/topologies/UKNet_CLE.json",
    };

    std::vector<std::string> rutas = {
        "./src/topologies/NSFNet_routes.json",
        "./src/topologies/NSFNet_routes.json",
        "./src/topologies/EuroCore_routes.json",
        "./src/topologies/EuroCore_routes.json",
        "./src/topologies/UKNet_routes.json",
        "./src/topologies/UKNet_routes.json",
    };

    // Vector de vectores int con cargas de trafico con el formato {Inicio,Final,Incremento}
    std::vector<std::vector<int>> traficos = {
        {1000, 4000, 250}, // NSFNet
        {1000, 4000, 250},
        {4500, 8000, 500}, // EuroCore
        {4500, 8000, 500},
        {4000, 8000, 500}, // UKNet
        {4000, 8000, 500},
    };

    // Peso para canales de 50 GHz
    pesoBitRate[0] = 1.0 * 0.5;
    pesoBitRate[1] = 1.5 * 0.5;
    pesoBitRate[2] = 3.0 * 0.5;
    pesoBitRate[3] = 2.25 * 0.5;

    // Para las semillas
    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());

    // Por cada arhivo de salida
    for (size_t a = 0; a < 6; a+=2){
        // Se crea el archivo
        file.open(archivosSalida[a], std::ios_base::app);

        // Setear variables de BBP
        for (size_t i = 0; i < 4; i++) {
            totalBitRate[i] = 0.0;
            bloqueadosBitRate[i] = 0.0;
        }

        // Simulacion BESTFIT
        for (int lambda = traficos[a][0]; lambda <= traficos[a][1]; lambda+=traficos[a][2]) {

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
                    "./src/profiles/bitrates_CLE.json",
                    BDM);

            USE_ALLOC_FUNCTION(BestFit, sim);
            USE_UNALLOC_FUNCTION_BDM(sim);
            sim.setGoalConnections(nConexiones);
            sim.setLambda(lambda);
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

            // Guardar datos
            file << lambda << "\t" << sim.getBlockingProbability() << "\t" << sim.wilsonCI() << "\t" << sim.wilsonCI() << "\\\\  " 
                << BBP(totalBitRate, bloqueadosBitRate, pesoBitRate) 
                << std::endl;
        }
        file << std::endl;
        file.close();

        // Archivo salida
        file.open(archivosSalida[a+1], std::ios_base::app);

        // Setear variables de BBP
        for (size_t i = 0; i < 4; i++) {
            totalBitRate[i] = 0.0;
            bloqueadosBitRate[i] = 0.0;
        }

        // Simulacion FIRSTFIT
        for (int lambda = traficos[a+1][0]; lambda <= traficos[a+1][1]; lambda+=traficos[a+1][2]) {

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
                    "./src/profiles/bitrates_CLE.json",
                    BDM);

            USE_ALLOC_FUNCTION(FirstFit, sim);
            USE_UNALLOC_FUNCTION_BDM(sim);
            sim.setGoalConnections(nConexiones);
            sim.setLambda(lambda);
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

            // Guardar datos
            file << lambda << "\t" << sim.getBlockingProbability() << "\t" << sim.wilsonCI() << "\t" << sim.wilsonCI() << "\\\\  " 
                << BBP(totalBitRate, bloqueadosBitRate, pesoBitRate) 
                << std::endl;
        }
        file << std::endl;
        file.close();
    }
    return 0;
}