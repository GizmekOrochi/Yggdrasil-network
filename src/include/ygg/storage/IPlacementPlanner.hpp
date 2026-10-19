#pragma once

/**
 * @file IPlacementPlanner.hpp
 * @brief Interface de selection de domaines de panne distincts.
 */

#include "ygg/core/Result.hpp"
#include "ygg/storage/Domain.hpp"

namespace ygg::storage {
    
/**
 * @brief Composant IPlacementPlanner du module storage.
 * @details Choisit trois destinations physiques independantes pour un segment encode 2+1.
 * @par Responsabilite du fichier Port de selection des nœuds hebergeant les shards.
 * @par Regles importantes En cas de candidats insuffisants, refuser une protection degradee plutot
 * que de revendiquer une tolerance a la panne inexistante.
 * @par Contrat de duree de vie Les dependances recues par reference restent propriete de leur
 * createur; les ressources internes sont liberees au plus tard au destructeur.

 * @par Contrat complementaire initial
 * Choose exactly three distinct physical failure domains.
 */
class IPlacementPlanner {
public:
    /**
     * @brief Libere les ressources de cette instance conformement aux regles RAII; ne doit pas
     * propager d exception.
     * @post Les ressources acquises ne sont plus accessibles.
     */
    virtual ~IPlacementPlanner() = default;
    /**
     * @brief Selectionne trois PairId dans trois domaines de panne differents et de capacite
     * suffisante.
     * @param candidates Capacites et domaines de panne annonces par les nœuds eligibles.
     * @param required_bytes_per_node Espace requis sur chacun des trois nœuds distincts.
     * @return Resultat explicite : valeur valide en succes, ErrorCode type en echec.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] virtual Result<Nodes3> select_three(const std::vector<NodeCapacity>& candidates, std::uint64_t required_bytes_per_node) const = 0;
};
} // namespace ygg::storage
