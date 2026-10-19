#pragma once

/**
 * @file SecureChannel.hpp
 * @brief Canal TLS verifie avec ressources RAII.
 */

#include "ygg/net/IChannel.hpp"
#include <memory>

namespace ygg::net {
    
/**
 * @brief Composant SecureChannel du module net.
 * @details Detient le socket SSL et les etats de lecture/ecriture, et propose des buffers
 * entierement possedes.
 * @par Responsabilite du fichier Adaptateur canal vers un flux TLS verifie.
 * @par Regles importantes Toutes les operations TLS doivent etre serialisees correctement; le
 * destructeur ferme et nettoie les ressources.
 * @par Contrat de duree de vie Les dependances recues par reference restent propriete de leur
 * createur; les ressources internes sont liberees au plus tard au destructeur.

 * @par Contrat complementaire initial
 * Owns a TLS stream and serializes concurrent reads and writes.
 */
class SecureChannel final : public IChannel {
public:
    /**
     * @brief Libere les ressources de cette instance conformement aux regles RAII; ne doit pas
     * propager d exception.
     * @post Les ressources acquises ne sont plus accessibles.
     */
    ~SecureChannel() override;
    /**
     * @brief Retourne le PairId derive du certificat mutuellement valide.
     * @return Identite stable du pair, issue des preuves cryptographiques.
     */
    [[nodiscard]] PeerId verified_peer() const noexcept override;
    /**
     * @brief Cumule les lectures TLS jusqu a obtenir count octets ou une erreur.
     * @param count Nombre d octets souhaites pour la plage, borne a 256 Kio en transfert.
     * @return Coroutine retournant Result<T> ; les echecs reseau/durabilite sont representes par
     * Error.
     * @note Coroutine : aucun verrou bloquant ni vue ByteView empruntee ne doit traverser un
     * co_await.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] Async<Bytes> read_exactly(std::size_t count) override;
    /**
     * @brief Emet tous les octets dans une voie TLS serialisee, en gerant l ecriture partielle.
     * @param data Donnees d entree possedees ou vue empruntee selon la signature.
     * @return Coroutine retournant Result<T> ; les echecs reseau/durabilite sont representes par
     * Error.
     * @note Coroutine : aucun verrou bloquant ni vue ByteView empruntee ne doit traverser un
     * co_await.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] Async<void> write_all(Bytes data) override;
    /**
     * @brief Arrete la session TLS, libere le socket et annule les I/O en cours.
     */
    void close() noexcept override;
private:
    friend class TcpTlsTransport;
    /**
     * @brief Operation du contrat SecureChannel ; les garanties de son interface et de son module
     * doivent etre respectees.
     */
    SecureChannel();
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
