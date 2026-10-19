#pragma once

/**
 * @file DistinctNodePlanner.hpp
 * @brief Placement initial sur trois noeuds distincts.
 */

#include "ygg/storage/IPlacementPlanner.hpp"

namespace ygg::storage {
    
/**
 * @brief Composant DistinctNodePlanner du module storage.
 * @details Retient les destinations admissibles selon disponibilite et capacite annonces, pour les
 * shards A/B/P.
 * @par Responsabilite du fichier Selection de trois domaines de panne distincts.
 * @par Regles importantes Une capacite annoncee n est pas une reservation; confirmer la place lors
 * du transfert et ne jamais poser deux shards sur le meme domaine.
 * @par Contrat de duree de vie Les dependances recues par reference restent propriete de leur
 * createur; les ressources internes sont liberees au plus tard au destructeur.

 * @par Contrat complementaire initial
 * Fail closed if three independent disk/host domains are unavailable.
 */
class DistinctNodePlanner final : public IPlacementPlanner {
public:
    /**
     * @brief Applique la politique de placement deterministe ou explicable aux candidats
     * compatibles.
     * @param candidates Capacites et domaines de panne annonces par les nœuds eligibles.
     * @param required_bytes_per_node Espace requis sur chacun des trois nœuds distincts.
     * @return Resultat explicite : valeur valide en succes, ErrorCode type en echec.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] Result<Nodes3> select_three(
        /**
         * @brief Champ candidates de DistinctNodePlanner : valeur interne conforme aux invariants
         * de ce type.
         */
        const std::vector<NodeCapacity>& candidates,
        /**
         * @brief Champ override de DistinctNodePlanner : valeur interne conforme aux invariants de
         * ce type.
         */
        std::uint64_t required_bytes_per_node) const override;
};
} // namespace ygg::storage
