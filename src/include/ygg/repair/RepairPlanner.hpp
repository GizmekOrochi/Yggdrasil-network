#pragma once

/**
 * @file RepairPlanner.hpp
 * @brief Planification apres delai de grace et sur domains distincts.
 */

#include "ygg/repair/RepairTypes.hpp"
#include "ygg/storage/IPlacementPlanner.hpp"
#include "ygg/core/Clock.hpp"
#include <optional>

namespace ygg::repair {

/**
 * @brief Decide SI et OU reparer, mais ne transfere aucune donnée.
 * @details Respecte les delais de grace et les domaines de panne; pas de reparation si moins de
 * deux shards sains.
 * @par Responsabilite du fichier Selection des segments a regenerer.
 * @par Regles importantes Une regeneration XOR 2+1 exige deux indices sains distincts. Ne jamais
 * recreer les trois shards sur le meme ordinateur.
 * @par Contrat de duree de vie Les dependances recues par reference restent propriete de leur
 * createur; les ressources internes sont liberees au plus tard au destructeur.

 * @par Contrat complementaire initial
 * Decide whether a degraded segment requires repair after grace period. Repairs require two
 * healthy source shares and an unused independent node.
 */
class RepairPlanner final {
public:
    /**
     * @brief Prepare la politique de delai de grace et de choix de placement independant.
     * @note Les references injectees ne sont pas possedees; leurs objets doivent survivre a cette
     * instance.
     * @param grace_period Duree minimale de deconnexion avant d envisager une reparation.
     * @param placement Selecteur de destinations independantes injecte.
     * @param clock Horloge monotone partagee pour les delais.
     */
    RepairPlanner(Duration grace_period, storage::IPlacementPlanner& placement, const core::IClock& clock);
    /**
     * @brief Retourne un plan pour un shard manquant seulement avec 2 shards sains et une cible
     * admissible.
     * @param report Etat d un segment documentant quels shards restent utilisables.
     * @param nodes Candidats de stockage eligibles selon capacites et domaines.
     * @return Resultat explicite : valeur valide en succes, ErrorCode type en echec.
     * @note Il faut mesurer depuis combien de temps le nœud est absent; cette duree ne figure pas
     * encore dans HealthReport.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] Result<std::optional<RepairPlan>> plan(const HealthReport& report, const std::vector<storage::NodeCapacity>& nodes);
private:
    /**
     * @brief Delai avant declenchement d une regeneration adaptee.
     */
    Duration grace_period_{};
    /**
     * @brief Planificateur de destinations externes.
     */
    storage::IPlacementPlanner& placement_;
    /**
     * @brief Horloge monotone empruntee pour decisions reproductibles.
     */
    const core::IClock& clock_;
};
} // namespace ygg::repair
