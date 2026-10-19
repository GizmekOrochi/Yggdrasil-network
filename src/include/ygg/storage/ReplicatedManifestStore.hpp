#pragma once

/**
 * @file ReplicatedManifestStore.hpp
 * @brief Replicateur de manifestes et leurs confirmations durables.
 */

#include "ygg/storage/IManifestStore.hpp"
#include "ygg/protocol/MessageRouter.hpp"

namespace ygg::storage {

/**
 * @brief Composant ReplicatedManifestStore du module storage.
 * @details Repique trois copies signees d un manifeste, puis recupere une copie qui correspond
 * exactement a son ID.
 * @par Responsabilite du fichier Adaptateur de manifestes immuables répliqués.
 * @par Regles importantes Ne retourner des receipts durables qu apres ecriture confirmee de
 * destinations independantes.
 * @par Contrat de duree de vie Les dependances recues par reference restent propriete de leur
 * createur; les ressources internes sont liberees au plus tard au destructeur.

 * @par Contrat complementaire initial
 * Persist manifests on three independent physical failure domains.
 */
class ReplicatedManifestStore final : public IManifestStore {
public:
    /**
     * @brief Memorise le routeur utilise pour la lecture et diffusion des manifestes.
     * @note Les references injectees ne sont pas possedees; leurs objets doivent survivre a cette
     * instance.
     * @param rpc Routeur RPC partage pour les manifestes.
     */
    explicit ReplicatedManifestStore(protocol::MessageRouter& rpc);
    /**
     * @brief Repique sur trois domaines de panne et collecte leurs accusés durables.
     * @param manifest Manifeste signe de la version et de ses segments.
     * @return Coroutine retournant Result<T> ; les echecs reseau/durabilite sont representes par
     * Error.
     * @note Coroutine : aucun verrou bloquant ni vue ByteView empruntee ne doit traverser un
     * co_await.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] Async<std::vector<ManifestReceipt>> publish(SignedManifest manifest) override;
    /**
     * @brief Recupere une copie verifiee du manifeste immutable parmi les pairs connus.
     * @param id Identifiant opaque de l objet recherche ou modifie.
     * @return Coroutine retournant Result<T> ; les echecs reseau/durabilite sont representes par
     * Error.
     * @note Coroutine : aucun verrou bloquant ni vue ByteView empruntee ne doit traverser un
     * co_await.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] Async<SignedManifest> fetch(ManifestId id) override;
private:
    /**
     * @brief Routeur RPC injecte pour contacter les nœuds du cluster.
     */
    protocol::MessageRouter& rpc_;
};
} // namespace ygg::storage
