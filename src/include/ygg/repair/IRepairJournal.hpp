#pragma once

/**
 * @file IRepairJournal.hpp
 * @brief Port de journal des reparations idempotentes.
 */

#include "ygg/core/Result.hpp"
#include "ygg/repair/RepairTypes.hpp"

namespace ygg::repair {
    
/**
 * @brief Composant IRepairJournal du module repair.
 * @details Enregistre l etat durable de chaque regeneration via une cle d idempotence.
 * @par Responsabilite du fichier Port de journal de progression des reparations.
 * @par Regles importantes Proteger contre plusieurs workers traitant simultanement le meme shard;
 * ne publier une annonce qu apres reception durable.
 * @par Contrat de duree de vie Les dependances recues par reference restent propriete de leur
 * createur; les ressources internes sont liberees au plus tard au destructeur.

 * @par Contrat complementaire initial
 * Durable idempotency journal keyed by repair identity.
 */
class IRepairJournal {
public:
    /**
     * @brief Libere les ressources de cette instance conformement aux regles RAII; ne doit pas
     * propager d exception.
     * @post Les ressources acquises ne sont plus accessibles.
     */
    virtual ~IRepairJournal() = default;
    /**
     * @brief Persiste l etat d une reparation identifiee par une cle stable et empêche les retours
     * en arriere invalides.
     * @param repair_key Identifiant d idempotence canonique de cette reparation.
     * @param state Etat durable a enregistrer ou decrire.
     * @return Resultat explicite : valeur valide en succes, ErrorCode type en echec.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] virtual Result<void> record(Digest repair_key, RepairState state) = 0;
    /**
     * @brief Retourne le dernier etat durable pour cette cle, ou une erreur si elle n existe pas.
     * @param repair_key Identifiant d idempotence canonique de cette reparation.
     * @return Resultat explicite : valeur valide en succes, ErrorCode type en echec.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] virtual Result<RepairState> lookup(Digest repair_key) const = 0;
};
} // namespace ygg::repair
