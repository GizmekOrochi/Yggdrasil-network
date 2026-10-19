#pragma once

/**
 * @file ITransport.hpp
 * @brief Port pour le transport reseau securise.
 */

#include "ygg/net/IChannel.hpp"
#include <memory>

namespace ygg::net {

/**
 * @brief Composant ITransport du module net.
 * @details Etablit ou accepte un canal securise independamment du framing YGG/1.
 * @par Responsabilite du fichier Port de transport de connexions directes.
 * @par Regles importantes Le canal n est remis au PairManager qu apres verification TLS et
 * autorisation explicite.
 * @par Contrat de duree de vie Les dependances recues par reference restent propriete de leur
 * createur; les ressources internes sont liberees au plus tard au destructeur.

 * @par Contrat complementaire initial
 * Transport accepts or dials approved secure sessions.
 */
class ITransport {
public:
    /**
     * @brief Libere les ressources de cette instance conformement aux regles RAII; ne doit pas
     * propager d exception.
     * @post Les ressources acquises ne sont plus accessibles.
     */
    virtual ~ITransport() = default;
    /**
     * @brief Etablit une connexion sortante et renvoie un canal uniquement apres validation TLS et
     * trust.
     * @param destination Adresse distante a contacter ou destination locale de sortie selon le
     * contexte.
     * @return Coroutine retournant Result<T> ; les echecs reseau/durabilite sont representes par
     * Error.
     * @note Coroutine : aucun verrou bloquant ni vue ByteView empruntee ne doit traverser un
     * co_await.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] virtual Async<std::shared_ptr<IChannel>> dial(Endpoint destination) = 0;
    /**
     * @brief Attend un canal entrant et retourne seulement un canal dont le pair est verifie et
     * autorise.
     * @return Coroutine retournant Result<T> ; les echecs reseau/durabilite sont representes par
     * Error.
     * @note Coroutine : aucun verrou bloquant ni vue ByteView empruntee ne doit traverser un
     * co_await.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] virtual Async<std::shared_ptr<IChannel>> accept() = 0;
    /**
     * @brief Arrete la prise de connexions et ferme les ressources du transport.
     */
    virtual void stop() noexcept = 0;
};

} // namespace ygg::net
