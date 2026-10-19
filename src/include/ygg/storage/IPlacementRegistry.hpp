#pragma once

/**
 * @file IPlacementRegistry.hpp
 * @brief Registre decentralise des annonces de localisation.
 */

#include "ygg/core/Result.hpp"
#include "ygg/storage/Domain.hpp"

namespace ygg::storage {
    
/**
 * @brief Composant IPlacementRegistry du module storage.
 * @details Recherche les annonces d hebergement pour un ShardId sans modifier le manifeste
 * immutable.
 * @par Responsabilite du fichier Annuaire mutable et signe des emplacements.
 * @par Regles importantes Verifier signature du fournisseur, expiration et preuve du fragment avant
 * de considerer une annonce comme fiable.
 * @par Contrat de duree de vie Les dependances recues par reference restent propriete de leur
 * createur; les ressources internes sont liberees au plus tard au destructeur.

 * @par Contrat complementaire initial
 * Mutable signed locations separate from immutable manifests.
 */
class IPlacementRegistry {
public:
    /**
     * @brief Libere les ressources de cette instance conformement aux regles RAII; ne doit pas
     * propager d exception.
     * @post Les ressources acquises ne sont plus accessibles.
     */
    virtual ~IPlacementRegistry() = default;
    /**
     * @brief Recherche les annonces de stockage associees a un fragment, sans faire confiance aux
     * annonces non verifiees.
     * @param shard Descripteur du shard attendu ou shard verifie selon le contexte.
     * @return Coroutine retournant Result<T> ; les echecs reseau/durabilite sont representes par
     * Error.
     * @note Coroutine : aucun verrou bloquant ni vue ByteView empruntee ne doit traverser un
     * co_await.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] virtual Async<std::vector<PlacementClaim>> locations(ShardId shard) = 0;
    /**
     * @brief Publie un emplacement signe du fournisseur apres preuve de durabilite du shard.
     * @param claim Argument fourni par l appelant; validite et contraintes definies par ce contrat.
     * @return Coroutine retournant Result<T> ; les echecs reseau/durabilite sont representes par
     * Error.
     * @note Coroutine : aucun verrou bloquant ni vue ByteView empruntee ne doit traverser un
     * co_await.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] virtual Async<void> announce(PlacementClaim claim) = 0;
};
} // namespace ygg::storage
