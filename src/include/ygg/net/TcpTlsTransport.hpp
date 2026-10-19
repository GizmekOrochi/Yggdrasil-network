#pragma once

/**
 * @file TcpTlsTransport.hpp
 * @brief Adaptateur TCP et TLS 1.3 a verification mutuelle.
 */

#include "ygg/net/ITransport.hpp"
#include "ygg/identity/NodeIdentity.hpp"
#include "ygg/identity/CertificateVerifier.hpp"
#include <boost/asio/io_context.hpp>

namespace ygg::net {

/**
 * @brief Composant TcpTlsTransport du module net.
 * @details Ecoute et compose des connexions directes, puis remet des SecureChannel verifies.
 * @par Responsabilite du fichier Adaptateur Boost.Asio/OpenSSL TLS 1.3.
 * @par Regles importantes Interdire les contournements du pinning, refuser les certificats
 * invalides, imposer l authentification mutuelle et borner les timeouts.
 * @par Contrat de duree de vie Les dependances recues par reference restent propriete de leur
 * createur; les ressources internes sont liberees au plus tard au destructeur.

 * @par Contrat complementaire initial
 * TCP/TLS 1.3 adapter backed by Boost.Asio and OpenSSL. Outbound and inbound peers must pass
 * certificate verification and trust checks before their IChannel is returned to the caller.
 */
class TcpTlsTransport final : public ITransport {
public:
    /**
     * @brief Configure boucle Asio, adresse d ecoute, identite locale et controleur de certificats.
     * @param io Boucle Boost.Asio injectee; doit survivre au transport.
     * @param bind_address Adresse locale d ecoute TCP.
     * @param self Identite locale de signature/certificat conservee par l application.
     * @param verifier Verificateur de certificats distants injecte.
     */
    TcpTlsTransport(boost::asio::io_context& io, Endpoint bind_address, const identity::NodeIdentity& self, const identity::CertificateVerifier& verifier);
    /**
     * @brief Compose un socket TCP, exige TLS 1.3 mutuel, valide la chaine et le pinning, puis
     * remet un canal.
     * @param destination Adresse distante a contacter ou destination locale de sortie selon le
     * contexte.
     * @return Coroutine retournant Result<T> ; les echecs reseau/durabilite sont representes par
     * Error.
     * @note Coroutine : aucun verrou bloquant ni vue ByteView empruntee ne doit traverser un
     * co_await.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] Async<std::shared_ptr<IChannel>> dial(Endpoint destination) override;
    /**
     * @brief Attend une connexion, effectue le handshake TLS serveur et valide le client avant de
     * la retourner.
     * @return Coroutine retournant Result<T> ; les echecs reseau/durabilite sont representes par
     * Error.
     * @note Coroutine : aucun verrou bloquant ni vue ByteView empruntee ne doit traverser un
     * co_await.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] Async<std::shared_ptr<IChannel>> accept() override;
    /**
     * @brief Ferme acceptor et canaux possedes, et interrompt l acceptation des connexions.
     */
    void stop() noexcept override;
private:
    /**
     * @brief Declaration anticipee de State; la definition/etat se trouve dans l implementation.
     */
    struct State;
    /**
     * @brief Etat prive de l adaptateur protege par les invariants RAII.
     */
    std::unique_ptr<State> state_;
};

} // namespace ygg::net
