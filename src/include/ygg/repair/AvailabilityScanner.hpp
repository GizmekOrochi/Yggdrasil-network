#pragma once

/**
 * @file AvailabilityScanner.hpp
 * @brief Analyse des fragments encore disponibles par segment.
 */

#include "ygg/repair/RepairTypes.hpp"
#include "ygg/storage/IPlacementRegistry.hpp"

namespace ygg::repair {

/**
 * @brief Composant AvailabilityScanner du module repair.
 * @details Compare manifeste signe et positions annoncees pour trouver les indices de shards encore
 * recuperables.
 * @par Responsabilite du fichier Diagnostic de la sante de chaque segment.
 * @par Regles importantes Une deconnexion transitoire ne signifie pas perte definitive; verifier
 * les hash et la preuve de possession si disponible.
 * @par Contrat de duree de vie Les dependances recues par reference restent propriete de leur
 * createur; les ressources internes sont liberees au plus tard au destructeur.

 * @par Contrat complementaire initial
 * Evaluate EACH segment using signed claims and verified shard health. Unreachable peers are not
 * automatically classified as permanently lost.
 */
class AvailabilityScanner final {
public:
    /**
     * @brief Reference le registre mutable des emplacements de fragments.
     * @note Les references injectees ne sont pas possedees; leurs objets doivent survivre a cette
     * instance.
     * @param registry Registre des placements mutable injecte.
     */
    explicit AvailabilityScanner(storage::IPlacementRegistry& registry);
    /**
     * @brief Evalue chaque segment selon indices de shards distincts, annonces valides et etat des
     * pairs.
     * @param manifest Manifeste signe de la version et de ses segments.
     * @return Coroutine retournant Result<T> ; les echecs reseau/durabilite sont representes par
     * Error.
     * @note Coroutine : aucun verrou bloquant ni vue ByteView empruntee ne doit traverser un
     * co_await.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] Async<std::vector<HealthReport>> scan(storage::SignedManifest manifest);
private:
    /**
     * @brief Registre d emplacements injecte, sans propriete transfert.
     */
    storage::IPlacementRegistry& registry_;
};
} // namespace ygg::repair
