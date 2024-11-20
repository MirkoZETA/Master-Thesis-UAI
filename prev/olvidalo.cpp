#include "main.hpp"
#include "./src/simulator.hpp"

// Temp diccionario de codigos debug:
// std::cout << "DEBUG: 1" << std::endl;

BEGIN_ALLOC_FUNCTION(FF_QoT_EON) {
    
    // Variables auxiliares
    std::vector<bool> totalSlots;
    std::map<char, int> bandIndex;
    int routeLength;
    int numberOfSlots;
    int currentNumberSlots;
    int currentSlotIndex;
    int blockingByGSNR = false;
    
    // RBMLSA: ->R (Por cada ruta).
    for (size_t r = 0; r < NUMBER_OF_ROUTES; r++){
        // RBMLSA: ->B (Por cada banda).
        for (size_t b = 0; b < NUMBER_OF_BANDS(r, 0); b++){

            // Debemos almacenar el estado de cada slot del link l de la ruta r de la banda b en el vector totalSlots.
            // Debido a la restricción de continuidad y contigüidad, podemos comprimir la información en un solo vector.
            totalSlots = std::vector<bool>(LINK_IN_ROUTE(r, 0)->getSlots(bands_order(b)), false);

            routeLength = 0;
            for (size_t l = 0; l < NUMBER_OF_LINKS(r); l++){ // Por cada link.
                routeLength += LINK_IN_ROUTE(r, l)->getLength();
                for (size_t s = 0; s < LINK_IN_ROUTE(r, l)->getSlots(bands_order(b)); s++){ // Por cada slot.
                    totalSlots[s] = totalSlots[s] | LINK_IN_ROUTE(r, l)->getSlot(s, bands_order(b));
                }
            }
            // std::cout << "Distance index: " << find_index_for_distance(routeLength) << std::endl;

            // RBMLSA: ->ML (Por cada modulación)
            for (size_t m = 0; m < NUMBER_OF_MODULATIONS; m++){
                // Debemos hacer la transformación del indice de la banda de como está almacenado en el simulador 
                // a como lo estamos manejando nosotros y en el orden que queremos las bandas.
                // En el Simulador es {C, L, S, E} y nosotros lo queremos en {S/E, L, C}

                bandIndex = REQ_POS_BANDS(m); // TO DO: ¿Por qué requerir la posición de las bandas en base a la modulación?
                                              // El orden debiera ser constante. De hecho no debiera ni existir esta necesidad.

                numberOfSlots = REQ_SLOTS_BDM(m, bandIndex[b]); // TO DO: REQ_SLOTS_BDM debiera recibir el char de la banda,
                                                                // como todas las otras funciones BDM!

                // Encontrar slots consecutivos libres para la asignación.
                // RBMLSA: ->SA (Por cada slot).
                currentNumberSlots = 0;
                currentSlotIndex = 0;

                for (size_t s = 0; s < totalSlots.size(); s++){
                    if (totalSlots[s] == false && get_gsnr_value_slot(routeLength, s, bands_order(b)) > gsnrModulation[m]){
                        currentNumberSlots++;
                    } else {
                        GO_TO:
                        currentNumberSlots = 0;
                        currentSlotIndex = s + 1;
                    }
                    
                    if (currentNumberSlots == numberOfSlots){
                        for (size_t s_candidato = currentSlotIndex; s_candidato < currentSlotIndex + numberOfSlots; s_candidato++){
                            if (get_gsnr_value_slot(routeLength, s_candidato, bands_order(b)) < gsnrModulation[m]){
                                //std::cout << "Slot: " << s_candidato << " - GSNR: " << get_gsnr_value(scenario, routeLength, s_candidato, bands_order(b)) << " - MODULATION: " << gsnrModulation[m] << std::endl;
                                blockingByGSNR = true;
                                currentNumberSlots = 0;
                                currentSlotIndex = s + 1;
                                goto GO_TO;
                            }
                        }
                        // Asignar slots en caso de encontrar lighpath factible.
                        for (size_t l = 0; l < NUMBER_OF_LINKS(r); l++){
                                                                    // TO DO: Esta funcion tambien recibe el char!
                            ALLOC_SLOTS_BDM(LINK_IN_ROUTE_ID(r, l), bands_order_CL[b], currentSlotIndex, REQ_SLOTS_BDM(m, bandIndex[b]));
                        }
                        return ALLOCATED;
                    }
                }
            }
        }        
    }
    if (blockingByGSNR){
        blockedByGSNR++;
    }
    return NOT_ALLOCATED;
}
END_ALLOC_FUNCTION

