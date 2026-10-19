#pragma once

/**
 * @file DiskShardStore.hpp
 * @brief Stockage physique atomique et durable des shards.
 */

#include "ygg/storage/IShardStore.hpp"
#include "ygg/identity/NodeIdentity.hpp"

namespace ygg::storage {
    
/**
 * @brief Composant DiskShardStore du module storage.
 * @details Recoit les chunks en zone temporaire, verifie longueurs et hash, applique les quotas
 * puis effectue une publication atomique.
 * @par Responsabilite du fichier Adaptateur de persistance locale des fragments.
 * @par Regles importantes Signer un receipt uniquement apres fsync du fichier et du repertoire.
 * Aucun ecrasement de shard existant avec contenu different.
 * @par Contrat de duree de vie Les dependances recues par reference restent propriete de leur
 * createur; les ressources internes sont liberees au plus tard au destructeur.

 * @par Contrat complementaire initial
 * Filesystem shard store with safe paths, atomic rename and quotas. Concurrent requests for the
 * same shard ID must be idempotent for matching bytes and rejected for conflicting hashes.
 */
class DiskShardStore final : public IShardStore {
public:
    /**
     * @brief Definit repertoire racine, quota physique et identite signataire des reçus durables.
     * @note Les references injectees ne sont pas possedees; leurs objets doivent survivre a cette
     * instance.
     * @param root Repertoire du depot de fragments.
     * @param quota_bytes Quota de disque reserve a ce store.
     * @param signer Identite locale autorisee a signer les receipts durables.
     */
    DiskShardStore(Path root, std::uint64_t quota_bytes,const identity::NodeIdentity& signer);
    /**
     * @brief Ecrit le flux de chunks sur disque de facon atomique et signe seulement apres
     * durabilite.
     * @param shard Descripteur du shard attendu ou shard verifie selon le contexte.
     * @param chunks Lecteur possede et asynchrone des chunks de shard.
     * @return Coroutine retournant Result<T> ; les echecs reseau/durabilite sont representes par
     * Error.
     * @note Coroutine : aucun verrou bloquant ni vue ByteView empruntee ne doit traverser un
     * co_await.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] Async<DurableReceipt> stage(ShardDescriptor shard, std::shared_ptr<IChunkReader> chunks) override;
    /**
     * @brief Effectue la lecture partielle selon offset et count apres validation des bornes.
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
    [[nodiscard]] Async<Bytes> read(ShardId id, std::uint64_t offset, std::uint32_t count) override;
    /**
     * @brief Recherche uniquement les shards completes et valides du depot.
     * @param id Identifiant opaque de l objet recherche ou modifie.
     * @return Coroutine retournant Result<T> ; les echecs reseau/durabilite sont representes par
     * Error.
     * @note Coroutine : aucun verrou bloquant ni vue ByteView empruntee ne doit traverser un
     * co_await.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] Async<bool> has(ShardId id) override;
private:
    /**
     * @brief Chemin racine du depot opaque de shards.
     */
    Path root_{};
    /**
     * @brief Plafond de stockage physique impose a ce depot.
     */
    std::uint64_t quota_bytes_{};
    /**
     * @brief Identite de noeud referencee pour signer les receipts.
     */
    const identity::NodeIdentity& signer_;
};
} // namespace ygg::storage
