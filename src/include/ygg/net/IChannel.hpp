#pragma once
/**
 * @file IChannel.hpp
 * @brief Port de flux d octets chiffre et authentifie.
 * @details Cache TCP/TLS a la couche superieure tout en exposant lecture exacte, ecriture complete
 * et identite verifiee.
 * @warning Tous les buffers asynchrones sont possedes; fermeture et EOF doivent annuler les
 * operations pendantes proprement.
 */


#include "ygg/core/Ids.hpp"
#include "ygg/core/Result.hpp"
#include "ygg/core/Types.hpp"
#include <cstddef>

namespace ygg::net {

/**
 * @brief Composant IChannel du module net.
 * @details Cache TCP/TLS a la couche superieure tout en exposant lecture exacte, ecriture complete
 * et identite verifiee.
 * @par Responsabilite du fichier Port de flux d octets chiffre et authentifie.
 * @par Regles importantes Tous les buffers asynchrones sont possedes; fermeture et EOF doivent
 * annuler les operations pendantes proprement.
 * @par Contrat de duree de vie Les dependances recues par reference restent propriete de leur
 * createur; les ressources internes sont liberees au plus tard au destructeur.

 * @par Contrat complementaire initial
 * Authenticated ordered byte channel; owns all data crossing co_await. A channel is not a file
 * service. Its peer identity is established by TLS.
 */
class IChannel {
public:
    /**
     * @brief Libere les ressources de cette instance conformement aux regles RAII; ne doit pas
     * propager d exception.
     * @post Les ressources acquises ne sont plus accessibles.
     */
    virtual ~IChannel() = default;
    /**
     * @brief Expose l identite effectivement prouvee durant le handshake TLS, jamais une
     * auto-declaration HELLO.
     * @return Identite stable du pair, issue des preuves cryptographiques.
     */
    [[nodiscard]] virtual PeerId verified_peer() const noexcept = 0;
    /**
     * @brief Lit exactement count bytes, en assemblant les lectures TCP ou echoue clairement en cas
     * d EOF.
     * @param count Nombre d octets souhaites pour la plage, borne a 256 Kio en transfert.
     * @return Coroutine retournant Result<T> ; les echecs reseau/durabilite sont representes par
     * Error.
     * @note Coroutine : aucun verrou bloquant ni vue ByteView empruntee ne doit traverser un
     * co_await.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] virtual Async<Bytes> read_exactly(std::size_t count) = 0;
    /**
     * @brief Emet la totalite du buffer ou retourne une erreur; ne laisse pas de ByteView en
     * suspens.
     * @param data Donnees d entree possedees ou vue empruntee selon la signature.
     * @return Coroutine retournant Result<T> ; les echecs reseau/durabilite sont representes par
     * Error.
     * @note Coroutine : aucun verrou bloquant ni vue ByteView empruntee ne doit traverser un
     * co_await.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] virtual Async<void> write_all(Bytes data) = 0;
    /**
     * @brief Interrompt le canal et les operations pendantes de facon idempotente.
     */
    virtual void close() noexcept = 0;
};

} // namespace ygg::net