BEGIN_ALLOC_FUNCTION(FF_EON) {
    
    // Variables auxiliares
    std::vector<bool> totalSlots;
    std::map<char, int> bandIndex;
    int routeLength;
    int numberOfSlots;
    int currentNumberSlots;
    int currentSlotIndex;
    int blockingByGSNR = false;
    
    // RBMLSA: ->R (Por cada ruta).
    for (size_t r = 0; r < NUMBER_OF_ROUTES; r++){
        // RBMLSA: ->B (Por cada banda).
        for (size_t b = 0; b < NUMBER_OF_BANDS(r, 0); b++){

            // Debemos almacenar el estado de cada slot del link l de la ruta r de la banda b en el vector totalSlots.
            // Debido a la restricción de continuidad y contigüidad, podemos comprimir la información en un solo vector.
            totalSlots = std::vector<bool>(LINK_IN_ROUTE(r, 0)->getSlots(bands_order(b)), false);

            routeLength = 0;
            for (size_t l = 0; l < NUMBER_OF_LINKS(r); l++){ // Por cada link.
                routeLength += LINK_IN_ROUTE(r, l)->getLength();
                for (size_t s = 0; s < LINK_IN_ROUTE(r, l)->getSlots(bands_order(b)); s++){ // Por cada slot.
                    totalSlots[s] = totalSlots[s] | LINK_IN_ROUTE(r, l)->getSlot(s, bands_order(b));
                }
            }
            

            // RBMLSA: ->ML (Por cada modulación)
            for (size_t m = 0; m < NUMBER_OF_MODULATIONS; m++){
                // Debemos hacer la transformación del indice de la banda de como está almacenado en el simulador 
                // a como lo estamos manejando nosotros y en el orden que queremos las bandas.
                // En el Simulador es {C, L, S, E} y nosotros lo queremos en {S/E, L, C}

                bandIndex = REQ_POS_BANDS(m); // TO DO: ¿Por qué requerir la posición de las bandas en base a la modulación?
                                              // El orden debiera ser constante. De hecho no debiera ni existir esta necesidad.

                numberOfSlots = REQ_SLOTS_BDM(m, bandIndex[b]); // TO DO: REQ_SLOTS_BDM debiera recibir el char de la banda,
                                                                // como todas las otras funciones BDM!

                // Verificamos si la modulación es factible.
                if (get_gsnr_value_band(routeLength, bands_order(b)) < gsnrModulation[m]){
                    //std::cout << "GSNR: " << get_gsnr_value_band(routeLength, bands_order(b)) << " - MODULATION: " << gsnrModulation[m] << std::endl;
                    continue;
                }

                // Encontrar slots consecutivos libres para la asignación.
                // RBMLSA: ->SA (Por cada slot).
                currentNumberSlots = 0;
                currentSlotIndex = 0;

                for (size_t s = 0; s < totalSlots.size(); s++){
                    if (totalSlots[s] == false){
                        currentNumberSlots++;
                    } else {
                        currentNumberSlots = 0;
                        currentSlotIndex = s + 1;
                    }
                    GO_TO:
                    if (currentNumberSlots == numberOfSlots){
                        // Asignar slots en caso de encontrar lighpath factible.
                        for (size_t l = 0; l < NUMBER_OF_LINKS(r); l++){
                                                                    // TO DO: Esta funcion tambien recibe el char!
                            ALLOC_SLOTS_BDM(LINK_IN_ROUTE_ID(r, l), bands_order_CL[b], currentSlotIndex, REQ_SLOTS_BDM(m, bandIndex[b]));
                        }
                        return ALLOCATED;
                    }
                }
            }
        }        
    }
    if (blockingByGSNR){
        blockedByGSNR++;
    }
    return NOT_ALLOCATED;
}
END_ALLOC_FUNCTION


BEGIN_UNALLOC_CALLBACK_FUNCTION{
    int z; // TO DO: Indignado porque tengo que hacer esto, una locura.
}
END_UNALLOC_CALLBACK_FUNCTION



int main(int argc, char* argv[]) {

    // Simulacion
    for (int lambda = 500; lambda < 5000; lambda+=250) {
        Simulator sim =
            Simulator(std::string("./src/topologies/NSFNet_CL.json"),
                std::string("./src/topologies/NSFNet_routes.json"),
                std::string("./src/profiles/bitrates_flexgrid.json"),
                BDM);

        USE_ALLOC_FUNCTION(FF_QoT_EON, sim);
        USE_UNALLOC_FUNCTION_BDM(sim);
        sim.setGoalConnections(1e4);
        sim.setLambda(lambda);
        sim.setMu(1);

        sim.init();
        sim.run();

        // Save to file
        std::ofstream file;
        file.open("results.csv", std::ios_base::app);
        file << lambda << "\t" << sim.getBlockingProbability() << "\t" << sim.wilsonCI() << "\t" << sim.wilsonCI() << "\tBlocked by GSNR: " << blockedByGSNR << std::endl;
        file.close();
        std::cout << "Lambda: " << lambda << " - Blocking Probability: " << sim.getBlockingProbability() << std::endl;
    }
    return 0;
}