#pragma once

/**
 * @file PeerManager.hpp
 * @brief Gestionnaire de pairs independant des applications.
 */

#include "ygg/net/ITransport.hpp"
#include "ygg/net/PeerSession.hpp"
#include "ygg/net/RetryPolicy.hpp"
#include "ygg/identity/PeerTrustStore.hpp"
#include <map>
#include <vector>
#include <array>

namespace ygg::net {

/**
 * @brief Autorite unique sur les sessions d un processus yggd.
 * @details Conserver au plus une session active par identite; l admission ne doit jamais depender d
 * un nom ou d une adresse non prouvee.
 * @par Responsabilite du fichier Orchestrateur de connexions par identite.
 * @par Regles importantes La table ne doit contenir que des sessions rattachees a un PeerId
 * authentifie. Toutes les operations concurrentes doivent etre serialisees ou synchronisees.
 * @par Contrat de duree de vie Les dependances recues par reference restent propriete de leur
 * createur; les ressources internes sont liberees au plus tard au destructeur.

 * @par Contrat complementaire initial
 * Manage authenticated connections without any knowledge of files. Competing inbound/outbound
 * sessions must be deduplicated deterministically using stable peer identifiers.
 */
class PeerManager final {
public:
    /**
     * @brief Injecte transport, registre de confiance et politique de reconnexion sans en prendre
     * possession.
     * @note Les references injectees ne sont pas possedees; leurs objets doivent survivre a cette
     * instance.
     * @param transport Transport securise injecte, dont la duree de vie est externe.
     * @param trust Registre ou verificateur de confiance injecte; ne pas le detruire avant ce
     * composant.
     * @param retry Politique de reconnexion partagee.
     */
    PeerManager(ITransport& transport, identity::PeerTrustStore& trust,const RetryPolicy& retry);
    /**
     * @brief Dial un endpoint candidat, verifie le PeerId et associe une session dedupliquee.
     * @param address Adresse candidate distante (IP/nom et port).
     * @return Coroutine retournant Result<T> ; les echecs reseau/durabilite sont representes par
     * Error.
     * @post Une session active est referencee par identite, non par endpoint. Toute collision
     * inbound/outbound est dedupliquee.
     * @note Coroutine : aucun verrou bloquant ni vue ByteView empruntee ne doit traverser un
     * co_await.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] Async<PeerId> connect(Endpoint address);
    /**
     * @brief Accepte les canaux entrants jusqu a arret, en exigeant l identite approuvee de chacun.
     * @return Coroutine retournant Result<T> ; les echecs reseau/durabilite sont representes par
     * Error.
     * @note Coroutine : aucun verrou bloquant ni vue ByteView empruntee ne doit traverser un
     * co_await.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] Async<void> accept_loop();
    /**
     * @brief Recherche la session ACTIVE et transmet une trame en respectant la backpressure.
     * @param target PairId de destination ou cible du travail selon le contexte.
     * @param frame Trame YGG/1 dont les donnees sont possedees.
     * @return Coroutine retournant Result<T> ; les echecs reseau/durabilite sont representes par
     * Error.
     * @note Coroutine : aucun verrou bloquant ni vue ByteView empruntee ne doit traverser un
     * co_await.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] Async<void> send(PeerId target, protocol::Frame frame);
    /**
     * @brief Retourne une reference partagee a la session du PairId authentifie, sinon unavailable.
     * @param id Identifiant opaque de l objet recherche ou modifie.
     * @return Resultat explicite : valeur valide en succes, ErrorCode type en echec.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] Result<std::shared_ptr<PeerSession>> get(PeerId id) const;
    /**
     * @brief Retourne un instantane des PeerId effectivement connectes et actifs.
     * @return Resultat de la lecture ou de l operation selon le type declare.
     */
    [[nodiscard]] std::vector<PeerId> connected_peers() const;
    /**
     * @brief Ferme localement la session du peer et supprime sa presence active.
     * @param id Identifiant opaque de l objet recherche ou modifie.
     */
    void disconnect(PeerId id) noexcept;
    /**
     * @brief Interrompt les connexions et ferme toutes les sessions; supporte plusieurs appels.
     */
    void shutdown() noexcept;
private:
    /**
     * @brief Port de transport externe; non possede par le gestionnaire.
     */
    ITransport& transport_;
    /**
     * @brief Store de confiance externe ou reference au verificateur.
     */
    identity::PeerTrustStore& trust_;
    /**
     * @brief Politique de connexion externe, reutilisable et immuable.
     */
    const RetryPolicy& retry_;
    /**
     * @brief Map des sessions courantes, indexee par octets du PeerId.
     */
    std::map<std::array<std::byte,32>, std::shared_ptr<PeerSession>> sessions_{};
};

} // namespace ygg::net
