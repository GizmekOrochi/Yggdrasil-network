#pragma once

/**
 * @file PeerSession.hpp
 * @brief Automate d une session P2P et files bornees.
 */

#include "ygg/net/IChannel.hpp"
#include "ygg/protocol/Frame.hpp"
#include <memory>

namespace ygg::net {

/**
 * @brief Liste des etats/codes stables de PeerState.
 * @details Les valeurs sont contractuelles; les transitions et interpretations doivent etre
 * explicitement validees.
 * @par Responsabilite du fichier Session applicative associee a un pair.
 * @par Regles importantes Ne jamais servir DATA avant Active; gerer les receptions fragmentees, la
 * pression memoire et la fermeture concurrente.

 * @par Contrat complementaire initial
 * Exact state progression for one authenticated peer session.
 */
enum class PeerState {
    /**
     * @brief Endpoint candidat non connecte.
     */
    discovered,
    /**
     * @brief Tentative d etablissement reseau.
     */
    dialing,
    /**
     * @brief Certificat et identite TLS verifies.
     */
    tls_verified,
    /**
     * @brief Identite egale a l entree de confiance approuvee.
     */
    authorized,
    /**
     * @brief Negociation HELLO/WELCOME terminee; DATA autorisee.
     */
    active,
    /**
     * @brief Attente avant nouvelle tentative transitoire.
     */
    backoff,
    /**
     * @brief Echec de confiance ou de politique, pas de retry automatique.
     */
    rejected,
    /**
     * @brief Session et canal definitivement fermes.
     */
    closed
};

/**
 * @brief Automate d un seul canal authentifie.
 * @details L ordre HELLO/WELCOME et l activation doivent etre finis avant toute donnée applicative.
 * @par Responsabilite du fichier Session applicative associee a un pair.
 * @par Regles importantes Ne jamais servir DATA avant Active; gerer les receptions fragmentees, la
 * pression memoire et la fermeture concurrente.
 * @par Contrat de duree de vie Les dependances recues par reference restent propriete de leur
 * createur; les ressources internes sont liberees au plus tard au destructeur.

 * @par Contrat complementaire initial
 * Application session over a secure channel. Read/write operations are serialized, receive
 * buffers and outbound queues are bounded, and no business logic is run inside transport
 * internals.
 */
class PeerSession final {
public:
    /**
     * @brief Associe un PeerId verifie a un IChannel et limite la profondeur de la file sortante.
     * @param peer Identite authentifiee du pair lie a la session.
     * @param channel Canal TLS deja verifie et conserve durant la session.
     * @param queue_limit Nombre maximal de trames ou demandes en attente, borne.
     */
    PeerSession(PeerId peer, std::shared_ptr<IChannel> channel, std::size_t queue_limit);
    /**
     * @brief Execute les controles protocolaires puis lance la gestion des trames et des queues.
     * @return Coroutine retournant Result<T> ; les echecs reseau/durabilite sont representes par
     * Error.
     * @pre Le IChannel correspond deja a un pair authentifie. Les messages HELLO/WELCOME doivent
     * etre controles avant Active.
     * @note Coroutine : aucun verrou bloquant ni vue ByteView empruntee ne doit traverser un
     * co_await.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] Async<void> start();
    /**
     * @brief Enfile/emet une trame sur le canal autorise; applique une limite de file pour eviter
     * la surconsommation.
     * @param frame Trame YGG/1 dont les donnees sont possedees.
     * @return Coroutine retournant Result<T> ; les echecs reseau/durabilite sont representes par
     * Error.
     * @note Coroutine : aucun verrou bloquant ni vue ByteView empruntee ne doit traverser un
     * co_await.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] Async<void> send(protocol::Frame frame);
    /**
     * @brief Renvoie l identite authentifiee et immuable de la session.
     * @return Identite stable du pair, issue des preuves cryptographiques.
     */
    [[nodiscard]] PeerId peer_id() const noexcept;
    /**
     * @brief Expose une lecture coherente de l etat de la machine a etats.
     * @return Etat courant de la session.
     */
    [[nodiscard]] PeerState state() const noexcept;
    /**
     * @brief Ferme la session et le canal associe sans laisser de coroutines pendantes.
     */
    void close() noexcept;
private:
    /**
     * @brief PairId de la session; reste constant apres le handshake.
     */
    PeerId peer_{};
    /**
     * @brief Canal securise partage dont la fermeture termine la session.
     */
    std::shared_ptr<IChannel> channel_{};
    /**
     * @brief Etat prive de l adaptateur protege par les invariants RAII.
     */
    PeerState state_{PeerState::tls_verified};
    /**
     * @brief Plafond de messages en attente pour cette session.
     */
    std::size_t queue_limit_{};
};

} // namespace ygg::net
