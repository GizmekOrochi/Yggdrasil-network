#pragma once

/**
 * @file IEncryptedCatalogBackup.hpp
 * @brief Port des sauvegardes chiffrées du catalogue.
 */

#include "ygg/core/Result.hpp"
#include "ygg/core/Ids.hpp"
#include "ygg/core/Types.hpp"

namespace ygg::storage {
    
/**
 * @brief Composant IEncryptedCatalogBackup du module storage.
 * @details Stocke et recupere des snapshots deja chiffres depuis des machines independantes.
 * @par Responsabilite du fichier Port de sauvegarde restaureable du catalogue prive.
 * @par Regles importantes L identifiant du snapshot et les secrets de restauration doivent survivre
 * a la disparition du PC source.
 * @par Contrat de duree de vie Les dependances recues par reference restent propriete de leur
 * createur; les ressources internes sont liberees au plus tard au destructeur.

 * @par Contrat complementaire initial
 * Recoverable replication of a fully encrypted private catalog snapshot. The owner must preserve
 * recovery secrets and a valid snapshot reference separately from the source computer.
 */
class IEncryptedCatalogBackup {
public:
    /**
     * @brief Libere les ressources de cette instance conformement aux regles RAII; ne doit pas
     * propager d exception.
     * @post Les ressources acquises ne sont plus accessibles.
     */
    virtual ~IEncryptedCatalogBackup() = default;
    /**
     * @brief Repique un snapshot deja chiffre sur plusieurs machines et retourne un ID restaurable.
     * @param encrypted_snapshot Sauvegarde privee deja chiffree du catalogue.
     * @return Coroutine retournant Result<T> ; les echecs reseau/durabilite sont representes par
     * Error.
     * @note Coroutine : aucun verrou bloquant ni vue ByteView empruntee ne doit traverser un
     * co_await.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] virtual Async<SnapshotId> publish(Bytes encrypted_snapshot) = 0;
    /**
     * @brief Recupere une copie complete et integre du snapshot chiffre via son ID.
     * @param id Identifiant opaque de l objet recherche ou modifie.
     * @return Coroutine retournant Result<T> ; les echecs reseau/durabilite sont representes par
     * Error.
     * @note Coroutine : aucun verrou bloquant ni vue ByteView empruntee ne doit traverser un
     * co_await.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] virtual Async<Bytes> restore(SnapshotId id) = 0;
};
} // namespace ygg::storage
