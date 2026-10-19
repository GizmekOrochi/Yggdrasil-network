#pragma once

/**
 * @file LocalApiServer.hpp
 * @brief Serveur local de commandes du daemon.
 */

#include "ygg/daemon/LocalApi.hpp"
#include "ygg/protocol/MessageRouter.hpp"

namespace ygg::daemon {

/**
 * @brief Frontiere de securite entre utilisateur local et services du demon.
 * @details Ne pas accepter de connexions TCP distantes ni exposer des clefs; imposer des
 * permissions sur le socket Unix.
 * @par Responsabilite du fichier Serveur de controle local du demon.
 * @par Regles importantes Verifier droits du socket, credentiels du client selon les besoins,
 * taille des requetes et absence de fuite de cles.
 * @par Contrat de duree de vie Les dependances recues par reference restent propriete de leur
 * createur; les ressources internes sont liberees au plus tard au destructeur.

 * @par Contrat complementaire initial
 * Local Unix-domain command server with filesystem ACL enforcement. This interface is not
 * exposed on the P2P network and may never return unencrypted secrets to unauthenticated local
 * clients.
 */
class LocalApiServer final {
public:
    /**
     * @brief Associe le chemin du socket Unix et le routeur des services locaux du demon.
     * @note Les references injectees ne sont pas possedees; leurs objets doivent survivre a cette
     * instance.
     * @param unix_socket Chemin restreint du socket de controle local.
     * @param services Routeur local injecte; le serveur ne possede pas cet objet.
     */
    LocalApiServer(Path unix_socket, protocol::MessageRouter& services);
    /**
     * @brief Cree et ecoute le socket Unix avec permissions restrictives et routage borne.
     * @return Coroutine retournant Result<T> ; les echecs reseau/durabilite sont representes par
     * Error.
     * @note Coroutine : aucun verrou bloquant ni vue ByteView empruntee ne doit traverser un
     * co_await.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] Async<void> listen();
    /**
     * @brief Valide et dispatch une commande locale en renvoyant une LocalResponse typée.
     * @param command Commande locale a executer.
     * @return Coroutine retournant Result<T> ; les echecs reseau/durabilite sont representes par
     * Error.
     * @warning Les ACL du socket Unix sont distinctes des permissions applicatives pour get_file et
     * trust_approve.
     * @note Coroutine : aucun verrou bloquant ni vue ByteView empruntee ne doit traverser un
     * co_await.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] Async<LocalResponse> handle(LocalCommand command);
    /**
     * @brief Cesse d accepter des clients locaux et ferme le socket de facon idempotente.
     */
    void stop() noexcept;
private:
    /**
     * @brief Chemin prive du socket Unix desservi par le daemon.
     */
    Path unix_socket_{};
    /**
     * @brief Routeur de services non possede par le serveur local.
     */
    protocol::MessageRouter& services_;
};

} // namespace ygg::daemon
