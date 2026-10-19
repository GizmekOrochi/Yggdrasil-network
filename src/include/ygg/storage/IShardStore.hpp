#pragma once

/**
 * @file IShardStore.hpp
 * @brief Port de persistance locale des shards en flux.
 */

#include "ygg/core/Result.hpp"
#include "ygg/storage/Domain.hpp"
#include <memory>
#include <optional>

namespace ygg::storage {

/**
 * @brief Composant IChunkReader du module storage.
 * @details Fournit la persistance durable d un fragment opaque verifie et une lecture partielle par
 * ID.
 * @par Responsabilite du fichier Port de depot de shards local et de lecture par plages.
 * @par Regles importantes Le nœud de stockage ne connait ni la DEK ni le fichier clair. Interdire
 * les dépassements de quotas et les receipts anticipes.
 * @par Contrat de duree de vie Les dependances recues par reference restent propriete de leur
 * createur; les ressources internes sont liberees au plus tard au destructeur.

 * @par Contrat complementaire initial
 * Owned stream of chunks; EOF represented by std::nullopt.
 */
class IChunkReader {
public:
    /**
     * @brief Libere les ressources de cette instance conformement aux regles RAII; ne doit pas
     * propager d exception.
     * @post Les ressources acquises ne sont plus accessibles.
     */
    virtual ~IChunkReader() = default;
    /**
     * @brief Renvoie le prochain chunk possede, ou nullopt en fin de flux, sans jamais exceder la
     * limite configuree.
     * @return Coroutine retournant Result<T> ; les echecs reseau/durabilite sont representes par
     * Error.
     * @note Coroutine : aucun verrou bloquant ni vue ByteView empruntee ne doit traverser un
     * co_await.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] virtual Async<std::optional<Bytes>> next() = 0;
};

/**
 * @brief Composant IShardStore du module storage.
 * @details Fournit la persistance durable d un fragment opaque verifie et une lecture partielle par
 * ID.
 * @par Responsabilite du fichier Port de depot de shards local et de lecture par plages.
 * @par Regles importantes Le nœud de stockage ne connait ni la DEK ni le fichier clair. Interdire
 * les dépassements de quotas et les receipts anticipes.
 * @par Contrat de duree de vie Les dependances recues par reference restent propriete de leur
 * createur; les ressources internes sont liberees au plus tard au destructeur.

 * @par Contrat complementaire initial
 * Local durable shard store, keyed by opaque shard identifiers. Implementations enforce quotas,
 * verify full hash/size, fsync file and containing directory, and issue receipts only after
 * durable commit.
 */
class IShardStore {
public:
    /**
     * @brief Libere les ressources de cette instance conformement aux regles RAII; ne doit pas
     * propager d exception.
     * @post Les ressources acquises ne sont plus accessibles.
     */
    virtual ~IShardStore() = default;
    /**
     * @brief Ecrit le shard en staging, controle digest/longueur/quota, fsync puis rend un receipt
     * signe.
     * @param shard Descripteur du shard attendu ou shard verifie selon le contexte.
     * @param chunks Lecteur possede et asynchrone des chunks de shard.
     * @return Coroutine retournant Result<T> ; les echecs reseau/durabilite sont representes par
     * Error.
     * @post DurableReceipt ne peut etre emis qu apres validation du SHA-256 ET fsync du fichier et
     * du repertoire.
     * @note Coroutine : aucun verrou bloquant ni vue ByteView empruntee ne doit traverser un
     * co_await.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] virtual Async<DurableReceipt> stage(ShardDescriptor shard, std::shared_ptr<IChunkReader> chunks) = 0;
    /**
     * @brief Retourne la plage demandee d un shard publie, jamais un shard en staging.
     * @param id Identifiant opaque de l objet recherche ou modifie.
     * @param offset Position de debut, en octets, du fragment ou transfert.
     * @param count Nombre d octets souhaites pour la plage, borne a 256 Kio en transfert.
     * @return Coroutine retournant Result<T> ; les echecs reseau/durabilite sont representes par
     * Error.
     * @note Coroutine : aucun verrou bloquant ni vue ByteView empruntee ne doit traverser un
     * co_await.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] virtual Async<Bytes> read(ShardId id, std::uint64_t offset, std::uint32_t count) = 0;
    /**
     * @brief Indique si le shard existe dans l etat durable valide du store.
     * @param id Identifiant opaque de l objet recherche ou modifie.
     * @return Coroutine retournant Result<T> ; les echecs reseau/durabilite sont representes par
     * Error.
     * @note Coroutine : aucun verrou bloquant ni vue ByteView empruntee ne doit traverser un
     * co_await.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] virtual Async<bool> has(ShardId id) = 0;
};
} // namespace ygg::storage
