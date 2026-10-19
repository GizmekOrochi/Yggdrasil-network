#pragma once

/**
 * @file IRemoteShardClient.hpp
 * @brief Port RPC des shards chiffrés.
 */

#include "ygg/storage/IShardStore.hpp"

namespace ygg::storage {

/**
 * @brief Composant IRemoteShardClient du module storage.
 * @details Transfere les shards vers les pairs et lit des plages de bytes sans exposer la DEK.
 * @par Responsabilite du fichier Port RPC d acces a des fragments distants.
 * @par Regles importantes Envoi par chunks au plus 256 Kio, receipts signes et erreurs explicites
 * si un pair disparaît.
 * @par Contrat de duree de vie Les dependances recues par reference restent propriete de leur
 * createur; les ressources internes sont liberees au plus tard au destructeur.

 * @par Contrat complementaire initial
 * Authenticated RPC for read/write of encrypted shard chunks. The remote peer never receives or
 * requires a DEK. Bulk messages obey 256-KiB chunking and do not hold entire files in memory.
 */
class IRemoteShardClient {
public:
    /**
     * @brief Libere les ressources de cette instance conformement aux regles RAII; ne doit pas
     * propager d exception.
     * @post Les ressources acquises ne sont plus accessibles.
     */
    virtual ~IRemoteShardClient() = default;
    /**
     * @brief Transmet par chunks un fragment chiffre au pair et retourne son receipt signe et
     * durable.
     * @param target PairId de destination ou cible du travail selon le contexte.
     * @param shard Descripteur du shard attendu ou shard verifie selon le contexte.
     * @param chunks Lecteur possede et asynchrone des chunks de shard.
     * @return Coroutine retournant Result<T> ; les echecs reseau/durabilite sont representes par
     * Error.
     * @note Coroutine : aucun verrou bloquant ni vue ByteView empruntee ne doit traverser un
     * co_await.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] virtual Async<DurableReceipt> put(PeerId target, ShardDescriptor shard, std::shared_ptr<IChunkReader> chunks) = 0;
    /**
     * @brief Telecharge une plage de fragment chiffre depuis le pair explicitement choisi.
     * @param host Nœud detenant le shard demande.
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
    [[nodiscard]] virtual Async<Bytes> get_chunk(PeerId host, ShardId id, std::uint64_t offset, std::uint32_t count) = 0;
};
} // namespace ygg::storage
