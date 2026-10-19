#pragma once

/**
 * @file RpcShardClient.hpp
 * @brief Adaptateur RPC du service de stockage.
 */

#include "ygg/storage/IRemoteShardClient.hpp"
#include "ygg/protocol/MessageRouter.hpp"

namespace ygg::storage {
    
/**
 * @brief Composant RpcShardClient du module storage.
 * @details Encapsule la mise en depot et les lectures partielles de shards via MessageRouter.
 * @par Responsabilite du fichier Adaptateur entre API des shards et requetes YGG/1.
 * @par Regles importantes Propager authentification, timeouts, backpressure et validation des
 * receipts sans fuite de cles de lecture.
 * @par Contrat de duree de vie Les dependances recues par reference restent propriete de leur
 * createur; les ressources internes sont liberees au plus tard au destructeur.

 * @par Contrat complementaire initial
 * Bridge from storage shard operations to generic YGG/1 RPC.
 */
class RpcShardClient final : public IRemoteShardClient {
public:
    /**
     * @brief Reference le routeur RPC generique qui transportera les operations de stockage.
     * @note Les references injectees ne sont pas possedees; leurs objets doivent survivre a cette
     * instance.
     * @param router Routeur RPC generique injecte.
     */
    explicit RpcShardClient(protocol::MessageRouter& router);
    /**
     * @brief Mappe un stockage de shard sur des echanges YGG/1 bornes et le receipt de retour.
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
    [[nodiscard]] Async<DurableReceipt> put(PeerId target, ShardDescriptor shard, std::shared_ptr<IChunkReader> chunks) override;
    /**
     * @brief Mappe une lecture partielle distante sur RPC en imposant l intervalle demande.
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
    [[nodiscard]] Async<Bytes> get_chunk(PeerId host, ShardId id, std::uint64_t offset, std::uint32_t count) override;
private:
    /**
     * @brief Routeur RPC injecte, sans dependance a un transport concret.
     */
    protocol::MessageRouter& router_;
};
} // namespace ygg::storage
