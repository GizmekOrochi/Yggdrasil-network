#pragma once

/**
 * @file IManifestStore.hpp
 * @brief Port de publication de manifestes immuables.
 */

#include "ygg/core/Result.hpp"
#include "ygg/storage/Domain.hpp"

namespace ygg::storage {

/**
 * @brief Composant IManifestStore du module storage.
 * @details Publie des manifestes signes sur plusieurs nœuds et fournit leurs copies pour lecture.
 * @par Responsabilite du fichier Port de diffusion et recuperation des manifestes immuables.
 * @par Regles importantes Verifier signatures et correspondance de l identifiant; exiger des recus
 * signés de domaines distincts avant toute certification de durabilite.
 * @par Contrat de duree de vie Les dependances recues par reference restent propriete de leur
 * createur; les ressources internes sont liberees au plus tard au destructeur.

 * @par Contrat complementaire initial
 * Replicated immutable manifest persistence with signed receipts.
 */
class IManifestStore {
public:
    /**
     * @brief Libere les ressources de cette instance conformement aux regles RAII; ne doit pas
     * propager d exception.
     * @post Les ressources acquises ne sont plus accessibles.
     */
    virtual ~IManifestStore() = default;
    /**
     * @brief Publie un manifeste immutable signe et renvoie les receipts durables collectes.
     * @param manifest Manifeste signe de la version et de ses segments.
     * @return Coroutine retournant Result<T> ; les echecs reseau/durabilite sont representes par
     * Error.
     * @post Le nombre de receipts independants doit etre controle par l orchestrateur avant
     * protection du fichier.
     * @note Coroutine : aucun verrou bloquant ni vue ByteView empruntee ne doit traverser un
     * co_await.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] virtual Async<std::vector<ManifestReceipt>> publish(SignedManifest manifest) = 0;
    /**
     * @brief Recupere un manifeste pour l identifiant cible et valide coherence et signature.
     * @param id Identifiant opaque de l objet recherche ou modifie.
     * @return Coroutine retournant Result<T> ; les echecs reseau/durabilite sont representes par
     * Error.
     * @note Coroutine : aucun verrou bloquant ni vue ByteView empruntee ne doit traverser un
     * co_await.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] virtual Async<SignedManifest> fetch(ManifestId id) = 0;
};
} // namespace ygg::storage
